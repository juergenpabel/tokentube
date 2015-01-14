#!/bin/bash

export TT_RUNTIME_BOOTDEVICE=boot.img
LD_PRELOAD=../../src/lib/.libs/libtokentube.so ../../src/bin/ssod/ssod boot/tokentube/sso.conf $@
