//***********************************************************************
//									*
// Filename:	ByteReader.cpp						*
//									*
//***********************************************************************

//==============================================================================
// Compiler-Options & Header-Files
//==============================================================================

#include "ByteReader.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h> 
#include <errno.h>

#include <netinet/in.h>

#include <sys/types.h>

//==============================================================================
// Internal Functions
//==============================================================================

static int OpenPort( const char * serverStr, const char * portStr )
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

//==============================================================================
// TByteReader Methods
//==============================================================================

TByteReader::TByteReader( const char * serverStr, const char * portStr )
: Fd( OpenPort( serverStr, portStr ) )
 {
  FD_ZERO( &FdSet );
  FD_SET( Fd, &FdSet );
 }

TByteReader::~TByteReader(void)
 {
  if( Fd >= 0 )
    close( Fd );
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

//==============================================================================
// End of ByteReader.cpp
//==============================================================================


