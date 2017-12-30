#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define DEF_DB_PATH "./db"
#define DEF_NUM_BUF 5
#define UNUSED(x) (void)(x)


int init_db(uint64_t buf_size);
int shutdown_db();
int open_table(char *pathname);
int close_table(int table_id);
int insert(int table_id, int64_t key, char *value);
char *find(int table_id, int64_t key);
int delete(int table_id, int64_t key);
int join_table(int table_id_1, int table_id_2, char * pathname);

#define NUM_CHARSET 62
char charset[NUM_CHARSET+1] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";


int table_id;
int table_id_test;

void set_val(char *buf){
	for (int i = 0; i < 119; i++){
		buf[i] = charset[rand()%NUM_CHARSET];
	}
	buf[119] = 0;
}

int shell(){
	char instruction;
	int64_t k;
	char v[120];
	char *find_ret;
	int ret, id;

	//printf("TABLE ID: %d, %d\n", table_id, table_id_test);
	printf("> ");
	while (scanf("%c", &instruction) != EOF) {
		switch (instruction) {
		case 'd':
			ret = scanf("%ld", &k);
			UNUSED(ret);
			if (delete(table_id, k) != 0){
				printf("Delete Failed\n");
			}
			break;
		case 'i':
			memset(v, 0, sizeof(v));
			ret = scanf("%d %ld %s", &id, &k, v);
			UNUSED(ret);
			if (insert(id, k, v) != 0){
				printf("Insert Failed\n");
			}
			break;
		case 'f':
			ret = scanf("%d %ld", &id, &k);
			UNUSED(ret);
			if ((find_ret = find(id, k)) == NULL){
				printf("Not found\n");
			} else{
				printf("TBL%d: %ld %s\n", id, k, find_ret);
				free(find_ret);
			}
			break;
		case 'j':
			if (join_table(table_id, table_id_test, "./join_result.txt") != 0) {
				printf("Join Failed\n");
			}
			break;
		case 'q':
			while (getchar() != (int)'\n');
			return 0;
			break;
		}
		while (getchar() != (int)'\n');
		printf("> ");
	}
	printf("\n");
	return 0;
}

int main(){
	srand((unsigned int)time(NULL));
	init_db(DEF_NUM_BUF);
	table_id = open_table(DEF_DB_PATH);
	table_id_test = open_table("./db2");
	shell();
	close_table(table_id);
	shutdown_db();
	return 0;
}
