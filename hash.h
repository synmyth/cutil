#ifndef _HASH_H_
#define _HASH_H_

typedef unsigned char uint8;
typedef unsigned int uint32;

#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))
#define mix(a,b,c) \
do { \
	a -= c;  a ^= rot(c, 4);  c += b; \
	b -= a;  b ^= rot(a, 6);  a += c; \
	c -= b;  c ^= rot(b, 8);  b += a; \
	a -= c;  a ^= rot(c,16);  c += b; \
	b -= a;  b ^= rot(a,19);  a += c; \
	c -= b;  c ^= rot(b, 4);  b += a; \
} while (0)

#define final(a,b,c) \
do { \
	c ^= b; c -= rot(b,14); \
	a ^= c; a -= rot(c,11); \
	b ^= a; b -= rot(a,25); \
	c ^= b; c -= rot(b,16); \
	a ^= c; a -= rot(c,4);  \
	b ^= a; b -= rot(a,14); \
	c ^= b; c -= rot(b,24); \
} while (0)

/* hash function for little endian 32bit machine */
static inline uint32 hashlittle32(const void *key, size_t length, uint32 initval)
{
	uint32 a,b,c;                                          /* internal state */

	/* Set up the internal state */
	a = b = c = 0xdeadbeef + ((uint32)length) + initval;

	const uint32 *k = (const uint32 *)key;         /* read 32-bit chunks */
	const uint8  *k8;

	/*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
	while (length > 12)
	{
		a += k[0];
		b += k[1];
		c += k[2];
		mix(a,b,c);
		length -= 12;
		k += 3;
	}

	/*----------------------------- handle the last (probably partial) block */
	/* 
	 * "k[2]&0xffffff" actually reads beyond the end of the string, but
	 * then masks off the part it's not allowed to read.  Because the
	 * string is aligned, the masked-off tail is in the same word as the
	 * rest of the string.  Every machine with memory protection I've seen
	 * does it on word boundaries, so is OK with this.  But VALGRIND will
	 * still catch it and complain.  The masking trick does make the hash
	 * noticably faster for short strings (like English words).
	 */
	k8 = (const uint8 *)k;
	switch(length)
	{
		case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
		case 11: c+=((uint32)k8[10])<<16;  /* fall through */
		case 10: c+=((uint32)k8[9])<<8;    /* fall through */
		case 9 : c+=k8[8];                   /* fall through */
		case 8 : b+=k[1]; a+=k[0]; break;
		case 7 : b+=((uint32)k8[6])<<16;   /* fall through */
		case 6 : b+=((uint32)k8[5])<<8;    /* fall through */
		case 5 : b+=k8[4];                   /* fall through */
		case 4 : a+=k[0]; break;
		case 3 : a+=((uint32)k8[2])<<16;   /* fall through */
		case 2 : a+=((uint32)k8[1])<<8;    /* fall through */
		case 1 : a+=k8[0]; break;
		case 0 : return c;
	}

	final(a,b,c);
	return c;
}

static inline uint32 hash(const void *key, size_t length)
{
	return hashlittle32(key, length, 0);
}

#endif
