/* lexequ.c - Compare two strings ignoring case */







#include <stdio.h>
#include "general.h"

int lexequ (char *str1, char *str2) {
	if (str1 == NULL)
		if (str2 == NULL)
			return (0);
		else
			return (-1);

	if (str2 == NULL)
		return (1);

	while (chrcnv[*str1] == chrcnv[*str2]) {
		if (*str1++ == NULL)
			return (0);
		str2++;
	}

	if (chrcnv[*str1] > chrcnv[*str2])
		return (1);
	else
		return (-1);
}
