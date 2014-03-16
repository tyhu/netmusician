#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

int main(){
	struct sockaddr_in serv_addr, play_addr, listen_addr;
	int sockfd,playfd,listenfd,playlen,listenlen,n,optval=1;
	char buffer[2];
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
	printf("waiting for a player..\n");
	if ( listen(sockfd, 10) == -1 ){
		printf("listen error\n");
		exit(1);
	}
	playlen=sizeof(play_addr);
	playfd=accept(sockfd, (struct sockaddr *) &play_addr, &playlen);
	if(playfd<0){
		printf("accept error\n");
		exit(1);
	}

	printf("waiting for a listener\n");
	if ( listen(sockfd, 10) == -1 ){
		printf("listen error\n");
		exit(1);
	}
	listenlen=sizeof(play_addr);
	listenfd=accept(sockfd, (struct sockaddr *) &play_addr, &listenlen);
	if(listenfd<0){
		printf("listen error\n");
		exit(1);
	}
	n=write(playfd,"ok",2);

	setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, (char *) &optval, sizeof(int));
	bzero(buffer,2);
	printf("start playing\n");
	while(1){
		n=read(playfd,buffer,2);
		if(n<0){
			printf("player connection shutdown\n");
			break;
		}
		printf("%s\n",buffer);
		n=write(listenfd,buffer,2);
		if(n<0){
			printf("listener connection shutdown\n");
			break;
		}
	}
	close(playfd);
	close(listenfd);
	return 0;
}
