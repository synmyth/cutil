#ifndef _ITERATOR_H_
#define _ITERATOR_H_

typedef struct {
	void *ptr;
	void *data;
	void *key;
	size_t bkt_index;
	size_t i;
	size_t size;
} iterator_t;

#define UTIL_FOREACH(it, c)				\
	if ((c))					\
		for ((c)->iter_head(&(it), (c));	\
				(it).ptr;		\
				(c)->iter_next(&(it), (c)))

#define UTIL_FOREACH_REVERSE(it, c)			\
	if ((c))					\
		for ((c)->iter_tail(&(it), (c));	\
				(it).ptr;		\
				(c)->iter_prev(&(it), (c)))

#define util_foreach		UTIL_FOREACH
#define util_foreach_reverse	UTIL_FOREACH_REVERSE

#endif
