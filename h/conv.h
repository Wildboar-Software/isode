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
