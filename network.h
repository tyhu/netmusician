#define _WIN32_WINNT 0x501

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 2
#define DEFAULT_PORT "27000"

class ClientSocket{
	public:
		ClientSocket(){}
		~ClientSocket(){}
		
		bool MakeConnection(const char *ip, const char *port){
			struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
			_csocket=INVALID_SOCKET;
			_iResult = WSAStartup(MAKEWORD(2,2), &_wsaData);
			if (_iResult != 0) {
				printf("WSAStartup failed with error: %d\n", _iResult);
				return false;
			}
			ZeroMemory( &hints, sizeof(hints) );
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			
			_iResult = getaddrinfo(ip, port, &hints, &result);
			if (_iResult != 0) {
				printf("getaddrinfo failed: %d\n", _iResult);
				WSACleanup();
				return false;
			}
			ptr=result;
			_csocket=socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (_csocket == INVALID_SOCKET) {
				printf("Error at socket(): %ld\n", WSAGetLastError());
				freeaddrinfo(result);
				WSACleanup();
				return false;
			}
			
			_iResult=connect( _csocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (_iResult == SOCKET_ERROR) {
				closesocket(_csocket);
				return false;
			}
			
			freeaddrinfo(result);
			return true;
		}
		
		bool WaitConnection(){ //solid ip required
			SOCKET ListenSocket = INVALID_SOCKET;
			struct addrinfo *result = NULL;
			struct addrinfo hints;
			int optval=1;
			_iResult = WSAStartup(MAKEWORD(2,2), &_wsaData);
			if (_iResult != 0) {
				printf("WSAStartup failed with error: %d\n", _iResult);
				return false;
			}
			
			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;

			_iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
			if ( _iResult != 0 ) {
				printf("getaddrinfo failed with error: %d\n", _iResult);
				WSACleanup();
				return false;
			}
			
			ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (ListenSocket == INVALID_SOCKET) {
				printf("socket failed with error: %ld\n", WSAGetLastError());
				freeaddrinfo(result);
				WSACleanup();
				return false;
			}
			
			_iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
			if (_iResult == SOCKET_ERROR) {
				printf("bind failed with error: %d\n", WSAGetLastError());
				freeaddrinfo(result);
				closesocket(ListenSocket);
				WSACleanup();
				return false;
			}
			
			freeaddrinfo(result);
			
			_iResult = listen(ListenSocket, SOMAXCONN);
			if (_iResult == SOCKET_ERROR) {
				printf("listen failed with error: %d\n", WSAGetLastError());
				closesocket(ListenSocket);
				WSACleanup();
				return false;
			}

			_csocket=accept(ListenSocket, NULL, NULL);
			
			if (_csocket == INVALID_SOCKET) {
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(ListenSocket);
				WSACleanup();
				return false;
			}
			
			if (setsockopt(_csocket, IPPROTO_TCP, TCP_NODELAY, (char *) &optval, sizeof(int)) < 0){
				printf("failed to set no_delay: %d\n", WSAGetLastError());
				closesocket(ListenSocket);
				WSACleanup();
				return false;
			}
			
			return true;
		} 
		
		bool CloseConnection(){
			_iResult = shutdown(_csocket, SD_SEND);
			if (_iResult == SOCKET_ERROR) {
				printf("shutdown failed with error: %d\n", WSAGetLastError());
				closesocket(_csocket);
				WSACleanup();
				return false;
			}
			
			do {
				_iResult = recv(_csocket, _recvbuf, DEFAULT_BUFLEN, 0);
				if ( _iResult > 0 )
					printf("residue bytes received: %d\n", _iResult);
				else if ( _iResult == 0 )
					printf("Connection closed\n");
				else
					printf("recv failed with error: %d\n", WSAGetLastError());
			} while( _iResult > 0 );
			closesocket(_csocket);
			WSACleanup();
			return true;
		}
		
		bool CSend(char *sendtext){
			//_iResult = send( _csocket, sendtext, (int)strlen(sendtext), 0 );
			_iResult = send( _csocket, sendtext, 2, 0 );
			if (_iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(_csocket);
				WSACleanup();
				return false;
			}
			return true;
		}
		char * CRecv(int recvbuflen){
			_iResult = recv(_csocket, _recvbuf, recvbuflen, 0);
			if( _iResult < 0){
				printf("connection shutdown!");
				Sleep(5000);
				ExitProcess(1);
			}
			return _recvbuf;
		}

	private:
		SOCKET _csocket;
		char _recvbuf[DEFAULT_BUFLEN];
		WSADATA _wsaData;
		int _iResult;		
};
