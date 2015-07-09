local file = io.open( "etc/tokentube/keys/luks.key", "r" )
data = file:read("*all");
file:close()

