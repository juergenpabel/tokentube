#!/bin/bash

export TT_RUNTIME_BOOTDEVICE=boot.img
LD_PRELOAD=../../src/lib/.libs/libtokentube.so ../../src/bin/cli/.libs/tokentube -c etc/tokentube/tokentube.conf $@
