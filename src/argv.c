#include "argv.h"
#include "str.h"
#include <stdlib.h>
#include <string.h>

struct _Argv {
	char **_list;
	int _max, _n;
};

char *argv_strlist( Argv *argv ) {

	int i, len;
	char *_slist;

	if ( !argv )
		return NULL;

	len = 0;

	for ( i=0; argv->_list[ i ]; i++ ) {
		len+=strlen( argv->_list[ i ] )+1;
	}

	_slist = str_alloc( len );

	if ( _slist ) {
		_slist[ 0 ] = '\0';
		for ( i=0; argv->_list[ i ]; i++ ) {
			strcat( _slist, argv->_list[ i ] );
			strcat( _slist, " ");
		}

	}

	return _slist;
}

void argv_clean( Argv *argv ) {

  int i;
  if ( !argv )
		return;

	if ( argv->_list ) {

    for ( i=0; i < argv->_n; i++ ) {
			free( argv->_list[ i ] );
			argv->_list[ i ] = NULL;
		}

  }

}

void argv_free( Argv *argv ) {

  if ( !argv )
    return;

  argv_clean( argv );

  if ( argv->_list ) {
    free( argv->_list );
    argv->_list = NULL;
  }

	free( argv );

}

Argv* argv_build( int q ) {

	int i;
	Argv *argv;

	argv = (Argv*) malloc( sizeof( Argv ) );

	if ( argv ) {
		argv->_list = (char**)malloc( q*sizeof( char* ) );
		if ( argv->_list ) {

			for (i=0; i<q; i++) {
				argv->_list[ i ] = NULL;
			}

			argv->_n = 0;
			argv->_max = q;

		} else {
			argv_free( argv );
		}
	}

	return argv;
}

void argv_add( Argv *argv, const char *str ) {

	int len;
	if ( !argv || !str )
		return;

	if ( argv->_n+1 < argv->_max ) {
		len = strlen( str );
		argv->_list[ argv->_n ] = str_alloc( len );
		strcpy( argv->_list[ argv->_n ], str );
		argv->_n++;
	}

}

char** argv_get_list( Argv *argv ) {
	if ( !argv )
		return NULL;

	return argv->_list;
}
