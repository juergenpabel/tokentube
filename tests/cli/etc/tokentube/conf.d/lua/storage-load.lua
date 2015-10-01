if( type == "user" ) then
	local file = io.open( "boot/tokentube/user/" .. identifier, "r" )
	data = file:read("*all");
	file:close()
end
if( type == "otp" ) then
	local file = io.open( "boot/tokentube/otp/" .. identifier, "r" )
	data = file:read("*all");
	file:close()
end
if( type == "key" ) then
	local file = io.open( "etc/tokentube/keys/" .. identifier, "r" )
	data = file:read("*all");
	file:close()
end
if( type == "helpdesk" ) then
	local file = io.open( "var/spool/tokentube/helpdesk/" .. identifier, "r" )
	data = file:read("*all");
	file:close()
end

