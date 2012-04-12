#ifndef _LIST_H_
#define _LIST_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "util_define.h"

typedef struct list list_t;

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

void list_init(list_t *l, size_t elem_size,
		void (*copy_func)(void *, void *), void (*free_func)(void *));
void list_destroy(list_t *l);
void list_clear(list_t *l);
void list_reverse(list_t *l);

static inline struct list_node* __alloc_list_node(list_t *l, void *element)
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

static inline void __free_list_node(list_t *l, struct list_node *n)
{
	if (l->free != NULL) {
		l->free(n->data);
	}

	free(n->data);
	free(n);
}

/* checks whether the container is empty */
static inline int list_empty(list_t *l)
{
	assert(l);
	return !l->size;
}

/* returns the number of elements */
static inline size_t list_size(list_t *l)
{
	assert(l);
	return l->size;
}

/* access the first element */
static inline void* list_front(list_t *l)
{
	assert(l && !list_empty(l));
	struct list_node *first = l->head.next;
	assert (first && first->data);
	return first->data;
}

/* access the last element */
static inline void* list_back(list_t *l)
{
	assert(l && !list_empty(l));
	struct list_node *last = l->head.prev;
	assert(last && last->data);
	return last->data;
}

/* inserts elements to the end */
static inline void list_push_back(list_t *l, void *element)
{
	assert(l && element);

	/* malloc new node */
	struct list_node *tmp = __alloc_list_node(l, element);
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
static inline void list_pop_back(list_t *l)
{
	assert(l && !list_empty(l));

	struct list_node *head = &l->head;
	struct list_node *last = head->prev;
	struct list_node *node_before_last = last->prev;

	/* remove last node */
	head->prev = node_before_last;
	node_before_last->next = head;
	__free_list_node(l, last);
	l->size--;
}

/* inserts elements to the beginning */
static inline void list_push_front(list_t *l, void *element)
{
	assert(l && element);

	/* malloc new node */
	struct list_node *tmp = __alloc_list_node(l, element);
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
static inline void list_pop_front(list_t *l)
{
	assert(l && !list_empty(l));

	struct list_node *head = &l->head;
	struct list_node *first = head->next;
	struct list_node *node_after_first = first->next;

	/* remove last node */
	head->next = node_after_first;
	node_after_first->prev = head;
	__free_list_node(l, first);
	l->size--;
}

#endif
