These are the compiled binaries.

The .so file (gamex86_64.so) is for 64 bit linux.
The .dll file (gamex64.dll) is for 32 bit windows.

//How many shells at rest allowed in world? (0 to disable)
shellnum (default "16")

//How many seconds should a shell stay (if world has room for it)
shellstay (default "8")

//How large a streak to obtain impressive
impstreak (default "5")

//How large a streak to obtain excellent
excstreak (default "12")

//If set, logs end-of-round stats in html format to given file, can be served with a webserver like moongoose, apache or iis.
boonehtml (default empty = not logging, example: set boonehtml "C:\www\boone.html")

//If set, logs boone events to given file in json format (one json entry per line)
boonejson (default empty, example: set boonejson "C:\aq2\boone.json")