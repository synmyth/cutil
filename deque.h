#ifndef _DEQUE_H_
#define _DEQUE_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "util_define.h"

#define DEFAULT_BLOCK_CAPACITY	512

struct block {
	void *array;
	size_t size;
	size_t begin;
	size_t end;
};

struct deque {
	struct block *array;
	size_t each_block_capacity;
	size_t capacity;
	size_t size;
	size_t begin;
	size_t end;
	size_t deque_size;
	size_t elem_size;
	void (*copy)(void *dest, void *src);
	void (*free)(void *element);
};

void deque_init(struct deque *d, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *));
inline void deque_destroy(struct deque *d);
inline int deque_empty(struct deque *d);
inline size_t deque_size(struct deque *d);
void* deque_at(struct deque *d, size_t position);
inline void* deque_front(struct deque *d);
inline void* deque_back(struct deque *d);
void deque_clear(struct deque *d);
void deque_push_back(struct deque *d, void *element);
void deque_pop_back(struct deque *d, void *element);
void deque_push_front(struct deque *d, void *element);
void deque_pop_front(struct deque *d, void *element);
void __expand(struct deque *d);
void __block_destroy(struct deque *d, struct block *b);

/* initialize deque */
void deque_init(struct deque *d, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *))
{
	assert(d && elem_size > 0);

	memset(d, 0, sizeof(struct deque));
	d->capacity = DEFAULT_CONTAINER_CAPACITY;
	d->array = malloc(sizeof(struct block) * d->capacity);
	assert(d->array);

	memset(d->array, 0, sizeof(struct block) * d->capacity);
	d->each_block_capacity = DEFAULT_BLOCK_CAPACITY;
	d->elem_size = elem_size;
	d->copy = copy_func;
	d->free = free_func;
}

/* destroy deque, free memory */
inline void deque_destroy(struct deque *d)
{
	assert(d);
	deque_clear(d);
	free(d->array);
}

/* check whether the container is empty */
inline int deque_empty(struct deque *d)
{
	assert(d);
	return !d->deque_size;
}

/* return the number of elements */
inline size_t deque_size(struct deque *d)
{
	assert(d);
	return d->deque_size;
}

/* access specified element with bounds checking */
void* deque_at(struct deque *d, size_t position)
{
	assert(d && d->array && position < deque_size(d));

	size_t block_index;
	size_t element_index;
	struct block *first_block = &d->array[d->begin];
	size_t first_block_size = first_block->size;
	if (position < first_block_size) {
		block_index = d->begin;
		element_index = first_block->begin + position;
	} else {
		size_t tmp = position - first_block_size;
		block_index = d->begin + tmp / d->each_block_capacity + 1;
		element_index = tmp % d->each_block_capacity;
	}

	struct block *b = &d->array[block_index % d->capacity];
	assert(b->array && element_index <= b->end);

	return (char *)b->array + element_index * d->elem_size;
}

/* access the first element */
inline void* deque_front(struct deque *d)
{
	assert(d);
	return deque_at(d, 0);
}

/* access the last element */
inline void* deque_back(struct deque *d)
{
	assert(d);
	return deque_at(d, deque_size(d) - 1);
}

/* clears the contents */
void deque_clear(struct deque *d)
{
	assert(d);

	if (d->array == NULL || d->size == 0) {
		goto reset;
	}

	size_t i;
	size_t array_end = (d->end < d->begin) ? (d->capacity + d->end) : d->end;
	for (i = d->begin; i <= array_end; i++) {
		__block_destroy(d, &d->array[i % d->capacity]);
	}

reset:
	d->size = 0;
	d->begin = 0;
	d->end = 0;
	d->deque_size = 0;
}

/* inserts elements to the end */
void deque_push_back(struct deque *d, void *element)
{
	assert(d && d->array && element);

	/* the block array is full, need to expand */
	struct block *last_block = &d->array[d->end];
	if (d->size == d->capacity &&
			last_block->end == d->each_block_capacity - 1) {
		__expand(d);
	}

	/* find which block to insert */
	size_t element_index;
	struct block *insert;
	last_block = &d->array[d->end];
	if (d->size == 0 || last_block->end == d->each_block_capacity - 1) {
		if (d->size != 0) {
			d->end = (d->end == d->capacity - 1) ? 0 : (d->end + 1);
		} else {
			d->end = d->begin;
		}

		d->size++;
		insert = &d->array[d->end];
		insert->array = malloc(d->elem_size * d->each_block_capacity);
		assert(insert->array);

		insert->begin = element_index = 0;
	} else {
		insert = last_block;
		element_index = last_block->end + 1;
	}

	/* copy the element to destination */
	void *dest = (char *)insert->array + d->elem_size * element_index;
	if (d->copy != NULL) {
		d->copy(dest, element);
	} else {
		__default_copy_func(dest, element, d->elem_size);
	}

	/* modify block descriptor */
	insert->end = element_index;
	insert->size++;
	d->deque_size++;
}

/* removes the last element */
void deque_pop_back(struct deque *d, void *element)
{
	assert(d && d->array && element && d->deque_size > 0);

	/* find the last block */
	struct block *last_block = &d->array[d->end];
	assert(last_block->array && last_block->size > 0);

	/* copy the element to destination */
	void *src = (char *)last_block->array + d->elem_size * last_block->end;
	if (d->copy != NULL) {
		d->copy(element, src);
	} else {
		__default_copy_func(element, src, d->elem_size);
	}

	/* modify block descriptor */
	if (last_block->size == 1) {
		__block_destroy(d, last_block);
		if (d->size != 1) {
			d->end = (d->end == 0) ? (d->capacity - 1) : (d->end - 1);
		} else {
			d->end = d->begin;
		}

		d->size--;
	} else {
		last_block->end--;
		last_block->size--;
	}

	d->deque_size--;
}

/* inserts elements to the beginging */
void deque_push_front(struct deque *d, void *element)
{
	assert(d && d->array && element);

	/* the block array is full, need to expand */
	struct block *first_block = &d->array[d->begin];
	if (d->size == d->capacity &&
			(first_block->size > 0 && first_block->begin == 0)) {
		__expand(d);
	}

	/* find which block to insert */
	size_t element_index;
	struct block *insert;
	first_block = &d->array[d->begin];
	if (d->size == 0 || (first_block->size > 0 && first_block->begin == 0)) {
		if (d->size != 0) {
			d->begin = (d->begin == 0) ? (d->capacity - 1) : (d->begin - 1);
		} else {
			d->begin = d->end;
		}

		d->size++;
		insert = &d->array[d->begin];
		insert->array = malloc(d->elem_size * d->each_block_capacity);
		assert(insert->array);

		/* insert to the last of the array */
		insert->end = element_index = d->each_block_capacity - 1;
	} else {
		insert = first_block;
		element_index = first_block->begin - 1;
	}

	/* copy the element to destination */
	void *dest = (char *)insert->array + d->elem_size * element_index;
	if (d->copy != NULL) {
		d->copy(dest, element);
	} else {
		__default_copy_func(dest, element, d->elem_size);
	}

	/* modify block descriptor */
	insert->begin = element_index;
	insert->size++;
	d->deque_size++;
}

/* removes the first element */
void deque_pop_front(struct deque *d, void *element)
{
	assert(d && d->array && element && d->deque_size > 0);

	/* find the last block */
	struct block *first_block = &d->array[d->begin];
	assert(first_block->array && first_block->size > 0);

	/* copy the element to destination */
	void *src = (char *)first_block->array + d->elem_size * first_block->begin;
	if (d->copy != NULL) {
		d->copy(element, src);
	} else {
		__default_copy_func(element, src, d->elem_size);
	}

	/* modify block descriptor */
	if (first_block->size == 1) {
		__block_destroy(d, first_block);
		if (d->size != 1) {
			d->begin = (d->begin == d->capacity - 1) ? 0 : (d->begin + 1);
		} else {
			d->begin = d->end;
		}

		d->size--;
	} else {
		first_block->begin++;
		first_block->size--;
	}

	d->deque_size--;
}

/* expand the block array */
void __expand(struct deque *d)
{
	size_t old_capacity = d->capacity;
	d->capacity *= 2;
	struct block *new_array = malloc(sizeof(struct block) * d->capacity);
	assert(new_array);

	memset(new_array, 0, sizeof(struct block) * d->capacity);
	/* copy blocks in old array to new array */
	size_t i;
	size_t array_end = (d->end < d->begin) ? (old_capacity + d->end) : d->end;
	for (i = d->begin; i <= array_end; i++) {
		struct block *old_block = &d->array[i % old_capacity];
		struct block *new_block = &new_array[i - d->begin];
		memcpy(new_block, old_block, sizeof(struct block));
	}

	d->begin = 0;
	d->end = d->size - 1;
	free(d->array);
	d->array = new_array;
}

/* destroy the block, free the memory */
void __block_destroy(struct deque *d, struct block *b)
{
	assert(b);
	if (b->array != NULL) {
		if (d->free != NULL) {
			size_t i;
			for (i = b->begin; i <= b->end; i++) {
				d->free((char *)b->array + i * d->elem_size);
			}
		}

		free(b->array);
	}

	memset(b, 0, sizeof(struct block));
}

#endif
