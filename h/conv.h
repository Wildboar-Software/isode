/* conv.h - range-checked integer conversions */

#ifndef	_CONV_
#define	_CONV_

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

/*
 * Each helper returns 0 on success and writes *out, or -1 if the value
 * cannot be represented in the destination type (or out is NULL).
 *
 * Callers must treat a failed conversion as an error: log, return NOTOK,
 * or otherwise abort the operation.  Do not proceed with a truncated or
 * sign-flipped result.
 *
 * Comparisons use the destination type's limits so the same code is
 * correct on 32-bit and 64-bit POSIX systems.
 */

static inline int
sizet2int (size_t n, int *out)
{
	if (out == NULL || n > (size_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

static inline int
int2sizet (int n, size_t *out)
{
	if (out == NULL || n < 0 || (uintmax_t) n > (uintmax_t) SIZE_MAX)
		return -1;
	*out = (size_t) n;
	return 0;
}

static inline int
ssize2int (ssize_t n, int *out)
{
	if (out == NULL || n < (ssize_t) INT_MIN || n > (ssize_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

static inline int
ssize2sizet (ssize_t n, size_t *out)
{
	if (out == NULL || n < 0 || (uintmax_t) n > (uintmax_t) SIZE_MAX)
		return -1;
	*out = (size_t) n;
	return 0;
}

static inline int
long2int (long n, int *out)
{
	if (out == NULL || n < (long) INT_MIN || n > (long) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

static inline int
long2uint (long n, unsigned int *out)
{
	if (out == NULL || n < 0L || (unsigned long) n > (unsigned long) UINT_MAX)
		return -1;
	*out = (unsigned int) n;
	return 0;
}

static inline int
long2sizet (long n, size_t *out)
{
	if (out == NULL || n < 0L || (uintmax_t) n > (uintmax_t) SIZE_MAX)
		return -1;
	*out = (size_t) n;
	return 0;
}

static inline int
ulong2int (unsigned long n, int *out)
{
	if (out == NULL || n > (unsigned long) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

static inline int
ulong2uint (unsigned long n, unsigned int *out)
{
	if (out == NULL || n > (unsigned long) UINT_MAX)
		return -1;
	*out = (unsigned int) n;
	return 0;
}

static inline int
uint2int (unsigned int n, int *out)
{
	if (out == NULL || n > (unsigned int) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

static inline int
int2uint (int n, unsigned int *out)
{
	if (out == NULL || n < 0)
		return -1;
	*out = (unsigned int) n;
	return 0;
}

static inline int
ptrdiff2int (ptrdiff_t n, int *out)
{
	if (out == NULL || n < (ptrdiff_t) INT_MIN || n > (ptrdiff_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

static inline int
ptrdiff2sizet (ptrdiff_t n, size_t *out)
{
	if (out == NULL || n < 0 || (uintmax_t) n > (uintmax_t) SIZE_MAX)
		return -1;
	*out = (size_t) n;
	return 0;
}

/* signed char lengths (NSAP fields, etc.) */
static inline int
char2sizet (char n, size_t *out)
{
	return int2sizet ((int) n, out);
}

static inline int
int2u8 (int n, uint8_t *out)
{
	if (out == NULL || n < 0 || n > 255)
		return -1;
	*out = (uint8_t) n;
	return 0;
}

static inline int
add_sizet_to_int (int *acc, size_t n)
{
	int i;

	if (acc == NULL || sizet2int (n, &i) != 0)
		return -1;
	if (*acc < 0 || *acc > INT_MAX - i)
		return -1;
	*acc += i;
	return 0;
}

static inline int
add_int_to_int (int *acc, int n)
{
	if (acc == NULL || n < 0 || *acc < 0 || *acc > INT_MAX - n)
		return -1;
	*acc += n;
	return 0;
}

static inline int
int2u16 (int n, uint16_t *out)
{
	if (out == NULL || n < 0 || n > 65535)
		return -1;
	*out = (uint16_t) n;
	return 0;
}

static inline int
int2u32 (int n, uint32_t *out)
{
	if (out == NULL || n < 0 || (uintmax_t) n > (uintmax_t) UINT32_MAX)
		return -1;
	*out = (uint32_t) n;
	return 0;
}

static inline int
u32toint (uint32_t n, int *out)
{
	if (out == NULL || n > (uint32_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/* Bit-clear using unsigned arithmetic so ~mask is not a negative int. */
static inline uint8_t
u8_bic (uint8_t v, unsigned bits)
{
	return (uint8_t) (v & ~bits);
}

static inline uint16_t
u16_bic (uint16_t v, unsigned bits)
{
	return (uint16_t) (v & ~bits);
}

/*
 * Copy up to cap bytes from src to dst.  len is a signed count (int or
 * pointer difference).  On success *out_cc is the number of bytes copied.
 * A negative or unrepresentable length yields *out_cc = 0 and -1.
 */
static inline int
copy_capped (const void *src, void *dst, ptrdiff_t len, size_t cap, int *out_cc)
{
	size_t n;

	if (out_cc == NULL)
		return -1;
	if (ptrdiff2sizet (len, &n) != 0) {
		*out_cc = 0;
		return -1;
	}
	if (n > cap)
		n = cap;
	if (sizet2int (n, out_cc) != 0) {
		*out_cc = 0;
		return -1;
	}
	if (*out_cc > 0) {
		if (src == NULL || dst == NULL) {
			*out_cc = 0;
			return -1;
		}
		memmove (dst, src, n);
	}
	return 0;
}

/*
 * Length-checked wrappers for the BSD memory routines.  Argument order
 * matches bcopy/bcmp/bzero.  A negative length is rejected rather than
 * converted into a huge size_t copy.
 */
static inline int
bcopy_int (const void *src, void *dst, int n)
{
	size_t len;

	if (int2sizet (n, &len) != 0)
		return -1;
	memmove (dst, src, len);
	return 0;
}

static inline int
bcmp_int (const void *a, const void *b, int n)
{
	size_t len;

	if (int2sizet (n, &len) != 0)
		return -1;
	return memcmp (a, b, len);
}

static inline int
bzero_int (void *s, int n)
{
	size_t len;

	if (int2sizet (n, &len) != 0)
		return -1;
	memset (s, 0, len);
	return 0;
}

static inline int
strncmp_int (const char *a, const char *b, int n)
{
	size_t len;

	if (int2sizet (n, &len) != 0)
		return -1;
	return strncmp (a, b, len);
}

#include <errno.h>
#include <unistd.h>

static inline ssize_t
read_int (int fd, void *buf, int n)
{
	size_t len;

	if (int2sizet (n, &len) != 0) {
		errno = EINVAL;
		return -1;
	}
	return read (fd, buf, len);
}

static inline ssize_t
write_int (int fd, const void *buf, int n)
{
	size_t len;

	if (int2sizet (n, &len) != 0) {
		errno = EINVAL;
		return -1;
	}
	return write (fd, buf, len);
}

#endif
