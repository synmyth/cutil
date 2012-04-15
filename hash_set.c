#include "hash_set.h"

/* function prototypes */
static int __hset_insert_node(hash_set_t *h, struct chain_node *n);
static void __hset_expand(hash_set_t *h);
static void __free_bucket(hash_set_t *h, struct bucket *b);
static void __hset_iter_head(iterator_t *it, hash_set_t *h);
static void __hset_iter_next(iterator_t *it, hash_set_t *h);

/* initialize the hash set */
void hset_init(hash_set_t *h, size_t elem_size,
		void (*copy_func)(void *, void *),
		void (*free_func)(void *),
		int (*cmp_func)(void *, void *))
{
	assert(h && elem_size > 0);

	memset(h, 0, sizeof(hash_set_t));
	h->bucket_size = DEFAULT_CONTAINER_CAPACITY;
	h->buckets = malloc(sizeof(struct bucket) * h->bucket_size);
	assert(h->buckets);
	memset(h->buckets, 0, sizeof(struct bucket) * h->bucket_size);

	h->elem_size = elem_size;
	h->key_size = elem_size;
	h->max_bucket_capacity = MAX_BUCKET_CAPACITY;
	h->copy = copy_func;
	h->free = free_func;
	h->compare = cmp_func;
	h->iter_head = __hset_iter_head;
	h->iter_next = __hset_iter_next;
}

/* remove all elements */
void hset_clear(hash_set_t *h)
{
	assert(h);

	size_t i;
	for (i = 0; i < h->bucket_size; i++) {
		__free_bucket(h, &h->buckets[i]);
	}
	h->size = 0;
}

/* inserts elements */
void hset_insert(hash_set_t *h, void *key)
{
	assert(h && key && h->buckets);

	struct chain_node *tmp = malloc(sizeof(struct chain_node) + h->elem_size);
	assert(tmp);
	tmp->next = NULL;
	CONTAINER_COPY(tmp->data, key, h);
	int ret = __hset_insert_node(h, tmp);
	if (ret < 0) {
		__free_chain_node(h, tmp);
	}
}

/* erases elements */
void hset_erase(hash_set_t *h, void *key)
{
	assert(h && key && h->buckets);

	size_t bkt_index = hash(key, h->key_size) % h->bucket_size;
	struct bucket *b = &h->buckets[bkt_index];
	struct chain_node *next = b->first;
	if (!next) {
		return;
	}

	struct chain_node *prev = NULL;
	int find = 0;
	if (EQUALS(key, next->data, h)) {
		find = 1;
	} else {
		prev = next;
		next = next->next;
		while (next) {
			if (EQUALS(key, next->data, h)) {
				find = 1;
				break;
			}

			prev = next;
			next = next->next;
		}
	}

	if (find) {
		if (prev) {
			prev->next = next->next;
		} else {
			b->first = next->next;
		}

		__free_chain_node(h, next);
		b->size--;
		h->size--;
	}
}

/* finds element with sepcific key */
void hset_find(hash_set_t *h, void *key, iterator_t *it)
{
	assert(h && key && it);

	if (h->buckets == NULL) {
		it->ptr = NULL;
		return;
	}

	size_t bkt_index = hash(key, h->key_size) % h->bucket_size;
	struct bucket *b = &h->buckets[bkt_index];
	struct chain_node *next = b->first;
	while (next != NULL) {
		if (EQUALS(key, next->data, h)) {
			it->ptr = next;
			it->data = next->data;
			it->key = it->data;
			return;
		}

		next = next->next;
	}

	it->ptr = NULL;
}

/* inserts node */
static int __hset_insert_node(hash_set_t *h, struct chain_node *n)
{
	assert(h && n && h->buckets && n->data);

	void *key = n->data;
	size_t bkt_index = hash(key, h->key_size) % h->bucket_size;
	struct bucket *b = &h->buckets[bkt_index];

	/* find duplicate */
	struct chain_node *next = b->first;
	while (next != NULL) {
		if (EQUALS(next->data, n->data, h)) {
			return -1;
		}

		next = next->next;
	}

	/* find which bucket to insert */
	int i = 0;
	while (1) {
		if (i > 0) {
			bkt_index = hash(key, h->key_size) % h->bucket_size;
			b = &h->buckets[bkt_index];
		}

		i++;
		if (b->size < h->max_bucket_capacity) {
			break;
		} else {
			if (h->has_long_chain && h->long_chain_index != bkt_index) {
				/* expand */
				__hset_expand(h);
			} else {
				h->has_long_chain = 1;
				h->long_chain_index = bkt_index;
				break;
			}
		}
	}

	n->next = b->first;
	b->first = n;
	b->size++;
	h->size++;

	return 0;
}

static void __hset_expand(hash_set_t *h)
{
	assert(h && h->buckets);

	size_t old_bkt_size = h->bucket_size;
	h->bucket_size *= 2;
	struct bucket *old_buckets = h->buckets;
	struct bucket *new_buckets = malloc(sizeof(struct bucket) * h->bucket_size);
	assert(new_buckets);
	memset(new_buckets, 0, sizeof(struct bucket) * h->bucket_size);

	h->buckets = new_buckets;
	h->size = 0;
	h->has_long_chain = 0;

	size_t i;
	for (i = 0; i < old_bkt_size; i++) {
		struct bucket *b = &old_buckets[i];
		struct chain_node *next = b->first;
		while (next != NULL) {
			struct chain_node *tmp = next->next;
			__hset_insert_node(h, next);
			next = tmp;
		}
	}

	free(old_buckets);
}

static void __hset_iter_head(iterator_t *it, hash_set_t *h)
{
	assert(it && h);

	memset(it, 0, sizeof(iterator_t));
	if (!hset_empty(h)) {
		struct bucket *b = NULL;
		size_t i;
		for (i = 0; i < h->bucket_size; i++) {
			struct bucket *tmp = &h->buckets[i];
			if (tmp->size > 0) {
				b = tmp;
				break;
			}
		}

		assert(b && b->first);
		it->ptr = b->first;
		it->data = b->first->data;
		it->key = it->data;
		it->bkt_index = i;
		it->i = 0;
		it->size = hset_size(h);
	} else {
		it->ptr = NULL;
	}
}

static void __hset_iter_next(iterator_t *it, hash_set_t *h)
{
	assert(it && h && it->ptr);

	struct chain_node *next = ((struct chain_node *)it->ptr)->next;
	size_t bkt_index = it->bkt_index;
	if (next == NULL) {
		size_t i;
		for (i = bkt_index + 1; i < h->bucket_size; i++) {
			struct bucket *b = &h->buckets[i];
			if (b->size > 0) {
				next = b->first;
				bkt_index = i;
				break;
			}
		}
	}

	if (next != NULL) {
		it->ptr = next;
		it->data = next->data;
		it->key = it->data;
		it->bkt_index = bkt_index;
		it->i++;
	} else {
		it->ptr = NULL;
	}
}

static void __free_bucket(hash_set_t *h, struct bucket *b)
{
	assert(b);

	struct chain_node *next = b->first;
	while (next != NULL) {
		struct chain_node *tmp = next->next;
		__free_chain_node(h, next);
		next = tmp;
	}

	b->first = NULL;
}
