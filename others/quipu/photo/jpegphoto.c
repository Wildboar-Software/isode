/* jpegphoto.c - your comments here */

/*
 *
 * jpegphoto- Read standard input and pipe output to correct displayer
 *
 * Russ Wright - Lawrence Berkeley Laboratory-  Oct 1991
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tailor.h"
#include "general.h"
#include "jpeg.h"
#include <signal.h>

#define BAD_EXIT        -1

char            command[512];

int mygetchar (void) {
	char            c;

	if (!read(0, &c, 1))
		return (EOF);
	else
		return (c);
}

/*
 * SkipAsn1Len - skip the ASN-1 encoded length (variable # of octets)
 */
void SkipAsn1Len (void) {
	unsigned char   c;

	c = mygetchar();
	if (c > 0x80) {
		c &= ~0x80;
		while (c--) {
			mygetchar();
		}
	}
}

void DoG3Fax (unsigned char firstChar) {
	char            buffer[8192];
	FILE           *newPipe;
	int             len;

	strncpy(command, isodefile("g3fax/Xphoto", 1), sizeof(command) - 1);
	fprintf(stderr, "command IS '%s' ", command);
	newPipe = popen(command, "w");
	if (newPipe == NULL) {
		fprintf(stderr, "command '%s' failed", command);
		perror(" ");
		exit(BAD_EXIT);
	}
	fwrite((char *)&firstChar, 1, 1, newPipe);
	while (len = read(0, buffer, sizeof(buffer))) {
		if (!fwrite(buffer, 1, len, newPipe)) {
			fprintf(stderr, "write to pipe failed for '%s'", command);
			perror(" ");
			exit(BAD_EXIT);
		}
	}
	pclose(newPipe);
	exit(0);
}

void DoNewJPEG (void) {
	SkipAsn1Len();
	strncpy(command, isodefile("g3fax/jpeg.sh", 1), sizeof(command) - 1);
	if (execl(command, "xphoto-jpeg", 0)) {
		fprintf(stderr, "command '%s' failed", command);
		perror(" ");
		exit(BAD_EXIT);
	}
	/*NOTREACHED*/
}

void DoJPEG (void) {
	SkipAsn1Len();
	strncpy(command, isodefile("g3fax/jpeg.sh", 1), sizeof(command) - 1);
	if (execl(command, "xphoto-jpeg", 0)) {
		fprintf(stderr, "command '%s' failed", command);
		perror(" ");
		exit(BAD_EXIT);
	}
	/*NOTREACHED*/
}

void main (int argc, char **argv, char **envp) {
	unsigned char   firstChar;

	firstChar = mygetchar();
	/* test first octet */
	switch (firstChar) {
	case NEW_JPEG_TAG:
		DoNewJPEG();
		break;
	case JPEG_TAG:
		DoJPEG();
		break;
	case OLD_G3Fax_TAG:
	case G3Fax_TAG:
		DoG3Fax(firstChar);
		break;
	default:
		fprintf(stderr, "Unknown Photo Format: %d\n", firstChar);
		exit(BAD_EXIT);
	}
	/*NOTREACHED*/
}
