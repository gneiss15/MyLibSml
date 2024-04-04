//###############################

#include "Decoder.h"

#include <stdio.h>
#include <unistd.h>

#include "Sml.h"
#include "ByteReader.h"

//##############################################
// main
//##############################################
int main( int argc, char *argv[] )
 {
  TDecoder Decoder;
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
        Decoder.SFlag = true;
        break;
      case 'v':
        Decoder.VFlag = true;
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
    serverStr = argv[ optind ];
  if( argc - optind > 1 )
    portStr = argv[ optind + 1 ];

  TByteReader r( serverStr, portStr );
  TSmlBuffer b( &Decoder );
  while( true )
    b.Put( r.Read() );

  return 0;
 }

