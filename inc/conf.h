#ifndef CONF_H
#define CONF_H

#define MAX_CONFIG 50
#define MAX_CHECKS 10

#define CONF_FILE_NAME "target.conf"

typedef enum {
	NONE,
	TARGET,
	TARGET_ARGS,
	SRCDIR,
	INCDIR,
	OBJDIR,
	DISTDIR,
	DIST_IGNORE,
	SRCS,
	CLOG,
	CFLAGS,
	LDFLAGS,
	VGR_FLAGS,
	ASCII_TITLE,
	ASCII_VERSION,
	DEFAULT_ENV
} Conf_p;

typedef enum {
	_NULL_CHECK=1,
	_FILE_CHECK,
	_DIR_CHECK,
	_END_CHECK
} Conf_check;

void conf_alloc_p( Conf_p p, int n );
void conf_free_p( Conf_p p );
char* conf_get_pidx( Conf_p p, int idx );
char** conf_get_p( Conf_p p );
void conf_set_p( Conf_p p, int idx, const char *str );
char* conf_get_p_name( Conf_p p );
int conf_check_p( Conf_p, ... );
void conf_free_all();
void conf_upd( char **__data, int size );
Conf_p conf_get_p_by_name( const char *_name );
char *conf_get_env();
void conf_set_env( const char *env );
int conf_load_env( const char *f_name, const char *env );
void conf_set_p_name( Conf_p p, const char *str );


#endif
