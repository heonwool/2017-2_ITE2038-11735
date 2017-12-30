#ifndef __BPT_H__
#define __BPT_H__

/*
int open_db(const char* filename);
char* find(uint64_t key);
int insert(uint64_t key, const char* value);
int delete(uint64_t key);
*/

typedef struct _Buffer {
    char frame[4096];
    int table_id;
    off_t page_offset;
    int is_dirty;
    int pin_count;
} Buffer;

int init_db(int buf_num);
int open_table(char * pathname);
int insert(int table_id, int64_t key, char * value);
int delete(int table_id, int64_t key);
int close_table(int table_id);
int shutdown_db(void);

void print_tree();
#endif // __BPT_H__
