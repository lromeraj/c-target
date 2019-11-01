#include "argv.h"
#include "str.h"
#include <stdlib.h>
#include <string.h>

struct _Argv {
	char **list;
	int max, n;
};

void argv_strlist( Argv *argv, char *dest, size_t size ) {

	int i, len;
	char *_slist;

	if ( !argv )
		return;

	len = 0;

	for ( i=0; argv->list[ i ]; i++ ) {
		len+=strlen( argv->list[ i ] )+1;
	}

	_slist = str_alloc( len );

	if ( _slist ) {

		_slist[ 0 ] = '\0';

		for ( i=0; argv->list[ i ]; i++ ) {
			strcat( _slist, argv->list[ i ] );
			strcat( _slist, " ");
		}

	}

	strncpy( dest, _slist, size );
	str_destroy( _slist );

}

void argv_clean( Argv *argv ) {

  int i;
  if ( !argv )
		return;

	if ( argv->list ) {

    for ( i=0; i < argv->n; i++ ) {
			free( argv->list[ i ] );
			argv->list[ i ] = NULL;
		}

  }

}

void argv_free( Argv *argv ) {

  if ( !argv )
    return;

  argv_clean( argv );

  if ( argv->list ) {
    free( argv->list );
    argv->list = NULL;
  }

	free( argv );

}

Argv* argv_build( int q ) {

	int i;
	Argv *argv;

	if ( q <= 0 )
		return NULL;

	argv = (Argv*) malloc( sizeof( Argv ) );

	if ( argv ) {
		argv->list = (char**)malloc( q*sizeof( char* ) );
		if ( argv->list ) {

			for (i=0; i<q; i++) {
				argv->list[ i ] = NULL;
			}

			argv->n = 0;
			argv->max = q;

		} else {
			argv_free( argv );
		}
	}

	return argv;
}

void argv_add( Argv *argv, const char *str ) {

	if ( !argv || !str )
		return;

	if ( argv->n < argv->max ) {
		argv->list[ argv->n ] = str_clone( str );
		argv->n++;
	}

}

char *argv_get_idx( Argv *argv, int idx ) {

	if ( !argv || idx < 0 )
		return NULL;

	if ( idx >= argv->n )
		return NULL;

	return argv->list[ 0 ];

}

int argv_n( Argv *argv ) {

	if ( !argv )
		return -1;

	return argv->n;
}
