#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libtokentube.h"


static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


static int base64_value(char c) {
	char* v = strchr(charset, c);

	if( v == NULL ) {
		return -1;
	}
	return v-charset;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_util_base64_encode(void const* bytes, size_t bytes_len, void* text, size_t* text_len) {
	const unsigned char*    input = bytes;
	size_t                  input_len = bytes_len;
	char*                   output = text;
	size_t*                 output_len = text_len;
	int			char_array_3[3];
	int			char_array_4[4];
	int			i=0, j=0;

	if( input == NULL || output_len == NULL ) {
		TT_LOG_ERROR( "library/util", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( input_len == 0 ) {
		input_len = strlen( (const char*)input );
	}
	if( output != NULL ) {
		memset( output, '\0', *output_len );
		*output_len = 0;
	}
	while( input_len-- ) {
		char_array_3[i++] = *(input++);
		if( i == 3 ) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for( i=0; i<4; i++ ) {
				if( output != NULL ) {
					*output++ = charset[char_array_4[i]];
				}
				(*output_len)++;
			}
			i = 0;
		}
	}

	if( i>0 ) {
		for( j=i; j<3; j++ ) {
			char_array_3[j] = '\0';
		}
		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for( j=0; j<i+1; j++ ) {
			if( output != NULL ) {
				*output++ = charset[char_array_4[j]];
			}
			(*output_len)++;
		}
		while( i++ < 3 ) {
			if( output != NULL ) {
				*output++ = '=';
			}
			(*output_len)++;
		}
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_util_base64_decode(void const* text, size_t text_len, void* bytes, size_t* bytes_len) {
	const unsigned char*    input = text;
	size_t                  input_len = text_len;
	char*                   output = bytes;
	size_t*                 output_len = bytes_len;
	char			char_array_4[4], char_array_3[3];
	int			i = 0, j = 0, pos = 0;
	char			c = 0;

	if( input == NULL || output_len == NULL ) {
		TT_LOG_ERROR( "library/util", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( input_len == 0 ) {
		input_len = strlen( (const char*)input );
	}
	if( output != NULL ) {
		memset( output, '\0', *output_len );
		*output_len = 0;
	}
	while( input_len-- && ( input[pos] != '=' ) ) {
		c = base64_value( input[pos] );
		if( c >= 0 ) {
			char_array_4[i++] = input[pos++];
			if( i == 4 ) {
				for( i=0; i<4; i++ ) {
					char_array_4[i] = base64_value( char_array_4[i] );
				}
				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for( i=0; i<3; i++ ) {
					if( output != NULL ) {
						*output++ = char_array_3[i];
					}
					(*output_len)++;
				}
				i = 0;
			}
		} else {
			TT_LOG_ERROR( "library/util", "invalid character '%c' at position %d in %s()", c, pos, __FUNCTION__ );
			return TT_ERR;
		}
	}

	if( i > 0 ) {
		for( j=i; j<4; j++ ) {
			char_array_4[j] = 0;
		}
		for( j=0; j<4; j++ ) {
			c = base64_value( char_array_4[j] );
			if( c >= 0 ) {
				char_array_4[j] = c;
			} else {
				TT_LOG_ERROR( "library/util", "invalid character '%c' at position %d in %s()", c, pos, __FUNCTION__ );
				return TT_ERR;
			}
		}
		char_array_3[0] =  (char_array_4[0]        << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +   char_array_4[3];

		for( j=0; j<i-1; j++ ) {
			if( output != NULL ) {
				*output++ = char_array_3[j];
			}
			(*output_len)++;
		}
	}
	return TT_OK;
}

