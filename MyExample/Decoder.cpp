//***********************************************************************
//									*
// Filename:	Decoder.cpp						*
//									*
//***********************************************************************

//==============================================================================
// Compiler-Options & Header-Files
//==============================================================================

#include "Decoder.h"

#include <stdio.h>
#include <math.h>

#include <sml/sml_file.h>

//==============================================================================
// Internal Functions
//==============================================================================

static void PrintObis( sml_list * entry )
 {
  uint8_t * hStr = entry->obj_name->str;
  printf( "%d-%d:%d.%d.%d*%d#", hStr[ 0 ], hStr[ 1 ], hStr[ 2 ], hStr[ 3 ], hStr[ 4 ], hStr[ 5 ] );
 }

static void PrintOctedStr( sml_list * entry )
 {
  PrintObis( entry );
  char * str;
  sml_value_to_strhex( entry->value, &str, true );
  printf( "%s#\n", str );
  free( str );
 }

static void PrintBool( sml_list * entry )
 {
  PrintObis( entry );
  printf( "%s#\n", entry->value->data.boolean ? "true" : "false" );
 }

static void PrintInteger( sml_list * entry )
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
      ( unit = SmlCodeToUnitStr( (uint8_t) *entry->unit ) ) != NULL )
    printf( "%s", unit );
  printf( "\n" );
  // flush the stdout puffer, that pipes work without waiting
  fflush( stdout );
 }

//==============================================================================
// TDecoder
//==============================================================================

TDecoder::TDecoder(void)
: SFlag( false )
, VFlag( false )
 {
 }

void TDecoder::Decode( sml_file * file )
 {
  // this prints some information about the file
  if( VFlag )
    sml_file_print(file);

  // read here some values ...
  if( VFlag )
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
      if( SFlag )
        exit( 0 ); // processed first message - exit
     }
   }

  // free the malloc'd memory
  sml_file_free( file );
 }

//==============================================================================
// End of Decoder.cpp
//==============================================================================

