/**
 * @file conv.h
 * @brief Range-checked conversions, bit ops, and size-checked libc wrappers.
 *
 * Conversion helpers named @c X2Y copy @p n into @p *out if it fits in
 * the destination type.  They return 0 on success, or -1 if @p out is
 * NULL or the value cannot be represented (truncation or sign flip).
 *
 * Callers must treat failure as an error: log, return NOTOK, or abort.
 * Do not proceed with a truncated result.
 *
 * Comparisons use the destination type's limits so the same code is
 * correct on 32-bit and 64-bit POSIX.
 *
 * Bit helpers use DEC/VAX names: @c bis is bit set (OR), @c bic is bit
 * clear (AND with complement), @c bxor is exclusive-or.
 */

#ifndef	_CONV_
#define	_CONV_

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

/**
 * @brief Convert @c size_t to @c int.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > INT_MAX.
 */
static inline int
sizet2int (size_t n, int *out)
{
	if (out == NULL || n > (size_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Convert @c strlen(s) to @c int.
 * @param s NUL-terminated string; NULL fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p s is NULL or the length does not fit.
 */
static inline int
strlen2int (const char *s, int *out)
{
	if (s == NULL)
		return -1;
	return sizet2int (strlen (s), out);
}

/**
 * @brief Convert a non-negative @c int to @c size_t.
 * @param n Value to convert; negative fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 on NULL @p out, negative @p n, or overflow.
 */
static inline int
int2sizet (int n, size_t *out)
{
	if (out == NULL || n < 0 || (uintmax_t) n > (uintmax_t) SIZE_MAX)
		return -1;
	*out = (size_t) n;
	return 0;
}

/**
 * @brief Convert @c ssize_t to @c int.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside INT_MIN..INT_MAX.
 */
static inline int
ssize2int (ssize_t n, int *out)
{
	if (out == NULL || n < (ssize_t) INT_MIN || n > (ssize_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Convert a non-negative @c ssize_t to @c size_t.
 * @param n Value to convert; negative fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 on NULL @p out, negative @p n, or overflow.
 */
static inline int
ssize2sizet (ssize_t n, size_t *out)
{
	if (out == NULL || n < 0 || (uintmax_t) n > (uintmax_t) SIZE_MAX)
		return -1;
	*out = (size_t) n;
	return 0;
}

/**
 * @brief Convert @c long to @c int.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside INT_MIN..INT_MAX.
 */
static inline int
long2int (long n, int *out)
{
	if (out == NULL || n < (long) INT_MIN || n > (long) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Convert a non-negative @c long to @c unsigned int.
 * @param n Value to convert; negative fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 on NULL @p out, negative @p n, or overflow of UINT_MAX.
 */
static inline int
long2uint (long n, unsigned int *out)
{
	if (out == NULL || n < 0L || (unsigned long) n > (unsigned long) UINT_MAX)
		return -1;
	*out = (unsigned int) n;
	return 0;
}

/**
 * @brief Convert a non-negative @c long to @c size_t.
 * @param n Value to convert; negative fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 on NULL @p out, negative @p n, or overflow.
 */
static inline int
long2sizet (long n, size_t *out)
{
	if (out == NULL || n < 0L || (uintmax_t) n > (uintmax_t) SIZE_MAX)
		return -1;
	*out = (size_t) n;
	return 0;
}

/**
 * @brief Convert @c unsigned long to @c int.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > INT_MAX.
 */
static inline int
ulong2int (unsigned long n, int *out)
{
	if (out == NULL || n > (unsigned long) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Convert @c unsigned long to @c unsigned int.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > UINT_MAX.
 */
static inline int
ulong2uint (unsigned long n, unsigned int *out)
{
	if (out == NULL || n > (unsigned long) UINT_MAX)
		return -1;
	*out = (unsigned int) n;
	return 0;
}

/**
 * @brief Convert @c unsigned int to @c int.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > INT_MAX.
 */
static inline int
uint2int (unsigned int n, int *out)
{
	if (out == NULL || n > (unsigned int) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Convert a non-negative @c int to @c unsigned int.
 * @param n Value to convert; negative fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n < 0.
 */
static inline int
int2uint (int n, unsigned int *out)
{
	if (out == NULL || n < 0)
		return -1;
	*out = (unsigned int) n;
	return 0;
}

/**
 * @brief Convert a non-negative @c int to @c unsigned long.
 * @param n Value to convert; negative fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n < 0.
 */
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

/**
 * @brief Convert @c uid_t to @c int, handling signed or unsigned @c uid_t.
 * @param uid User id.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or the id does not fit in @c int.
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

/**
 * @brief Convert @c int to @c uid_t, handling signed or unsigned @c uid_t.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n does not fit in @c uid_t.
 */
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

/**
 * @brief Convert @c gid_t to @c int, handling signed or unsigned @c gid_t.
 * @param gid Group id.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or the id does not fit in @c int.
 */
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

/**
 * @brief Convert @c int to @c gid_t, handling signed or unsigned @c gid_t.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n does not fit in @c gid_t.
 */
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

/**
 * @brief Convert @c int to @c mode_t, handling signed or unsigned @c mode_t.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n does not fit in @c mode_t.
 */
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

/**
 * @brief Convert @c mode_t to @c int, handling signed or unsigned @c mode_t.
 * @param m File mode.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p m does not fit in @c int.
 */
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

/**
 * @brief @c chmod() with a range-checked @c int mode.
 * @param path Pathname.
 * @param mode Mode as @c int (historic ISODE style).
 * @return Result of @c chmod, or -1 with @c errno = EINVAL if @p mode does not fit in @c mode_t.
 */
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

/**
 * @brief Convert a non-negative @c long to @c unsigned long.
 * @param n Value to convert; negative fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n < 0.
 */
static inline int
long2ulong (long n, unsigned long *out)
{
	if (out == NULL || n < 0L)
		return -1;
	*out = (unsigned long) n;
	return 0;
}

/**
 * @brief Convert @c unsigned long to @c long.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > LONG_MAX.
 */
static inline int
ulong2long (unsigned long n, long *out)
{
	if (out == NULL || n > (unsigned long) LONG_MAX)
		return -1;
	*out = (long) n;
	return 0;
}

/**
 * @brief Convert a non-negative @c off_t to @c size_t.
 * @param n Offset; negative fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 on NULL @p out, negative @p n, or overflow.
 */
static inline int
off2sizet (off_t n, size_t *out)
{
	if (out == NULL || n < 0 || (uintmax_t) n > (uintmax_t) SIZE_MAX)
		return -1;
	*out = (size_t) n;
	return 0;
}

#include <sys/socket.h>
#include <netinet/in.h>

/**
 * @brief Convert @c int to @c socklen_t (signed or unsigned).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n does not fit.
 */
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

/**
 * @brief Convert @c size_t to @c socklen_t (signed or unsigned).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n does not fit.
 */
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

/**
 * @brief Convert @c int to @c tcflag_t (termios flag word).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n does not fit.
 */
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

/**
 * @brief Bit-clear on a @c tcflag_t: @c *out = v & ~bits.
 * @param v Current flag word.
 * @param bits Mask as @c int (converted via int2tflag).
 * @param[out] out Result.
 * @return 0 on success, -1 if @p bits does not fit in @c tcflag_t.
 */
static inline int
tflag_bic (tcflag_t v, int bits, tcflag_t *out)
{
	tcflag_t m;

	if (int2tflag (bits, &m) != 0)
		return -1;
	*out = v & ~m;
	return 0;
}

/**
 * @brief Bit-set on a @c tcflag_t: @c *out = v | bits.
 * @param v Current flag word.
 * @param bits Mask as @c int (converted via int2tflag).
 * @param[out] out Result.
 * @return 0 on success, -1 if @p bits does not fit in @c tcflag_t.
 */
static inline int
tflag_bis (tcflag_t v, int bits, tcflag_t *out)
{
	tcflag_t m;

	if (int2tflag (bits, &m) != 0)
		return -1;
	*out = v | m;
	return 0;
}

/**
 * @brief Convert termios @c cc_t to @c char.
 * @param c Control character.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p c > CHAR_MAX.
 */
static inline int
cct2char (cc_t c, char *out)
{
	if (out == NULL || (uintmax_t) c > (uintmax_t) CHAR_MAX)
		return -1;
	*out = (char) c;
	return 0;
}

/**
 * @brief POSIX "leave uid unchanged" value for chown/fchown (all-bits-one).
 * @return ~(uid_t)0
 */
static inline uid_t
uid_nochg (void)
{
	return ~(uid_t) 0;
}

/**
 * @brief POSIX "leave gid unchanged" value for chown/fchown (all-bits-one).
 * @return ~(gid_t)0
 */
static inline gid_t
gid_nochg (void)
{
	return ~(gid_t) 0;
}

/**
 * @brief Replace file-type bits: @c *out = (mode & ~old_fmt) | new_fmt.
 * @param mode Current mode.
 * @param old_fmt Bits to clear (typically S_IFMT), as @c int.
 * @param new_fmt Bits to set (an S_IF* type), as @c int.
 * @param[out] out Resulting mode.
 * @return 0 on success, -1 if @p out is NULL or a format does not fit in @c mode_t.
 */
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

/**
 * @brief Convert @c ptrdiff_t to @c int.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside INT_MIN..INT_MAX.
 */
static inline int
ptrdiff2int (ptrdiff_t n, int *out)
{
	if (out == NULL || n < (ptrdiff_t) INT_MIN || n > (ptrdiff_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Convert a non-negative @c ptrdiff_t to @c size_t.
 * @param n Value to convert; negative fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 on NULL @p out, negative @p n, or overflow.
 */
static inline int
ptrdiff2sizet (ptrdiff_t n, size_t *out)
{
	if (out == NULL || n < 0 || (uintmax_t) n > (uintmax_t) SIZE_MAX)
		return -1;
	*out = (size_t) n;
	return 0;
}

/**
 * @brief Convert a signed @c char length (NSAP fields, etc.) to @c size_t.
 * @param n Length; negative fails via int2sizet.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p n is negative or does not fit.
 */
static inline int
char2sizet (char n, size_t *out)
{
	return int2sizet ((int) n, out);
}

/**
 * @brief Convert @c int to @c uint8_t (0..255).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside 0..255.
 */
static inline int
int2u8 (int n, uint8_t *out)
{
	if (out == NULL || n < 0 || n > 255)
		return -1;
	*out = (uint8_t) n;
	return 0;
}

/**
 * @brief Convert @c uint8_t to @c int (always succeeds if @p out is non-NULL).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL.
 */
static inline int
u8toint (uint8_t n, int *out)
{
	if (out == NULL)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Store an octet in a C @c char; values that do not fit in @c char fail.
 * @param n Octet.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > CHAR_MAX (high bit on signed char).
 * @see octet2char for keeping all 256 bit patterns.
 */
static inline int
u8tochar (uint8_t n, char *out)
{
	if (out == NULL || n > (uint8_t) CHAR_MAX)
		return -1;
	*out = (char) n;
	return 0;
}

/**
 * @brief Add a @c size_t to a non-negative @c int accumulator without overflow.
 * @param[in,out] acc Accumulator; must be >= 0.
 * @param n Amount to add.
 * @return 0 on success, -1 if @p acc is NULL, @p n does not fit, or the sum overflows INT_MAX.
 */
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

/**
 * @brief Add a non-negative @c int to a non-negative @c int accumulator.
 * @param[in,out] acc Accumulator; must be >= 0.
 * @param n Amount to add; negative fails.
 * @return 0 on success, -1 if @p acc is NULL, @p n < 0, or the sum overflows INT_MAX.
 */
static inline int
add_int_to_int (int *acc, int n)
{
	if (acc == NULL || n < 0 || *acc < 0 || *acc > INT_MAX - n)
		return -1;
	*acc += n;
	return 0;
}

/**
 * @brief Convert @c strlen(s)+1 to @c int (NUL-terminated protocol payloads).
 * @param s NUL-terminated string.
 * @param[out] out Destination.
 * @return 0 on success, -1 if length or length+1 does not fit in @c int.
 */
static inline int
strlen1_to_int (const char *s, int *out)
{
	int n;

	if (out == NULL || strlen2int (s, &n) != 0)
		return -1;
	if (add_int_to_int (&n, 1) != 0)
		return -1;
	*out = n;
	return 0;
}

/**
 * @brief Convert @c int to @c uint16_t (0..65535).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside 0..65535.
 */
static inline int
int2u16 (int n, uint16_t *out)
{
	if (out == NULL || n < 0 || n > 65535)
		return -1;
	*out = (uint16_t) n;
	return 0;
}

/**
 * @brief Convert a non-negative @c int to @c uint32_t.
 * @param n Value to convert; negative fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside 0..UINT32_MAX.
 */
static inline int
int2u32 (int n, uint32_t *out)
{
	if (out == NULL || n < 0 || (uintmax_t) n > (uintmax_t) UINT32_MAX)
		return -1;
	*out = (uint32_t) n;
	return 0;
}

/**
 * @brief Convert @c uint32_t to @c int.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > INT_MAX.
 */
static inline int
u32toint (uint32_t n, int *out)
{
	if (out == NULL || n > (uint32_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Bit-set an 8-bit value: @c v | bits (unsigned, so ~mask is not a negative int).
 * @param v Current bits.
 * @param bits Mask to OR in.
 * @return @p v with @p bits set, truncated to 8 bits.
 */
static inline uint8_t
u8_bis (uint8_t v, unsigned bits)
{
	return (uint8_t) (v | bits);
}

/**
 * @brief Bit-clear an 8-bit value: @c v & ~bits.
 * @param v Current bits.
 * @param bits Mask to clear.
 * @return @p v with @p bits cleared, truncated to 8 bits.
 */
static inline uint8_t
u8_bic (uint8_t v, unsigned bits)
{
	return (uint8_t) (v & ~bits);
}

/**
 * @brief Bit-set a 16-bit value: @c v | bits.
 * @param v Current bits.
 * @param bits Mask to OR in.
 * @return @p v with @p bits set, truncated to 16 bits.
 */
static inline uint16_t
u16_bis (uint16_t v, unsigned bits)
{
	return (uint16_t) (v | bits);
}

/**
 * @brief Bit-clear a 16-bit value: @c v & ~bits.
 * @param v Current bits.
 * @param bits Mask to clear.
 * @return @p v with @p bits cleared, truncated to 16 bits.
 */
static inline uint16_t
u16_bic (uint16_t v, unsigned bits)
{
	return (uint16_t) (v & ~bits);
}

/**
 * @brief Reinterpret a C @c short as a 16-bit flag word (all 65536 values).
 * @param n Signed short whose bit pattern is a flag word.
 * @return Those bits as @c uint16_t (high bit is not a sign).
 */
static inline uint16_t
as_ushort (short n)
{
	return *(unsigned short *) &n;
}

/**
 * @brief Store a 16-bit flag word into a C @c short (all 65536 values).
 * @param n Bits to store.
 * @param[out] out Destination @c short.
 * @return 0 on success, -1 if @p out is NULL.
 */
static inline int
ushort2short (uint16_t n, short *out)
{
	if (out == NULL)
		return -1;
	*(unsigned short *) out = n;
	return 0;
}

/**
 * @brief Bit-set in place on a @c short used as a 16-bit flag word.
 * @param[in,out] p Flag word.
 * @param bits Mask; must fit in 16 bits.
 * @return 0 on success, -1 if @p p is NULL or @p bits > 0xffff.
 */
static inline int
short_bis (short *p, unsigned bits)
{
	if (p == NULL || bits > 0xffffU)
		return -1;
	return ushort2short (u16_bis (as_ushort (*p), bits), p);
}

/**
 * @brief Bit-clear in place on a @c short used as a 16-bit flag word.
 * @param[in,out] p Flag word.
 * @param bits Mask; must fit in 16 bits.
 * @return 0 on success, -1 if @p p is NULL or @p bits > 0xffff.
 */
static inline int
short_bic (short *p, unsigned bits)
{
	if (p == NULL || bits > 0xffffU)
		return -1;
	return ushort2short (u16_bic (as_ushort (*p), bits), p);
}

/**
 * @brief Bit-set in place on an @c unsigned short.
 * @param[in,out] p Flag word.
 * @param bits Mask; must fit in 16 bits.
 * @return 0 on success, -1 if @p p is NULL or @p bits > 0xffff.
 */
static inline int
ushort_bis (unsigned short *p, unsigned bits)
{
	if (p == NULL || bits > 0xffffU)
		return -1;
	*p = u16_bis (*p, bits);
	return 0;
}

/**
 * @brief Bit-clear in place on an @c unsigned short.
 * @param[in,out] p Flag word.
 * @param bits Mask; must fit in 16 bits.
 * @return 0 on success, -1 if @p p is NULL or @p bits > 0xffff.
 */
static inline int
ushort_bic (unsigned short *p, unsigned bits)
{
	if (p == NULL || bits > 0xffffU)
		return -1;
	*p = u16_bic (*p, bits);
	return 0;
}

/**
 * @brief Convert @c size_t to @c unsigned short.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > USHRT_MAX.
 */
static inline int
sizet2ushort (size_t n, unsigned short *out)
{
	if (out == NULL || n > (size_t) USHRT_MAX)
		return -1;
	*out = (unsigned short) n;
	return 0;
}

/**
 * @brief Convert @c unsigned to @c unsigned short.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > USHRT_MAX.
 */
static inline int
uint2ushort (unsigned n, unsigned short *out)
{
	if (out == NULL || n > (unsigned) USHRT_MAX)
		return -1;
	*out = (unsigned short) n;
	return 0;
}

/**
 * @brief Convert @c time_t to @c int (signed or unsigned @c time_t).
 * @param n Time value.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n does not fit in @c int.
 */
static inline int
time_t2int (time_t n, int *out)
{
	if (out == NULL)
		return -1;
	if ((time_t) -1 > (time_t) 0) {
		if ((uintmax_t) n > (uintmax_t) INT_MAX)
			return -1;
	} else if ((intmax_t) n < (intmax_t) INT_MIN
			   || (intmax_t) n > (intmax_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Convert @c later - now to a non-negative @c int (0 if @p later <= @p now).
 * @param later Later time.
 * @param now Earlier time.
 * @param[out] out Delta in seconds, or 0.
 * @return 0 on success, -1 if @p out is NULL or the positive delta does not fit.
 */
static inline int
time_delta2int (time_t later, time_t now, int *out)
{
	if (out == NULL)
		return -1;
	if (later <= now) {
		*out = 0;
		return 0;
	}
	return time_t2int (later - now, out);
}

/**
 * @brief Add @p n to @c *p as @c unsigned short without wrapping past USHRT_MAX.
 * @param[in,out] p Accumulator.
 * @param n Amount to add.
 * @return 0 on success, -1 if @p p is NULL or the sum does not fit.
 */
static inline int
ushort_add (unsigned short *p, unsigned n)
{
	if (p == NULL)
		return -1;
	return uint2ushort ((unsigned) *p + n, p);
}

/**
 * @brief Convert @c off_t to @c int (signed or unsigned @c off_t).
 * @param n Offset.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n does not fit in @c int.
 */
static inline int
off2int (off_t n, int *out)
{
	if (out == NULL)
		return -1;
	if ((off_t) -1 > (off_t) 0) {
		if ((uintmax_t) n > (uintmax_t) INT_MAX)
			return -1;
	} else if ((intmax_t) n < (intmax_t) INT_MIN
			   || (intmax_t) n > (intmax_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Reinterpret a C @c char as a protocol octet (all 256 values).
 * @param c Byte stored in a @c char.
 * @return Bits as @c uint8_t via @c unsigned char.
 */
static inline uint8_t
as_octet (char c)
{
	return (uint8_t) (unsigned char) c;
}

/**
 * @brief Copy up to @p cap bytes from @p src to @p dst.
 * @param src Source; required if the copy length is > 0.
 * @param dst Destination; required if the copy length is > 0.
 * @param len Signed count (int or pointer difference); negative fails.
 * @param cap Maximum bytes to copy.
 * @param[out] out_cc Bytes copied on success; 0 on failure.
 * @return 0 on success, -1 if @p out_cc is NULL, @p len is unusable, or pointers are NULL when a copy is needed.
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

/**
 * @brief Length-checked @c memmove with BSD @c bcopy argument order.
 * @param src Source.
 * @param dst Destination.
 * @param n Byte count as @c int; negative is rejected (not a huge @c size_t).
 * @return 0 on success, -1 if @p n does not fit in @c size_t.
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

/**
 * @brief Length-checked @c memcmp with BSD @c bcmp argument order.
 * @param a First buffer.
 * @param b Second buffer.
 * @param n Byte count as @c int; negative fails.
 * @return @c memcmp result, or -1 if @p n does not fit in @c size_t.
 */
static inline int
bcmp_int (const void *a, const void *b, int n)
{
	size_t len;

	if (int2sizet (n, &len) != 0)
		return -1;
	return memcmp (a, b, len);
}

/**
 * @brief Length-checked @c memset-to-zero with BSD @c bzero argument order.
 * @param s Buffer.
 * @param n Byte count as @c int; negative fails.
 * @return 0 on success, -1 if @p n does not fit in @c size_t.
 */
static inline int
bzero_int (void *s, int n)
{
	size_t len;

	if (int2sizet (n, &len) != 0)
		return -1;
	memset (s, 0, len);
	return 0;
}

/**
 * @brief @c strncmp with a range-checked @c int length.
 * @param a First string.
 * @param b Second string.
 * @param n Max bytes as @c int; negative fails.
 * @return @c strncmp result, or -1 if @p n does not fit in @c size_t.
 */
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

/**
 * @brief @c malloc with a range-checked @c int size.
 * @param n Byte count; negative fails.
 * @return Allocated block, or NULL if @p n is invalid or allocation fails.
 */
static inline void *
malloc_int (int n)
{
	size_t len;

	if (int2sizet (n, &len) != 0)
		return NULL;
	return malloc (len);
}

/**
 * @brief @c realloc with a range-checked @c int size.
 * @param p Existing block, or NULL.
 * @param n New byte count; negative fails.
 * @return Reallocated block, or NULL if @p n is invalid or allocation fails.
 */
static inline void *
realloc_int (void *p, int n)
{
	size_t len;

	if (int2sizet (n, &len) != 0)
		return NULL;
	return realloc (p, len);
}

/**
 * @brief @c malloc of @p n elements of @p size, checking overflow of @c n * size.
 * @param n Element count as @c int; negative fails.
 * @param size Element size.
 * @return Allocated block, or NULL on invalid count, overflow, or allocation failure.
 */
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

/**
 * @brief Compute @p n * @p size as @c size_t without overflow.
 * @param n Element count as @c int; negative fails.
 * @param size Element size.
 * @param[out] out Product in bytes.
 * @return 0 on success, -1 if @p out is NULL, @p n is invalid, or the product overflows.
 */
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

/**
 * @brief @c calloc of @p n elements of @p size, checking overflow.
 * @param n Element count as @c int; negative fails.
 * @param size Element size.
 * @return Zeroed block, or NULL on invalid count, overflow, or allocation failure.
 */
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

/**
 * @brief @c realloc of @p n elements of @p size, checking overflow.
 * @param p Existing block, or NULL.
 * @param n Element count as @c int; negative fails.
 * @param size Element size.
 * @return Reallocated block, or NULL on invalid count, overflow, or allocation failure.
 */
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

/**
 * @brief @c malloc of @p base + @p extra bytes without overflow.
 * @param base Base size.
 * @param extra Extra bytes as @c int; negative fails.
 * @return Allocated block, or NULL on invalid extra, overflow, or allocation failure.
 */
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

/**
 * @brief @c qsort with a range-checked @c int element count.
 * @param base Array.
 * @param n Element count; negative fails.
 * @param size Element size.
 * @param cmp Comparison function.
 * @return 0 on success, -1 if @p n does not fit in @c size_t.
 */
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

/**
 * @brief @c qsort with a range-checked @c ptrdiff_t element count.
 * @param base Array.
 * @param n Element count; negative fails.
 * @param size Element size.
 * @param cmp Comparison function.
 * @return 0 on success, -1 if @p n does not fit in @c size_t.
 */
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

/**
 * @brief Write min(len, cap) to @p *out as @c int.
 * @param len Signed remaining-byte count; negative fails.
 * @param cap Capacity.
 * @param[out] out The smaller of @p len and @p cap.
 * @return 0 on success, -1 if @p out is NULL or the min does not fit in @c int.
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

/**
 * @brief @c read with a range-checked @c int length.
 * @param fd File descriptor.
 * @param buf Buffer.
 * @param n Byte count; negative fails with EINVAL.
 * @return @c read result, or -1 with EINVAL if @p n is invalid.
 */
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

/**
 * @brief @c write with a range-checked @c int length.
 * @param fd File descriptor.
 * @param buf Buffer.
 * @param n Byte count; negative fails with EINVAL.
 * @return @c write result, or -1 with EINVAL if @p n is invalid.
 */
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

/**
 * @brief @c fread with a range-checked @c int member count; result as @c int.
 * @param ptr Buffer.
 * @param size Member size.
 * @param nmemb Member count as @c int; negative fails with EINVAL.
 * @param stream File.
 * @return Members read, or -1 with EINVAL/EOVERFLOW on conversion failure.
 */
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

/**
 * @brief @c fwrite with a range-checked @c int member count; result as @c int.
 * @param ptr Buffer.
 * @param size Member size.
 * @param nmemb Member count as @c int; negative fails with EINVAL.
 * @param stream File.
 * @return Members written, or -1 with EINVAL/EOVERFLOW on conversion failure.
 */
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

/**
 * @brief @c fgets(s, (ep - s) + 1, fp) with a range-checked size.
 * @param s Start of buffer.
 * @param ep Last byte @c fgets may write (the NUL slot).
 * @param fp Stream.
 * @return Result of @c fgets, or NULL if pointers are NULL or the size overflows.
 */
static inline char *
fgets_room (char *s, char *ep, FILE *fp)
{
	int n;

	if (s == NULL || ep == NULL || fp == NULL)
		return NULL;
	if (min_len_cap (ep - s, (size_t) INT_MAX - 1U, &n) != 0)
		return NULL;
	if (add_int_to_int (&n, 1) != 0)
		return NULL;
	return fgets (s, n, fp);
}

/**
 * @brief Convert @c ptrdiff_t + 1 to @c int (e.g. inclusive length).
 * @param n Value before adding 1.
 * @param[out] out @p n + 1.
 * @return 0 on success, -1 if @p n does not fit or @p n == INT_MAX.
 */
static inline int
ptrdiff_plus1_to_int (ptrdiff_t n, int *out)
{
	int i;

	if (ptrdiff2int (n, &i) != 0 || i == INT_MAX)
		return -1;
	*out = i + 1;
	return 0;
}

/**
 * @brief Convert @c int to @c char (CHAR_MIN..CHAR_MAX).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside CHAR_MIN..CHAR_MAX.
 */
static inline int
int2char (int n, char *out)
{
	if (out == NULL || n < (int) CHAR_MIN || n > (int) CHAR_MAX)
		return -1;
	*out = (char) n;
	return 0;
}

/**
 * @brief Convert @c int to @c short.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside SHRT_MIN..SHRT_MAX.
 */
static inline int
int2short (int n, short *out)
{
	if (out == NULL || n < (int) SHRT_MIN || n > (int) SHRT_MAX)
		return -1;
	*out = (short) n;
	return 0;
}

/**
 * @brief Store a protocol octet as a C @c char, keeping all 256 bit patterns.
 * @param n Octet.
 * @param[out] out Destination; written via @c unsigned char.
 * @return 0 on success, -1 if @p out is NULL.
 * @see int2char for text and signed length fields that must not wrap.
 */
static inline int
octet2char (uint8_t n, char *out)
{
	if (out == NULL)
		return -1;
	*(unsigned char *) out = n;
	return 0;
}

/**
 * @brief Bit-set in place on a @c char used as an 8-bit flag/octet.
 * @param[in,out] p Byte.
 * @param bits Mask; must fit in 8 bits.
 * @return 0 on success, -1 if @p p is NULL or @p bits > 255.
 */
static inline int
char_bis (char *p, unsigned bits)
{
	uint8_t v;

	if (p == NULL || bits > 255U)
		return -1;
	v = u8_bis (as_octet (*p), bits);
	return octet2char (v, p);
}

/**
 * @brief Bit-clear in place on a @c char used as an 8-bit flag/octet.
 * @param[in,out] p Byte.
 * @param bits Mask; must fit in 8 bits.
 * @return 0 on success, -1 if @p p is NULL or @p bits > 255.
 */
static inline int
char_bic (char *p, unsigned bits)
{
	uint8_t v;

	if (p == NULL || bits > 255U)
		return -1;
	v = u8_bic (as_octet (*p), bits);
	return octet2char (v, p);
}

/**
 * @brief XOR bits in place on a @c char used as an 8-bit flag/octet.
 * @param[in,out] p Byte.
 * @param bits Mask; must fit in 8 bits.
 * @return 0 on success, -1 if @p p is NULL or @p bits > 255.
 */
static inline int
char_bxor (char *p, unsigned bits)
{
	uint8_t v;

	if (p == NULL || bits > 255U)
		return -1;
	v = (uint8_t) (as_octet (*p) ^ bits);
	return octet2char (v, p);
}

/**
 * @brief Convert @c int 0..255 to a @c char octet (all 256 patterns).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p n is outside 0..255 or @p out is NULL.
 */
static inline int
int2octet (int n, char *out)
{
	uint8_t u;

	if (int2u8 (n, &u) != 0)
		return -1;
	return octet2char (u, out);
}

/**
 * @brief Write an octet at @c **pp and advance the pointer by one.
 * @param[in,out] pp Pointer to the write cursor.
 * @param n Octet as @c int (0..255).
 * @return 0 on success, -1 if @p pp or @c *pp is NULL or @p n is not an octet.
 */
static inline int
put_octet (char **pp, int n)
{
	if (pp == NULL || *pp == NULL)
		return -1;
	if (int2octet (n, *pp) != 0)
		return -1;
	(*pp)++;
	return 0;
}

/**
 * @brief Convert @c uint16_t to @c uint8_t.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > 255.
 */
static inline int
u16to8 (uint16_t n, uint8_t *out)
{
	if (out == NULL || n > 255U)
		return -1;
	*out = (uint8_t) n;
	return 0;
}

/**
 * @brief Convert @c uint16_t to a @c char octet (all 256 patterns).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p n > 255 or @p out is NULL.
 */
static inline int
u16tooctet (uint16_t n, char *out)
{
	uint8_t u;

	if (u16to8 (n, &u) != 0)
		return -1;
	return octet2char (u, out);
}

/**
 * @brief Convert @c uint32_t to @c uint16_t.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > 65535.
 */
static inline int
u32to16 (uint32_t n, uint16_t *out)
{
	if (out == NULL || n > 65535U)
		return -1;
	*out = (uint16_t) n;
	return 0;
}

/**
 * @brief Convert a non-negative @c long to @c uint32_t.
 * @param n Value to convert; negative fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside 0..UINT32_MAX.
 */
static inline int
long2u32 (long n, uint32_t *out)
{
	if (out == NULL || n < 0L || (uintmax_t) n > (uintmax_t) UINT32_MAX)
		return -1;
	*out = (uint32_t) n;
	return 0;
}

/**
 * @brief Convert @c long to a signed @c char (via @c int).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p n does not fit in @c int then @c char.
 */
static inline int
long2char (long n, char *out)
{
	int i;

	if (long2int (n, &i) != 0)
		return -1;
	return int2char (i, out);
}

/**
 * @brief Convert @c long to a @c char octet (0..255, all bit patterns).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p n does not fit in @c int then 0..255.
 */
static inline int
long2octet (long n, char *out)
{
	int i;

	if (long2int (n, &i) != 0)
		return -1;
	return int2octet (i, out);
}

/**
 * @brief Convert @c size_t to a signed @c char.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p n does not fit in @c int then @c char.
 */
static inline int
sizet2char (size_t n, char *out)
{
	int i;

	if (sizet2int (n, &i) != 0)
		return -1;
	return int2char (i, out);
}

/**
 * @brief Convert @c size_t to @c uint8_t.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > 255.
 */
static inline int
sizet2u8 (size_t n, uint8_t *out)
{
	if (out == NULL || n > 255U)
		return -1;
	*out = (uint8_t) n;
	return 0;
}

/**
 * @brief Subtract @p b from octet @p a into an @c int, failing on underflow.
 * @param a Minuend (octet).
 * @param b Subtrahend.
 * @param[out] out @p a - @p b.
 * @return 0 on success, -1 if @p out is NULL or @p a < @p b.
 */
static inline int
u8_minus_sizet (uint8_t a, size_t b, int *out)
{
	if (out == NULL || (size_t) a < b)
		return -1;
	return sizet2int ((size_t) a - b, out);
}

/**
 * @brief Convert @c size_t to a @c char octet (0..255, all bit patterns).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p n > 255 or @p out is NULL.
 */
static inline int
sizet2octet (size_t n, char *out)
{
	uint8_t u;

	if (sizet2u8 (n, &u) != 0)
		return -1;
	return octet2char (u, out);
}

/**
 * @brief Convert @c ptrdiff_t to a signed @c char.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p n does not fit in @c int then @c char.
 */
static inline int
ptrdiff2char (ptrdiff_t n, char *out)
{
	int i;

	if (ptrdiff2int (n, &i) != 0)
		return -1;
	return int2char (i, out);
}

/**
 * @brief Convert @c ptrdiff_t to a @c char octet (0..255, all bit patterns).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p n does not fit in @c int then 0..255.
 */
static inline int
ptrdiff2octet (ptrdiff_t n, char *out)
{
	int i;

	if (ptrdiff2int (n, &i) != 0)
		return -1;
	return int2octet (i, out);
}

/**
 * @brief Convert @c int to @c float without rounding (24-bit IEEE-754 significand).
 * @param n Value to convert; must be in -16777216..16777216.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n would not convert exactly.
 */
static inline int
int2float (int n, float *out)
{
	if (out == NULL || n < -16777216 || n > 16777216)
		return -1;
	*out = (float) n;
	return 0;
}

/**
 * @brief Truncate @c double toward zero into @c int (same as a C cast).
 * @param n Value to convert; NaN fails.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL, @p n is NaN, or out of INT_MIN..INT_MAX.
 */
static inline int
double2int (double n, int *out)
{
	if (out == NULL || n != n)
		return -1;
	if (n < (double) INT_MIN || n > (double) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Convert @c int32_t to @c int (no-op on ILP32/LP64 if int is 32-bit).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside INT_MIN..INT_MAX.
 */
static inline int
int32_to_int (int32_t n, int *out)
{
	if (out == NULL || n < (int32_t) INT_MIN || n > (int32_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Convert @c long to @c int32_t.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside INT32_MIN..INT32_MAX.
 */
static inline int
long2int32 (long n, int32_t *out)
{
	if (out == NULL || n < (long) INT32_MIN || n > (long) INT32_MAX)
		return -1;
	*out = (int32_t) n;
	return 0;
}

/**
 * @brief Convert @c long long to @c int32_t.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside INT32_MIN..INT32_MAX.
 */
static inline int
llong2int32 (long long n, int32_t *out)
{
	if (out == NULL || n < (long long) INT32_MIN
			|| n > (long long) INT32_MAX)
		return -1;
	*out = (int32_t) n;
	return 0;
}

/**
 * @brief Store a @c long into a signed integer object of size @p outsz.
 * @param n Value to store.
 * @param[out] out Buffer of @p outsz bytes.
 * @param outsz Width: sizeof int32_t, long, int, short, or char (utmp time fields).
 * @return 0 on success, -1 if @p out is NULL, @p outsz is unsupported, or @p n does not fit.
 */
static inline int
long2sint_n (long n, void *out, size_t outsz)
{
	if (out == NULL)
		return -1;
	if (outsz == sizeof (int32_t)) {
		int32_t v;

		if (long2int32 (n, &v) != 0)
			return -1;
		memcpy (out, &v, sizeof v);
		return 0;
	}
	if (outsz == sizeof (long)) {
		memcpy (out, &n, sizeof n);
		return 0;
	}
	if (outsz == sizeof (int)) {
		int v;

		if (long2int (n, &v) != 0)
			return -1;
		memcpy (out, &v, sizeof v);
		return 0;
	}
	if (outsz == sizeof (short)) {
		int v;
		short s;

		if (long2int (n, &v) != 0 || int2short (v, &s) != 0)
			return -1;
		memcpy (out, &s, sizeof s);
		return 0;
	}
	if (outsz == sizeof (char)) {
		int v;
		char c;

		if (long2int (n, &v) != 0 || int2char (v, &c) != 0)
			return -1;
		memcpy (out, &c, sizeof c);
		return 0;
	}
	return -1;
}

/**
 * @brief Convert @c int to @c sa_family_t (signed or unsigned).
 * @param n Address family.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n does not fit.
 */
static inline int
int2safamily (int n, sa_family_t *out)
{
	if (out == NULL)
		return -1;
	if ((sa_family_t) -1 > (sa_family_t) 0) {
		if (n < 0 || (uintmax_t) n > (uintmax_t) (sa_family_t) -1)
			return -1;
	} else {
		sa_family_t f = (sa_family_t) n;

		if ((int) f != n)
			return -1;
		*out = f;
		return 0;
	}
	*out = (sa_family_t) n;
	return 0;
}

/**
 * @brief Convert @c int to @c in_port_t (signed or unsigned).
 * @param n Port number.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n does not fit.
 */
static inline int
int2inport (int n, in_port_t *out)
{
	if (out == NULL)
		return -1;
	if ((in_port_t) -1 > (in_port_t) 0) {
		if (n < 0 || (uintmax_t) n > (uintmax_t) (in_port_t) -1)
			return -1;
	} else {
		in_port_t p = (in_port_t) n;

		if ((int) p != n)
			return -1;
		*out = p;
		return 0;
	}
	*out = (in_port_t) n;
	return 0;
}

/**
 * @brief Recover a small integer previously stored in a pointer (@c ot_info, etc.).
 * @param p Pointer-sized integer.
 * @param[out] out Destination.
 * @return 0 on success, -1 if the value does not fit in @c int.
 */
static inline int
caddr2int (const void *p, int *out)
{
	return ssize2int ((ssize_t) p, out);
}

/**
 * @brief Convert @c uintmax_t to @c int.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n > INT_MAX.
 */
static inline int
uintmax2int (uintmax_t n, int *out)
{
	if (out == NULL || n > (uintmax_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Convert @c intmax_t to @c int.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or @p n is outside INT_MIN..INT_MAX.
 */
static inline int
intmax2int (intmax_t n, int *out)
{
	if (out == NULL || n < (intmax_t) INT_MIN || n > (intmax_t) INT_MAX)
		return -1;
	*out = (int) n;
	return 0;
}

/**
 * @brief Convert @c long to @c double only if the conversion is exact.
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p out is NULL or casting back to @c long does not yield @p n.
 */
static inline int
long2double (long n, double *out)
{
	double d;

	if (out == NULL)
		return -1;
	d = (double) n;
	if ((long) d != n)
		return -1;
	*out = d;
	return 0;
}

/**
 * @brief Convert @c unsigned 0..255 to a @c char octet (all bit patterns).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p n > 255 or @p out is NULL.
 */
static inline int
uint2octet (unsigned n, char *out)
{
	if (n > 255U)
		return -1;
	return octet2char ((uint8_t) n, out);
}

/**
 * @brief Convert @c uint32_t 0..255 to a @c char octet (all bit patterns).
 * @param n Value to convert.
 * @param[out] out Destination.
 * @return 0 on success, -1 if @p n > 255 or @p out is NULL.
 */
static inline int
u32tooctet (uint32_t n, char *out)
{
	if (n > 255U)
		return -1;
	return octet2char ((uint8_t) n, out);
}

/**
 * @brief Interval in centiseconds between two @c timeval values.
 * @param later Later timestamp.
 * @param earlier Earlier timestamp.
 * @param[out] out Elapsed time in 1/100 s, saturating a negative usec borrow.
 * @return 0 on success, -1 if a pointer is NULL or the result overflows @c int.
 */
static inline int
timeval_centisecs (const struct timeval *later,
		   const struct timeval *earlier, int *out)
{
	int	secs,
		usec_part,
		ticks;
	long	usec;

	if (later == NULL || earlier == NULL || out == NULL)
		return -1;
	if (time_delta2int (later -> tv_sec, earlier -> tv_sec, &secs) != 0)
		return -1;
	usec = (long) later -> tv_usec - (long) earlier -> tv_usec;
	if (usec < 0L) {
		if (secs == 0)
			usec = 0L;
		else {
			secs--;
			usec += 1000000L;
		}
	}
	if (secs > INT_MAX / 100)
		return -1;
	ticks = secs * 100;
	if (long2int (usec / 10000L, &usec_part) != 0)
		return -1;
	if (ticks > INT_MAX - usec_part)
		return -1;
	*out = ticks + usec_part;
	return 0;
}

/**
 * @brief Convert a @c timeval to milliseconds as @c uint32_t.
 * @param tv Timestamp (seconds and microseconds, both treated as non-negative).
 * @param[out] out @c tv_sec * 1000 + tv_usec / 1000.
 * @return 0 on success, -1 if a pointer is NULL, a field is negative, or the result overflows.
 */
static inline int
timeval_millis (const struct timeval *tv, uint32_t *out)
{
	int	secs,
		msec;

	if (tv == NULL || out == NULL)
		return -1;
	if (time_t2int (tv -> tv_sec, &secs) != 0 || secs < 0)
		return -1;
	if (long2int ((long) (tv -> tv_usec / 1000), &msec) != 0 || msec < 0)
		return -1;
	if (secs > (INT_MAX - msec) / 1000)
		return -1;
	return int2u32 (secs * 1000 + msec, out);
}

#endif
