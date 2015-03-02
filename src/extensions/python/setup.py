#!/usr/bin/python3

from distutils.core import setup, Extension

setup(name='tokentube', version='1.0', ext_modules=[
	Extension('tokentube', sources=['extension.c', 'user.c', 'otp.c'], include_dirs=['../../../include'], libraries=['tokentube'])
	])

