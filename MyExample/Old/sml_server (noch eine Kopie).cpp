//###############################
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h> 
#include <unistd.h>
#include <math.h>
#include <errno.h> // for errno

#include <sml/sml_file.h>
#include <sml/sml_transport.h>
#include <sml/sml_value.h>

#include "unit.h"

typedef long unsigned int uint_t;

#if 0
#include <fcntl.h>
#include <getopt.h>
#include <ctype.h>
#include <termios.h>
#include <stdbool.h>
#include <sys/ioctl.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
//##############################################
static uint16_t const crctab[ 256 ] =
 {
  //   0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F
  0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf, 0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
  0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e, 0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
  0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd, 0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
  0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c, 0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
  0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb, 0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
  0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a, 0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
  0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9, 0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
  0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738, 0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
  0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7, 0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
  0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036, 0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
  0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5, 0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
  0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134, 0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
  0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3, 0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
  0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232, 0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
  0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1, 0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
  0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330, 0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
 };
static uint16_t _crc16( uint16_t crc, uint8_t const * p, uint8_t const * pEnd )
 {
  while( p < pEnd )
    crc = ( crc >> 8 ) ^ crctab[ ( crc ^ *p++ ) & 0xff ];
  return crc;
 }
uint16_t SmlCrc16( uint8_t const * p, uint8_t const * pEnd )
 {
  uint16_t crc = 0xffff;
  while( p < pEnd )
    crc = ( crc >> 8 ) ^ crctab[ ( crc ^ *p++ ) & 0xff ];
  crc ^= 0xffff;
  return ( ( crc & 0xff ) << 8 ) | ( ( crc & 0xff00 ) >> 8 );
 }
uint16_t SmlCrc16_2( uint8_t const * p, uint8_t const * pEnd )
 {
  uint16_t crc = 0x91dc;
  while( p < pEnd )
    crc = ( crc >> 8 ) ^ crctab[ ( crc ^ *p++ ) & 0xff ];
  crc ^= 0xffff;
  return ( ( crc & 0xff ) << 8 ) | ( ( crc & 0xff00 ) >> 8 );
 }
//##############################################
// globals
bool sflag = false;	// flag to process only a single data stream
bool vflag = false;	// verbose flag
//##############################################
void Error( const char * msg, ... )
 {
  va_list va;
  va_start( va, msg );
  vfprintf( stderr, msg, va );
  va_end( va );
  exit( 1 );
 }
//##############################################
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
//##############################################
class TByteReader
 {
 private:
  int		Fd;
  fd_set	FdSet;
 public:
  		TByteReader( int fd );
  uint8_t	Read(void);
 };
TByteReader::TByteReader( int fd )
: Fd( fd )
 {
  FD_ZERO( &FdSet );
  FD_SET( Fd, &FdSet );
 }
uint8_t TByteReader::Read(void)
 {
  uint8_t res;
  ssize_t r = 0;
  while( r <= 0 )
   {
    select( Fd + 1, &FdSet, 0, 0, 0 );
    if( FD_ISSET( Fd, &FdSet ) )
     {
      r = read( Fd, &res, 1 );
      if( r == 0 )
        Error( "TByteReader:Read: EOF\n" );
      if( r < 0 )
       {
        if( errno == EINTR || errno == EAGAIN )
          continue; // should be ignored
        Error( "TByteReader:Read: read error\n" );
       }
     }
   }
  return res;
 }
//##############################################
void PrintObis( sml_list * entry )
 {
  uint8_t * hStr = entry->obj_name->str;
  printf( "%d-%d:%d.%d.%d*%d#", hStr[ 0 ], hStr[ 1 ], hStr[ 2 ], hStr[ 3 ], hStr[ 4 ], hStr[ 5 ] );
 }
void PrintOctedStr( sml_list * entry )
 {
  PrintObis( entry );
  char * str;
  sml_value_to_strhex( entry->value, &str, true );
  printf( "%s#\n", str );
  free( str );
 }
void PrintBool( sml_list * entry )
 {
  PrintObis( entry );
  printf( "%s#\n", entry->value->data.boolean ? "true" : "false" );
 }
void PrintInteger( sml_list * entry )
 {
  double value = sml_value_to_double( entry->value );
  int scaler = ( entry->scaler ) ? *entry->scaler : 0;
  int prec = -scaler;
  if( prec < 0 )
    prec = 0;
  value = value * pow( 10, scaler );
  PrintObis( entry );
  printf( "%.*f#", prec, value );
  const char * unit = NULL;
  if( entry->unit &&  // do not crash on null (unit is optional)
      ( unit = dlms_get_unit( (unsigned char) *entry->unit ) ) != NULL )
    printf( "%s", unit );
  printf( "\n" );
  // flush the stdout puffer, that pipes work without waiting
  fflush( stdout );
 }
//##############################################
//#define OldVers
#ifdef OldVers
void Receiver( uint8_t * buffer, size_t buffer_len )
 {
  sml_file * file = sml_file_parse( buffer, buffer_len );
  // the sml file is parsed now
 #else
void Decoder( sml_file * file )
 {
#endif
  // this prints some information about the file
  if( vflag )
    sml_file_print(file);

  // read here some values ...
  if( vflag )
    printf( "OBIS data\n" );
  for( int i = 0;  i < file->messages_len; i++ )
   {
    sml_message * message = file->messages[ i ];
    if( *message->message_body->tag == SML_MESSAGE_OPEN_RESPONSE )
     {
      sml_open_response * open_response = (sml_open_response *)message->message_body->data;
      sml_time * time = open_response->ref_time;
      printf( "ref_time   :%u#\n",  *( time->data.sec_index ) );
     }
     else if( *message->message_body->tag == SML_MESSAGE_GET_LIST_RESPONSE )
     {
      sml_list * entry;
      sml_get_list_response * body;
      body = (sml_get_list_response *)message->message_body->data;
      for( entry = body->val_list;  entry != NULL;  entry = entry->next )
       {
        if( !entry->value )	// do not crash on null value
         {
          fprintf( stderr, "Error in data stream. entry->value should not be NULL. Skipping this.\n" );
          continue;
         }
        if( entry->value->type == SML_TYPE_OCTET_STRING )
          PrintOctedStr( entry );
         else if( entry->value->type == SML_TYPE_BOOLEAN )
          PrintBool( entry );
         else if( ( ( entry->value->type & SML_TYPE_FIELD ) == SML_TYPE_INTEGER ) ||
                    ( ( entry->value->type & SML_TYPE_FIELD ) == SML_TYPE_UNSIGNED ) )
          PrintInteger( entry );
       }
      if( sflag )
        exit( 0 ); // processed first message - exit
     }
   }

  // free the malloc'd memory
  sml_file_free( file );
 }
//##############################################
class TSmlBuffer
 {
 private:
  uint8_t	Buf[ 8096 ];		//!
  uint8_t *	BufP;
  uint8_t *	BufEnd;
  uint8_t	State;
  uint_t	FramesOk;
  uint_t	FramesNOk;

  uint16_t	Padding;
  uint16_t	CrcMsg;

 public:
  		TSmlBuffer(void);
  void		Put( uint8_t c );
 };
TSmlBuffer::TSmlBuffer(void)
: BufP( Buf )
, BufEnd( Buf + sizeof( Buf ) )
, State( 0 )
, FramesOk( 0 )
, FramesNOk( 0 )
 {
 }
void TSmlBuffer::Put( uint8_t c )
 {
  uint16_t crcCalc;
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
  #pragma GCC diagnostic ignored "-Wimplicit-fallthrough="

  switch( State )
   {
    // start sequence part1 found, search for start sequence part2 (0x01, 0x01, 0x01, 0x01)
    case  4:
    case  5:
    case  6:
    case  7:
      if( c == 0x01 )
       {
        ++State;
        break;
       }
      // fall thru (Do as if State == 0)

    // search for start sequence part1 (0x1b, 0x1b, 0x1b, 0x1b)
    case  0:
      State = 0;
      BufP = Buf;
      // fall thru
    case  1:
    case  2:
    case  3:
      if( c != 0x1b )
       {
        State = 0;				// back to Start
        return;
       }
      ++State;
      break;

    // start sequence found, inside frame, watch for esc sequence (0x1b, 0x1b, 0x1b, 0x1b, 0x1a)
    case  8:
    case  9:
    case 10:
    case 11:
      if( c == 0x1b )
        ++State;
       else
        State = 8;				// back to "search for esc"
      break;
    case 12:
      if( c == 0x1a )
        ++State;				//!! Check that it is not (0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1a)
       else
        State = ( c == 0x1b ) ? 9 : 8;		// back to "search for esc"
      break;
    case 13:					// PADDING
      Padding = c;
      ++State;
      break;
    case 14:					// CRC MSB
      CrcMsg = c;
      CrcMsg <<= 8;				// Shift MSB into place
      ++State;
      break;
    case 15:					// CRC LSB
      *BufP++ = c;
      CrcMsg |= c;				// or in LSB
      crcCalc = SmlCrc16( Buf, BufP - 2 );
      if( CrcMsg != crcCalc )
       {
        ++FramesNOk;
        fprintf( stderr, "TSmlBuffer:Put: CRC-Error: CrcMsg: %u, crcCalc: %u, FramesOk: %lu, FramesNOk: %lu\n", CrcMsg, crcCalc, FramesOk, FramesNOk );
       }
       else
       {
        ++FramesOk;
        uint8_t * buffer = Buf + 8;
        size_t buffer_len = BufP - buffer - 8 - (size_t)Padding;
        #ifdef OldVers
          Receiver( buffer, buffer_len );
         #else
          sml_file * file = sml_file_parse( buffer, buffer_len );
          Decoder( file );
          //Decoder( sml_file_parse( buffer, buffer_len ) );
        #endif
       }
      State = 0;				// back to Start
      return;
   }
  #pragma GCC diagnostic pop
  if( BufP >= BufEnd )
    Error( "TSmlBuffer::Put: BufP >= BufEnd\n" );
  *BufP++ = c;					// store bytes
 }
//##############################################
int main2( int, char ** )
 {
  static uint8_t const msg[] =
   {
    0x1b, 0x1b, 0x1b, 0x1b, 0x01, 0x01, 0x01, 0x01
   };
  
  printf( "_crc16( 0xffff, msg, msg + sizeof( msg ) ): %x\n", _crc16( 0xffff, msg, msg + sizeof( msg ) ) );
   
  return 0;
 }
//##############################################
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
    serverStr = argv[ optind ];
  if( argc - optind > 1 )
    portStr = argv[ optind + 1 ];

  int fd = OpenPort( serverStr, portStr );
  TByteReader r( fd );
  TSmlBuffer b;
  while( true )
    b.Put( r.Read() );
  close( fd );

  return 0;
 }

