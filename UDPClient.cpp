#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[])
{
    //Step 1: Initiate WinSock
    WSADATA wsaData;
    WORD wVersion = MAKEWORD(2, 2);
    if (WSAStartup(wVersion, &wsaData)) {
        printf("Winsock 2.2 is not supported\n");
        return 0;
    }

    //Step 2: Construct socket
    SOCKET client;
    client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client == INVALID_SOCKET) {
        printf("Error %d: cannot create server socket.", WSAGetLastError());
        return 0;
    }

    printf("Client started!\n");

    //(Optional) Set time-out for receiving
    int tv = 10000; //Time-out interval: 10000ms
    setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

    //Step 3: Specify server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

    //Step 4: Communicate with server
    char buff[BUFF_SIZE];
    int ret, serverAddrLen = sizeof(serverAddr), messageLen;
    int totalBytesSent = 0;
    while (1) {
        //Send message
        printf("Send to server: ");
        /*gets_s(buff, BUFF_SIZE);*/
        fgets(buff, BUFF_SIZE, stdin);
        size_t ln = strlen(buff) - 1;
        if (buff[ln] == '\n') buff[ln] = '\0';
        messageLen = strlen(buff);
        /*if (messageLen == 0) break;*/
        if (strcmp(buff, "bye") == 0) {
            printf("Total bytes sent: %d\n", totalBytesSent);
            break;
        }

        ret = sendto(client, buff, messageLen, 0, (sockaddr*)&serverAddr, serverAddrLen);
        if (ret == SOCKET_ERROR) {
            printf("Error %d: Cannot send message.", WSAGetLastError());
        }
        else {
            totalBytesSent += ret;
        }

        //Receive echo message
        ret = recvfrom(client, buff, BUFF_SIZE, 0, (sockaddr*)&serverAddr, &serverAddrLen);

        if (ret == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAETIMEDOUT)
                printf("Time-out!");
            else printf("Error %d: Cannot receive message.", WSAGetLastError());
        }
        else if (strlen(buff) > 0) {
            buff[ret] = 0;
            printf("Receive from server: %s\n", buff);
        }
    } //end while

    //Step 5: Close socket
    closesocket(client);

    //Step 6: Terminate Winsock
    WSACleanup();

    return 0;
}