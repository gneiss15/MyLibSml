//***********************************************************************
//									*
// Filename:	SmlBuffer.h						*
//									*
//***********************************************************************

#ifndef SmlBufferH	// sentry
#define SmlBufferH

//==============================================================================
// Compiler-Options & Header-Files
//==============================================================================

#include "SmlCommon.h"
#include "SmlDecoder.h"

//==============================================================================
// TSmlBuffer
//==============================================================================

class TSmlBuffer
 {
 private:
  TSmlDecoder * Decoder;
  uint8_t	Buf[ 8096 ];		//!
  uint8_t *	BufP;
  uint8_t *	BufEnd;
  uint8_t	State;
  uint_t	FramesOk;
  uint_t	FramesNOk;

  uint16_t	Padding;
  uint16_t	CrcMsg;

 public:
  		TSmlBuffer( TSmlDecoder * decoder );
  void		Put( uint8_t c );
 };

//==============================================================================
// End of SmlBuffer.h
//==============================================================================
#endif	// sentry

