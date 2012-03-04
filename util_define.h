#ifndef _UTIL_DEFINE_H_
#define _UTIL_DEFINE_H_
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CONTAINER_CAPACITY	32
#define CONTAINER_COPY(dest, src, c)	((c)->copy) ?	\
	(c)->copy(dest, src) : memcpy(dest, src, (c)->elem_size)

#endif
