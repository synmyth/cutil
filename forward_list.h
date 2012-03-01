#ifndef _FORWARD_LIST_H_
#define _FORWARD_LIST_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "util_define.h"

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
};

void forward_list_init(struct forward_list *l, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *));
void forward_list_destroy(struct forward_list *l);
inline int forward_list_empty(struct forward_list *l);
inline size_t forward_list_size(struct forward_list *l);
inline void* forward_list_front(struct forward_list *l);
void forward_list_clear(struct forward_list *l);
inline void forward_list_push_front(struct forward_list *l, void *element);
inline void forward_list_pop_front(struct forward_list *l);
void forward_list_reverse(struct forward_list *l);
inline struct flist_node* __alloc_new_flist_node(struct forward_list *l, void *element);
inline void __free_flist_node(struct forward_list *l, struct flist_node *n);

/* initialize the list */
void forward_list_init(struct forward_list *l, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *))
{
	assert(l && elem_size > 0);

	memset(l, 0, sizeof(struct forward_list));
	l->elem_size = elem_size;
	l->copy = copy_func;
	l->free = free_func;
}

/* destroy the list */
void forward_list_destroy(struct forward_list *l)
{
	assert(l);
	forward_list_clear(l);
}

/* checks whether the container is empty */
inline int forward_list_empty(struct forward_list *l)
{
	assert(l);
	return !l->size;
}

/* returns the number of elements */
inline size_t forward_list_size(struct forward_list *l)
{
	assert(l);
	return l->size;
}

/* access the first element */
inline void* forward_list_front(struct forward_list *l)
{
	assert(l && !forward_list_empty(l));
	struct flist_node *first = l->head.next;
	assert (first && first->data);
	return first->data;
}

/* clears the contents */
void forward_list_clear(struct forward_list *l)
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
inline void forward_list_push_front(struct forward_list *l, void *element)
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
inline void forward_list_pop_front(struct forward_list *l)
{
	assert(l && !forward_list_empty(l));

	struct flist_node *head = &l->head;
	struct flist_node *first = head->next;
	struct flist_node *node_after_first = first->next;

	/* remove last node */
	head->next = node_after_first;
	__free_flist_node(l, first);
	l->size--;
}

/* reverses the order of the elements */
void forward_list_reverse(struct forward_list *l)
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

inline struct flist_node* __alloc_new_flist_node(struct forward_list *l, void *element)
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

inline void __free_flist_node(struct forward_list *l, struct flist_node *n)
{
	if (l->free != NULL) {
		l->free(n->data);
	}

	free(n->data);
	free(n);
}

#endif
