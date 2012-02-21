#ifndef _UTIL_DEFINE_H_
#define _UTIL_DEFINE_H_

#define DEFAULT_CONTAINER_CAPACITY	32

inline void __default_copy_func(void *dest, void *src, size_t elem_size)
{
	memcpy(dest, src, elem_size);
}

#endif
