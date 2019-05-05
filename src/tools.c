#include "tools.h"
#include "conf.h"
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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

void _p( PrintType type, const char *frmt, ... ) {

	va_list _list;
	char _buff[ 1024 ];

	va_start( _list, frmt );
	vsprintf( _buff, frmt, _list );
	va_end( _list );

	switch ( type ) {

		case _ERROR:
			printf( "%serror%s: %s", ANSI_FG_BRED, ANSI_RESET, _buff );
			break;
		case _SUCCESS:
			printf( "%ssuccess%s: %s", ANSI_FG_BGREEN, ANSI_RESET, _buff );
			break;
		case _INFO:
			printf( "%sinfo%s: %s", ANSI_FG_BCYAN, ANSI_RESET, _buff );
			break;
		case _WARN:
			printf( "%swarn%s: %s", ANSI_FG_BYELLOW, ANSI_RESET, _buff );
			break;
		case _TASK_START:
			printf("======== [%s%s%s] %s%s%s\n", ANSI_FG_BPURPLE, conf_get_env(), ANSI_RESET, ANSI_FG_BGREEN, _buff, ANSI_RESET );
			break;
		case _NOTE:
			printf("%snote%s: %s%s", ANSI_FG_BPURPLE, ANSI_RESET, _buff, ANSI_RESET );
			break;
		case _TASK_END:
			printf("======== [%s%s%s] %s%s%s\n", ANSI_FG_BPURPLE, conf_get_env(), ANSI_RESET, ANSI_FG_BRED, _buff, ANSI_RESET );
			break;
	}

	fflush( stdout );

}
