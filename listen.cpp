#include "network.h"
#include <stdio.h>
#include "player2.h"

int main(int argc, char* argv[])
{
    
	char *recvstr;
    Instrument instrument("notemap.txt",44100, 16, 1);
    
	ClientSocket clientSckt;
	clientSckt.MakeConnection(argv[1],DEFAULT_PORT);
	while(1){
		recvstr=clientSckt.CRecv(2);
		if(recvstr[1]=='d'){
			instrument.playnote(recvstr[0]);
			//printf("%c down\n",recvstr[0]);
		}
		else if(recvstr[1]=='u'){
			instrument.resetnote(recvstr[0]);
			//printf("%c up\n",recvstr[0]);
		}
	}
    return 0;
}