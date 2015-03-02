#!/usr/bin/python3

import os, sys

f = open( "./etc/tokentube/keys/luks.key", "rb" )
key = f.read()
f.close()

