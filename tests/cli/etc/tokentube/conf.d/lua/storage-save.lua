if( type == "user" ) then
	local file = io.open( "boot/tokentube/user/" .. identifier .. ".dat", "w" )
	file:write( data );
	file:close()
end
if( type == "otp" ) then
	local file = io.open( "boot/tokentube/otp/" .. identifier .. ".dat", "w" )
	file:write( data );
	file:close()
end
if( type == "key" ) then
	local file = io.open( "etc/tokentube/keys/" .. identifier, "w" )
	file:write( data );
	file:close()
end

