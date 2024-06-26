//***********************************************************************
//									*
// Filename:	SmlByteReader.h						*
//									*
//***********************************************************************

#ifndef SmlByteReaderH	// sentry
#define SmlByteReaderH

//==============================================================================
// Compiler-Options & Header-Files
//==============================================================================

#include "SmlCommon.h"

//==============================================================================
// TSmlByteReader
//==============================================================================

class TSmlByteReader
 {
 public:
  			TSmlByteReader(void)	{}
  virtual uint8_t	Read(void)		= 0;
 };

//==============================================================================
// End of SmlByteReader.h
//==============================================================================
#endif	// sentry

