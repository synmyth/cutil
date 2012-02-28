#ifndef _STACK_H_
#define _STACK_H_
#include "deque.h"

struct stack {
	struct deque d;
};

inline void stack_init(struct stack *s, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *));
inline void stack_destroy(struct stack *s);
inline int stack_empty(struct stack *s);
inline size_t stack_size(struct stack *s);
inline void stack_push(struct stack *s, void *element);
inline void stack_pop(struct stack *s, void* element);
inline void* stack_top(struct stack *s);

/* initialize the stack */
inline void stack_init(struct stack *s, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *))
{
	assert(s);
	deque_init(&s->d, elem_size, copy_func, free_func);
}

/* destroy the stack */
inline void stack_destroy(struct stack *s)
{
	assert(s);
	deque_destroy(&s->d);
}

/* checks whether the container is empty */
inline int stack_empty(struct stack *s)
{
	assert(s);
	return deque_empty(&s->d);
}

/* returns the number of elements */
inline size_t stack_size(struct stack *s)
{
	assert(s);
	return deque_size(&s->d);
}

/* inserts element at the top */
inline void stack_push(struct stack *s, void *element)
{
	assert(s && element);
	deque_push_back(&s->d, element);
}

/* removes the top element */
inline void stack_pop(struct stack *s, void *element)
{
	assert(s && !stack_empty(s) && element);
	deque_pop_back(&s->d, element);
}

/* access the top element */
inline void* stack_top(struct stack *s)
{
	assert(s && !stack_empty(s));
	return deque_back(&s->d);
}

#endif
