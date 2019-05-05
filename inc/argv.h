#ifndef ARGV_H
#define ARGV_H

typedef struct _Argv Argv;

Argv* argv_build();
void argv_add( Argv *argv, const char *str );
char** argv_get_list( Argv *argv );
char* argv_strlist( Argv *argv );
void argv_free( Argv *argv );
void argv_clean( Argv *argv );

#endif
