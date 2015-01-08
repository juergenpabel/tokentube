#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libtokentube.h"


static const char charset[] = "0123456789abcdef";


static int hex_value(char c) {
	char* v = strchr(charset, c);

	if( v == NULL ) {
		return -1;
	}
	return v-charset;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_util_hex_encode(char const* input, size_t input_len, char* output, size_t* output_len) {
	size_t	i = 0;

	if( input == NULL || output_len == NULL || ( output_len != NULL && *output_len < 2*input_len+1 ) ) {
		TT_LOG_ERROR( "library/util", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	for( i=0; i<input_len; i++ ) {
		output[i*2+0] = charset[(unsigned char)input[i]/16];
		output[i*2+1] = charset[(unsigned char)input[i]%16];
	}
	output[input_len*2] = '\0';
	*output_len = input_len*2;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_util_hex_decode(char const* input, size_t input_len, char* output, size_t* output_len) {
	size_t	i = 0;

	if( input == NULL || output == NULL || output_len == NULL || *output_len < (input_len/2)+1 ) {
		TT_LOG_ERROR( "library/util", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	for( i=0; i<input_len; i+=2 ) {
		output[i/2] = 0;
		output[i/2] |= hex_value( (unsigned char)input[i+0] ) << 0;
		output[i/2] |= hex_value( (unsigned char)input[i+1] ) << 4;
	}
	output[input_len/2] = '\0';
	return TT_OK;
}

