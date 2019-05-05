#include "ui.h"
#include "str.h"
#include "stack.h"
#include "argv.h"
#include "conf.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define _VERSION "v0.7.2"

enum { HELP_HEAD, HELP_BODY };

void _kill( int errc );


int _rm( const char *f_path );


void ascii_decorations();

void basename( char *dest, char *src, size_t size ) {

	int len;
	char *_str, *tok1, *tok2;

	if ( !dest || !src )
		return;

	len = strlen( src );
	_str = (char*) malloc( (len+1)*sizeof( char ) );
	strcpy( _str, src );

	tok1 = tok2 = strtok( _str, "/" );

	while( tok2 ) {
		tok2 = strtok( NULL, "/" );
		tok1 = tok2 ? tok2 : tok1;
	}

	strncpy( dest, tok1, size );

	free( _str );
}

int _comp();
int _link();
int _clean();
void _run();
void _dist();
void _ini();
void _help( int argc, char *argv[] );

long _statmd( const char *f_path ) {

	time_t md;
	struct stat f_info = {0};

	if ( !f_path )
		return -1;

	stat( f_path, &f_info );
	md = f_info.st_mtime;

	return (long)md;
}

void _run( const char *_args) {

	int i, sts;
	char _buff[ 128 ] = "";
	char _cmd[ 2048 ] = "";
	char **_target_args;
	char *_target;

	sts = 0;

	sts += conf_check_p( TARGET, _NULL_CHECK, _END_CHECK );

	if ( sts )
		return;

	_target = conf_get_pidx( TARGET, 1 );
	_target_args = conf_get_p( TARGET_ARGS );

 	if ( !exists_file( _target ) ) {
		_p( _WARN, "not compiled yet\n" );
		return;
	}

	strcat( _cmd, "./" );
	strcat( _cmd, _target );
	strcat( _cmd, " " );

	if ( _args != NULL ) {
		strcat( _cmd, _args );
	} else if ( _target_args != NULL ) {

		for (i=1; _target_args[ i ]; i++ ) {
			sprintf( _buff, "%s ", _target_args[ i ] );
			strcat( _cmd, _buff );
		}

	}

	system( _cmd );

}

int main( int argc, char *argv[] ) {

	int i, j, sts;
	FILE *_f;
	char *_arg, *_str;
	char _cmd[ 2048 ] = "";

	if ( argc < 2 ) {
		_p( _ERROR, "too few args\n");
		_kill( 1 );
	}

	/* check for configuration file */
	_f = fopen( CONF_FILE_NAME, "r" );

	if ( !_f && strcmptok( argv[ 1 ], "-i,--init,-h,--help", "," ) ) {
		_p( _ERROR, "%s%s%s: no such file\n", ANSI_FG_BWHITE, CONF_FILE_NAME, ANSI_RESET );
		_kill( 1 );
	}

	if ( _f ) fclose( _f );

	conf_set_p_name( DEFAULT_ENV, "DEFAULT_ENV" );
	conf_set_p_name( SRCDIR, "SRCDIR" );
	conf_set_p_name( INCDIR, "INCDIR" );
	conf_set_p_name( OBJDIR, "OBJDIR" );
	conf_set_p_name( DISTDIR, "DISTDIR" );
	conf_set_p_name( DIST_IGNORE, "DIST_IGNORE" );
	conf_set_p_name( CFLAGS, "CFLAGS" );
	conf_set_p_name( LDFLAGS, "LDFLAGS" );
	conf_set_p_name( TARGET, "TARGET" );
	conf_set_p_name( TARGET_ARGS, "TARGET_ARGS" );
	conf_set_p_name( VGR_FLAGS, "VGR_FLAGS" );
	conf_set_p_name( ASCII_TITLE, "ASCII_TITLE" );
	conf_set_p_name( ASCII_VERSION, "ASCII_VERSION" );
	conf_set_p_name( CLOG, "CLOG" );
	conf_set_p_name( SRCS, "SRCS" );

	/* load by default global configuration */
	conf_load_env( CONF_FILE_NAME, "GLOBAL" );

	/* load default environment if it is defined */
	_str = conf_get_pidx( DEFAULT_ENV, 1 );

	if ( _str && strcmptok( argv[ 1 ], "-e,--env,-i,--init,-h,--help", "," ) ) {

		sts = conf_load_env( CONF_FILE_NAME, _str );

		if ( !sts ) {
			_p( _INFO, "using [%s%s%s] as default environment\n", ANSI_FG_YELLOW, _str, ANSI_RESET );
			ascii_decorations();
		} else {
			_p( _ERROR, "default env [%s] was not found\n", _str );
			_kill( 1 );
		}

	}

	for ( i=1; i<argc; i++ ) {

		_arg = argv[ i ];

		if ( !strcmptok( _arg, "-e,--env", "," ) ) {

			if ( i == argc-1 || !strncmp( argv[ i + 1 ], "-", 1 ) ) {
				_p( _ERROR, "env: too few args\n", _arg );
				shift_to( argv, argc, "-e,--env", ",", &i );
			} else {

				sts = conf_load_env( CONF_FILE_NAME, argv[ i + 1 ] );

				if ( !sts ) {
					ascii_decorations();
				} else if ( sts == 2 ) {
					_p( _ERROR, "env: [%s] not found\n", argv[ i + 1 ] );
				}

				if ( sts ) {
					shift_to( argv, argc, "-e,--env", ",", &i );
				} else {
					i++; /* shift next */
				}

			}

		} else if ( !strcmptok( _arg, "-d,--dist", "," ) ) {
			_p( _TASK_START, "DIST START" );
			_dist();
			_p( _TASK_END, "DIST END" );

		} else if ( !strcmptok( _arg, "-m,--cmem", "," ) ) {

			_p( _TASK_START, "MEMCHECK START" );

			conf_check_p( TARGET, _NULL_CHECK, _END_CHECK );

			if ( !exists_cmd( "valgrind" ) ) {
				_p( _ERROR, "valgrind: command not found\n" );
				shift_to( argv, argc, "-e,--env", ",", &i );
			} else {

				strcpy( _cmd, "valgrind " );

				j=1;
				_str = conf_get_pidx( VGR_FLAGS, j );

				while ( _str ) {
					strcat( _cmd, _str );
					strcat( _cmd, " " );
					j++;
					_str = conf_get_pidx( VGR_FLAGS, j );
				}

				strcat( _cmd, " ./" );
				strcat( _cmd, conf_get_pidx( TARGET, 1 ) );
				strcat( _cmd, " " );

				_str = shift_collect_to( argv, argc, "-e,--env", ",", &i );

				if ( _str ) {
					strcat( _cmd, _str );
					free( _str );
					_str = NULL;
				} else {

					j=1;
					_str = conf_get_pidx( TARGET_ARGS, j );

					while ( _str ) {
						strcat( _cmd, _str );
						strcat( _cmd, " " );
						j++;
						_str = conf_get_pidx( TARGET_ARGS, j );
					}

				}

				system( _cmd );

			}

			_p( _TASK_END, "MEMCHECK END" );

		} else if ( !strcmptok( _arg, "-r,--run", "," ) ) {

			_p( _TASK_START, "RUN START" );

			_str = shift_collect_to( argv, argc, "-e,--env", ",", &i );

			_run( _str );

			if ( _str ) {
				free( _str );
				_str = NULL;
			}

			_p( _TASK_END, "RUN END" );

		} else if ( !strcmptok( _arg, "-i,--init", "," ) ) {

			argc=0;
			_ini();

		} else if ( !strcmptok( _arg, "-cl,--clean", "," ) ) {

			_p( _TASK_START, "CLEAN START" );
			_clean();
			_p( _TASK_END, "CLEAN END" );

		} else if ( !strcmptok( _arg, "-c,--comp", "," ) ) {

			_p( _TASK_START, "COMPILATION START" );

			sts = _comp();

			if ( sts <= 0 ) {

				if ( sts == 0 ) {

					if ( _link() == 0 ) {
						_p( _SUCCESS, "link success -> %s%s%s\n", ANSI_FG_BCYAN, (char*)conf_get_pidx( TARGET, 1 ), ANSI_RESET );
					} else {
						_p( _ERROR, "link failed\n" );
					}

				} else {
					/* nothing to be linked */
				}

			} else {
				_p( _ERROR, "compilation failed\n");
			}
			_p( _TASK_END, "COMPILATION END" );

		} else if ( !strcmptok( _arg, "-h,--help", "," ) ) {
			_help( argc, argv );
			argc=0;
		} else {

			_p( _WARN, "unknown option '%s'\n", _arg );

		}

	}

	_kill( 0 );

	return EXIT_SUCCESS;
}

int _rm( const char *path ) {

	int sts;
	pid_t pid;

	if ( !path )
		return 1;

	sts = 1;

	if ( exists_file( path ) || exists_dir( path ) ) {

		pid = fork();
		if ( pid == 0 ) {
			_p( _INFO, "removing: %s%s%s\n", ANSI_FG_BYELLOW, path, ANSI_RESET );
			execl( "/bin/rm", "rm", "-rf" , path, NULL );
		} else {
			waitpid( pid, &sts, 0 );
		}

	}	else {
		_p( _INFO, "already removed: %s%s%s\n", ANSI_FG_BWHITE, path, ANSI_RESET );
	}

	return sts;
}

int _clean() {

	int i;
	char *_obj = conf_get_pidx( OBJDIR, 1 );
	char *_target = conf_get_pidx( TARGET, 1 );
	char **_srcs = conf_get_p( SRCS );
	char file_o[ 128 ] = ""; /* temporary object file name */

	if ( _target ) {
		_rm( _target );
	}

	if ( _srcs ) {

		for (i=1; _srcs[ i ]; i++ ) {
			sprintf( file_o, "%s/%s.o", _obj, _srcs[ i ] );
			_rm( file_o );
		}

	}

	return 0;

}

int _link( ) {

	int q, i, sts;
	char *_obj = conf_get_pidx( OBJDIR, 1 );
	char *_target = conf_get_pidx( TARGET, 1 );
	char **_srcs = conf_get_p( SRCS );
	char **_ldflags = conf_get_p( LDFLAGS );
	char file_o[ 128 ] = ""; /* temporary object file name */
	Argv *_argv;
	char *cmd;

	sts = 0;

	sts+=conf_check_p( SRCS, _NULL_CHECK, _END_CHECK );

	if ( sts )
	 	return 1;

	q = 10;

	if ( _ldflags ) {
		for ( i=0; _ldflags[ i ]; i++, q++ );
	}
	for ( i=0; _srcs[ i ]; i++, q++ );


	_argv = argv_build( q );

	argv_add( _argv, "gcc" );
	argv_add( _argv, "-fdiagnostics-color=always" );

	for ( i=1; _srcs[ i ]; i++ ) {
		sprintf( file_o, "%s/%s.o", _obj, _srcs[ i ] );
		argv_add( _argv, file_o );
	}

	if ( _ldflags ) {
		for ( i=1; _ldflags[ i ]; i++ ) {
			argv_add( _argv, _ldflags[ i ] );
		}
	}

	argv_add( _argv, "-o" );
	argv_add( _argv, _target );

	cmd = argv_strlist( _argv );

	if ( cmd ) {
		sts = system( cmd );
		str_destroy( cmd );
	} else {
		sts = 1;
	}

	argv_free( _argv );

	return sts;
}

void _dist() {

	int i, sts;
	FILE *pf;
	char _v[ 64 ];
	char _dir[ 256 ];
	char _cmd[ 1024 ];
	char _zname[ 256 ];
	char **_ignore;
	char *_ascii_v, *_target, *_dist, *_clog;
	time_t t;
	struct tm tm;

	sts = 0;

	sts += conf_check_p( TARGET, _NULL_CHECK, _END_CHECK );

	if ( sts )
	 	return;

	_cmd[0] = 0;
	_dir[0] = 0;
	_v[0] = 0;

	t = time(NULL);
	tm = *localtime(&t);

	_ignore = conf_get_p( DIST_IGNORE );
	_target = conf_get_pidx( TARGET, 1 );
	_dist = conf_get_pidx( DISTDIR, 1 );
	_clog = conf_get_pidx( CLOG, 1 );

	getcwd( _dir, sizeof( _dir ) );
	basename( _dir, _dir, sizeof( _dir ) );

	if ( _clog ) {

		sprintf( _cmd, "grep -Po -m1 'v[0-9].*' %s", _clog );

		pf = popen( _cmd, "r" );
		while( fgets( _v, sizeof( _v ), pf ) );
		strcln( _v ); /* clean \n */

		pclose( pf );

	}

	if ( _v[0] ) {

		_ascii_v = conf_get_pidx( ASCII_VERSION, 1 );


		if ( exists_cmd( "figlet" ) && exists_file( _ascii_v ) ) {

			_p( _INFO, "updating ASCII file: %s%s%s\n", ANSI_FG_BWHITE, _ascii_v, ANSI_RESET );

			sprintf( _cmd, "figlet %s > %s", _v, _ascii_v );
			system( _cmd );

			printf( "%s", ANSI_FG_BYELLOW );
			print_file( stdout, _ascii_v );
			printf( "%s", ANSI_RESET );

		}

		sprintf( _zname, "%s_%s_%04d%02d%02d.zip", _target, _v, tm.tm_year+1900, tm.tm_mday, tm.tm_mon );
	} else {
		sprintf( _zname, "%s_%04d%02d%02d.zip", _target, tm.tm_year+1900, tm.tm_mday, tm.tm_mon );
	}

	sprintf( _cmd, "zip -r -y -q %s %s", _zname, _dir );

	if ( _ignore ) {

		strcat( _cmd, " -x ");
		for (i=1; _ignore[ i ]; i++ ) {
			strcat( _cmd, _dir );
			strcat( _cmd, "/" );
			strcat( _cmd, _ignore[ i ] );
			strcat( _cmd, " " );
		}
	}

	_p( _INFO, "zipping: %s%s%s ... ", ANSI_FG_BYELLOW, _zname, ANSI_RESET );

	chdir( ".." );

	sts = system( _cmd );

	if ( !sts ) {

		printf("%sDONE%s\n", ANSI_FG_BGREEN, ANSI_RESET );

		/* move zip file to the main project directory */
		sprintf( _cmd, "mv %s %s", _zname, _dir );
		system( _cmd );

		chdir( _dir ); /* go back again to the main project directory */

		if ( exists_dir( _dist ) ) {
			sprintf( _cmd, "mv %s %s", _zname, _dist );
			system( _cmd );
		} else {
			_p( _WARN, "%s was ignored\n", conf_get_p_name( DISTDIR ) );
		}

	} else {
		printf("%sERROR%s\n", ANSI_FG_BRED, ANSI_RESET );
	}

	chdir( _dir );

}

void _ini() {

	FILE *_f;
	char _cmd[ 2048 ];
	char _src[ 128 ] = "./src";
	char _obj[ 128 ] = "./obj";
	char _inc[ 128 ] = "./inc";
	char _dist[ 128 ] = "./out";
	char _ascii_v[ 128 ] = "./.ascii_version";
	char _ascii_t[ 128 ] = "./.ascii_title";
	char _name[ 16 ];
	char _env[ 128 ] = "main_env";
	char _v[ 64 ] = "v0.0.1";
	char _clog[ 128 ] = "CHANGELOG.md";
	char _buff[ 1024 ] = "";

	_f = fopen( CONF_FILE_NAME, "r" );

	if ( _f ) {
		_p( _WARN, "this project is already initialized\n" );
		fclose( _f );
		return;
	}

	_f = fopen( CONF_FILE_NAME, "w" );

	if ( !_f ) {
		_p( _ERROR, "could not initialize configuration file\n" );
		return;
	}

	/* request project name */
	printf("project name [%ld]: ", (long)sizeof( _name ) );
	fgets( _name, sizeof( _name ), stdin );
	strcln( _name );

	_p( _INFO, "building project structure ... \n");

	sprintf( _cmd, "mkdir -p c-target %s %s %s %s", _src, _inc, _obj, _dist );
	system( _cmd );

	_p( _INFO, "generating %s%s%s ... \n", ANSI_FG_YELLOW, CONF_FILE_NAME, ANSI_RESET );

	fprintf( _f, "[GLOBAL]\n" );
	fprintf( _f, "SRCDIR=%s  # source directory\n", _src );
	fprintf( _f, "INCDIR=%s  # include directory\n", _inc );
	fprintf( _f, "OBJDIR=%s  # object directory\n", _obj );
	fprintf( _f, "DISTDIR=%s  # distribution dir\n", _dist );
	fprintf( _f, "CFLAGS=[ -g, -Wall ]  # compilation flags\n" );
	fprintf( _f, "LDFLAGS=[ -lm ]  # link flags\n" );
	fprintf( _f, "DIST_IGNORE=[ %s/*.zip, %s/*.o ]  # files to be ignored when distribuiting\n", _dist, _obj );
	fprintf( _f, "DEFAULT_ENV=%s  # default environment\n", _env );
	fprintf( _f, "CLOG=%s # changelog file\n", _clog );

	if ( exists_cmd( "figlet" ) ) {

		_p( _INFO, "building ASCII decorations ... \n" );

		fprintf( _f, "ASCII_VERSION=%s  # ascii version decoration\n", _ascii_v );
		fprintf( _f, "ASCII_TITLE=%s  # ascii title decoration\n", _ascii_t );

		sprintf( _cmd, "figlet %s > %s", _name, _ascii_t );
		system( _cmd );

		sprintf( _cmd, "figlet %s > %s", _v, _ascii_v );
		system( _cmd );
	}

	fprintf( _f, "\n[%s]\n", _env );
	fprintf( _f, "TARGET=main\n" );
	fprintf( _f, "SRCS=[ main ]  # dependency modules\n" );

	fclose( _f );

	sprintf( _buff, "%s/main.c", _src );
	_f = fopen( _buff, "w" );

	if ( _f ) {

		_p( _INFO, "generating main file %s%s%s ... \n", ANSI_FG_YELLOW, _buff, ANSI_RESET );

		fprintf( _f, "#include <stdlib.h>\n");
		fprintf( _f, "#include <stdio.h>\n\n");

		fprintf( _f, "int main()\n" );
		fprintf( _f, "{\n" );
		fprintf( _f, "	printf(\"let's build awesome code!\\n\");\n" );
		fprintf( _f, "	return 0;\n" );
		fprintf( _f, "}\n" );

		fclose( _f );

	}

	_f = fopen( _clog, "w" );

	if ( _f ) {

		_p( _INFO, "generating changelog %s%s%s ... \n", ANSI_FG_YELLOW, _clog, ANSI_RESET );

		fprintf( _f, "## %s\n", _v );
		fprintf( _f, "### `[OVERALL]`\n");
		fprintf( _f, "- Initial build of the project named '%s'.\n", _name );
		fprintf( _f, "### `[FIXES]`\n");
		fprintf( _f, "- None by the moment ...\n");
		fprintf( _f, "### `[IMPROVEMENTS]`\n");
		fprintf( _f, "- None by the moment ...\n");

		fclose( _f );

	}

	_f = fopen( "./.clang_complete", "w" );

	if ( _f ) {
		_p( _INFO, "generating %s%s%s ... \n", ANSI_FG_YELLOW, ".clang_complete", ANSI_RESET );
		fprintf( _f, "-I%s\n", _inc );
		fclose( _f );
	}

	_p( _SUCCESS, "build success\n" );

}

int _comp() {

	int i, sts;
	int chgs; /* changes */
	bool cit; /* compile it flag */
	long md;
	char 	*_str, /* temporary strin pointer */
				*_src, /* source path */
				*_inc, /* include path */
				*_obj, /* object path */
				*_target, /* target name */
				**_cflags, /* compilation flags */
				**_srcs; /* source modules */
	char file_h[ 128 ], file_c[ 128 ], file_o[ 128 ]; /* temporary file names */
	char _regexp[ 256 ], _cmd[ 1024 ], _flags[ 256 ];

	bool _mod[ 100 ] = {0}; /* (modified) files considered as modified */
	Stack *_dmod; /* (directly_modified) files that were directly modified */

	sts = 0;

	sts += conf_check_p( TARGET, _NULL_CHECK, _END_CHECK );
	sts += conf_check_p( SRCDIR, _NULL_CHECK, _DIR_CHECK, _END_CHECK );
	sts += conf_check_p( OBJDIR, _NULL_CHECK, _DIR_CHECK, _END_CHECK );
	sts += conf_check_p( SRCS, _NULL_CHECK, _END_CHECK );

	_inc = conf_get_pidx( INCDIR, 1 );
	_src = conf_get_pidx( SRCDIR, 1 );
	_obj = conf_get_pidx( OBJDIR, 1 );
	_target = conf_get_pidx( TARGET, 1 );
	_srcs = conf_get_p( SRCS );
	_cflags = conf_get_p( CFLAGS );

	if ( _inc ) {

		if ( !exists_dir( _inc ) ) {
			sts=1;
			_p( _ERROR, "invalid INCDIR: '%s'\n", _inc );
		}

	} else if ( sts == 0 ) {

		/* clone SRCDIR into INCDIR */
		conf_alloc_p( INCDIR, 2 );
		conf_set_p( INCDIR, 0, conf_get_p_name( INCDIR ) );
		conf_set_p( INCDIR, 1, _src );
		_inc = conf_get_pidx( INCDIR, 1 );
		_p( _WARN, "using %s as %s%s%s\n", conf_get_p_name( INCDIR ), ANSI_FG_CYAN, _src, ANSI_RESET );

	}

	if ( sts != 0 )
		return 1;

	if ( _cflags ) { /* concat compilation flags */
		_flags[0] = 0;
		for ( i=1; _cflags[ i ]; i++ ) {
			strcat( _flags, _cflags[ i ] );
			strcat( _flags, " " );
		}

	}

	_dmod = stack_ini(
		(stack_destroy_it)str_destroy,
		(stack_cpy_it)str_clone,
		(stack_cmp_it)strcmp,
		NULL
	);

	chgs = 0;

	/* searh for modified files in source directory */
	for ( i=1; _srcs[ i ]; i++ ) {

		sprintf( file_o, "%s/%s.o", _obj, _srcs[ i ] );
		sprintf( file_c, "%s/%s.c", _src, _srcs[ i ] );
		sprintf( file_h, "%s/%s.h", _inc, _srcs[ i ] );

		if ( !exists_file( file_c ) ) {
			sts=1;
			_p( _ERROR, "no such file: %s%s%s\n", ANSI_FG_BRED, file_c, ANSI_RESET );
			break;
		}

		cit = true;

		if ( exists_file( _target ) ) {

			md = _statmd( _target );

			if ( md > _statmd( file_c ) && md > _statmd( file_h ) ) {
				cit=false;
			}

		} else {

			if ( exists_file( file_o ) ) {
				if ( _statmd( file_o ) > _statmd( file_c ) ) {
					cit=false;
				}
			}
		}

		if ( cit ) {
			chgs++;
			stack_push( _dmod, _srcs[ i ] );
			_mod[ i ] = true;
		}

	}

	while ( !stack_isEmpty( _dmod ) ) {

		_str = stack_pop( _dmod );

		for (i=1; _srcs[ i ]; i++ ) {

			if ( _mod[ i ] == true ) continue;

			sprintf( file_c, "%s/%s.c", _src, _srcs[ i ] );
			sprintf( file_h, "%s/%s.h", _inc, _srcs[ i ] );

			sprintf( _regexp, "'#include[ ]{1,}\"%s.h\"'", _str );

			if ( exists_file( file_c ) ) {
				sprintf( _cmd, "grep -Po -m1 %s %s > /dev/null", _regexp, file_c );
				if ( !system( _cmd ) ) {
					_mod[ i ] = true;
				}
			}

			if ( exists_file( file_h ) ) {
				sprintf( _cmd, "grep -Po -m1 %s %s > /dev/null", _regexp, file_h );
				if ( !system( _cmd ) ) {
					_mod[ i ] = true;
				}
			}

		}

		str_sdestroy( &_str );
	}

	 /* compile all modified files */
	for ( i=1; _srcs[ i ]; i++ ) {

		sprintf( file_o, "%s/%s.o", _obj, _srcs[ i ] );
		sprintf( file_c, "%s/%s.c", _src, _srcs[ i ] );
		sprintf( file_h, "%s/%s.h", _inc, _srcs[ i ] );

		if ( _mod[ i ] ) {

			_p( _INFO, "compiling: %s%s%s -> %s\n", ANSI_FG_BYELLOW, file_c, ANSI_RESET, file_o );

			sprintf( _cmd, "gcc -fdiagnostics-color=always %s -I %s -c %s -o %s ", _flags, _inc, file_c, file_o );
			sts = system( _cmd );

		} else {
			_p( _INFO, "up to date: %s%s%s -> %s\n", ANSI_FG_BGREEN, file_c, ANSI_RESET, file_o );
		}

	}

	if ( chgs == 0 && exists_file( _target ) ) {
		sts = -1;
	}

	stack_destroy( _dmod );

	return sts;

}

void _help( int argc, char *argv[] ) {

	Ui *ui;
	char *arg;
	ui = ui_init( 80, 22 );

	ui_new_box( ui, HELP_HEAD, 0, 0, 80, 1 );
	ui_new_box( ui, HELP_BODY, 0, 1, 80, 21 );
	ui_box_put( ui, HELP_HEAD, "@{1;33}C-target@{0} by @{1;36}@lromeraj@{0} @{1}%s@{0}", _VERSION );

	if ( argc > 2 ) {

		arg = argv[ 2 ];

		ui_box_put( ui, HELP_BODY, "Help me with --> @{1;%d}%s@{0}\n", FG_PURPLE, arg );

		if ( !strcmptok( arg, "-i,--init", "," ) ) {
			ui_box_put( ui, HELP_BODY, "This flag says to C-target that you want to build a new @{1;3}C project@{0}.\n" );
			ui_box_put( ui, HELP_BODY, "C-target will start by requesting you the project name.\n" );
			ui_box_put( ui, HELP_BODY, "After this C-target will build a basic compilable project.\n" );
			ui_box_put( ui, HELP_BODY, "A default @{33}target.conf@{0} file will be created, modify it at your own.\n");
			ui_box_put( ui, HELP_BODY, "@{3}NOTE@{0}: if C-target detects a current working directory, it will reject the initialization.");
		} else if ( !strcmptok( arg, "-r,--run", "," ) ) {
			ui_box_put( ui, HELP_BODY, "This flag says to C-target that you want to execute the target of some environment.\n" );
			ui_box_put( ui, HELP_BODY, "C-target will look for the next @{3}-e,--env@{0} and will collect every argument between them. ");
			ui_box_put( ui, HELP_BODY, "If @{1}there is no argument@{0} C-target will use the default arguments @{1}defined in the configuration file@{0}.\n");
			ui_box_put( ui, HELP_BODY, "If the executable does not exists C-target will alert about it" );
		} else if ( !strcmptok( arg, "-d,--dist", "," ) ) {
			ui_box_put( ui, HELP_BODY, "This flag says to C-target that you want to distribute some environment.\n" );
			ui_box_put( ui, HELP_BODY, "C-target will look for the last version inside @{2}%s@{0} file.\n", conf_get_p_name( CLOG ) );
			ui_box_put( ui, HELP_BODY, "\
If you have a version definition following this regex @{2}v[0-9].*@{0}, \
C-target will read that version, and it will update your @{2}%s@{0} decoration file.\n", conf_get_p_name( ASCII_VERSION ) );
			ui_box_put( ui, HELP_BODY, "(@{33}figlet@{0} must be installed and accessible from your @{2}$PATH@{0})\n" );
			ui_box_put( ui, HELP_BODY, "If C-target finds a version inside your @{2}%s@{0} file it will generate a zip named as `@{2}<%s>@{0}_@{2}<version>@{0}_@{2}<date>@{0}.zip`\n", conf_get_p_name( CLOG ), conf_get_p_name( TARGET ) );
			ui_box_put( ui, HELP_BODY, "If C-target can't find a version, the zip will be named as\n`@{2}<%s>@{0}_@{2}<date>@{0}.zip`\n", conf_get_p_name( TARGET ) );
		} else if ( !strcmptok( arg, "-c,--comp", "," ) ) {
			ui_box_put( ui, HELP_BODY, "This flag says to C-target that you want to compile an environment.\n" );
			ui_box_put( ui, HELP_BODY, "(@{33}gcc@{0} must be installed and accessible from your @{2}$PATH@{0})\n" );
			ui_box_put( ui, HELP_BODY, "C-target will read your @{2}SRCS@{0} modules, if some of them needs to be compiled it will.\n" );
			ui_box_put( ui, HELP_BODY, "If you have modified a file that is included in other files, these files will be compiled too.\n" );
		} else {
			ui_box_put( ui, HELP_BODY, "There are are no entries for '@{1;%d}%s@{0}'\n", FG_RED, arg );
			ui_box_put( ui, HELP_BODY, "Try using other key words ..." );
		}

	} else {

		ui_box_put( ui, HELP_BODY, "  @{1}-r, --run@{0}     Executes the target of the environment\n" );
		ui_box_put( ui, HELP_BODY, "  @{1}-i, --init@{0}    Initializes a new project\n" );
		ui_box_put( ui, HELP_BODY, "  @{1}-e, --env@{0}     Selects an environment\n" );
		ui_box_put( ui, HELP_BODY, "  @{1}-c, --comp@{0}    Compiles an environment\n" );
		ui_box_put( ui, HELP_BODY, "  @{1}-cl, --clean@{0}  Cleans an environment\n" );
		ui_box_put( ui, HELP_BODY, "  @{1}-m, --cmem@{0}    Checks memory usage using valgrind\n" );
		ui_box_put( ui, HELP_BODY, "  @{1}-d, --dist@{0}    Distributes an environment\n" );
		ui_box_put( ui, HELP_BODY, "  @{1}-h, --help@{0}    Shows this helpdesk\n" );

		ui_box_put( ui, HELP_BODY, "\nSee @{3;36}https://github.com/lromeraj/c-target@{0} for more information\n" );

	}


	ui_dump_box( ui, HELP_HEAD );
	ui_dump_box( ui, HELP_BODY );

	ui_draw( stdout, ui );

	ui_destroy( ui );

}


void ascii_decorations() {

	char *_ascii_t, *_ascii_v;

	_ascii_t = conf_get_pidx( ASCII_TITLE, 1 );
	_ascii_v = conf_get_pidx( ASCII_VERSION, 1 );

	/* print ascii decorations */
	if ( _ascii_t ) {
		printf("%s", ANSI_FG_BPURPLE );
		print_file( stdout, _ascii_t );
		printf("%s", ANSI_RESET );
	}

	if ( _ascii_v ) {
		printf("%s", ANSI_FG_BYELLOW );
		print_file( stdout, _ascii_v );
		printf("%s", ANSI_RESET );
	}

}

void _kill( int errc ) {

	/* clean up config data */
	conf_free_all();

	exit( errc );
}
