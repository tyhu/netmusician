Virtual Online Musical Instruments
Ver. 1.1.0   2014/2/15
Ting-Yao Hu      benken.tyhu@gmail.com

This project aims to construct an simple online environment for instrumental ensemble.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

INSTALLATION:

ver 1.0.0
This version is only for windows (tested on win7/XP).

The three programs (play, listen, onlineplayer) should be execuable directly. I compiled them through MinGW. Commands for compilation are recorded in "compilenote.txt".

linux_server/nmserver.c is required linux OS to compile and execute.

ver. 1.1.0
One program added (coplaying), should be execuable directly.
linux_server/nmserver2.c (for co-playing) is required linux OS to compile and execute.


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

USAGE:

Ver. 1.0.0, two functions provided.

1. Local Playing:
(1) "execute makenotes.bat"
I utilize the program "soundstretch"[1] to make notes for different pitches from the sound source (piano_A4.wav), and save them in the directory "notes".

(2) "execute play.exe"
Now your keyboard has become a virtual piano. Type and you'll find the key 'z' produce music note 'C', 's' for 'C#'... and so on. The complete mapping between keys and notes is in "notemap.txt"

You may modify "makenotes.bat" and "notemap.txt" to reach any kind of scale you want, or using other sound resource instead of "piano_A4.wav".

2. Online playing (one player, one listener)
(1) "execute makenotes.bat" on two windows machines (say, winA and winB)
if you've done this before, ignore this part.

(2) Find a linux machine with fixed ip, compile linux_server/nmserver.c, and execute it.
"gcc -o nmserver nmserver.c"

(3) cmd "onlineplayer [ip address of linux machine]" in winA

(4) cmd "listen [ip address of linux machine]" in winB

(5) The one executing "onlineplaer" (winA) now can operate it as "play". Both two are able to listen.
Only the information of notes is transmitted through internet.

Ver 1.1.0
3. Co-playing:
(1) "execute makenotes.bat" on two windows machines (say, winA and winB)
if you've done this before, ignore this part.

(2) Find a linux machine with fixed ip, compile linux_server/nmserver2.c, and execute it.
(compilation command is in "compilenote.txt")

(3) cmd "coplaying.bat [ip address of linux machine]" in both winA and winB.
now you can playing an ensemble.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

REFERENCE:
[1]: http://www.surina.net/soundtouch/soundstretch.html
[2]: MSDN, for waveOut api and socket
