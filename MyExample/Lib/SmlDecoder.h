//***********************************************************************
//									*
// Filename:	SmlDecoder.h						*
//									*
//***********************************************************************

#ifndef SmlDecoderH	// sentry
#define SmlDecoderH

//==============================================================================
// Compiler-Options & Header-Files
//==============================================================================

#include "Sml.h"

//==============================================================================
// TSmlDecoder
//==============================================================================

class TSmlDecoder
 {
 public:
  		TSmlDecoder(void)		{}
  virtual void	Decode( sml_file * file )	= 0;
 };

//==============================================================================
// End of SmlDecoder.h
//==============================================================================
#endif	// sentry
