#include <windows.h>
#include <stdio.h>
#include "player.h"

int main(int argc, char* argv[])
{
    
    Instrument instrument("notemap.txt",44100, 16, 1);
    
    HANDLE hStdin;
    DWORD cNumRead;
    INPUT_RECORD irInBuf;
    KEY_EVENT_RECORD ker;
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
	printf("Type your keyboard and play! Ctrl+C to leave\n");
    while(1){
        ReadConsoleInput(hStdin,&irInBuf,1,&cNumRead);
        //printf("Key event: ");
        ker=irInBuf.Event.KeyEvent;
        if(ker.bKeyDown){
            //printf("%c key pressed\n",ker.uChar.AsciiChar);
            instrument.playnote(ker.uChar.AsciiChar);
        }
        else{
            //printf("key released\n");
            instrument.resetnote(ker.uChar.AsciiChar);
        }
    }

    return 0;
}
