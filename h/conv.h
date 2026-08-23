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
strlen2int (const char *s, int *out)
{
	if (s == NULL)
		return -1;
	return sizet2int (strlen (s), out);
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
int2ulong (int n, unsigned long *out)
{
	if (out == NULL || n < 0)
		return -1;
	*out = (unsigned long) n;
	return 0;
}

/*
 * uid_t / gid_t / mode_t may be signed or unsigned, and their width is
 * not assumed.  (uid_t)-1 > 0 is a compile-time unsigned test.
 */
static inline int
uid2int (uid_t uid, int *out)
{
	if (out == NULL)
		return -1;
	if ((uid_t) -1 > (uid_t) 0) {
		if ((uintmax_t) uid > (uintmax_t) INT_MAX)
			return -1;
	} else if ((intmax_t) uid < (intmax_t) INT_MIN
			   || (intmax_t) uid > (intmax_t) INT_MAX)
		return -1;
	*out = (int) uid;
	return 0;
}

static inline int
int2uid (int n, uid_t *out)
{
	if (out == NULL)
		return -1;
	if ((uid_t) -1 > (uid_t) 0) {
		if (n < 0 || (uintmax_t) n > (uintmax_t) (uid_t) -1)
			return -1;
	} else {
		uid_t uid = (uid_t) n;

		if ((int) uid != n)
			return -1;
		*out = uid;
		return 0;
	}
	*out = (uid_t) n;
	return 0;
}

static inline int
gid2int (gid_t gid, int *out)
{
	if (out == NULL)
		return -1;
	if ((gid_t) -1 > (gid_t) 0) {
		if ((uintmax_t) gid > (uintmax_t) INT_MAX)
			return -1;
	} else if ((intmax_t) gid < (intmax_t) INT_MIN
			   || (intmax_t) gid > (intmax_t) INT_MAX)
		return -1;
	*out = (int) gid;
	return 0;
}

static inline int
int2gid (int n, gid_t *out)
{
	if (out == NULL)
		return -1;
	if ((gid_t) -1 > (gid_t) 0) {
		if (n < 0 || (uintmax_t) n > (uintmax_t) (gid_t) -1)
			return -1;
	} else {
		gid_t gid = (gid_t) n;

		if ((int) gid != n)
			return -1;
		*out = gid;
		return 0;
	}
	*out = (gid_t) n;
	return 0;
}

static inline int
int2mode (int n, mode_t *out)
{
	if (out == NULL)
		return -1;
	if ((mode_t) -1 > (mode_t) 0) {
		if (n < 0 || (uintmax_t) n > (uintmax_t) (mode_t) -1)
			return -1;
	} else {
		mode_t m = (mode_t) n;

		if ((int) m != n)
			return -1;
		*out = m;
		return 0;
	}
	*out = (mode_t) n;
	return 0;
}

static inline int
mode2int (mode_t m, int *out)
{
	if (out == NULL)
		return -1;
	if ((mode_t) -1 > (mode_t) 0) {
		if ((uintmax_t) m > (uintmax_t) INT_MAX)
			return -1;
	} else if ((intmax_t) m < (intmax_t) INT_MIN
			   || (intmax_t) m > (intmax_t) INT_MAX)
		return -1;
	*out = (int) m;
	return 0;
}

#include <sys/stat.h>
#include <errno.h>

static inline int
chmod_int (const char *path, int mode)
{
	mode_t m;

	if (int2mode (mode, &m) != 0) {
		errno = EINVAL;
		return -1;
	}
	return chmod (path, m);
}

static inline int
long2ulong (long n, unsigned long *out)
{
	if (out == NULL || n < 0L)
		return -1;
	*out = (unsigned long) n;
	return 0;
}

static inline int
ulong2long (unsigned long n, long *out)
{
	if (out == NULL || n > (unsigned long) LONG_MAX)
		return -1;
	*out = (long) n;
	return 0;
}

static inline int
off2sizet (off_t n, size_t *out)
{
	if (out == NULL || n < 0 || (uintmax_t) n > (uintmax_t) SIZE_MAX)
		return -1;
	*out = (size_t) n;
	return 0;
}

#include <sys/socket.h>

static inline int
int2socklen (int n, socklen_t *out)
{
	if (out == NULL)
		return -1;
	if ((socklen_t) -1 > (socklen_t) 0) {
		if (n < 0 || (uintmax_t) n > (uintmax_t) (socklen_t) -1)
			return -1;
	} else {
		socklen_t sl = (socklen_t) n;

		if ((int) sl != n)
			return -1;
		*out = sl;
		return 0;
	}
	*out = (socklen_t) n;
	return 0;
}

static inline int
sizet2socklen (size_t n, socklen_t *out)
{
	if (out == NULL)
		return -1;
	if ((socklen_t) -1 > (socklen_t) 0) {
		if ((uintmax_t) n > (uintmax_t) (socklen_t) -1)
			return -1;
	} else {
		socklen_t sl = (socklen_t) n;

		if (sl < 0 || (size_t) sl != n)
			return -1;
		*out = sl;
		return 0;
	}
	*out = (socklen_t) n;
	return 0;
}

#include <termios.h>

static inline int
int2tflag (int n, tcflag_t *out)
{
	if (out == NULL)
		return -1;
	if ((tcflag_t) -1 > (tcflag_t) 0) {
		if (n < 0 || (uintmax_t) n > (uintmax_t) (tcflag_t) -1)
			return -1;
	} else {
		tcflag_t f = (tcflag_t) n;

		if ((int) f != n)
			return -1;
		*out = f;
		return 0;
	}
	*out = (tcflag_t) n;
	return 0;
}

static inline int
tflag_bic (tcflag_t v, int bits, tcflag_t *out)
{
	tcflag_t m;

	if (int2tflag (bits, &m) != 0)
		return -1;
	*out = v & ~m;
	return 0;
}

static inline int
tflag_bis (tcflag_t v, int bits, tcflag_t *out)
{
	tcflag_t m;

	if (int2tflag (bits, &m) != 0)
		return -1;
	*out = v | m;
	return 0;
}

static inline int
cct2char (cc_t c, char *out)
{
	if (out == NULL || (uintmax_t) c > (uintmax_t) CHAR_MAX)
		return -1;
	*out = (char) c;
	return 0;
}

/* POSIX chown/fchown: all-bits-one means "leave this id unchanged". */
static inline uid_t
uid_nochg (void)
{
	return ~(uid_t) 0;
}

static inline gid_t
gid_nochg (void)
{
	return ~(gid_t) 0;
}

/* Replace file-type bits (S_IFMT) with a new S_IF* type. */
static inline int
mode_retype (mode_t mode, int old_fmt, int new_fmt, mode_t *out)
{
	mode_t oldm,
		   newm;

	if (out == NULL || int2mode (old_fmt, &oldm) != 0
			|| int2mode (new_fmt, &newm) != 0)
		return -1;
	*out = (mode & ~oldm) | newm;
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
u8toint (uint8_t n, int *out)
{
	if (out == NULL)
		return -1;
	*out = (int) n;
	return 0;
}

/* Store an octet in a C char.  Values that do not fit in char fail. */
static inline int
u8tochar (uint8_t n, char *out)
{
	if (out == NULL || n > (uint8_t) CHAR_MAX)
		return -1;
	*out = (char) n;
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

/* Interpret a C char as a protocol octet (all 256 values). */
static inline uint8_t
as_octet (char c)
{
	return (uint8_t) (unsigned char) c;
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
#include <stdlib.h>
#include <unistd.h>

static inline void *
malloc_int (int n)
{
	size_t len;

	if (int2sizet (n, &len) != 0)
		return NULL;
	return malloc (len);
}

static inline void *
realloc_int (void *p, int n)
{
	size_t len;

	if (int2sizet (n, &len) != 0)
		return NULL;
	return realloc (p, len);
}

static inline void *
malloc_nmemb (int n, size_t size)
{
	size_t count;

	if (int2sizet (n, &count) != 0)
		return NULL;
	if (size != 0 && count > SIZE_MAX / size)
		return NULL;
	return malloc (count * size);
}

static inline int
nmemb_bytes (int n, size_t size, size_t *out)
{
	size_t count;

	if (out == NULL || int2sizet (n, &count) != 0)
		return -1;
	if (size != 0 && count > SIZE_MAX / size)
		return -1;
	*out = count * size;
	return 0;
}

static inline void *
calloc_int (int n, size_t size)
{
	size_t count;

	if (int2sizet (n, &count) != 0)
		return NULL;
	if (size != 0 && count > SIZE_MAX / size)
		return NULL;
	return calloc (count, size);
}

static inline void *
realloc_nmemb (void *p, int n, size_t size)
{
	size_t count;

	if (int2sizet (n, &count) != 0)
		return NULL;
	if (size != 0 && count > SIZE_MAX / size)
		return NULL;
	return realloc (p, count * size);
}

static inline void *
malloc_plus_int (size_t base, int extra)
{
	size_t n;

	if (int2sizet (extra, &n) != 0)
		return NULL;
	if (n > SIZE_MAX - base)
		return NULL;
	return malloc (base + n);
}

static inline int
qsort_int (void *base, int n, size_t size,
	   int (*cmp) (const void *, const void *))
{
	size_t count;

	if (int2sizet (n, &count) != 0)
		return -1;
	qsort (base, count, size, cmp);
	return 0;
}

static inline int
qsort_ptrdiff (void *base, ptrdiff_t n, size_t size,
	       int (*cmp) (const void *, const void *))
{
	size_t count;

	if (ptrdiff2sizet (n, &count) != 0)
		return -1;
	qsort (base, count, size, cmp);
	return 0;
}

/*
 * Take a signed count of remaining bytes and a capacity, write the
 * smaller value to *out as an int.
 */
static inline int
min_len_cap (ptrdiff_t len, size_t cap, int *out)
{
	size_t n;

	if (out == NULL)
		return -1;
	if (ptrdiff2sizet (len, &n) != 0)
		return -1;
	if (n > cap)
		n = cap;
	return sizet2int (n, out);
}

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

#include <stdio.h>

static inline int
fread_int (void *ptr, size_t size, int nmemb, FILE *stream)
{
	size_t n,
		got;
	int out;

	if (int2sizet (nmemb, &n) != 0) {
		errno = EINVAL;
		return -1;
	}
	got = fread (ptr, size, n, stream);
	if (sizet2int (got, &out) != 0) {
		errno = EOVERFLOW;
		return -1;
	}
	return out;
}

static inline int
fwrite_int (const void *ptr, size_t size, int nmemb, FILE *stream)
{
	size_t n,
		put;
	int out;

	if (int2sizet (nmemb, &n) != 0) {
		errno = EINVAL;
		return -1;
	}
	put = fwrite (ptr, size, n, stream);
	if (sizet2int (put, &out) != 0) {
		errno = EOVERFLOW;
		return -1;
	}
	return out;
}

#endif
