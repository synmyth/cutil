#ifndef _QUEUE_H_
#define _QUEUE_H_
#include "deque.h"

struct queue {
	struct deque d;
};

inline void queue_init(struct queue *q, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *));
inline void queue_destroy(struct queue *q);
inline int queue_empty(struct queue *q);
inline size_t queue_size(struct queue *q);
inline void* queue_front(struct queue *q);
inline void* queue_back(struct queue *q);
inline void queue_push(struct queue *q, void *element);
inline void queue_pop(struct queue *q, void *element);

/* initialize the queue */
inline void queue_init(struct queue *q, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *))
{
	assert(q && elem_size > 0);
	deque_init(&q->d, elem_size, copy_func, free_func);
}

/* destroy the queue */
inline void queue_destroy(struct queue *q)
{
	assert(q);
	deque_destroy(&q->d);
}

/* checks whether the container is empty */
inline int queue_empty(struct queue *q)
{
	assert(q);
	return deque_empty(&q->d);
}

/* returns the number of elements */
inline size_t queue_size(struct queue *q)
{
	assert(q);
	return deque_size(&q->d);
}

/* access the first element */
inline void* queue_front(struct queue *q)
{
	assert(q && !queue_empty(q));
	return deque_front(&q->d);
}

/* access the last element */
inline void* queue_back(struct queue *q)
{
	assert(q && !queue_empty(q));
	return deque_back(&q->d);
}

/* inserts element at the end */
inline void queue_push(struct queue *q, void *element)
{
	assert(q && element);
	deque_push_back(&q->d, element);
}

/* removes the first element */
inline void queue_pop(struct queue *q, void *element)
{
	assert(q && element && !queue_empty(q));
	deque_pop_front(&q->d, element);
}

#endif
