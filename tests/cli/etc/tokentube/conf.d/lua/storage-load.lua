if( type == "user" ) then
	local file = io.open( "boot/tokentube/user/" .. identifier .. ".dat", "r" )
	data = file:read("*all");
	file:close()
end
if( type == "otp" ) then
	local file = io.open( "boot/tokentube/otp/" .. identifier .. ".dat", "r" )
	data = file:read("*all");
	file:close()
end
if( type == "key" ) then
	local file = io.open( "etc/tokentube/keys/" .. identifier, "r" )
	data = file:read("*all");
	file:close()
end

