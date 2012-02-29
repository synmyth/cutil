#ifndef _UTIL_DEFINE_H_
#define _UTIL_DEFINE_H_
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CONTAINER_CAPACITY	32
#define CONTAINER_COPY(dest, src, container)				\
do {									\
	if ((container)->copy != NULL) {				\
		(container)->copy(dest, src);				\
	} else {							\
		__default_copy_func(dest, src, (container)->elem_size);	\
	}								\
} while (0)

inline void __default_copy_func(void *dest, void *src, size_t elem_size)
{
	memcpy(dest, src, elem_size);
}

#endif
