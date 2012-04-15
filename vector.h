#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "util_define.h"
#include "iterator.h"

#define VECTOR_INIT(v, elem_size)	vector_init((v), (elem_size), NULL, NULL)

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
void vector_insert(vector_t *v, void *element, size_t position);
void vector_replace(vector_t *v, void *element, size_t position);
void vector_clear(vector_t *v);
void vector_delete(vector_t *v, size_t position);

/* destroy vector, free memory */
static inline void vector_destroy(vector_t *v)
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
static inline int vector_empty(vector_t *v)
{
	assert(v);
	return !v->size;
}

/* returns the number of elements that can be held in currently allocated storage */
static inline size_t vector_capacity(vector_t *v)
{
	assert(v);
	return v->capacity;
}

/* returns the number of elements */
static inline size_t vector_size(vector_t *v)
{
	assert(v);
	return v->size;
}

/* access specified element with bounds checking */
static inline void* vector_at(vector_t *v, size_t position)
{
	assert(v && v->array && position < v->size);
	return (char *)v->array + position * v->elem_size;
}

/* access the first element */
static inline void* vector_front(vector_t *v)
{
	assert(v);
	return vector_at(v, 0);
}

/* access the last element */
static inline void* vector_back(vector_t *v)
{
	assert(v && v->size);
	return vector_at(v, v->size - 1);
}

/* inserts elements to the end */
static inline void vector_push_back(vector_t *v, void *element)
{
	assert(v && element && v->array);
	vector_insert(v, element, v->size);
}

/* removes the last element */
static inline void vector_pop_back(vector_t *v)
{
	assert(v && v->array && !vector_empty(v));
	vector_delete(v, v->size - 1);
}

/* reserves storage */
static inline void vector_reserve(vector_t *v, size_t n)
{
	assert(v && n >= v->size);

	if (v->capacity != n) {
		v->capacity = n;
		v->array = realloc(v->array, v->capacity * v->elem_size);
	}
}

/* resizes storage, set extended storage to zero */
static inline void vector_resize(vector_t *v, size_t n)
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

#endif
