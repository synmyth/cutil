#ifndef _STACK_H_
#define _STACK_H_
#include "deque.h"

#define STACK_INIT(s, elem_size)	stack_init((s), (elem_size), NULL, NULL)

typedef struct {
	deque_t d;
} stack_t;

/* initialize the stack */
static inline void stack_init(stack_t *s, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *))
{
	assert(s && elem_size > 0);
	deque_init(&s->d, elem_size, copy_func, free_func);
}

/* destroy the stack */
static inline void stack_destroy(stack_t *s)
{
	assert(s);
	deque_destroy(&s->d);
}

/* checks whether the container is empty */
static inline int stack_empty(stack_t *s)
{
	assert(s);
	return deque_empty(&s->d);
}

/* returns the number of elements */
static inline size_t stack_size(stack_t *s)
{
	assert(s);
	return deque_size(&s->d);
}

/* inserts element at the top */
static inline void stack_push(stack_t *s, void *element)
{
	assert(s && element);
	deque_push_back(&s->d, element);
}

/* removes the top element */
static inline void stack_pop(stack_t *s)
{
	assert(s && !stack_empty(s));
	deque_pop_back(&s->d);
}

/* access the top element */
static inline void* stack_top(stack_t *s)
{
	assert(s && !stack_empty(s));
	return deque_back(&s->d);
}

#endif
