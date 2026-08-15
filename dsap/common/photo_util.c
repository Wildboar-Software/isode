/* interface.c - photo bit manipulation utility routines */

#include <stdio.h>
#include "quipu/photo.h"

/**
 * @file photo_util.c
 *
 * Utility routines used by both the encoding and decoding programs.
 * The routines are concerned with getting and setting bits of a bit
 * string.
 *
 * All these routines work in basically the same way. A mask is used to
 * get at each individual bit within a byte. Each time the next bit is
 * required, the mask is shifted right; when the mask is zero, the byte
 * is either written to the file, or the next byte is read in, depending
 * upon the routine.
 */

int PIC_LINESIZE,STOP,NUMLINES;

/**
 * @brief Get the next bit from the input.
 *
 * @param lineptr Bit string to read from.
 * @return 0 if the bit is zero, 1 if the bit is one.
 */
char get_bit (bit_string *lineptr)
{
	unsigned char    result;

	/* Anding the mask and the data gives a 0 if the bit masked is 0, 1 otherwis
	e */
	result = lineptr->mask & lineptr->pos;
	lineptr->mask  >>= 1;
	if (lineptr->mask == 0) {
		lineptr->pos = *lineptr->dbuf++;
		lineptr->mask = BIT_MASK;
	}
	if( result != 0 )    /* may not be 1, may be 0001000 for example */
		result = 1;
	return ( (char) result );
}

/**
 * @brief Set the next bit of a bit string to one.
 *
 * @param lineptr Bit string to write to.
 */
void set_bit (bit_string *lineptr)
{
	/* This sets the masked bit */
	lineptr->pos |= lineptr->mask;
	lineptr->mask  >>= 1;
	if (lineptr->mask == 0) {
		*lineptr->dbuf++ = lineptr->pos;
		lineptr->mask = BIT_MASK;
	}
}

/**
 * @brief Clear the next bit of a bit string (set it to zero).
 *
 * @param lineptr Bit string to write to.
 */
void clr_bit (bit_string *lineptr)
{
	/* clear the masked bit */
	lineptr->pos &=   ~(lineptr->mask) ;
	lineptr->mask  >>= 1;         /* right shift the mask */
	if (lineptr->mask == 0) {     /* may need to move on to the next byte */
		*lineptr->dbuf++ = lineptr->pos;
		lineptr->mask = BIT_MASK;
	}
}
