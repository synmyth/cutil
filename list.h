#ifndef _LIST_H_
#define _LIST_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "util_define.h"

struct list_node {
	struct list_node *prev;
	struct list_node *next;
	void *data;
};

struct list {
	struct list_node head;
	size_t size;
	size_t elem_size;
	void (*copy)(void *dest, void *src);
	void (*free)(void *element);
};

void list_init(struct list *l, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *));
void list_destroy(struct list *l);
inline int list_empty(struct list *l);
inline size_t list_size(struct list *l);
inline void* list_front(struct list *l);
inline void* list_back(struct list *l);
void list_clear(struct list *l);
inline void list_push_back(struct list *l, void *element);
inline void list_pop_back(struct list *l, void *element);
inline void list_push_front(struct list *l, void *element);
inline void list_pop_front(struct list *l, void *element);
void list_reverse(struct list *l);
inline struct list_node* __alloc_new_list_node(struct list *l, void *element);
inline void __free_list_node(struct list *l, struct list_node *n);

/* initialize the list */
void list_init(struct list *l, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *))
{
	assert(l && elem_size > 0);

	memset(l, 0, sizeof(struct list));
	struct list_node *head = &l->head;
	head->prev = head->next = head;
	l->elem_size = elem_size;
	l->copy = copy_func;
	l->free = free_func;
}

/* destroy the list */
void list_destroy(struct list *l)
{
	assert(l);
	list_clear(l);
}

/* checks whether the container is empty */
inline int list_empty(struct list *l)
{
	assert(l);
	return !l->size;
}

/* returns the number of elements */
inline size_t list_size(struct list *l)
{
	assert(l);
	return l->size;
}

/* access the first element */
inline void* list_front(struct list *l)
{
	assert(l && !list_empty(l));
	struct list_node *first = l->head.next;
	assert (first && first->data);
	return first->data;
}

/* access the last element */
inline void* list_back(struct list *l)
{
	assert(l && !list_empty(l));
	struct list_node *last = l->head.prev;
	assert(last && last->data);
	return last->data;
}

/* clears the contents */
void list_clear(struct list *l)
{
	assert(l);

	struct list_node *head = &l->head;
	struct list_node *next = head->next;
	while (next != head) {
		struct list_node *tmp = next->next;
		head->next = tmp;
		tmp->prev = head;
		__free_list_node(l, next);
		next = tmp;
	}

	head->prev = head->next = head;
	head->data = NULL;
	l->size = 0;
}

/* inserts elements to the end */
inline void list_push_back(struct list *l, void *element)
{
	assert(l && element);

	/* malloc new node */
	struct list_node *tmp = __alloc_new_list_node(l, element);
	assert(tmp);

	/* inserts the new node to list */
	struct list_node *head = &l->head;
	struct list_node *last = head->prev;
	head->prev = tmp;
	tmp->next = head;
	tmp->prev = last;
	last->next = tmp;

	l->size++;
}

/* removes the last element */
inline void list_pop_back(struct list *l, void *element)
{
	assert(l && element && !list_empty(l));

	struct list_node *head = &l->head;
	struct list_node *last = head->prev;
	struct list_node *node_before_last = last->prev;

	/* copy element */
	CONTAINER_COPY(element, last->data, l);

	/* remove last node */
	head->prev = node_before_last;
	node_before_last->next = head;
	__free_list_node(l, last);
	l->size--;
}

/* inserts elements to the beginning */
inline void list_push_front(struct list *l, void *element)
{
	assert(l && element);

	/* malloc new node */
	struct list_node *tmp = __alloc_new_list_node(l, element);
	assert(tmp);

	/* inserts the new node to list */
	struct list_node *head = &l->head;
	struct list_node *first = head->next;
	head->next = tmp;
	tmp->prev = head;
	tmp->next = first;
	first->prev = tmp;

	l->size++;
}

/* removes the first element */
inline void list_pop_front(struct list *l, void *element)
{
	assert(l && element && !list_empty(l));

	struct list_node *head = &l->head;
	struct list_node *first = head->next;
	struct list_node *node_after_first = first->next;

	/* copy element */
	CONTAINER_COPY(element, first->data, l);

	/* remove last node */
	head->next = node_after_first;
	node_after_first->prev = head;
	__free_list_node(l, first);
	l->size--;
}

/* reverses the order of the elements */
void list_reverse(struct list *l)
{
	assert(l);

	struct list_node *head = &l->head;
	struct list_node *prev = head;
	struct list_node *next = head->next;
	while (next != head) {
		struct list_node *tmp = next->next;
		prev->prev = next;
		next->next = prev;
		prev = next;
		next = tmp;
	}

	prev->prev = next;
	next->next = prev;
}

inline struct list_node* __alloc_new_list_node(struct list *l, void *element)
{
	/* malloc new node */
	struct list_node *tmp = malloc(sizeof(struct list_node));
	assert(tmp);
	tmp->data = malloc(l->elem_size);
	assert(tmp->data);

	/* copy element to new node */
	CONTAINER_COPY(tmp->data, element, l);

	return tmp;
}

inline void __free_list_node(struct list *l, struct list_node *n)
{
	if (l->free != NULL) {
		l->free(n->data);
	}

	free(n->data);
	free(n);
}

#endif
