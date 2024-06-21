#include<stdio.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[]) {
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}

	SOCKET server;
	server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server == INVALID_SOCKET) {
		printf("Error %d: Cannot create server socket.", WSAGetLastError());
		return 0;
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);
	if (bind(server, (sockaddr*)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error %d: Cannot bind this address.", WSAGetLastError());
		getchar();
		return 0;
	}
	printf("Server started!");

	sockaddr_in clientAddr;
	char buff[BUFF_SIZE], clientIP[INET6_ADDRSTRLEN];
	int ret, clientAddrLen = sizeof(clientAddr), clientPort;

	while (1) {
		ret = recvfrom(server, buff, BUFF_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrLen);
		if (ret == SOCKET_ERROR)
			printf("Error %d: Cannot receive data.", WSAGetLastError());
		else if (strlen(buff) > 0) {
			buff[ret] = 0;
			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
			clientPort = ntohs(clientAddr.sin_port);
			printf("Receive from client[%s:%d] %s\n", clientIP, clientPort, buff);


			ret = sendto(server, buff, strlen(buff), 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr));
			if (ret == SOCKET_ERROR)
				printf("Error %d: Cannot send data", WSAGetLastError());
		}
	}

	closesocket(server);
	WSACleanup();
	return 0;
}