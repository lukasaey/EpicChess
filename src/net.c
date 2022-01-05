#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "net.h"

int send_move(SOCKET sock, game_t *game, Header *head, GameData *data)
{
    int sendRes = send(sock, (char *)head, sizeof *head, 0);
    if (sendRes == SOCKET_ERROR) {
        fprintf(stderr, "Could not send header to server.\n");
        return 1;
    }

    sendRes = send(sock, (char *)data, sizeof *data, 0);
    if (sendRes == SOCKET_ERROR) {
        fprintf(stderr, "Could not send data to server.\n");
        return 1;
    }
    
    int bytesReceived = recv(sock, (char*)game, sizeof *game, 0);
    if (bytesReceived == SOCKET_ERROR) {
        fprintf(stderr, "Error in getting response from server\n");
        return 1;
    }
    return 0;
}

SOCKET init_socket()
{
    WSADATA wsData;
    WORD ver = MAKEWORD(2,2);

    if (WSAStartup(ver, &wsData)) {
        fprintf(stderr, "can't init winsock.\n");
        return EXIT_FAILURE;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        puts("failed to create sock\n");
        return EXIT_FAILURE;
    }

    //	Create a hint structure for the server we're connecting with

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &hint.sin_addr);

    //	Connect to the server on the socket
    int connectRes = connect(sock, (struct sockaddr*)&hint, sizeof(hint));
    if (connectRes == SOCKET_ERROR)
    {
        puts("failed to connect()\n");
        return EXIT_FAILURE;
    }

    return sock;
}

void sock_cleanup(SOCKET sock)
{
    closesocket(sock);
}