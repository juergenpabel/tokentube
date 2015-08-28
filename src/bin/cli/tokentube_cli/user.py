#!/usr/bin/python3
import os, sys, argparse, getpass, tokentube


def init(parser):
	parser_user_create = parser.add_parser('user-create')
	parser_user_create.add_argument('--username', '-u', action='store', help='username')
	parser_user_create.add_argument('--password', '-p', action='store', help='password')
	parser_user_create.set_defaults(func=create)

	parser_user_update = parser.add_parser('user-update')
	parser_user_update.add_argument('--username', '-u', action='store', help='username')
	parser_user_update.add_argument('--password-cur', '-p', action='store', help='current password')
	parser_user_update.add_argument('--password-new', '-n', action='store', help='new password')
	parser_user_update.set_defaults(func=update)

	parser_user_exists = parser.add_parser('user-exists')
	parser_user_exists.add_argument('--username', '-u', action='store', help='username')
	parser_user_exists.set_defaults(func=exists)

	parser_user_delete = parser.add_parser('user-delete')
	parser_user_delete.add_argument('--username', '-u', action='store', help='username')
	parser_user_delete.set_defaults(func=delete)

	parser_user_verify = parser.add_parser('user-verify')
	parser_user_verify.add_argument('--username', '-u', action='store', help='username')
	parser_user_verify.add_argument('--password', '-p', action='store', help='password')
	parser_user_verify.set_defaults(func=verify)

	parser_user_key_add = parser.add_parser('user-add-key')
	parser_user_key_add.add_argument('--username', '-u', action='store', help='username')
	parser_user_key_add.add_argument('--password', '-p', action='store', help='password')
	parser_user_key_add.add_argument('--identifier', '-i', action='store', help='identifier')
	parser_user_key_add.set_defaults(func=addkey)

	parser_user_key_del = parser.add_parser('user-del-key')
	parser_user_key_del.add_argument('--username', '-u', action='store', help='username')
	parser_user_key_del.add_argument('--password', '-p', action='store', help='password')
	parser_user_key_del.add_argument('--identifier', '-i', action='store', help='identifier')
	parser_user_key_del.set_defaults(func=delkey)


def get_username():
	return input( "Enter username: " )


def get_password():
	return getpass.getpass( "Enter password: " )


def get_identifier():
	return input( "Enter key identifier: " )


def message(args, msg):
	if args.quiet is False:
		print( msg )


def create(args):
	if args.username is None:
		args.username = get_username()
	if tokentube.user_exists( args.username ) is True:
		message( args, "FAILURE: user '%s' already exists" % args.username )
		return -1
	if args.password is None:
		args.password = get_password()
	if tokentube.user_create( args.username, args.password ) is False:
		message( args, "FAILURE: user '%s' not created" % args.username )
		return -1
	message( args, "SUCCESS: user '%s' created" % args.username )
	return 0


def update(args):
	if args.username is None:
		args.username = get_username()
	if tokentube.user_exists( args.username ) is False:
		message( args, "FAILURE: user '%s' does not exist" % args.username )
		return -1
	if args.password_cur is None:
		args.password_cur = get_password()
	if args.password_new is None:
		args.password_new = get_password()
	if tokentube.user_update( args.username, args.password_cur, args.password_new ) is False:
		message( args, "FAILURE: user '%s' not updated" % args.username )
		return -1
	message( args, "SUCCESS: user '%s' updated" % args.username )
	return 0


def exists(args):
	if args.username is None:
		args.username = get_username()
	if tokentube.user_exists( args.username ) is False:
		message( args, "WARNING: user '%s' does not exist" % args.username )
		return -1
	message( args, "SUCCESS: user '%s' exists" % args.username )
	return 0


def delete(args):
	if args.username is None:
		args.username = get_username()
	if tokentube.user_exists( args.username ) is False:
		message( args, "WARNING: user '%s' does not exist" % args.username )
		return 0
	if tokentube.user_delete( args.username ) is False:
		message( args, "FAILURE: user '%s' not deleted" % args.username )
		return -1
	message( args, "SUCCESS: user '%s' deleted" % args.username )
	return 0


def verify(args):
	if args.username is None:
		args.username = get_username()
	if tokentube.user_exists( args.username ) is False:
		message( args, "FAILURE: user '%s' does not exist" % args.username )
		return -1
	if args.password is None:
		args.password = get_password()
	if tokentube.user_execute_verify( args.username, args.password ) is False:
		message( args, "FAILURE: user '%s' not verified" % args.username )
		return -1
	message( args, "SUCCESS: user '%s' verified" % args.username )
	return 0


def addkey(args):
	if args.username is None:
		args.username = get_username()
	if tokentube.user_exists( args.username ) is False:
		message( args, "FAILURE: user '%s' does not exist" % args.username )
		return -1
	if args.password is None:
		args.password = get_password()
	if args.identifier is None:
		args.identifier = get_identifier()
	if tokentube.user_key_add( args.username, args.password, args.identifier ) is False:
		message( args, "WARNING: key '%s' not added to user '%s' (key was already present)" % ( args.identifier, args.username ) )
		return 0
	message( args, "SUCCESS: key '%s' added to user '%s'" % ( args.identifier, args.username ) )
	return 0


def delkey(args):
	if args.username is None:
		args.username = get_username()
	if tokentube.user_exists( args.username ) is False:
		message( args, "FAILURE: user '%s' does not exist" % args.username )
		return -1
	if args.password is None:
		args.password = get_password()
	if args.identifier is None:
		args.identifier = get_identifier()
	if tokentube.user_key_del( args.username, args.password, args.identifier ) is False:
		message( args, "WARNING: key '%s' not deleted from user '%s' (key was not present)" % ( args.identifier, args.username ) )
		return 0
	message( args, "SUCCESS: key '%s' deleted from user '%s'" % ( args.identifier, args.username ) )
	return 0

