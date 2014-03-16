#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#define MAXNUMTHREADS 4

int numofPlayer,itr;
pthread_mutex_t mutex;
int playIds[MAXNUMTHREADS],listenIds[MAXNUMTHREADS];

int broadcast(char *sendtext,int length){
	for(itr=0;itr<numofPlayer;itr++){
		if(write(listenIds[itr],sendtext,length)<0){
			printf("listener connection shutdown\n");
			return 0;
		}
	}
	return 1;
}

void playThreadFunc(void *ptr){
	char buffer[2];
	bzero(buffer,2);
	int sockId,send;
	sockId=*((int *) ptr);
	write(sockId,"ok",2);
	while(1){
		if(read(sockId,buffer,2)<0){
			printf("player connection shutdown\n");
			break;
		}
		pthread_mutex_lock (&mutex);
		printf("sent\n");
		send=broadcast(buffer,2);
		pthread_mutex_unlock(&mutex);
		if(!send)
			break;
	}
}

int main(){
	struct sockaddr_in serv_addr;
	struct sockaddr_in play_addr[MAXNUMTHREADS];
	struct sockaddr_in listen_addr[MAXNUMTHREADS];
	int sockfd,optval=1;
	char buffer[2];
	pthread_t threads[MAXNUMTHREADS];
	int playlen[MAXNUMTHREADS],listenlen[MAXNUMTHREADS];
	if ((sockfd=socket(AF_INET, SOCK_STREAM, 0))==-1){
		printf("socket initialization error\n");
		exit(1);
	}
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(27000);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero( &(serv_addr.sin_zero), sizeof(serv_addr) );
	if ( bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr)) == -1 ){
		printf("bind error\n");
		exit(1);
	}
	numofPlayer=0;
	while(1){
		printf("waiting for listen socket\n");
		if ( listen(sockfd, 10) == -1 ){
			printf("listen error\n");
			exit(1);
		}
		listenlen[numofPlayer]=sizeof(listen_addr);
		listenIds[numofPlayer]=accept(sockfd, (struct sockaddr *) &listen_addr[numofPlayer], &listenlen[numofPlayer]);
		if(listenIds[numofPlayer]<0){
			printf("accept error\n");
			exit(1);
		}
		setsockopt(listenIds[numofPlayer], IPPROTO_TCP, TCP_NODELAY, (char *) &optval, sizeof(int));

		printf("waiting for play socket..\n");
		if ( listen(sockfd, 10) == -1 ){
			printf("listen error\n");
			exit(1);
		}
		playlen[numofPlayer]=sizeof(play_addr);
		playIds[numofPlayer]=accept(sockfd, (struct sockaddr *) &play_addr[numofPlayer], &playlen[numofPlayer]);
		if(playIds[numofPlayer]<0){
			printf("accept error\n");
			exit(1);
		}

		pthread_create(&threads[numofPlayer],NULL,(void *) &playThreadFunc,&playIds[numofPlayer]);
		numofPlayer++;
	}
	return 0;
}


