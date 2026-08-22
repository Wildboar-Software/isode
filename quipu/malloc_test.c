#include <stdlib.h>
#include <stdio.h>
void attempt_restart (void);


int	mem_heap;

int	start, finish;

int main (int argc, char **argv) {
	int	times = 1000;
	int	i;
	char	*big;

	if (argc != 1 && argc != 2) {
		printf("usage: %s [number]\n");
		exit(1);
	}
	if (argc == 2)
		times = atoi(argv[1]);
	start = (int) sbrk(0);
	printf("Before starting, sbrk is %d\n", start);
	big = (char *) malloc(sizeof(char *));
	for (i = 1; i < times + 1; i++) {
		big = (char *) realloc(big, i * sizeof(char *));
	}
	finish = (int) sbrk(0);
	printf("After %d calls to realloc sbrk is %d\n", times, finish);
	printf("Total bytes actually allocated is %d\n", times*sizeof(char *));
	printf("Total memory growth is %d\n", finish - start );
}

void attempt_restart (void) {
	finish = (int) sbrk(0);
	printf("trouble...sbrk is %d, total growth is %d\n", finish, finish - start);
	exit(1);
}
