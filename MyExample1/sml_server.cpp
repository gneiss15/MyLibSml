//###############################
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h> 
#include <unistd.h>
#include <math.h>

#include <sml/sml_file.h>
#include <sml/sml_transport.h>
#include <sml/sml_value.h>

#include "unit.h"

#if 0
#include <fcntl.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>
#include <termios.h>
#include <stdbool.h>
#include <sys/ioctl.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

// globals
bool sflag = false;	// flag to process only a single data stream
bool vflag = false;	// verbose flag

void Error( const char * msg, ... )
 {
  va_list va;
  va_start( va, msg );
  vfprintf( stderr, msg, va );
  va_end( va );
  exit( 1 );
 }

int OpenPort( const char * serverStr, const char * portStr )
 {
  int fd;
  int portNo;
  struct sockaddr_in servAddr;    		// server address
  struct hostent *hp;				// host information

  fd = socket( AF_INET, SOCK_STREAM, 0 );
  if( fd < 0 )
    Error( "ERROR opening socket" );

  portNo = atoi( portStr );

  // fill in the server's address and data
  memset( (void*)&servAddr, 0, sizeof( servAddr ) );
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons( portNo );
  
  hp = gethostbyname( serverStr );
  if( !hp )
   {
    close( fd );
    Error( "could not obtain address of %s\n", serverStr );
   }

  // put the host's address into the server address structure
  memcpy( (void*)&servAddr.sin_addr, hp->h_addr, hp->h_length );

  if( connect( fd, (struct sockaddr *)&servAddr, sizeof( servAddr ) ) < 0 )
   {
    close( fd );
    Error( "Connecton to %s failed\n", serverStr );
   }

  return fd;
 }

void Receiver(unsigned char *buffer, size_t buffer_len)
 {
  fprintf( stderr, "Receiver: buffer_len: %u\n", buffer_len - 16 );

	int i;
	// the buffer contains the whole message, with transport escape sequences.
	// these escape sequences are stripped here.
	sml_file *file = sml_file_parse(buffer + 8, buffer_len - 16);
	// the sml file is parsed now

	// this prints some information about the file
	if (vflag)
		sml_file_print(file);

	// read here some values ...
	if (vflag)
		printf("OBIS data\n");
	for (i = 0; i < file->messages_len; i++) {
		sml_message *message = file->messages[i];
		if (*message->message_body->tag == SML_MESSAGE_GET_LIST_RESPONSE) {
			sml_list *entry;
			sml_get_list_response *body;
			body = (sml_get_list_response *) message->message_body->data;
			for (entry = body->val_list; entry != NULL; entry = entry->next) {
				if (!entry->value) { // do not crash on null value
					fprintf(stderr, "Error in data stream. entry->value should not be NULL. Skipping this.\n");
					continue;
				}
				if (entry->value->type == SML_TYPE_OCTET_STRING) {
					char *str;
					printf("%d-%d:%d.%d.%d*%d#%s#\n",
						entry->obj_name->str[0], entry->obj_name->str[1],
						entry->obj_name->str[2], entry->obj_name->str[3],
						entry->obj_name->str[4], entry->obj_name->str[5],
						sml_value_to_strhex(entry->value, &str, true));
					free(str);
				} else if (entry->value->type == SML_TYPE_BOOLEAN) {
					printf("%d-%d:%d.%d.%d*%d#%s#\n",
						entry->obj_name->str[0], entry->obj_name->str[1],
						entry->obj_name->str[2], entry->obj_name->str[3],
						entry->obj_name->str[4], entry->obj_name->str[5],
						entry->value->data.boolean ? "true" : "false");
				} else if (((entry->value->type & SML_TYPE_FIELD) == SML_TYPE_INTEGER) ||
						((entry->value->type & SML_TYPE_FIELD) == SML_TYPE_UNSIGNED)) {
					double value = sml_value_to_double(entry->value);
					int scaler = (entry->scaler) ? *entry->scaler : 0;
					int prec = -scaler;
					if (prec < 0)
						prec = 0;
					value = value * pow(10, scaler);
					printf("%d-%d:%d.%d.%d*%d#%.*f#",
						entry->obj_name->str[0], entry->obj_name->str[1],
						entry->obj_name->str[2], entry->obj_name->str[3],
						entry->obj_name->str[4], entry->obj_name->str[5], prec, value);
					const char *unit = NULL;
					if (entry->unit &&  // do not crash on null (unit is optional)
						(unit = dlms_get_unit((unsigned char) *entry->unit)) != NULL)
						printf("%s", unit);
					printf("\n");
					// flush the stdout puffer, that pipes work without waiting
					fflush(stdout);
				}
			}
			if (sflag)
				exit(0); // processed first message - exit
		}
	}

	// free the malloc'd memory
	sml_file_free(file);
 }

int main( int argc, char *argv[] )
 {
  int c;
  while( ( c = getopt( argc, argv, "+hsv" ) ) != -1 )
   {
    switch( c )
     {
      case 'h':
        printf( "usage: %s [-h][-s][-v] host port\n", argv[ 0 ] );
        printf( "host - ip or name of host to connect to\n" );
        printf( "port - port to connect to\n" );
        printf( "-h - help\n" );
        printf( "-s - process only one data stream (single)\n");
        printf( "-v - verbose\n" );
        exit(0);
      case 's':
        sflag = true;
        break;
      case 'v':
        vflag = true;
        break;
      case '?':		// get a not specified switch, error message is printed by getopt()
        printf( "Use %s -h for help.\n", argv[ 0 ] );
        exit( 1 );
      default:
        break;
     }
   }
  
  const char * serverStr = "esp-link";
  const char * portStr = "23";

  if( argc - optind > 0 )
   {
    serverStr = argv[ optind ];
    fprintf( stderr, "Using server: %s\n", serverStr );
   }
   else
    fprintf( stderr, "Using standard server: %s\n", serverStr );
  if( argc - optind > 1 )
   {
    portStr = argv[ optind + 1 ];
    fprintf( stderr, "Using port: %s\n", portStr );
   }
   else
    fprintf( stderr, "Using standard port: %s\n", portStr );

  int fd = OpenPort( serverStr, portStr );
  sml_transport_listen( fd, &Receiver );
  close( fd );

  return 0;
 }

