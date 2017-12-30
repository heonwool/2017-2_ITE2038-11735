#include "bptree.h"

static conn c;

int init_db(uint64_t num_buf){
	DEC_RET;
	RET(open_conn(&c, num_buf));
	return ret;
}

int shutdown_db(){
	close_conn(&c);
	return 0;
}

int open_table(char *pathname){
	int table_id = open_table_low(&c, pathname);

#ifdef VERBOSE_TREE
	if (table_id >= 0)
		print_tree(&c.tbls[table_id]);
#endif
	return table_id;
}

int close_table(int table_id){
	close_table_low(&c.tbls[table_id]);
	return 0;
}

int insert(int table_id, int64_t key, char *value){
	DEC_RET;
	record r;
	r.k = key;
	memcpy(r.v, value, VALUE_SIZE);
	RET(insert_low(&c.tbls[table_id], &r));
#ifdef DEBUG_TREE
	printf("%d\n", c.bfm->tot_pincnt);
#endif
#ifdef VERBOSE_TREE
	print_tree(&c.tbls[table_id]);
#endif
	return ret;
}

char *find(int table_id, int64_t key){
	record r;
	char *ret = (char *)malloc(sizeof(char)*VALUE_SIZE);
	if (find_low(&c.tbls[table_id], key, &r) != 0){
		return NULL;
	}
#ifdef DEBUG_TREE
	printf("%d\n", c.bfm->tot_pincnt);
#endif
	memcpy(ret, r.v, VALUE_SIZE);
	return ret;
}

int delete(int table_id, int64_t key){
	DEC_RET;
	RET(delete_low(&c.tbls[table_id], key));
#ifdef DEBUG_TREE
	printf("%d\n", c.bfm->tot_pincnt);
#endif
#ifdef VERBOSE_TREE
	print_tree(&c.tbls[table_id]);
#endif
	return ret;
}

void join_write(FILE * fp, int result_id, int flag) {
	if(flag == 0) {
		fwrite(c.bfm->pages[result_id].b, 4096, 1, fp);
	}

	else {
		fwrite(c.bfm->pages[result_id].b, 256 * flag, 1, fp);
	}
}

int join_table(int table_id_1, int table_id_2, char * pathname) {
	char temp[256];

	FILE * join_fp = fopen(pathname, "w");
	table *t1 = &c.tbls[table_id_1];
	table *t2 = &c.tbls[table_id_2];

	hpage *hp = get_hpage(t1);
	npage *np;
	nblock *nb;

	char *find_result;

	if (B(hp)->root == ADDR_NOT_EXIST) {
		release_page(t1, hp);
		return -1;
	}

	release_page(t1, hp);
	np = get_root(t1);
	nb = B(np);

	while(!nb->is_leaf) {
		np = get_npage(t1, nb->i_leftmost);
		nb = B(np);
	}

	while(1) {
		for(int i = 0; i < nb->num_keys; i++) {
			find_result = find(table_id_2, nb->l_recs[i].k);
			if(find_result != NULL) {
				sprintf(temp, "%ld,%s,%ld,%s\n", nb->l_recs[i].k, nb->l_recs[i].v, nb->l_recs[i].k, find_result);
				fprintf(join_fp, "%s", temp);
			}
		}

		if(nb->l_sib == ADDR_NOT_EXIST) {
			break;
		}
		release_page(t1, np);
		np = get_npage(t1, nb->l_sib);
		nb = B(np);
	}

	fclose(join_fp);
	return 0;
}
