local file = io.open( "etc/tokentube/keys/luks.key", "r" )
result = file:read("*all");
file:close()

