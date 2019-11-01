#include "str.h"
#include "tools.h"
#include "conf.h"
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void basename( char *dest, char *src, size_t size ) {

	char *_str, *tok1, *tok2, *_tok;

	if ( !dest || !src )
		return;

	_str = str_clone( src );

	tok1 = tok2 = strtok_r( _str, "/", &_tok );

	while( tok2 ) {
		tok2 = strtok_r( NULL, "/", &_tok );
		tok1 = tok2 ? tok2 : tok1;
	}

	strncpy( dest, tok1, size );

	str_destroy( _str );

}

bool exists_cmd( const char *_cmd ) {

	int _sts;
	char _buff[ 128 ];

	sprintf( _buff, "command -v %s > /dev/null 2>&1 ", _cmd );
	_sts = system( _buff );

	return _sts ? false : true;
}

bool exists_dir( const char *path ) {

	DIR *dir;
	bool is;

	if ( !path )
		return false;

	dir = opendir( path );
	is = dir ? true : false;
	closedir( dir );

	return is;
}

bool exists_file( const char *f_path ) {

	if ( !f_path )
		return false;

	return access( f_path, F_OK ) != -1 ? true : false;
}

bool find_word( char *str, char *word ) {

	int _slen, _wlen, _idxm, i;

	if ( !word || !str ) return false;

	_idxm = 0; /* index matches */
	_slen = strlen( str );
	_wlen = strlen( word );

	for ( i=0; i<_slen; i++ ) {

		if ( word[ _idxm ] == str[ i ] ) {
			_idxm++;
		} else {
			_idxm = 0;
		}

		if ( _idxm == _wlen-1 ) break;

	}

	return _idxm == _wlen-1 ? true : false;

}

char* shift_collect_to(
	char **argv,
	int max,
	char *strl,
	const char *del, /* delimeter */
	int *to
) {

	int i, len;
	char *_args;
	char _buff[ 2048 ] = "";

	for ( i=(*to+1); i<max; i++ ) {
		if ( strl && !strcmptok( argv[ i ], strl, del ) ) {
			break;
		} else {
			strcat( _buff, argv[ i ] );
			strcat( _buff, " " );
		}
	}

	*to = --i;

	_args = NULL;
	len = strlen( _buff );

	if ( len ) {
		_args = (char*) malloc( (len+1)*sizeof( char ) );
		strcpy( _args, _buff );
	}

	return _args;
}

void shift_to(
	char **argv,
	int max,
	char *strl,
	const char *del,
	int *to
) {

	int i;

	for ( i=(*to+1); i<max; i++ ) {
		if ( !strcmptok( argv[ i ], strl, del ) )
			break;
	}

	*to = --i;

}

int print_file( FILE *stream, const char *f_path ) {

	FILE *f;
	char c;
	int bytes;

	if ( !f_path )
		return 0;

	bytes = 0;
	f = fopen( f_path, "r" );

	if ( !f )
		return 0;

	while ( fscanf( f, "%c", &c ) == 1 ) {
		bytes+=fprintf( stream, "%c", c );
	}

	fclose( f );

	return bytes;
}

void _p( PrintType type, const char *frmt, ... ) {

	va_list _list;
	char _buff[ 1024 ];

	va_start( _list, frmt );
	vsprintf( _buff, frmt, _list );
	va_end( _list );

	switch ( type ) {

		case _ERROR:
			fprintf( stderr, "%serror%s: %s", ANSI_FG_BRED, ANSI_RESET, _buff );
			break;
		case _SUCCESS:
			fprintf( stdout, "%ssuccess%s: %s", ANSI_FG_BGREEN, ANSI_RESET, _buff );
			break;
		case _INFO:
			fprintf( stdout, "%sinfo%s: %s", ANSI_FG_BCYAN, ANSI_RESET, _buff );
			break;
		case _WARN:
			fprintf( stderr, "%swarn%s: %s", ANSI_FG_BYELLOW, ANSI_RESET, _buff );
			break;
		case _TASK_START:
			fprintf( stdout, "======== [%s%s%s] %s%s%s\n", ANSI_FG_BPURPLE, conf_get_env(), ANSI_RESET, ANSI_FG_BGREEN, _buff, ANSI_RESET );
			break;
		case _NOTE:
			fprintf( stdout, "%snote%s: %s%s", ANSI_FG_BPURPLE, ANSI_RESET, _buff, ANSI_RESET );
			break;
		case _TASK_END:
			fprintf( stdout, "======== [%s%s%s] %s%s%s\n", ANSI_FG_BPURPLE, conf_get_env(), ANSI_RESET, ANSI_FG_BRED, _buff, ANSI_RESET );
			break;
	}

	fflush( stdout );
	fflush( stderr );

}
