#include "list.h"

/* initialize the list */
void list_init(list_t *l, size_t elem_size,
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
void list_destroy(list_t *l)
{
	assert(l);
	list_clear(l);
}

/* clears the contents */
void list_clear(list_t *l)
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

/* reverses the order of the elements */
void list_reverse(list_t *l)
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
