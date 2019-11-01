#ifndef ARGV_H
#define ARGV_H

#include <stdlib.h>

typedef struct _Argv Argv;

Argv* argv_build();
void argv_add( Argv *argv, const char *str );
void argv_strlist( Argv *argv, char *dest, size_t len );
char *argv_get_idx( Argv *argv, int idx );
void argv_free( Argv *argv );
void argv_clean( Argv *argv );
int argv_n( Argv *argv );

#endif
