
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "mbug.h"

//-------------------------------------------------------------------------------

const char* usage =
"                                                                              \n"
"LSMBUG command line tool: List available mbug devices.                        \n"
"                                                                              \n"
"Usage:   lsmbug  [cmd]                                                        \n"
"                                                                              \n"
"    For convenience, commands have multiple abbrevations.  Commands can       \n"
"    be prepended by on or multiple '-' (unix option style),  '/' (windows     \n"
"    option style), or nothing.  Some commands take a parameter. The parameter \n"
"    has to be  separeted by a ':' or '=' (NO whitespaces!).                   \n"
"                                                                              \n"
"Commands:                                                                     \n"
"                                                                              \n"
"    t=<type>      Select device type to list.                                 \n"
"    type=         Default: all types.                                         \n"
"                                                                              \n"
"    h             Display this usage info.                                    \n"
"    help                                                                      \n";

//--------------------------------------------------------------------------------

int device_type = 0;
enum Action { List, Help } action = List;

//---------------------------------------------------------------

void errorf( const char *format, ... )
{
   va_list args;
   va_start( args, format );
   vfprintf( stderr, format, args );
   va_end( args );
   exit(1);
}

int str_in( const char* str, ... )
{
	va_list args;
	int match = 0;
	va_start(args, str);
	while(1)
	{
		const char *next = va_arg(args, const char*);
		if (next==0 || *next==0) break;
		if (match = !strcmp(str,next)) break;
	}
	va_end(args);
	return match;
}

char *str_toupper( char *str )
{
	char *p = str;
	while (*p) {
		*p = toupper(*p);
		p++;
	}
	return str;
}

long str_to_int( char* str )
{
	char *endp = 0;
	long val = -1;
	if (str==0 || *str=='\0') return -1;
	errno = 0;
	val = strtoul(str, &endp, 10 );
	if (errno || *endp!='\0') return -1;
	return val;
}

void parse_type( char* str )
{
	str_toupper( str );
	if (strncmp(str,"MBUG-",5) == 0)
		device_type = str_to_int(str+5);
	else device_type = str_to_int(str);
	if (device_type < 0) errorf( "#### Invalid device type: %s", str );
}

void parse_options( int argc, char* argv[] )
{
	int i;
	for (i=1; i<argc; i++)
	{
		char* ap = argv[i];
		while ( *ap=='-' || *ap=='/' )  ap++;
		ap = strtok( ap, ":= ");

		if (str_in( ap, "l", "list", 0 ))
			action = List;
		else if (str_in( ap, "h", "help", 0 ))
			action = Help;
		else if (str_in( ap, "t", "type", 0 ))
			parse_type( strtok( 0, "" ) );
		else
			errorf( "#### Unknown command: %s", argv[i]);
	}
}


//---------------------------------------------------------------

int main( int argc, char* argv[] )
{
	parse_options( argc, argv );
	
	if (action==List)
		{
			// List all attached devices
			int i;
			mbug_device_list list = mbug_get_device_list(device_type);
			for (i=0; list[i]!=0; i++ ) {
				puts(list[i]);
			}
			return 0;
		}

	if (action==Help)
		{
			puts(usage);
			return 0;
		}

	return 0;
}

