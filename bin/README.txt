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

//If set, logs end-of-round stats in html format to given file.
//default empty = not logging. Examples: set boonehtml "C:\www\boone.html" --- set boonehtml "mylog.html" --- set boonehtml "/home/dusted/log.html"
boonehtml (default empty = no loggin)

//If boonehtml is set, and you set booneport to for example 8080, it will start a webserver on that port
//The webserver will not start until a bone-event occurs (like a plum, imp, exl or acc).
//Windows only, for now at least, may crash ? turn it off it it does.
booneport (default 0 = don't start webserver. example: set boonehtml "c:\aq2\boone.html"; set booneport 8080)


//If set, logs boone events to given file in json format (one json entry per line)
boonejson (default empty = not logging. Example: set boonejson "/home/dusted/boone.json")
