#include <stdlib.h>
#include <string.h>
#include <confuse.h>
#include "libtokentube.h"


static const char* const CHARSET = "abcdefghkmnopqrstuvwxyz023456789";


__attribute__ ((visibility ("hidden")))
int libtokentube_util_base32_encode(const void* bytes, size_t bytes_len, void* text, size_t* text_len) {
	const unsigned char*	input = bytes;
	size_t			input_len = bytes_len;
	char*			output = text;
	size_t*			output_len = text_len;
	size_t			input_pos = 0, output_pos = 0;

	if( input == NULL || output == NULL || output_len == NULL || input_len*2+1 > *output_len ) {
		TT_LOG_ERROR( "library/util", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( input_len == 0 ) {
		*output_len = 0;
		return TT_OK;
	}
	while( input_pos < input_len ) {
		unsigned char b[4] = {0};
		unsigned int i, bits=0, val=0;

		b[0] = (input[input_pos+0] & 0xf0) >> 4;
		b[1] = (input[input_pos+0] & 0x0f) >> 0;
		b[2] = (input[input_pos+1] & 0xf0) >> 4;
		b[3] = (input[input_pos+1] & 0x0f) >> 0;

		bits = (input[input_pos+0] << 8) | (input[input_pos+1] << 0);
		bits <<= 4;
		bits |= (b[0] ^ b[1] ^ b[2] ^ b[3]) & 0x0f;

		for( i=0; i<4; i++ ) {
			val = bits & 0x1f;
			bits >>= 5;
			output[output_pos+3-i] = CHARSET[val];
		}
		output_pos += 4;
		input_pos += 2;
	}
	output[output_pos] = '\0';
	*output_len = output_pos;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_util_base32_decode(const void* text, size_t text_len, void* bytes, size_t* bytes_len) {
	const unsigned char*	input = text;
	size_t			input_len = text_len;
	char*			output = bytes;
	size_t*			output_len = bytes_len;
	size_t	input_pos = 0, output_pos = 0;
	char*	p = NULL;

	if( input == NULL || output_len == NULL || *output_len < input_len/2+1 ) {
		TT_LOG_ERROR( "library/util", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	while( input_pos < input_len ) {
		unsigned char b[5] = {0};
		unsigned int i=0, bits=0, val=0;

		for( i=0; i<4; i++ ) {
			p = strchr(CHARSET, input[input_pos+i]);
			if( p != NULL ) {
				b[i] = p - CHARSET;
			} else {
				TT_LOG_ERROR( "library/util", "invalid charset in %s()", __FUNCTION__ );
				return TT_ERR;
			}
		}
		bits = 0;
		for( i=0; i<4; i++ ) {
			bits <<= 5;
			bits |= b[i] & 0x1f;
		}
		val = (bits & 0xffff0) >> 4;

		for( i=0; i<5; i++ ) {
			b[i] = bits & 0xf;
			bits >>= 4;
		}
		if((b[0] ^ b[1] ^ b[2] ^ b[3] ^ b[4]) != 0) {
			TT_LOG_ERROR( "library/util", "invalid xor in %s()", __FUNCTION__ );
			return TT_ERR;
		}
		if( output != NULL ) {
			output[output_pos+0] = (val & 0xff00) >> 8;
			output[output_pos+1] = (val & 0x00ff) >> 0;
		}
		output_pos += 2;
		input_pos += 4;
	}
	*output_len = output_pos;
	return TT_OK;
}

