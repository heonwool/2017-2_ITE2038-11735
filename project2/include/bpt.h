/*
 * Project 2: API services
 *
 * Author: Jeongwoo Son
 * Original Date: 2017-10-27
 *
 */

#ifndef __BPT_H__
#define __BPT_H__

// Uncomment the line below if you are compiling on Windows.
// #define WINDOWS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef WINDOWS
#define bool char
#define false 0
#define true 1
#endif

#define LEAF_ORDER 31
#define INTERNAL_ORDER 249

// FUNCTION PROTOTYPES.

// Requested APIs

/* Open existing data file using ‘pathname’ or create one if not existed.
   success, return 0. Otherwise, return non-zero value. */
int open_db( char * pathname );

/* Insert input ‘key/value’ (record) to data file at the right place.
   If success, return 0. Otherwise, return non-zero value. */
int insert( int64_t key, char * value );

/* Find the record containing input ‘key’.
   If found matching ‘key’, return matched ‘value’ string. Otherwise, return NULL. */
char * find( int64_t key );

/* Find the matching record and delete it if found.
   If success, return 0. Otherwise, return non-zero value. */
int delete( int64_t key );

// Output and utility.

void sync_datafile();
void terminateDBfile();

int isLeaf(off_t offset);
int getNumOfKeys(off_t offset);
void setNumOfKeys(off_t offset, int keys);

int64_t getParent(off_t offset);
void setParent(off_t offset, off_t parent);

int64_t internal_getKeyByIndex(int64_t offset, int index);
void internal_setKeyByIndex(int64_t offset, int index, int64_t key);
off_t internal_getPageByIndex(off_t offset, int index);
void internal_setPageByIndex(off_t offset, int index, off_t page);

int64_t leaf_getKeyByIndex(off_t offset, int index);
void leaf_setKeyByIndex(off_t offset, int index, int64_t key);
char * leaf_getValueByIndex(off_t offset, int index);
void leaf_setValueByIndex(off_t offset, int index, char * value);
off_t leaf_getRightSibling(off_t offset);
void leaf_setRightSibling(off_t offset, int64_t right_offset);

off_t find_leaf( int64_t key );
int cut( int length );

// Insertion.

off_t make_node( void );
off_t make_leaf( void );
int get_left_index(off_t parent, off_t left);

int insert_into_leaf( off_t leaf, int64_t key, char * value );
int insert_into_leaf_after_splitting(off_t leaf, int64_t key, char * value);

int insert_into_node(off_t n, int left_index, int64_t key, off_t right);
int insert_into_node_after_splitting(off_t old_node, int left_index,
    int64_t key, off_t right);

int insert_into_parent(off_t left, int64_t key, off_t right);
int insert_into_new_root(off_t left, int64_t key, off_t right);
int start_new_tree(int64_t key, char * value);

// Deletion.

int get_neighbor_index( off_t n );  // OK
off_t adjust_root( void );    // OK
off_t coalesce_nodes(off_t n, off_t neighbor, int neighbor_index, int k_prime); // HERE!
off_t redistribute_nodes(off_t n, off_t neighbor, int neighbor_index,
                      int k_prime_index, int k_prime);
off_t remove_entry_from_node(off_t n, int64_t key); // OK
off_t delete_entry( off_t n, int64_t key ); // OK

#endif /* __BPT_H__*/
