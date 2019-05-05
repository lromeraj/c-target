#include "str.h"
#include "conf.h"
#include "tools.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* ===== GLOBAL ===== */
char ENV[ 100 ] = "";
void *CONF_DATA[ MAX_CONFIG ] = {NULL};
char CONF_NAMES[ MAX_CONFIG ][ 128 ] = {{0}};

void conf_free_all() {
	int i;
	for ( i=0; i<MAX_CONFIG; i++ ) {
		conf_free_p( i );
	}
}

char *conf_get_env() {
	return ENV;
}

void conf_set_env( const char *env ) {

	if ( !env )
		return;
	strcpy( ENV, env );

}


int conf_load_env( const char *f_name, const char *env ) {

	FILE *cf;

	int i, j,
			sts,
			vi, /* value index */
			len; /* used for string lengths */
	char c;
	Conf_p _p; /* conf parameter */
	char _line[ 1024 ] = "";
	char _sbuff[ 1024 ] = "";
	char _vbuff[ 100 ][ 256 ] = {{0}};
	bool	envSearch, /* environment search process */
				addChar, /* indicates is a char shold be added to the buffer */
				isArr, /* indicates if the current value is an array */
				isStr, /* indicates if the current value is a string */
				pend; /* indicates the end of a parameter */

	cf = fopen( f_name, "r" );

	if ( !cf )
		return -1;

	vi=0;
	sts=0;
	pend=false;
	isArr=false;
	isStr=false;
	envSearch = true;
	_p = NONE;

	while ( fgets( _line, sizeof( _line ), cf ) ) {

		_line[ strlen( _line ) - 1 ] = 0; /* cleans \n */

		/* env search */
		if ( envSearch ) {

			if ( !strncmp( _line, "[", 1 ) ) {

				i=1; j=0;
				while ( _line[ i ] != ']' ) {

					_sbuff[ j ] = _line[ i ];
					_sbuff[ j + 1 ] = 0;
					j++;
					i++;
				}

				if ( !strcmp( _sbuff, env ) ) {
					envSearch = false;
					j=0;
				}

			}
			continue;
		}

		/* stop parsing when other environment starts */
		if ( !envSearch && !strncmp( _line, "[", 1 ) ) {
			break;
		}

		len = strlen( _line ); /* get the length of the line */


		for (i=0; i<=len; i++) {

			c = _line[ i ];
			addChar = true;

			if ( c == '#' && !isStr ) {
				i = len;
				addChar = false;
			}

			if ( c == '=' && vi == 0 && !isStr ) {
				vi=1;
				j=0;
				addChar = false;
			}

			if ( c == ' ' || c == '\t' ) {
				addChar = false;
			}

			if ( c == '[' ) {
				isArr=true;
				addChar=false;
			}

			if ( c == ']' ) {
				isArr=false;
				addChar=false;
				pend = true;
			}

			if ( c == ',' && isArr && !isStr ) {
				vi++;
				j=0;
				addChar = false;
			}

			if ( c == '"' ) {
				isStr = !isStr;
				addChar = false;
			}

			if ( isStr && c != '"' ) addChar = true;

			if ( c == '\0' || c == ' ' || c == '#' ) {

				if ( _vbuff[ 1 ][ 0 ] != '\0' && vi == 1 && !isStr && !isArr ) {
					pend = true;
					addChar = false;
				}

			}

			if ( addChar && c ) {
				_vbuff[ vi ][ j ] = c;
				_vbuff[ vi ][ j + 1 ] = 0;
				j++;
			}

			if ( pend ) {

				vi++;
				_p = conf_get_p_by_name(  _vbuff[ 0 ] );

				if ( _p != NONE ) {

					conf_alloc_p( _p, vi );

					for ( j=0; j<vi; j++ ) {
						conf_set_p( _p, j, _vbuff[ j ] );
					}

				}

				/* clean buffers */
				for ( j=0; j<vi; j++ ) {
					_vbuff[ j ][ 0 ] = 0;
				}
				_sbuff[0] = 0;

				/* reset flags */
				j=0;
				vi=0;
				isArr=false;
				isStr=false;
				pend=false;
				addChar = false;
			}

		}

	}

	if ( envSearch ) {
		sts = 2;
	} else {
		conf_set_env( env );
	}

	fclose( cf );

	return sts;
}

void conf_set_p_name( Conf_p p, const char *str ) {

	if ( p < 0 || p >= MAX_CONFIG )
		return;

	strcpy( CONF_NAMES[ p ], str );

}

void conf_alloc_p( Conf_p p, int n ) {

	int i, q;

	if ( p < 0 || p >= MAX_CONFIG )
		return;

	conf_free_p( p );

	q = n+1;
	CONF_DATA[ p ] = (char**)malloc( q*sizeof(char*) );
	for (i=0; i<q; i++) {
		((char**)CONF_DATA[ p ])[ i ] = NULL;
	}

}


void conf_set_p( Conf_p p, int idx, const char *str ) {

	int  len;
	char **__arr;

	if ( p < 0 || p >= MAX_CONFIG )
		return;

	len = strlen( str );

	__arr = ((char**)CONF_DATA[ p ]);

	if ( __arr[ idx ] ) {
		free( __arr[ idx ] );
	}
	__arr[ idx ] = str_alloc( len );

	if ( __arr[ idx ] ) {
		strcpy( __arr[ idx ], str );
	}

}

Conf_p conf_get_p_by_name( const char *_name ) {

	int i;
	Conf_p _p;

	if ( !_name )
		return NONE;

	_p = NONE;

	for ( i=0; i<MAX_CONFIG; i++ ) {
		if ( !strcmp( CONF_NAMES[ i ], _name ) ) {
			_p = (Conf_p)i;
			break;
		}
	}

	return _p;
}

char* conf_get_pidx( Conf_p p, int idx ) {

	int i;
	void *vp;

	if ( p < 0 || p >= MAX_CONFIG )
		return NULL;

	vp = NULL;

	if ( CONF_DATA[ p ] ) {

		i = 0;
		while ( i != idx ) {

			if ( !((char**)CONF_DATA[ p ])[ i ] ) {
				break;
			} else {
				i++;
			}
		}

		vp = ((char**)CONF_DATA[ p ])[ i ];

	}

	return vp;

}

char** conf_get_p( Conf_p p ) {

	if ( p < 0 || p >= MAX_CONFIG )
		return NULL;

	return ((char**)CONF_DATA[ p ]);

}

void conf_free_p( Conf_p p ) {

	int i;

	if ( p < 0 || p >= MAX_CONFIG )
		return;

	if ( CONF_DATA[ p ] ) {

		for (i=0; ((char**)CONF_DATA[p])[i]; i++) {
			free( ((char**)CONF_DATA[p])[i] );
		}

		free( CONF_DATA[ p ] );
	}

}

int conf_check_p( Conf_p p, ... ) {

	int i, j, sts;
	void *data;
	char *str;
	va_list _list;
	Conf_check chk;
	int to_check[ MAX_CHECKS ] = {0};

	if ( p < 0 || p >= MAX_CONFIG )
		return -1;

	sts = 0;
	data = CONF_DATA[ p ];

	va_start( _list, p );

	chk = va_arg( _list, Conf_check );

	while ( chk != _END_CHECK ) {

		if ( chk >= 0 && chk < MAX_CHECKS ) {
			to_check[ chk ] = 1;
		}
		chk = va_arg( _list, Conf_check );
	}

	for ( i=0; i < MAX_CHECKS; i++ ) {

		j = to_check[ i ];
		if ( j != 1 ) continue;

		if ( i == _NULL_CHECK ) {

			if ( !data ) {
				sts = _NULL_CHECK;
				_p( _ERROR, "$%s is not defined\n", CONF_NAMES[ p ] );
				break;
			}

		}

		str = conf_get_pidx( p, 1 );

		if ( str ) {

			if ( i == _FILE_CHECK && !exists_file( str ) ) {
				sts = _FILE_CHECK;
				_p( _ERROR, "%s: '%s' no such file\n", CONF_NAMES[ p ], str );
			}

			if ( i == _DIR_CHECK && !exists_dir( str ) ) {
				sts = _DIR_CHECK;
				_p( _ERROR, "%s: '%s' no such directory\n", CONF_NAMES[ p ], str );
			}

		}

	}

	va_end( _list );

	return sts;
}

void conf_upd( char **__data, int size ) {

	Conf_p p;
	char *pn; /* parameter name */

	p = NONE;

	if ( !__data )
		return;

	pn = __data[ 0 ];

	p = conf_get_p_by_name( pn );

	if ( p == NONE )
		return;

	conf_free_p( p );
	CONF_DATA[ p ] = __data;

}

char *conf_get_p_name( Conf_p p ) {

	if ( p < 0 || p >= MAX_CONFIG )
		return NULL;

	return CONF_NAMES[ p ];
}
