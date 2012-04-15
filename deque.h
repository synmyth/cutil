#ifndef _DEQUE_H_
#define _DEQUE_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "util_define.h"
#include "iterator.h"

#define DEFAULT_BLOCK_CAPACITY	512
typedef struct deque deque_t;

struct block {
	void *array;
	size_t size;
	size_t begin;
	size_t end;
};

struct deque {
	struct block *array;
	size_t each_block_capacity;
	size_t capacity;
	size_t size;
	size_t begin;
	size_t end;
	size_t deque_size;
	size_t elem_size;
	void (*copy)(void *dest, void *src);
	void (*free)(void *element);
	void (*iter_head)(iterator_t *it, deque_t *d);
	void (*iter_next)(iterator_t *it, deque_t *d);
	void (*iter_tail)(iterator_t *it, deque_t *d);
	void (*iter_prev)(iterator_t *it, deque_t *d);
};

void deque_init(deque_t *d, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *));
void* deque_at(deque_t *d, size_t position);
void deque_clear(deque_t *d);
void deque_push_back(deque_t *d, void *element);
void deque_pop_back(deque_t *d);
void deque_push_front(deque_t *d, void *element);
void deque_pop_front(deque_t *d);

/* destroy deque, free memory */
static inline void deque_destroy(deque_t *d)
{
	assert(d);
	deque_clear(d);
	free(d->array);
	d->array = NULL;
	d->capacity = 0;
}

/* check whether the container is empty */
static inline int deque_empty(deque_t *d)
{
	assert(d);
	return !d->deque_size;
}

/* return the number of elements */
static inline size_t deque_size(deque_t *d)
{
	assert(d);
	return d->deque_size;
}

/* access the first element */
static inline void* deque_front(deque_t *d)
{
	assert(d);
	return deque_at(d, 0);
}

/* access the last element */
static inline void* deque_back(deque_t *d)
{
	assert(d);
	return deque_at(d, deque_size(d) - 1);
}

#endif
