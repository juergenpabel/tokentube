#!/bin/bash

export PYTHONPATH=./usr/lib/python3
export LD_LIBRARY_PATH=../../src/lib/.libs
TT_RUNTIME_BOOTDEVICE=boot.img ../../src/bin/cli/tokentube -c etc/tokentube/tokentube.conf $@
