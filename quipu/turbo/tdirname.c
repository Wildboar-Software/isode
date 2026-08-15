#include <strings.h>
#include "general.h"
#include <string.h>
#include <unistd.h>

int main (int argc, char **argv) {
	char	*p;

	if ( argc < 2 || ((p = rindex( argv[ 1 ], '/' )) == 0) ) {
		write( 1, ".\n", sizeof( ".\n" ) );
		return( 0 );
	}
	*p = '\0';
	write( 1, argv[ 1 ], strlen( argv[ 1 ] ) );
	write( 1, "\n", 1 );
	return( 0 );
}
