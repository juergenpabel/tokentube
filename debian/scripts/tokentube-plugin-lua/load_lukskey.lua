lukskey = {}
local f = io.open("/etc/tokentube/keys/luks.key", "rb")
lukskey = f:read("*a")

