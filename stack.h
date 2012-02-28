#ifndef _STACK_H_
#define _STACK_H_
#include "vector.h"

struct stack {
	struct vector v;
	size_t elem_size;
	void (*copy)(void *dest, void *src);
	void (*free)(void *element);
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

	vector_init(&s->v, elem_size, copy_func, free_func);
	s->elem_size = elem_size;
	s->copy = copy_func;
	s->free = free_func;
}

/* destroy the stack */
inline void stack_destroy(struct stack *s)
{
	assert(s);
	vector_destroy(&s->v);
}

/* checks whether the container is empty */
inline int stack_empty(struct stack *s)
{
	assert(s);
	return vector_empty(&s->v);
}

/* returns the number of elements */
inline size_t stack_size(struct stack *s)
{
	assert(s);
	return vector_size(&s->v);
}

/* inserts element at the top */
inline void stack_push(struct stack *s, void *element)
{
	assert(s && element);
	vector_append(&s->v, element);
}

/* removes the top element */
inline void stack_pop(struct stack *s, void *element)
{
	assert(s);

	void *tmp = stack_top(s);
	if (s->copy != NULL) {
		s->copy(element, tmp);
	} else {
		__default_copy_func(element, tmp, s->elem_size);
	}

	vector_delete(&s->v, vector_size(&s->v) - 1);
}

/* access the top element */
inline void* stack_top(struct stack *s)
{
	assert(s);
	return vector_back(&s->v);
}

#endif
