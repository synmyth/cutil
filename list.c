#include "list.h"

/* function prototypes */
static void __list_iter_head(iterator_t *it, list_t *l);
static void __list_iter_next(iterator_t *it, list_t *l);
static void __list_iter_tail(iterator_t *it, list_t *l);
static void __list_iter_prev(iterator_t *it, list_t *l);

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
	l->iter_head = __list_iter_head;
	l->iter_next = __list_iter_next;
	l->iter_tail = __list_iter_tail;
	l->iter_prev = __list_iter_prev;
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

/* iterator head function for list */
static void __list_iter_head(iterator_t *it, list_t *l)
{
	assert(it && l);

	it->ptr = (!list_empty(l)) ? l->head.next : NULL;
	it->data = (it->ptr) ? ((struct list_node *)it->ptr)->data : NULL;
	it->i = 0;
	it->size = list_size(l);
}

/* iterator next function for list */
static void __list_iter_next(iterator_t *it, list_t *l)
{
	assert(it && l);

	it->ptr = (++(it->i) < it->size) ?
		((struct list_node *)it->ptr)->next : NULL;
	it->data = (it->ptr) ? ((struct list_node *)it->ptr)->data : NULL;
}

/* iterator tail function for list */
static void __list_iter_tail(iterator_t *it, list_t *l)
{
	assert(it && l);

	it->ptr = (!list_empty(l)) ? l->head.prev : NULL;
	it->data = (it->ptr) ? ((struct list_node *)it->ptr)->data : NULL;
	it->i = 0;
	it->size = list_size(l);
}

/* iterator prev function for list */
static void __list_iter_prev(iterator_t *it, list_t *l)
{
	assert(it && l);

	it->ptr = (++(it->i) < it->size) ?
		((struct list_node *)it->ptr)->prev : NULL;
	it->data = (it->ptr) ? ((struct list_node *)it->ptr)->data : NULL;
}
