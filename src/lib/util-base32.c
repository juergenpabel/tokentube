#include <stdlib.h>
#include <string.h>
#include <confuse.h>
#include "libtokentube.h"


static const char* const CHARSET = "abcdefghkmnopqrstuvwxyz023456789";


__attribute__ ((visibility ("hidden")))
int libtokentube_util_base32_encode(const char* input, size_t input_len, char* output, size_t* output_len) {
	size_t input_pos = 0, output_pos = 0;

	if( input == NULL || output == NULL || output_len == NULL || (input_len*2)+(input_len/2) > *output_len+1 ) {
		TT_LOG_ERROR( "library/util", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( input_len == 0 ) {
		*output_len = 0;
		return TT_OK;
	}
	while( input_pos < input_len ) {
		char b[4] = {0};
		int i, bits=0, val=0;

		b[0] = (input[input_pos+0] & 0x0f);
		b[1] = (input[input_pos+0] & 0xf0) >> 4;
		b[2] = (input[input_pos+1] & 0x0f);
		b[3] = (input[input_pos+1] & 0xf0) >> 4;

		bits |= input[input_pos+1] & 0xff;
		bits <<= 8;
		bits |= input[input_pos+0] & 0xff;
		bits <<= 4;
		bits |= b[0] ^ b[1] ^ b[2] ^ b[3];

		for( i=0; i<4; i++ ) {
			val = bits & 0x1f;
			bits >>= 5;
			output[output_pos+i] = CHARSET[val];
		}
		output_pos +=4;
		input_pos += 2;
		if( output_pos < *output_len ) {
			output[output_pos] = '-';
			output_pos++;
		}
	}
	output_pos--;
	output[output_pos] = '\0';
	*output_len = output_pos;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_util_base32_decode(const char* input, size_t input_len, char* output, size_t* output_len) {
	size_t	input_pos = 0, output_pos = 0;
	char*	p = NULL;

	if( input == NULL || output_len == NULL || (*output_len < 2*((input_len+1)/5)) ) {
		TT_LOG_ERROR( "library/util", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	while( input_pos < input_len ) {
		char b[5] = {0};
		int i=0, bits=0, val=0;

		for( i=0; i<4; i++ ) {
			p = strchr(CHARSET, input[input_pos+i]);
			if( p != NULL ) {
				b[i] = p - CHARSET;
			} else {
				return TT_ERR;
			}
		}
		for( i=0; i<4; i++ ) {
			bits <<=5;
			bits |= b[3-i];
		}
		val = (bits & 0xffff0) >> 4;

		for( i=0; i<5; i++ ) {
			b[4-i] = bits & 0xf;
			bits >>= 4;
		}
		if((b[0] ^ b[1] ^ b[2] ^ b[3] ^ b[4]) != 0) {
			return TT_ERR;
		}
		if( output != NULL ) {
			output[output_pos+1] = (val & 0xff00) >> 8;
			output[output_pos+0] = (val & 0x00ff);
		}
		output_pos += 2;
		input_pos += 4;
		if( input_pos < input_len && input[input_pos] != '-' ) {
			return TT_ERR;
		}
		input_pos++;
	}
	*output_len = output_pos;
	return TT_OK;
}

