#!/usr/bin/python3

import os, sys


mapping = {
	"USER": "./boot/tokentube/user",
	"OTP":  "./boot/tokentube/otp",
	"KEY":  "./etc/tokentube/keys"
}


path = mapping.get( sys.argv[1] );
if( path is not None ):
	f = open( path + "/" + sys.argv[2] + ".dat", "wb+" );
	f.write( data )
	f.close()

