#ifndef _STACK_H_
#define _STACK_H_
#include "deque.h"

typedef struct {
	deque_t d;
} stack_t;

inline void stack_init(stack_t *s, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *));
inline void stack_destroy(stack_t *s);
inline int stack_empty(stack_t *s);
inline size_t stack_size(stack_t *s);
inline void stack_push(stack_t *s, void *element);
inline void stack_pop(stack_t *s);
inline void* stack_top(stack_t *s);

/* initialize the stack */
inline void stack_init(stack_t *s, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *))
{
	assert(s && elem_size > 0);
	deque_init(&s->d, elem_size, copy_func, free_func);
}

/* destroy the stack */
inline void stack_destroy(stack_t *s)
{
	assert(s);
	deque_destroy(&s->d);
}

/* checks whether the container is empty */
inline int stack_empty(stack_t *s)
{
	assert(s);
	return deque_empty(&s->d);
}

/* returns the number of elements */
inline size_t stack_size(stack_t *s)
{
	assert(s);
	return deque_size(&s->d);
}

/* inserts element at the top */
inline void stack_push(stack_t *s, void *element)
{
	assert(s && element);
	deque_push_back(&s->d, element);
}

/* removes the top element */
inline void stack_pop(stack_t *s)
{
	assert(s && !stack_empty(s));
	deque_pop_back(&s->d);
}

/* access the top element */
inline void* stack_top(stack_t *s)
{
	assert(s && !stack_empty(s));
	return deque_back(&s->d);
}

#endif
