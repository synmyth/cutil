#include "vector.h"

/* function prototypes */
static void __vector_iter_head(iterator_t *it, vector_t *v);
static void __vector_iter_next(iterator_t *it, vector_t *v);
static void __vector_iter_tail(iterator_t *it, vector_t *v);
static void __vector_iter_prev(iterator_t *it, vector_t *v);

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

/* iterator head function for vector */
static void __vector_iter_head(iterator_t *it, vector_t *v)
{
	assert(it && v);

	it->ptr = (!vector_empty(v)) ? vector_front(v) : NULL;
	it->data = it->ptr;
	it->i = 0;
	it->size = vector_size(v);
}

/* iterator next function for vector */
static void __vector_iter_next(iterator_t *it, vector_t *v)
{
	assert(it && v);

	it->ptr = (++(it->i) < v->size) ? vector_at(v, it->i) : NULL;
	it->data = it->ptr;
}

/* iterator tail function for vector */
static void __vector_iter_tail(iterator_t *it, vector_t *v)
{
	assert(it && v);

	it->ptr = (!vector_empty(v)) ? vector_back(v) : NULL;
	it->data = it->ptr;
	it->i = 0;
	it->size = vector_size(v);
}

/* iterator previous function for vector */
static void __vector_iter_prev(iterator_t *it, vector_t *v)
{
	assert(it && v);

	it->ptr = (++(it->i) < it->size) ?
		vector_at(v, it->size - it->i - 1) : NULL;
	it->data = it->ptr;
}
