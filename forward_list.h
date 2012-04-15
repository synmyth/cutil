#ifndef _FORWARD_LIST_H_
#define _FORWARD_LIST_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "util_define.h"
#include "iterator.h"

#define FLIST_INIT(l, elem_size)	flist_init((l), (elem_size), NULL, NULL)

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
void flist_clear(forward_list_t *l);
void flist_reverse(forward_list_t *l);
void flist_copy(void *dest, void *src);
void flist_free(void *element);

static inline struct flist_node* __alloc_flist_node(forward_list_t *l, void *element)
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

static inline void __free_flist_node(forward_list_t *l, struct flist_node *n)
{
	if (l->free != NULL) {
		l->free(n->data);
	}

	free(n->data);
	free(n);
}

/* checks whether the container is empty */
static inline int flist_empty(forward_list_t *l)
{
	assert(l);
	return !l->size;
}

/* returns the number of elements */
static inline size_t flist_size(forward_list_t *l)
{
	assert(l);
	return l->size;
}

/* access the first element */
static inline void* flist_front(forward_list_t *l)
{
	assert(l && !flist_empty(l));
	struct flist_node *first = l->head.next;
	assert (first && first->data);
	return first->data;
}

/* inserts elements to the beginning */
static inline void flist_push_front(forward_list_t *l, void *element)
{
	assert(l && element);

	/* malloc new node */
	struct flist_node *tmp = __alloc_flist_node(l, element);
	assert(tmp);

	/* inserts the new node to list */
	struct flist_node *head = &l->head;
	struct flist_node *first = head->next;
	head->next = tmp;
	tmp->next = first;

	l->size++;
}

/* removes the first element */
static inline void flist_pop_front(forward_list_t *l)
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

#endif
