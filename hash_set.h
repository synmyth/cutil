#ifndef _HASH_SET_H_
#define _HASH_SET_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "util_define.h"
#include "iterator.h"
#include "hash.h"

#define MAX_BUCKET_CAPACITY	11
#define EQUALS(e1, e2, h)	(h->compare) ?	\
	(!h->compare(e1, e2)) : (!memcmp(e1, e2, h->key_size))

typedef struct hash_set hash_set_t;

struct chain_node {
	struct chain_node *next;
	void *data;
};

struct bucket {
	struct chain_node *first;
	size_t size;
};

struct hash_set {
	struct bucket *buckets;
	size_t elem_size;
	size_t key_size;
	size_t size;
	size_t bucket_size;
	size_t max_bucket_capacity;
	int has_long_chain;
	size_t long_chain_index;
	void (*copy)(void *dest, void *src);
	void (*free)(void *element);
	int (*compare)(void *e1, void *e2);
	void (*iter_head)(iterator_t *it, hash_set_t *h);
	void (*iter_next)(iterator_t *it, hash_set_t *h);
};

/* function prototype */
void hset_init(hash_set_t *h, size_t elem_size,
		void (*copy_func)(void *, void *),
		void (*free_func)(void *),
		int (*cmp_func)(void *, void *));
void hset_clear(hash_set_t *h);
void hset_insert(hash_set_t *h, void *key);
void hset_erase(hash_set_t *h, void *key);
void hset_find(hash_set_t *h, void *key, iterator_t *it);

/* destroy the hash set */
static inline void hset_destroy(hash_set_t *h)
{
	assert(h);
	hset_clear(h);
	free(h->buckets);
	h->buckets = NULL;
}

/* checks whether the hash set is empty */
static inline int hset_empty(hash_set_t *h)
{
	assert(h);
	return !h->size;
}

/* return the number of elements */
static inline size_t hset_size(hash_set_t *h)
{
	assert(h);
	return h->size;
}

static inline void __set_key_size(hash_set_t *h, size_t key_size)
{
	assert(h && key_size > 0 && key_size <= h->elem_size);
	h->key_size = key_size;
}

static inline void __free_chain_node(hash_set_t *h, struct chain_node *n)
{
	assert(h && n);
	if (h->free != NULL) {
		h->free(n->data);
	}

	free(n->data);
	free(n);
}

#endif
