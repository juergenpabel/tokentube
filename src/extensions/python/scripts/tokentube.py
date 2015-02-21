#!/usr/bin/python3
import sys, tokentube

tokentube.initialize()
tokentube.configure()
print( tokentube.user_exists( sys.argv[1] ) )
tokentube.finalize()

