#ifndef _FORWARD_LIST_H_
#define _FORWARD_LIST_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "util_define.h"
#include "iterator.h"

typedef struct forward_list forward_list_t;

struct flist_node {
	struct flist_node *next;
	void *data;
};

struct forward_list {
	struct flist_node head;
	size_t size;
	size_t elem_size;
	void (*copy)(void *dest, void *src);
	void (*free)(void *element);
	void (*iter_head)(iterator_t *it, forward_list_t *l);
	void (*iter_next)(iterator_t *it, forward_list_t *l);
};

void flist_init(forward_list_t *l, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *));
void flist_destroy(forward_list_t *l);
inline int flist_empty(forward_list_t *l);
inline size_t flist_size(forward_list_t *l);
inline void* flist_front(forward_list_t *l);
void flist_clear(forward_list_t *l);
inline void flist_push_front(forward_list_t *l, void *element);
inline void flist_pop_front(forward_list_t *l);
void flist_reverse(forward_list_t *l);
void flist_copy(void *dest, void *src);
void flist_free(void *element);
void __flist_iter_head(iterator_t *it, forward_list_t *l);
void __flist_iter_next(iterator_t *it, forward_list_t *l);
inline struct flist_node* __alloc_new_flist_node(forward_list_t *l, void *element);
inline void __free_flist_node(forward_list_t *l, struct flist_node *n);

/* initialize the list */
void flist_init(forward_list_t *l, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *))
{
	assert(l && elem_size > 0);

	memset(l, 0, sizeof(forward_list_t));
	l->elem_size = elem_size;
	l->copy = copy_func;
	l->free = free_func;
	l->iter_head = __flist_iter_head;
	l->iter_next = __flist_iter_next;
}

/* destroy the list */
void flist_destroy(forward_list_t *l)
{
	assert(l);
	flist_clear(l);
}

/* checks whether the container is empty */
inline int flist_empty(forward_list_t *l)
{
	assert(l);
	return !l->size;
}

/* returns the number of elements */
inline size_t flist_size(forward_list_t *l)
{
	assert(l);
	return l->size;
}

/* access the first element */
inline void* flist_front(forward_list_t *l)
{
	assert(l && !flist_empty(l));
	struct flist_node *first = l->head.next;
	assert (first && first->data);
	return first->data;
}

/* clears the contents */
void flist_clear(forward_list_t *l)
{
	assert(l);

	struct flist_node *head = &l->head;
	struct flist_node *next = head->next;
	while (next != NULL) {
		struct flist_node *tmp = next->next;
		head->next = tmp;
		__free_flist_node(l, next);
		next = tmp;
	}

	head->next = NULL;
	head->data = NULL;
	l->size = 0;
}

/* inserts elements to the beginning */
inline void flist_push_front(forward_list_t *l, void *element)
{
	assert(l && element);

	/* malloc new node */
	struct flist_node *tmp = __alloc_new_flist_node(l, element);
	assert(tmp);

	/* inserts the new node to list */
	struct flist_node *head = &l->head;
	struct flist_node *first = head->next;
	head->next = tmp;
	tmp->next = first;

	l->size++;
}

/* removes the first element */
inline void flist_pop_front(forward_list_t *l)
{
	assert(l && !flist_empty(l));

	struct flist_node *head = &l->head;
	struct flist_node *first = head->next;
	struct flist_node *node_after_first = first->next;

	/* remove last node */
	head->next = node_after_first;
	__free_flist_node(l, first);
	l->size--;
}

/* reverses the order of the elements */
void flist_reverse(forward_list_t *l)
{
	assert(l);

	struct flist_node *prev = l->head.next;
	if (prev == NULL) {
		return;
	}

	struct flist_node *next = prev->next;
	prev->next = NULL;
	while (next != NULL) {
		struct flist_node *tmp = next->next;
		next->next = prev;
		prev = next;
		next = tmp;
	}

	l->head.next = prev;
}

void flist_copy(void *dest, void *src)
{
	assert(dest && src);

	forward_list_t *dest_list = (forward_list_t *)dest;
	forward_list_t *src_list = (forward_list_t *)src;
	memset(dest_list, 0, sizeof(forward_list_t));
	iterator_t it;
	util_foreach(it, src_list) {
		flist_push_front(dest_list, it.data);
	}
}

void flist_free(void *element)
{
	assert(element);
	forward_list_t *l = (forward_list_t *)element;
	flist_destroy(l);
}

void __flist_iter_head(iterator_t *it, forward_list_t *l)
{
	assert(it && l);

	struct flist_node *first = l->head.next;
	it->ptr = first;
	it->data = (first) ? first->data : NULL;
	it->i = 0;
	it->size = flist_size(l);
}

void __flist_iter_next(iterator_t *it, forward_list_t *l)
{
	assert(it && l);

	struct flist_node *next = ((struct flist_node *)it->ptr)->next;
	it->ptr = next;
	it->data = (next) ? next->data : NULL;
	it->i++;
}

inline struct flist_node* __alloc_new_flist_node(forward_list_t *l, void *element)
{
	/* malloc new node */
	struct flist_node *tmp = malloc(sizeof(struct flist_node));
	assert(tmp);
	tmp->data = malloc(l->elem_size);
	assert(tmp->data);

	/* copy element to new node */
	CONTAINER_COPY(tmp->data, element, l);

	return tmp;
}

inline void __free_flist_node(forward_list_t *l, struct flist_node *n)
{
	if (l->free != NULL) {
		l->free(n->data);
	}

	free(n->data);
	free(n);
}

#endif
