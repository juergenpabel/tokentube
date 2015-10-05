#!/usr/bin/python3
import os, sys, argparse, tokentube


def init(parser):
	parser_otp_create = parser.add_parser('otp-create')
	parser_otp_create.add_argument('--identifier', '-i', action='store', help='identifier')
	parser_otp_create.set_defaults(func=create)

	parser_otp_exists = parser.add_parser('otp-exists')
	parser_otp_exists.add_argument('--identifier', '-i', action='store', help='identifier')
	parser_otp_exists.set_defaults(func=exists)

	parser_otp_delete = parser.add_parser('otp-delete')
	parser_otp_delete.add_argument('--identifier', '-i', action='store', help='identifier')
	parser_otp_delete.set_defaults(func=delete)


def get_identifier():
	return input( "Enter identifier: " )


def get_challenge():
	return input( "Enter challenge: " )


def get_response():
	return input( "Enter response: " )


def message(args, msg):
	if args.quiet is False:
		print( msg )


def create(args):
	if args.identifier is None:
		args.identifier = get_identifier()
	if tokentube.otp_exists( args.identifier ) is True:
		message( args, "FAILURE: otp '%s' already exists" % args.identifier )
		return -1
	if tokentube.otp_create( args.identifier ) is False:
		message( args, "FAILURE: otp '%s' not created" % args.identifier )
		return -1
	message( args, "SUCCESS: otp '%s' created" % args.identifier )
	return 0


def exists(args):
	if args.identifier is None:
		args.identifier = get_identifier()
	if tokentube.otp_exists( args.identifier ) is False:
		message( args, "WARNING: otp '%s' does not exist" % args.identifier )
		return -1
	message( args, "SUCCESS: otp '%s' exists" % args.identifier )
	return 0


def delete(args):
	if args.identifier is None:
		args.identifier = get_identifier()
	if tokentube.otp_exists( args.identifier ) is False:
		message( args, "WARNING: otp '%s' does not exist" % args.identifier )
		return 0
	if tokentube.otp_delete( args.identifier ) is False:
		message( args, "FAILURE: otp '%s' not deleted" % args.identifier )
		return -1
	message( args, "SUCCESS: otp '%s' deleted" % args.identifier )
	return 0

