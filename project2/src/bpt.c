/*
 *  bpt.c
 */

/*
 * Project 2: API services
 *
 * Author: Jeongwoo Son
 * Original Date: 2017-10-27
 *
 */

#define Version "1.14"
/*
 *
 *  bpt:  B+ Tree Implementation
 *  Copyright (C) 2010-2016  Amittai Aviram  http://www.amittai.com
 *  All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.

 *  3. Neither the name of the copyright holder nor the names of its
 *  contributors may be used to endorse or promote products derived from this
 *  software without specific prior written permission.

 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.

 *  Author:  Amittai Aviram
 *    http://www.amittai.com
 *    amittai.aviram@gmail.edu or afa13@columbia.edu
 *  Original Date:  26 June 2010
 *  Last modified: 17 June 2016
 *
 *  This implementation demonstrates the B+ tree data structure
 *  for educational purposes, includin insertion, deletion, search, and display
 *  of the search path, the leaves, or the whole tree.
 *
 *  Must be compiled with a C99-compliant C compiler such as the latest GCC.
 *
 *  Usage:  bpt [order]
 *  where order is an optional argument
 *  (integer MIN_ORDER <= order <= MAX_ORDER)
 *  defined as the maximal number of pointers in any node.
 *
 */

#include "bpt.h"

// GLOBALS

FILE * DB_fp;
off_t Header_free, Header_root;
int64_t Header_num_pages;

// FUNCTION DEFINITIONS.

// REQUESTED APIs

/* Open existing data file using ‘pathname’ or create one if not existed.
   success, return 0. Otherwise, return non-zero value. */
int open_db( char * pathname ) {
    // Before opening the DB file, check the existence.
    int isExist = access(pathname, F_OK);

    if (isExist == 0) DB_fp = fopen(pathname, "r+");
    else DB_fp = fopen(pathname, "w+");

    if(DB_fp == NULL) return -1;

    /* Header Page Initialization */

    if (isExist == 0) {
        // 1. Free page offset
        fseek(DB_fp, 0L, SEEK_SET);
        fread(&Header_free, sizeof(off_t), 1, DB_fp);

        // 2. Root page offset
        fseek(DB_fp, 8L, SEEK_SET);
        fread(&Header_root, sizeof(off_t), 1, DB_fp);

        // 3. Number of pages.
        fseek(DB_fp, 16L, SEEK_SET);
        fread(&Header_num_pages, sizeof(int64_t), 1, DB_fp);
    }

    else {
        // 1. Free page offset
        Header_free = 4096;
        fseek(DB_fp, 0L, SEEK_SET);
        fwrite(&Header_free, 8, 1, DB_fp);

        // 2. Root page offset
        Header_root = -1;
        fseek(DB_fp, 8L, SEEK_SET);
        fwrite(&Header_root, 8, 1, DB_fp);

        // 3. Number of pages.
        Header_num_pages = 1;
        fseek(DB_fp, 16L, SEEK_SET);
        fwrite(&Header_num_pages, 8, 1, DB_fp);
    }

    return 0;
}

// OUTPUT AND UTILITIES

void sync_datafile() {
    fflush(DB_fp);
    fsync(fileno(DB_fp));
}

void terminateDBfile() {
    fclose(DB_fp);
}

int isLeaf(off_t offset) {
    int leafFlag;
    fseek(DB_fp, offset + 8L, SEEK_SET);
    fread(&leafFlag, 4, 1, DB_fp);
    return leafFlag;
}

int getNumOfKeys(off_t offset) {
    int keys;
    fseek(DB_fp, offset + 12L, SEEK_SET);
    fread(&keys, 4, 1, DB_fp);
    return keys;
}

void setNumOfKeys(off_t offset, int keys) {
    fseek(DB_fp, offset + 12L, SEEK_SET);
    fwrite(&keys, 4, 1, DB_fp);
}

off_t getParent(off_t offset) {
  off_t parent;
  fseek(DB_fp, offset, SEEK_SET);
  fread(&parent, 8, 1, DB_fp);
  return parent;
}

void setParent(off_t offset, off_t parent) {
  fseek(DB_fp, offset, SEEK_SET);
  fwrite(&parent, 8, 1, DB_fp);
}

int64_t internal_getKeyByIndex(off_t offset, int index) {
    int64_t key;
    fseek(DB_fp, offset + 128 + 16 * index, SEEK_SET);
    fread(&key, 8, 1, DB_fp);
    return key;
}

void internal_setKeyByIndex(off_t offset, int index, int64_t key) {
  fseek(DB_fp, offset + 128 + 16 * index, SEEK_SET);
  fwrite(&key, 8, 1, DB_fp);
}

off_t internal_getPageByIndex(off_t offset, int index) {
  off_t page;
  fseek(DB_fp, offset + 120 + 16 * index, SEEK_SET);
  fread(&page, 8, 1, DB_fp);
  return page;
}

void internal_setPageByIndex(int64_t offset, int index, int64_t page) {
  fseek(DB_fp, offset + 120 + 16 * index, SEEK_SET);
  fwrite(&page, 8, 1, DB_fp);
}

int64_t leaf_getKeyByIndex(off_t offset, int index) {
  int64_t key;
  fseek(DB_fp, offset + 128 + 128 * index, SEEK_SET);
  fread(&key, 8, 1, DB_fp);
  return key;
}

void leaf_setKeyByIndex(off_t offset, int index, int64_t key) {
  fseek(DB_fp, offset + 128 + 128 * index, SEEK_SET);
  fwrite(&key, 8, 1, DB_fp);
}

char * leaf_getValueByIndex(off_t offset, int index) {
  char * value = calloc(120, sizeof(char));
  fseek(DB_fp, offset + 136 + 128 * index, SEEK_SET);
  fread(value, 120, 1, DB_fp);
  return value;
}

void leaf_setValueByIndex(off_t offset, int index, char * value) {
  fseek(DB_fp, offset + 136 + 128 * index, SEEK_SET);
  fwrite(value, 120, 1, DB_fp);
}

off_t leaf_getRightSibling(off_t offset) {
  off_t right_sibling;
  fseek(DB_fp, offset + 120, SEEK_SET);
  fread(&right_sibling, 8, 1, DB_fp);
  return right_sibling;
}

void leaf_setRightSibling(off_t offset, off_t right_sibling) {
  fseek(DB_fp, offset + 120, SEEK_SET);
  fwrite(&right_sibling, 8, 1, DB_fp);
}

/* Traces the path from the root to a leaf, searching by key.
   Returns the leaf containing the given key. */
off_t find_leaf( int64_t key ) {
//  printf("IN: find_leaf()\n");

    int i = 0;
    off_t c = Header_root;
    if (c < 0) {
        return c;
    }

    while (!isLeaf(c)) {
        i = 0;
        while (i < getNumOfKeys(c)) {
            if (key >= internal_getKeyByIndex(c, i)) i++;
            else break;
        }
        c = internal_getPageByIndex(c, i);
    }
    return c;
}

/* Find the record containing input ‘key’.
   If found matching ‘key’, return matched ‘value’ string. Otherwise, return NULL. */
char * find( int64_t key ) {
//  printf("IN: find()\n");

    int i = 0;
    off_t c = find_leaf( key );
    if (c < 0) return NULL;
    for (i = 0; i < getNumOfKeys(c); i++)
        if (leaf_getKeyByIndex(c, i) == key) break;

    if (i == getNumOfKeys(c))
        return NULL;
    else
        return leaf_getValueByIndex(c, i);
}

/* Finds the appropriate place to split a node that is too big into two. */
int cut( int length ) {
    if (length % 2 == 0)
        return length/2;
    else
        return length/2 + 1;
}

// INSERTION

/* Creates a new general page, which can be adapted
 * to serve as either a leaf or an internal page.
 */
off_t make_node( void ) {
    off_t new_page = Header_free;
    off_t next_free_page = Header_free + 4096;

    // Assign next free page offset to header page.
    fseek(DB_fp, 0, SEEK_SET);
    fwrite(&next_free_page, 8, 1, DB_fp);
    Header_free = next_free_page;

    // Increase the number of pages in Header page.
    Header_num_pages++;
    fseek(DB_fp, 0, SEEK_SET);
    fwrite(&Header_num_pages, 8, 1, DB_fp);

    // Initial page header
    int is_leaf = false, num_keys = 0;
    off_t page_offset = -1, right_sibling_offset = 100;

    fseek(DB_fp, new_page, SEEK_SET);
    fwrite(&page_offset, 8, 1, DB_fp);
    fwrite(&is_leaf, 4, 1, DB_fp);
    fwrite(&num_keys, 4, 1, DB_fp);

    fseek(DB_fp, new_page + 120L, SEEK_SET);
    fwrite(&right_sibling_offset, 8, 1, DB_fp);

    return new_page;
}

/* Creates a new leaf by creating a page
 * and then adapting it appropriately.
 */
off_t make_leaf( void ) {
//  printf("IN: make_leaf()\n");

    off_t leaf = make_node();
    int is_leaf = true;

    fseek(DB_fp, leaf + 8L, SEEK_SET);
    fwrite(&is_leaf, 4, 1, DB_fp);

    return leaf;
}


/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to
 * the node to the left of the key to be inserted.
 */
int get_left_index(off_t parent, off_t left) {
//    printf("IN: get_left_index()\n");

    int left_index = 0;
    while (left_index <= getNumOfKeys(parent) &&
            internal_getPageByIndex(parent, left_index) != left)
        left_index++;
    return left_index;
}

/* Inserts a new pointer to a record and its corresponding
 * key into a leaf.
 * Returns the altered leaf.
 */
int insert_into_leaf( off_t leaf, int64_t key, char * value ) {
//    printf("IN: insert_into_leaf()\n");

    int i, insertion_point;

    insertion_point = 0;
    while (insertion_point < getNumOfKeys(leaf) && \
      leaf_getKeyByIndex(leaf, insertion_point) < key)
        insertion_point++;

    for (i = getNumOfKeys(leaf); i > insertion_point; i--) {
        leaf_setKeyByIndex(leaf, i, leaf_getKeyByIndex(leaf, i - 1));
        leaf_setValueByIndex(leaf, i, leaf_getValueByIndex(leaf, i - 1));
    }

    leaf_setKeyByIndex(leaf, insertion_point, key);
    leaf_setValueByIndex(leaf, insertion_point, value);
    setNumOfKeys(leaf, getNumOfKeys(leaf) + 1);

    sync_datafile();
    return 0;
}

/* Inserts a new key and pointer
 * to a new record into a leaf so as to exceed
 * the tree's order, causing the leaf to be split
 * in half.
 */
int insert_into_leaf_after_splitting(off_t leaf, int64_t key, char * value) {
//    printf("IN: insert_into_leaf_after_splitting()\n");

    off_t new_leaf;
    int64_t temp_keys[LEAF_ORDER];
    char temp_pointers[LEAF_ORDER][120];
    int insertion_index, split, new_key, i, j;

    new_leaf = make_leaf();

    insertion_index = 0;
    while (insertion_index < LEAF_ORDER - 1 && leaf_getKeyByIndex(leaf, insertion_index) < key)
        insertion_index++;

    for (i = 0, j = 0; i < getNumOfKeys(leaf); i++, j++) {
        if (j == insertion_index) j++;
        temp_keys[j] = leaf_getKeyByIndex(leaf, i);
        strcpy(temp_pointers[j], leaf_getValueByIndex(leaf, i));
    }

    temp_keys[insertion_index] = key;
    strcpy(temp_pointers[insertion_index], value);

    setNumOfKeys(leaf, 0);

    split = cut(LEAF_ORDER - 1);

    for (i = 0; i < split; i++) {
        leaf_setKeyByIndex(leaf, i, temp_keys[i]);
        leaf_setValueByIndex(leaf, i, temp_pointers[i]);
        setNumOfKeys(leaf, getNumOfKeys(leaf) + 1);
    }

    for (i = split, j = 0; i < LEAF_ORDER; i++, j++) {
        leaf_setKeyByIndex(new_leaf, j, temp_keys[i]);
        leaf_setValueByIndex(new_leaf, j, temp_pointers[i]);
        setNumOfKeys(new_leaf, getNumOfKeys(new_leaf) + 1);
    }

    off_t tmp_offset = leaf_getRightSibling(leaf);

    leaf_setRightSibling(leaf, new_leaf);
    setParent(new_leaf, getParent(leaf));
    leaf_setRightSibling(leaf, tmp_offset);

    new_key = leaf_getKeyByIndex(new_leaf, 0);

    return insert_into_parent(leaf, new_key, new_leaf);
}


/* Inserts a new key and pointer to a node
 * into a node into which these can fit
 * without violating the B+ tree properties.
 */
 int insert_into_node(off_t n, int left_index, int64_t key, off_t right) {
//    printf("IN: insert_into_node()\n");

    int i;

    for (i = getNumOfKeys(n); i > left_index; i--) {
        internal_setPageByIndex(n, i + 1, internal_getPageByIndex(n, i));
        internal_setKeyByIndex(n, i, internal_getKeyByIndex(n, i - 1));
    }

    internal_setPageByIndex(n, left_index + 1, right);
    internal_setKeyByIndex(n, left_index, key);
    setNumOfKeys(n, getNumOfKeys(n) + 1);

    sync_datafile();
    return 0;
}


/* Inserts a new key and pointer to a node
 * into a node, causing the node's size to exceed
 * the order, and causing the node to split into two.
 */
 int insert_into_node_after_splitting(off_t old_node, int left_index,
         int64_t key, off_t right) {

//    printf("IN: insert_into_node_after_splitting()\n");

    int i, j, split;
    int64_t k_prime;
    off_t new_node, child;
    int64_t *temp_keys;
    off_t * temp_pointers;

    /* First create a temporary set of keys and pointers
     * to hold everything in order, including
     * the new key and pointer, inserted in their
     * correct places.
     * Then create a new node and copy half of the
     * keys and pointers to the old node and
     * the other half to the new.
     */

    temp_pointers = calloc( (INTERNAL_ORDER + 1), sizeof(off_t) );
    temp_keys = calloc( INTERNAL_ORDER, sizeof(int64_t) );

    for (i = 0, j = 0; i < getNumOfKeys(old_node) + 1; i++, j++) {
        if (j == left_index + 1) j++;
        temp_pointers[j] = internal_getPageByIndex(old_node, i);
    }

    for (i = 0, j = 0; i < getNumOfKeys(old_node); i++, j++) {
        if (j == left_index) j++;
        temp_keys[j] = internal_getKeyByIndex(old_node, i);
    }

    temp_pointers[left_index + 1] = right;
    temp_keys[left_index] = key;

    /* Create the new node and copy
     * half the keys and pointers to the
     * old and half to the new.
     */
    split = cut(INTERNAL_ORDER);
    new_node = make_node();
    setNumOfKeys(old_node, 0);

    for (i = 0; i < split - 1; i++) {
        internal_setPageByIndex(old_node, i, temp_pointers[i]);
        internal_setKeyByIndex(old_node, i, temp_keys[i]);
        setNumOfKeys(old_node, getNumOfKeys(old_node) + 1);
    }

    internal_setPageByIndex(old_node, i, temp_pointers[i]);
    k_prime = temp_keys[split - 1];

    for (++i, j = 0; i < INTERNAL_ORDER; i++, j++) {
        internal_setPageByIndex(new_node, j, temp_pointers[i]);
        internal_setKeyByIndex(new_node, j, temp_keys[i]);
        setNumOfKeys(new_node, getNumOfKeys(new_node) + 1);
    }

    internal_setPageByIndex(new_node, j, temp_pointers[i]);

    free(temp_pointers);
    free(temp_keys);

    setParent(new_node, getParent(old_node));

    for (i = 0; i <= getNumOfKeys(new_node); i++) {
        child = internal_getPageByIndex(new_node, i);
        setParent(child, new_node);
    }

    /* Insert a new key into the parent of the two
     * nodes resulting from the split, with
     * the old node to the left and the new to the right.
     */

    return insert_into_parent(old_node, k_prime, new_node);
}



/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
int insert_into_parent(off_t left, int64_t key, off_t right) {
//    printf("IN: insert_into_parent()\n");

    int left_index;
    off_t parent = getParent(left);

    // Case: new root.
    if (parent == -1)
        return insert_into_new_root(left, key, right);

    // Case: leaf or node. (Remainder of function body.)

    // Find the parent's pointer to the left node.
    left_index = get_left_index(parent, left);

    // Simple case: the new key fits into the node.
    if (getNumOfKeys(parent) < INTERNAL_ORDER - 1)
        return insert_into_node(parent, left_index, key, right);

    // Harder case:  split a node in order to preserve the B+ tree properties.
    return insert_into_node_after_splitting(parent, left_index, key, right);
}


/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
int insert_into_new_root(off_t left, int64_t key, off_t right) {
//    printf("IN: insert_into_new_root()\n");

    off_t root = make_node();
    if(root < 0) return root;

    internal_setKeyByIndex(root, 0, key);
    internal_setPageByIndex(root, 0, left);
    internal_setPageByIndex(root, 1, right);

    setNumOfKeys(root, getNumOfKeys(root) + 1);
    setParent(root, -1);
    setParent(left, root);
    setParent(right, root);

    Header_root = root;
    fseek(DB_fp, 8, SEEK_SET);
    fwrite(&Header_root, 8, 1, DB_fp);

    sync_datafile();
    return 0;
}



/* First insertion:
 * start a new tree.
 */
int start_new_tree(int64_t key, char * value) {
//    printf("IN: start_new_tree()\n");

    Header_root = make_leaf();

    fseek(DB_fp, 8, SEEK_SET);
    fwrite(&Header_root, 8, 1, DB_fp);

    int num_key = getNumOfKeys(Header_root) + 1;
    fseek(DB_fp, Header_root + 12L, SEEK_SET);
    fwrite(&num_key, 4, 1, DB_fp);

    fseek(DB_fp, Header_root + 128L, SEEK_SET);
    fwrite(&key, 8, 1, DB_fp);
    fwrite(value, 120, 1, DB_fp);

    sync_datafile();
    return 0;
}


/* Master insertion function.
   Insert input ‘key/value’ (record) to data file at the right place.
   If success, return 0. Otherwise, return non-zero value. */
int insert(int64_t key, char * value) {
//    printf("IN: insert() -> Now inserting key: %ld\n", key);

    off_t leaf;

    // The current implementation ignores duplicates.
    if (find(key) != NULL) return -1;

    // Case: the tree does not exist yet. Start a new tree.
    if (Header_root < 0)
        return start_new_tree(key, value);

    // Case: the tree already exists. (Rest of function body.)
    leaf = find_leaf(key);

    // Case: leaf has room for key and pointer.
    if (getNumOfKeys(leaf) < LEAF_ORDER - 1)
        return insert_into_leaf(leaf, key, value);

    // Case:  leaf must be split.
    return insert_into_leaf_after_splitting(leaf, key, value);  // REVISE
}


// DELETION.
/* Utility function for deletion.  Retrieves
 * the index of a node's nearest neighbor (sibling)
 * to the left if one exists.  If not (the node
 * is the leftmost child), returns -1 to signify
 * this special case.
 */
int get_neighbor_index( off_t n ) {

    int i;

    /* Return the index of the key to the left
     * of the pointer in the parent pointing
     * to n.
     * If n is the leftmost child, this means
     * return -1.
     */
    for (i = 0; i <= getNumOfKeys(getParent(n)); i++)
        if ( internal_getPageByIndex(getParent(n), i) == n)
            return i - 1;

    // Error state.
    printf("ERROR occured in get_neighbor_index():\n");
    printf("  Search for nonexistent pointer to node in parent.\n");
    exit(EXIT_FAILURE);
}


off_t remove_entry_from_node(off_t n, int64_t key) {
//    printf("IN remove_entry_from_node() -> ");
  //  printf("offset n(%ld) is ", n);
  //  if(isLeaf(n)) printf("leaf.\n");
  //  else printf("internal.\n");

    int i = 0, j, num_pointers;

      int k;

    // Case: offset n points leaf page.
    if(isLeaf(n)) {
        while (leaf_getKeyByIndex(n, i) != key) i++;

        for(j = i + 1; j < getNumOfKeys(n); j++) {
            leaf_setKeyByIndex(n, j - 1, leaf_getKeyByIndex(n, j));
            leaf_setValueByIndex(n, j - 1, leaf_getValueByIndex(n, j));
        }

        setNumOfKeys(n, getNumOfKeys(n) - 1);
    }

    // Case: offset n points internal page.
    else {
        while (internal_getKeyByIndex(n, i) != key) i++;

        for(j = i + 1; j < getNumOfKeys(n); j++) {
            internal_setPageByIndex(n, j - 1, internal_getPageByIndex(n, j));
            internal_setKeyByIndex(n, j - 1, internal_getKeyByIndex(n, j));
        }

        internal_setPageByIndex(n, j - 1, internal_getPageByIndex(n, j));
        internal_setPageByIndex(n, j, 0);

        setNumOfKeys(n, getNumOfKeys(n) - 1);
    }

    return n;
}


off_t adjust_root( void ) {
//    printf("IN adjust_root()\n");

    off_t root = Header_root, new_root;

    /* Case: nonempty root. */
    // Key and pointer have already been deleted, so nothing to be done.

    if (getNumOfKeys(root) > 0)
        return root;

    /* Case: empty root. */
    // If it has a child, promote the first (only) child as the new root.

    if (!isLeaf(root)) {
        new_root = internal_getPageByIndex(root, 0);
        setParent(new_root, -1);
    }

    // If it is a leaf (has no children),
    // then the whole tree is empty.

    else
        new_root = -1;

    return new_root;
}


/* Coalesces a node that has become
 * too small after deletion
 * with a neighboring node that
 * can accept the additional entries
 * without exceeding the maximum.
 */
off_t coalesce_nodes(off_t n, off_t neighbor, int neighbor_index, int k_prime) {

//    printf("IN coalesce_nodes()\n");

    int i, j, neighbor_insertion_index, n_end;
    off_t tmp, root = Header_root;

    /* Swap neighbor with node if node is on the
     * extreme left and neighbor is to its right.
     */

    if (neighbor_index != -1) {
        tmp = n;
        n = neighbor;
        neighbor = tmp;
    }

    /* Starting point in the neighbor for copying
     * keys and pointers from n.
     * Recall that n and neighbor have swapped places
     * in the special case of n being a leftmost child.
     */

    neighbor_insertion_index = getNumOfKeys(neighbor);

    /* Case:  nonleaf node.
     * Append k_prime and the following pointer.
     * Append all pointers and keys from the neighbor.
     */

    if (!isLeaf(n)) {

        // Append k_prime.

        internal_setKeyByIndex(neighbor, neighbor_insertion_index, k_prime);
        setNumOfKeys(neighbor, getNumOfKeys(neighbor) + 1);

        n_end = getNumOfKeys(n);

        for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
            internal_setKeyByIndex(neighbor, i, internal_getKeyByIndex(n, j));
            internal_setPageByIndex(neighbor, i, internal_getPageByIndex(n, j));
            setNumOfKeys(neighbor, getNumOfKeys(neighbor) + 1);
            setNumOfKeys(n, getNumOfKeys(n) - 1);
        }

        // The number of pointers is always one more than the number of keys.

        internal_setPageByIndex(neighbor, i, internal_getPageByIndex(n, j));

        // All children must now point up to the same parent.

        for (i = 0; i < getNumOfKeys(neighbor) + 1; i++) {
            tmp = internal_getPageByIndex(neighbor, i);
            setParent(tmp, neighbor);
        }
    }

    /* In a leaf, append the keys and pointers of n to the neighbor.
     * Set the neighbor's last pointer to point to
     * what had been n's right neighbor.
     */

    else {
        for (i = neighbor_insertion_index, j = 0; j < getNumOfKeys(n); i++, j++) {
            leaf_setKeyByIndex(neighbor, i, leaf_getKeyByIndex(n, j));
            leaf_setValueByIndex(neighbor, i, leaf_getValueByIndex(n, j));
            setNumOfKeys(neighbor, getNumOfKeys(neighbor) + 1);
        }
        leaf_setValueByIndex(neighbor, LEAF_ORDER - 1, leaf_getValueByIndex(n, LEAF_ORDER - 1));
    }

    root = delete_entry(getParent(n), k_prime);

    return root;
}


/* Redistributes entries between two nodes when
 * one has become too small after deletion
 * but its neighbor is too big to append the
 * small node's entries without exceeding the
 * maximum
 */
off_t redistribute_nodes(off_t n, off_t neighbor, int neighbor_index,
        int k_prime_index, int k_prime) {

//    printf("IN redistribute_nodes()\n");

    int i;
    off_t tmp, root = Header_root;

    /* Case: n has a neighbor to the left.
     * Pull the neighbor's last key-pointer pair over
     * from the neighbor's right end to n's left end.
     */

    if (neighbor_index != -1) {
        if (!isLeaf(n))
            internal_setPageByIndex(n, getNumOfKeys(n) + 1, internal_getPageByIndex(n, getNumOfKeys(n)));

        if (!isLeaf(n)) {
            for (i = getNumOfKeys(n); i > 0; i--) {
                internal_setKeyByIndex(n, i, internal_getKeyByIndex(n, i - 1));
                internal_setPageByIndex(n, i, internal_getPageByIndex(n, i - 1));
            }

            internal_setPageByIndex(n, 0, internal_getPageByIndex(neighbor, getNumOfKeys(neighbor)));
            tmp = internal_getPageByIndex(n, 0);
            setParent(tmp, n);
            internal_setPageByIndex(neighbor, getNumOfKeys(neighbor), -1);
            internal_setKeyByIndex(n, 0, k_prime);
            internal_setKeyByIndex(getParent(n), k_prime_index, internal_getKeyByIndex(neighbor, getNumOfKeys(neighbor) - 1));
        }

        else {
            for (i = getNumOfKeys(n); i > 0; i--) {
                leaf_setKeyByIndex(n, i, leaf_getKeyByIndex(n, i - 1));
                leaf_setValueByIndex(n, i, leaf_getValueByIndex(n, i - 1));
            }

            leaf_setValueByIndex(n, 0, leaf_getValueByIndex(neighbor, getNumOfKeys(neighbor) - 1));
            leaf_setValueByIndex(neighbor, getNumOfKeys(neighbor) - 1, NULL);
            leaf_setKeyByIndex(n, 0, leaf_getKeyByIndex(neighbor, getNumOfKeys(neighbor) - 1));
            internal_setKeyByIndex(getParent(n), k_prime_index, leaf_getKeyByIndex(n, 0));
        }
    }

    /* Case: n is the leftmost child.
     * Take a key-pointer pair from the neighbor to the right.
     * Move the neighbor's leftmost key-pointer pair
     * to n's rightmost position.
     */

    else {
        if (isLeaf(n)) {
            leaf_setKeyByIndex(n, getNumOfKeys(n), leaf_getKeyByIndex(neighbor, 0));
            leaf_setValueByIndex(n, getNumOfKeys(n), leaf_getValueByIndex(neighbor, 0));
            internal_setKeyByIndex(getParent(n), k_prime_index, leaf_getKeyByIndex(neighbor, 1));
        }

        else {
            internal_setKeyByIndex(n, getNumOfKeys(n), k_prime);
            internal_setPageByIndex(n, getNumOfKeys(n) + 1, internal_getPageByIndex(neighbor, 0));
            tmp = internal_getPageByIndex(neighbor, getNumOfKeys(n) + 1);
            setParent(tmp, n);
            internal_setKeyByIndex(getParent(n), k_prime_index, internal_getKeyByIndex(neighbor, 0));
        }

        for (i = 0; i < getNumOfKeys(neighbor) - 1; i++) {
            if(isLeaf(neighbor)) {
                leaf_setKeyByIndex(neighbor, i, leaf_getKeyByIndex(neighbor, i + 1));
                leaf_setValueByIndex(neighbor, i, leaf_getValueByIndex(neighbor, i + 1));
            }

            else {
                internal_setKeyByIndex(neighbor, i, internal_getKeyByIndex(neighbor, i + 1));
                internal_setPageByIndex(neighbor, i, internal_getPageByIndex(neighbor, i + 1));
          }
        }

        if (!isLeaf(n))
            internal_setPageByIndex(neighbor, i, internal_getPageByIndex(neighbor, i + 1));
    }

    /* n now has one more key and one more pointer;
     * the neighbor has one fewer of each.
     */

    setNumOfKeys(n, getNumOfKeys(n) + 1);
    setNumOfKeys(neighbor, getNumOfKeys(neighbor) - 1);

    return root;
}


/* Deletes an entry from the B+ tree.
 * Removes the record and its key and pointer
 * from the leaf, and then makes all appropriate
 * changes to preserve the B+ tree properties.
 */
 off_t delete_entry( off_t n, int64_t key ) {
//     printf("IN delete_entry() -> offset: %ld, key: %ld\n", n, key);

     off_t root = Header_root;

     int min_keys;
     off_t neighbor;
     int neighbor_index;
     int k_prime_index, k_prime;
     int capacity;

     // Remove key and pointer from node.
     n = remove_entry_from_node(n, key);

     /* Case:  deletion from the root. */
     if (n == root) return adjust_root();

     /* Case:  deletion from a node below the root.
        (Rest of function body.) */

     // Determine minimum allowable size of node, to be preserved after deletion.

     min_keys = isLeaf(n) ? cut(LEAF_ORDER - 1) : cut(INTERNAL_ORDER) - 1;

     /* Case:  node stays at or above minimum. (The simple case.) */

     if (getNumOfKeys(n) >= min_keys)
         return root;

     /* Case:  node falls below minimum.
      * Either coalescence or redistribution
      * is needed.
      */

     /* Find the appropriate neighbor node with which
      * to coalesce.
      * Also find the key (k_prime) in the parent
      * between the pointer to node n and the pointer
      * to the neighbor.
      */

    neighbor_index = get_neighbor_index(n);
    k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
    k_prime = internal_getKeyByIndex(getParent(n), k_prime_index);

    neighbor = neighbor_index == -1 ? internal_getPageByIndex(getParent(n), 1) :
        internal_getPageByIndex(getParent(n), neighbor_index);

    capacity = isLeaf(n) ? LEAF_ORDER : INTERNAL_ORDER;

    /* Coalescence. */

    if (getNumOfKeys(neighbor) + getNumOfKeys(n) < capacity)
        return coalesce_nodes(n, neighbor, neighbor_index, k_prime);

    /* Redistribution. */

    else
        return redistribute_nodes(n, neighbor, neighbor_index, k_prime_index, k_prime);
 }



/* Master deletion function.
 */
int delete(int64_t key) {
//    printf("IN delete() -> try to delete %ld\n", key);

    off_t key_leaf = find_leaf(key);
    char * key_value = find(key);

    if (key_value != NULL && key_leaf != 0) {
        Header_root = delete_entry(key_leaf, key);
        free(key_value);

        // Update the data.
        fseek(DB_fp, 8L, SEEK_SET);
        fwrite(&Header_root, 8, 1, DB_fp);
        sync_datafile();

        return 0;
    }

    else return -1;
}
