#!/usr/bin/python3
import os, sys, argparse, getpass, tokentube


def init(parser):
	parser_user_verify = parser.add_parser('auth-user-verify')
	parser_user_verify.add_argument('--username', '-u', action='store', help='username')
	parser_user_verify.add_argument('--password', '-p', action='store', help='password')
	parser_user_verify.set_defaults(func=user_verify)

	parser_otp_challenge = parser.add_parser('auth-otp-challenge')
	parser_otp_challenge.add_argument('--identifier', '-i', action='store', help='identifier')
	parser_otp_challenge.set_defaults(func=otp_challenge)

	parser_otp_response = parser.add_parser('auth-otp-response')
	parser_otp_response.add_argument('--identifier', '-i', action='store', help='identifier')
	parser_otp_response.add_argument('--challenge',  '-x', action='store', help='challenge')
	parser_otp_response.set_defaults(func=otp_response)

	parser_otp_loadkey = parser.add_parser('auth-otp-loadkey')
	parser_otp_loadkey.add_argument('--identifier', '-i', action='store', help='identifier')
	parser_otp_loadkey.add_argument('--challenge',  '-x', action='store', help='challenge')
	parser_otp_loadkey.add_argument('--response',   '-y', action='store', help='response')
	parser_otp_loadkey.set_defaults(func=otp_loadkey)


def get_username():
	return input( "Enter username: " )


def get_password():
	return getpass.getpass( "Enter password: " )


def get_identifier():
	return input( "Enter key identifier: " )


def get_challenge():
	return input( "Enter challenge: " )


def get_response():
	return input( "Enter response: " )


def message(args, msg):
	if args.quiet is False:
		print( msg )


def user_verify(args):
	if args.username is None:
		args.username = get_username()
	if tokentube.user_exists( args.username ) is False:
		message( args, "FAILURE: user '%s' does not exist" % args.username )
		return -1
	if args.password is None:
		args.password = get_password()
	if tokentube.auth_user_verify( args.username, args.password ) is False:
		message( args, "FAILURE: user '%s' not verified" % args.username )
		return -1
	message( args, "SUCCESS: user '%s' verified" % args.username )
	return 0


def otp_challenge(args):
	challenge = " " * 2*tokentube.TT_OTP_TEXT_MAX;
	if args.identifier is None:
		args.identifier = get_identifier()
	if tokentube.otp_exists( args.identifier ) is False:
		message( args, "WARNING: otp '%s' does not exist" % args.identifier )
		return 0
	if tokentube.auth_otp_challenge( args.identifier, challenge ) is False:
		message( args, "FAILURE: failed to process challenge for otp '%s'" % args.identifier )
		return -1
	message( args, "SUCCESS: challenge = '%s'" % challenge )
	return 0


def otp_response(args):
	response = " " * 2*tokentube.TT_OTP_TEXT_MAX;
	if args.identifier is None:
		args.identifier = get_identifier()
	if tokentube.otp_exists( args.identifier ) is False:
		message( args, "WARNING: otp '%s' does not exist" % args.identifier )
		return 0
	if args.challenge is None:
		args.challenge = get_challenge()
	if tokentube.auth_otp_response( args.identifier, args.challenge, response ) is False:
		message( args, "FAILURE: failed to process response for otp '%s'" % args.identifier )
		return -1
	message( args, "SUCCESS: response = '%s'" % response )
	return 0


def otp_loadkey(args):
	key = " " * 2*tokentube.TT_OTP_TEXT_MAX;
	if args.identifier is None:
		args.identifier = get_identifier()
	if tokentube.otp_exists( args.identifier ) is False:
		message( args, "WARNING: otp '%s' does not exist" % args.identifier )
		return 0
	if args.challenge is None:
		args.challenge = get_challenge()
	if args.response is None:
		args.response = get_response()
	if tokentube.auth_otp_loadkey( args.identifier, args.challenge, args.response, key ) is False:
		message( args, "FAILURE: failed to process otp '%s'" % args.identifier )
		return -1
	message( args, "SUCCESS: key = '%s'" % key )
	return 0

