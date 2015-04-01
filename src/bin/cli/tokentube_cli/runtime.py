#!/usr/bin/python3
import os, sys, argparse, tokentube


def init(parser):
	parser_pba_install = parser.add_parser('pba-install')
	parser_pba_install.add_argument('--type', '-T', action='store', help='PBA type (initrd|initramfs)')
	parser_pba_install.add_argument('--path', '-P', action='store', help='PBA path')
	parser_pba_install.set_defaults(func=install)


def message(args, msg):
	if args.quiet is False:
		print( msg )


def install(args):
	if tokentube.runtime_install_pba( args.type, args.path ) is False:
		message( args, "FAILURE: PBA '%s'/'%s' not installed" % ( args.type, args.path ) )
		return -1
	return 0

