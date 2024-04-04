//***********************************************************************
//									*
// Filename:	SmlBuffer.cpp						*
//									*
//***********************************************************************

//==============================================================================
// Compiler-Options & Header-Files
//==============================================================================

#include "SmlBuffer.h"
#include "SmlCrc16.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <sml/sml_file.h>

//==============================================================================
// TSmlBuffer
//==============================================================================

TSmlBuffer::TSmlBuffer( TSmlDecoder * decoder )
: Decoder( decoder )
, BufP( Buf )
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
        return;
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
      return;

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
      CrcMsg = uint16_t( c ) << 8;		// Shift MSB into place
      ++State;
      break;
    case 15:					// CRC LSB
      *BufP++ = c;
      CrcMsg |= c;				// or in LSB
      // crc = 0x91dc		Start value as if StartSeq (0x1b, 0x1b, 0x1b, 0x1b, 0x01, 0x01, 0x01, 0x01) alread calced
      crcCalc = SmlCrc16( Buf, BufP - 2, 0x91dc );
      if( CrcMsg != crcCalc )
       {
        ++FramesNOk;
        fprintf( stderr, "TSmlBuffer:Put: CRC-Error: CrcMsg: %u, crcCalc: %u, FramesOk: %lu, FramesNOk: %lu\n", CrcMsg, crcCalc, FramesOk, FramesNOk );
       }
       else
       {
        ++FramesOk;
        size_t buffer_len = BufP - Buf - 8 - (size_t)Padding;
        sml_file * file = sml_file_parse( Buf, buffer_len );
        Decoder->Decode( file );
       }
      State = 0;				// back to Start
      return;
   }
  #pragma GCC diagnostic pop
  if( BufP >= BufEnd )
    Error( "TSmlBuffer::Put: BufP >= BufEnd\n" );
  *BufP++ = c;					// store bytes
 }

//==============================================================================
// End of SmlBuffer.cpp
//==============================================================================

