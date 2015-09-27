#!/usr/bin/python3

import os, sys

# todo: user/otp/key handling
f = open( "./etc/tokentube/keys/" + sys.argv[1], "rb" )
data = f.read()
f.close()

