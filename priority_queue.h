#ifndef _PRIORITY_QUEUE_H
#define _PRIORITY_QUEUE_H
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "util_define.h"
#include "vector.h"

#define parent(x)	((x - 1) / 2)
#define lchild(x)	(2 * x + 1)
#define rchild(x)	(2 * x + 2)

typedef struct {
	vector_t v;
	void *tmp_elem;
	int (*compare)(void *e1, void *e2);
} priority_queue_t;

/* function prototype */
void pqueue_init(priority_queue_t *q, size_t elem_size,
		void (*copy_func)(void *, void *),
		void (*free_func)(void *),
		int (*cmp_func)(void *, void *));
void pqueue_destroy(priority_queue_t *q);
void pqueue_push(priority_queue_t *q, void *element);
void pqueue_pop(priority_queue_t *q);

/* checks whether the underlying container is empty */
static inline int pqueue_empty(priority_queue_t *q)
{
	assert(q);
	return vector_empty(&q->v);
}

/* returns the number of elements */
static inline size_t pqueue_size(priority_queue_t *q)
{
	assert(q);
	return vector_size(&q->v);
}

/* access the top element */
static inline void* pqueue_top(priority_queue_t *q)
{
	assert(q && !pqueue_empty(q));
	return vector_at(&q->v, 0);
}

/* exchange 2 elements of the vector */
static inline void __exchange(priority_queue_t *q, size_t p1, size_t p2)
{
	assert(q && q->tmp_elem);
	vector_t *v = &q->v;
	assert(p1 < vector_size(v) && p2 < vector_size(v));

	void *ptr1 = vector_at(v, p1);
	void *ptr2 = vector_at(v, p2);

	memcpy(q->tmp_elem, ptr1, v->elem_size);
	memcpy(ptr1, ptr2, v->elem_size);
	memcpy(ptr2, q->tmp_elem, v->elem_size);
}

#endif
