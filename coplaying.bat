start listen.exe %1
@ping 127.0.0.1 -n 2 -w 1000 > nul
start coplayer.exe %1
