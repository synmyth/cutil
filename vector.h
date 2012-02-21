#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CONTAINER_CAPACITY	32

struct vector {
	void *array;
	size_t capacity;
	size_t size;
	size_t elem_size;
	void (*copy)(void *dest, void *src);
	void (*free)(void *element);
};

/* function prototype */
void vector_init(struct vector *v, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *));
void vector_destroy(struct vector *v);
inline int vector_empty(struct vector *v);
inline size_t vector_capacity(struct vector *v);
inline size_t vector_size(struct vector *v);
inline void* vector_at(struct vector *v, size_t position);
inline void* vector_front(struct vector *v);
inline void* vector_back(struct vector *v);
inline void vector_append(struct vector *v, void *element);
void vector_insert(struct vector *v, void *element, size_t position);
void vector_replace(struct vector *v, void *element, size_t position);
void vector_reserve(struct vector *v, size_t n);
void vector_clear(struct vector *v);
void vector_delete(struct vector *v, size_t position);
inline void __default_copy_func(void *dest, void *src, size_t elem_size);

void vector_init(struct vector *v, size_t elem_size,
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
}

void vector_destroy(struct vector *v)
{
	assert(v);

	if (v->array != NULL) {
		vector_clear(v);
		free(v->array);
		v->array = NULL;
	}
}

inline int vector_empty(struct vector *v)
{
	assert(v);
	return !v->size;
}

inline size_t vector_capacity(struct vector *v)
{
	assert(v);
	return v->capacity;
}

inline size_t vector_size(struct vector *v)
{
	assert(v);
	return v->size;
}

inline void* vector_at(struct vector *v, size_t position)
{
	assert(v && v->array && position < v->size);
	return (char *)v->array + position * v->elem_size;
}

inline void* vector_front(struct vector *v)
{
	assert(v);
	return vector_at(v, 0);
}

inline void* vector_back(struct vector *v)
{
	assert(v && v->size);
	return vector_at(v, v->size - 1);
}

inline void vector_append(struct vector *v, void *element)
{
	assert(v);
	vector_insert(v, element, v->size);
}

void vector_insert(struct vector *v, void *element, size_t position)
{
	assert(v && v->array && element && position <= v->size);

	if (v->size == v->capacity) {
		vector_reserve(v, v->capacity * 2);
	}

	char *pos_ptr = (char *)v->array + position * v->elem_size;
	memmove(pos_ptr + v->elem_size, pos_ptr,
			(v->size - position) * v->elem_size);
	if (v->copy != NULL) {
		v->copy(pos_ptr, element);
	} else {
		__default_copy_func(pos_ptr, element, v->elem_size);
	}
	v->size++;
}

void vector_replace(struct vector *v, void *element, size_t position)
{
	assert(v && element && position < v->size);

	void *pos_ptr = (char *)v->array + position * v->elem_size;
	if (v->free != NULL) {
		v->free(pos_ptr);
	}

	if (v->copy != NULL) {
		v->copy(pos_ptr, element);
	} else {
		__default_copy_func(pos_ptr, element, v->elem_size);
	}
}

void vector_reserve(struct vector *v, size_t n)
{
	assert(v && n > v->capacity);

	v->capacity = n;
	v->array = realloc(v->array, v->capacity * v->elem_size);
}

void vector_clear(struct vector *v)
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

void vector_delete(struct vector *v, size_t position)
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

inline void __default_copy_func(void *dest, void *src, size_t elem_size)
{
	memcpy(dest, src, elem_size);
}

#endif
