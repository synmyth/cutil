#ifndef _QUEUE_H_
#define _QUEUE_H_
#include "deque.h"

#define QUEUE_INIT(q, elem_size)	queue_init((q), (elem_size), NULL, NULL)

typedef struct {
	deque_t d;
} queue_t;

/* initialize the queue */
static inline void queue_init(queue_t *q, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *))
{
	assert(q && elem_size > 0);
	deque_init(&q->d, elem_size, copy_func, free_func);
}

/* destroy the queue */
static inline void queue_destroy(queue_t *q)
{
	assert(q);
	deque_destroy(&q->d);
}

/* checks whether the container is empty */
static inline int queue_empty(queue_t *q)
{
	assert(q);
	return deque_empty(&q->d);
}

/* returns the number of elements */
static inline size_t queue_size(queue_t *q)
{
	assert(q);
	return deque_size(&q->d);
}

/* access the first element */
static inline void* queue_front(queue_t *q)
{
	assert(q && !queue_empty(q));
	return deque_front(&q->d);
}

/* access the last element */
static inline void* queue_back(queue_t *q)
{
	assert(q && !queue_empty(q));
	return deque_back(&q->d);
}

/* inserts element at the end */
static inline void queue_push(queue_t *q, void *element)
{
	assert(q && element);
	deque_push_back(&q->d, element);
}

/* removes the first element */
static inline void queue_pop(queue_t *q)
{
	assert(q && !queue_empty(q));
	deque_pop_front(&q->d);
}

#endif
