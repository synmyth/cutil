#include "forward_list.h"

/* function portotypes */
static void __flist_iter_head(iterator_t *it, forward_list_t *l);
static void __flist_iter_next(iterator_t *it, forward_list_t *l);

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

static void __flist_iter_head(iterator_t *it, forward_list_t *l)
{
	assert(it && l);

	struct flist_node *first = l->head.next;
	it->ptr = first;
	it->data = (first) ? first->data : NULL;
	it->i = 0;
	it->size = flist_size(l);
}

static void __flist_iter_next(iterator_t *it, forward_list_t *l)
{
	assert(it && l);

	struct flist_node *next = ((struct flist_node *)it->ptr)->next;
	it->ptr = next;
	it->data = (next) ? next->data : NULL;
	it->i++;
}
