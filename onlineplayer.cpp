#include "network.h"
#include <stdio.h>
#include "player2.h"

int main(int argc, char* argv[])
{
	char sendstr[2];
	char *recvstr;
	HANDLE hStdin;
    DWORD cNumRead;
    INPUT_RECORD irInBuf;
    KEY_EVENT_RECORD ker;
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    Instrument instrument("notemap.txt",44100, 16, 1);
	ClientSocket clientSckt;
	//clientSckt.WaitConnection();
	clientSckt.MakeConnection(argv[1],DEFAULT_PORT);
	printf("wait for a listener..\n");
    recvstr=clientSckt.CRecv(2);
	printf("start playing!\n");
    while(1){
        ReadConsoleInput(hStdin,&irInBuf,1,&cNumRead);
        //printf("Key event: ");
        ker=irInBuf.Event.KeyEvent;
        if(ker.bKeyDown){
            //printf("%c key pressed\n",ker.uChar.AsciiChar);
            if(instrument.playnote(ker.uChar.AsciiChar)){
				sendstr[0]=ker.uChar.AsciiChar;
				sendstr[1]='d';
				if(!clientSckt.CSend(sendstr)){
					printf("remote connection closed");
					ExitProcess(1);
				}
			}
        }
        else{
            //printf("key released\n");
            if(instrument.resetnote(ker.uChar.AsciiChar)){
				sendstr[0]=ker.uChar.AsciiChar;
				sendstr[1]='u';
				if(!clientSckt.CSend(sendstr)){
					printf("remote connection closed");
					ExitProcess(1);
				}
			}
        }
    }

    return 0;
}