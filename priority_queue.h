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
inline int pqueue_empty(priority_queue_t *q);
inline size_t pqueue_size(priority_queue_t *q);
inline void* pqueue_top(priority_queue_t *q);
void pqueue_push(priority_queue_t *q, void *element);
void pqueue_pop(priority_queue_t *q);

/* initialize the priority queue */
void pqueue_init(priority_queue_t *q, size_t elem_size,
		void (*copy_func)(void *, void *),
		void (*free_func)(void *),
		int (*cmp_func)(void *, void *))
{
	assert(q && elem_size > 0 && cmp_func);

	memset(q, 0, sizeof(priority_queue_t));
	vector_init(&q->v, elem_size, copy_func, free_func);
	q->compare = cmp_func;
	q->tmp_elem = malloc(elem_size);
	assert(q->tmp_elem);
}

/* destroy the priority queue */
void pqueue_destroy(priority_queue_t *q)
{
	assert(q);
	vector_destroy(&q->v);
	free(q->tmp_elem);
	q->tmp_elem = NULL;
}

/* checks whether the underlying container is empty */
inline int pqueue_empty(priority_queue_t *q)
{
	assert(q);
	return vector_empty(&q->v);
}

/* returns the number of elements */
inline size_t pqueue_size(priority_queue_t *q)
{
	assert(q);
	return vector_size(&q->v);
}

/* access the top element */
inline void* pqueue_top(priority_queue_t *q)
{
	assert(q && !pqueue_empty(q));
	return vector_at(&q->v, 0);
}

/* exchange 2 elements of the vector */
inline void __exchange(priority_queue_t *q, size_t p1, size_t p2)
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

/* inserts element and sorts the underlying container */
void pqueue_push(priority_queue_t *q, void *element)
{
	assert(q && element);

	size_t x = vector_size(&q->v);
	vector_push_back(&q->v, element);
	if (x == 0) {
		return;
	}

	while (x > 0 && q->compare(vector_at(&q->v, parent(x)),
				vector_at(&q->v, x)) < 0) {
		__exchange(q, parent(x), x);
		x = parent(x);
	}
}

/* removes the first element */
void pqueue_pop(priority_queue_t *q)
{
	assert(q && !pqueue_empty(q));

	if (vector_size(&q->v) > 1) {
		__exchange(q, 0, vector_size(&q->v) - 1);
	}
	vector_pop_back(&q->v);

	size_t cur = 0;
	while (cur < vector_size(&q->v)) {
		size_t left = lchild(cur), right = rchild(cur), largest;
		if (left < vector_size(&q->v) && q->compare(vector_at(&q->v, left),
					vector_at(&q->v, cur)) > 0) {
			largest = left;
		} else {
			largest = cur;
		}

		if (right < vector_size(&q->v) && q->compare(vector_at(&q->v, right),
					vector_at(&q->v, largest)) > 0) {
			largest = right;
		}

		if (largest != cur) {
			__exchange(q, cur, largest);
			cur = largest;
		} else {
			break;
		}
	}
}

#endif
