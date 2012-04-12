#include "priority_queue.h"

/* initialize the priority queue */
void pqueue_init(priority_queue_t *q, size_t elem_size,
		void (*copy_func)(void *, void *),
		void (*free_func)(void *),
		int (*cmp_func)(void *, void *))
{
	assert(q && elem_size > 0 && cmp_func);

	memset(q, 0, sizeof(priority_queue_t));
	vector_init(&q->v, elem_size, copy_func, free_func);
	q->compare = cmp_func;
	q->tmp_elem = malloc(elem_size);
	assert(q->tmp_elem);
}

/* destroy the priority queue */
void pqueue_destroy(priority_queue_t *q)
{
	assert(q);
	vector_destroy(&q->v);
	free(q->tmp_elem);
	q->tmp_elem = NULL;
}

/* inserts element and sorts the underlying container */
void pqueue_push(priority_queue_t *q, void *element)
{
	assert(q && element);

	size_t x = vector_size(&q->v);
	vector_push_back(&q->v, element);
	if (x == 0) {
		return;
	}

	while (x > 0 && q->compare(vector_at(&q->v, parent(x)),
				vector_at(&q->v, x)) < 0) {
		__exchange(q, parent(x), x);
		x = parent(x);
	}
}

/* removes the first element */
void pqueue_pop(priority_queue_t *q)
{
	assert(q && !pqueue_empty(q));

	if (vector_size(&q->v) > 1) {
		__exchange(q, 0, vector_size(&q->v) - 1);
	}
	vector_pop_back(&q->v);

	size_t cur = 0;
	while (cur < vector_size(&q->v)) {
		size_t left = lchild(cur), right = rchild(cur), largest;
		if (left < vector_size(&q->v) && q->compare(vector_at(&q->v, left),
					vector_at(&q->v, cur)) > 0) {
			largest = left;
		} else {
			largest = cur;
		}

		if (right < vector_size(&q->v) && q->compare(vector_at(&q->v, right),
					vector_at(&q->v, largest)) > 0) {
			largest = right;
		}

		if (largest != cur) {
			__exchange(q, cur, largest);
			cur = largest;
		} else {
			break;
		}
	}
}
