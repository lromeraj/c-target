#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdbool.h>

#define ANSI_RESET   				"\033[0m"
#define ANSI_FG_RED     		"\033[31m"
#define ANSI_FG_GREEN   		"\033[32m"
#define ANSI_FG_YELLOW  		"\033[33m"
#define ANSI_FG_BLUE    		"\033[34m"
#define ANSI_FG_PURPLE 			"\033[35m"
#define ANSI_FG_WHITE 			"\033[97m"
#define ANSI_FG_CYAN    		"\033[36m"

#define ANSI_FG_BRED     		"\033[1;31m"
#define ANSI_FG_BWHITE 			"\033[1;97m"
#define ANSI_FG_BGREEN   		"\033[1;32m"
#define ANSI_FG_BYELLOW  		"\033[1;33m"
#define ANSI_FG_BBLUE    		"\033[1;34m"
#define ANSI_FG_BPURPLE 		"\033[1;35m"
#define ANSI_FG_BCYAN    		"\033[1;36m"

typedef enum {
	_ERROR,
	_WARN,
	_SUCCESS,
	_INFO,
	_NOTE,
	_TASK_START,
	_TASK_END
} PrintType;


void basename( char *dest, char *src, size_t size );
void buildSrcs( char **srcs );
void _p( PrintType type, const char *frmt, ... );
bool exists_dir( const char *path );
bool exists_file( const char *f_path );
bool exists_cmd( const char *_cmd );
bool find_word( char *str, char *word );
char* shift_collect_to(	char **argv, int max, char *strl,	const char *del, int *to );
void shift_to( char **argv, int max, char *strl,	const char *del, int *to );
int print_file( FILE *stream, const char *f_name );

#endif
