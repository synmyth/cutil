#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "util_define.h"
#include "iterator.h"

typedef struct vector vector_t;

struct vector {
	void *array;
	size_t capacity;
	size_t size;
	size_t elem_size;
	void (*copy)(void *dest, void *src);
	void (*free)(void *element);
	void (*iter_head)(iterator_t *it, vector_t *v);
	void (*iter_next)(iterator_t *it, vector_t *v);
	void (*iter_tail)(iterator_t *it, vector_t *v);
	void (*iter_prev)(iterator_t *it, vector_t *v);
};

/* function prototype */
void vector_init(vector_t *v, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *));
inline void vector_destroy(vector_t *v);
inline int vector_empty(vector_t *v);
inline size_t vector_capacity(vector_t *v);
inline size_t vector_size(vector_t *v);
inline void* vector_at(vector_t *v, size_t position);
inline void* vector_front(vector_t *v);
inline void* vector_back(vector_t *v);
inline void vector_push_back(vector_t *v, void *element);
inline void vector_pop_back(vector_t *v);
void vector_insert(vector_t *v, void *element, size_t position);
void vector_replace(vector_t *v, void *element, size_t position);
inline void vector_reserve(vector_t *v, size_t n);
inline void vector_resize(vector_t *v, size_t n);
void vector_clear(vector_t *v);
void vector_delete(vector_t *v, size_t position);
void __vector_iter_head(iterator_t *it, vector_t *v);
void __vector_iter_next(iterator_t *it, vector_t *v);
void __vector_iter_tail(iterator_t *it, vector_t *v);
void __vector_iter_prev(iterator_t *it, vector_t *v);

/* initialize the vector */
void vector_init(vector_t *v, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *))
{
	assert(v);

	v->array = malloc(elem_size * DEFAULT_CONTAINER_CAPACITY);
	assert(v->array);
	v->capacity = DEFAULT_CONTAINER_CAPACITY;
	v->size = 0;
	v->elem_size = elem_size;
	v->copy = copy_func;
	v->free = free_func;
	v->iter_head = __vector_iter_head;
	v->iter_next = __vector_iter_next;
	v->iter_tail = __vector_iter_tail;
	v->iter_prev = __vector_iter_prev;
}

/* destroy vector, free memory */
inline void vector_destroy(vector_t *v)
{
	assert(v);

	if (v->array != NULL) {
		vector_clear(v);
		free(v->array);
		v->array = NULL;
	}

	v->capacity = 0;
}

/* checks whether the container is empty */
inline int vector_empty(vector_t *v)
{
	assert(v);
	return !v->size;
}

/* returns the number of elements that can be held in currently allocated storage */
inline size_t vector_capacity(vector_t *v)
{
	assert(v);
	return v->capacity;
}

/* returns the number of elements */
inline size_t vector_size(vector_t *v)
{
	assert(v);
	return v->size;
}

/* access specified element with bounds checking */
inline void* vector_at(vector_t *v, size_t position)
{
	assert(v && v->array && position < v->size);
	return (char *)v->array + position * v->elem_size;
}

/* access the first element */
inline void* vector_front(vector_t *v)
{
	assert(v);
	return vector_at(v, 0);
}

/* access the last element */
inline void* vector_back(vector_t *v)
{
	assert(v && v->size);
	return vector_at(v, v->size - 1);
}

/* inserts elements to the end */
inline void vector_push_back(vector_t *v, void *element)
{
	assert(v && element && v->array);
	vector_insert(v, element, v->size);
}

/* removes the last element */
inline void vector_pop_back(vector_t *v)
{
	assert(v && v->array && !vector_empty(v));
	vector_delete(v, v->size - 1);
}

/* inserts elements */
void vector_insert(vector_t *v, void *element, size_t position)
{
	assert(v && v->array && element && position <= v->size);

	if (v->size == v->capacity) {
		vector_reserve(v, v->capacity * 2);
	}

	char *pos_ptr = (char *)v->array + position * v->elem_size;
	if (position < v->size) {
		memmove(pos_ptr + v->elem_size, pos_ptr,
				(v->size - position) * v->elem_size);
	}
	CONTAINER_COPY(pos_ptr, element, v);
	v->size++;
}

/* replaces specified element */
void vector_replace(vector_t *v, void *element, size_t position)
{
	assert(v && element && position < v->size);

	void *pos_ptr = (char *)v->array + position * v->elem_size;
	if (v->free != NULL) {
		v->free(pos_ptr);
	}

	CONTAINER_COPY(pos_ptr, element, v);
}

/* reserves storage */
inline void vector_reserve(vector_t *v, size_t n)
{
	assert(v && n >= v->size);

	if (v->capacity != n) {
		v->capacity = n;
		v->array = realloc(v->array, v->capacity * v->elem_size);
	}
}

inline void vector_resize(vector_t *v, size_t n)
{
	assert(v && n >= v->size);

	if (v->capacity != n) {
		v->capacity = n;
		v->array = realloc(v->array, v->capacity * v->elem_size);
	}

	void *dest = (char *)v->array + v->elem_size * v->size;
	size_t length = (n - v->size) * v->elem_size;
	memset(dest, 0, length);
	v->size = n;
}

/* clears the contents */
void vector_clear(vector_t *v)
{
	assert(v && v->array);

	if (v->free != NULL) {
		size_t i;
		for (i = 0; i < v->size; i++) {
			v->free((char *)v->array + i * v->elem_size);
		}
	}
	v->size = 0;
}

/* deletes element */
void vector_delete(vector_t *v, size_t position)
{
	assert(v && v->array && position < v->size);

	char *pos_ptr = (char *)v->array + position * v->elem_size;
	if (v->free != NULL) {
		v->free(pos_ptr);
	}

	memmove(pos_ptr, pos_ptr + v->elem_size,
			(v->size - position - 1) * v->elem_size);
	v->size--;
}

void __vector_iter_head(iterator_t *it, vector_t *v)
{
	assert(it && v);

	it->ptr = (!vector_empty(v)) ? vector_front(v) : NULL;
	it->data = it->ptr;
	it->i = 0;
	it->size = vector_size(v);
}

void __vector_iter_next(iterator_t *it, vector_t *v)
{
	assert(it && v);

	it->ptr = (++(it->i) < v->size) ? vector_at(v, it->i) : NULL;
	it->data = it->ptr;
}

void __vector_iter_tail(iterator_t *it, vector_t *v)
{
	assert(it && v);

	it->ptr = (!vector_empty(v)) ? vector_back(v) : NULL;
	it->data = it->ptr;
	it->i = vector_size(v) - 1;
	it->size = vector_size(v);
}

void __vector_iter_prev(iterator_t *it, vector_t *v)
{
	assert(it && v);

	it->ptr = ((it->i)-- > 0) ? vector_at(v, it->i) : NULL;
	it->data = it->ptr;
}

#endif
