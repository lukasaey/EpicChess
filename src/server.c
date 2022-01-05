#include <stdio.h>
#include <stdlib.h>

#include <WS2tcpip.h>

#include "../src/net.h"
#include "../src/game.h"
#include "../src/logic.h"


int receive(SOCKET sock, void* dest, size_t len)
{
    int received = recv(sock, (char *)dest, len, 0);
    if (received == SOCKET_ERROR) {
        fprintf(stderr, "error in recv().\n");
        return 1;
    }
    return 0;
}

void handle_move(game_t *game, GameData data)
{
    unsigned int origin = data.y1 * BOARD_N + data.x1;
    unsigned int dest = data.y2 * BOARD_N + data.x2;

    LegalInfo legal = is_legal(game, origin, dest);
    if (!legal.legal) {
        puts("returned");
        return;
    }

    if (legal.enpassant) {
        game->board[dest] = game->board[origin];
        game->board[game->en_passantable] = NO_PIECE;
        game->board[origin] = NO_PIECE;
        game->en_passantable = NONE_SELECTED;
        return;
    }

    uint8_t piece = game->board[origin];

    if (legal.castle) {
        int sign = (data.x1 == 7) ? -1 : 1;
        game->board[dest+(2*sign)] = game->board[dest];
        game->board[dest] = NO_PIECE;
        game->board[dest+sign] = game->board[origin];
        game->board[origin] = NO_PIECE;

        int *king_pos = (piece & WHITE) ? &game->white_king_pos : &game->black_king_pos;
        *king_pos = dest+sign;
        return;
    }

    if (piece & KING) {
        int *king_pos = (piece & WHITE) ? &game->white_king_pos : &game->black_king_pos;
        *king_pos = dest;
    }

    if (legal.firstmove) game->en_passantable = dest;

    game->board[dest] = game->board[origin];
    game->board[origin] = NO_PIECE;
}

int main(void)
{
    WSADATA wsData;
    WORD ver = MAKEWORD(2,2); /* asking for version 2.2 */

    if (WSAStartup(ver, &wsData)) {
        fprintf(stderr, "can't init winsock.\n");
        return EXIT_FAILURE;
    }

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        fprintf(stderr, "can't create socket.\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(listening, (struct sockaddr*)&hint, sizeof(hint));

    printf("listening on port %d\n", PORT);
    listen(listening, SOMAXCONN);

    struct sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(listening, (struct sockaddr*)&client, &clientSize);

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    if (!getnameinfo((struct sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0)) {
        printf("%s connected on port %s\n", host, service);
    } else {
        printf("%s connected on port %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    }

    closesocket(listening);

    Header head;
    game_t game = DEFAULT_GAME_T;

    int quit = 1;
    while(quit)
    {
        if (receive(clientSocket, &head, sizeof head)) {
            fprintf(stderr, "Error in receive()\n");
            break;
        }

        switch (head.type) 
        {
        case GAME_HEADER:
            GameData data;
            receive(clientSocket, &data, sizeof data);
            printf("client: %d, %d, %d, %d\n", data.x1, data.y1, data.x2, data.y2);
            handle_move(&game, data);
            game.player = (game.player == WHITE_PLAYER) ? BLACK_PLAYER : WHITE_PLAYER;
            puts("handled");
            send(clientSocket, (char*)&game, sizeof game, 0);
            puts("sent");
            break;
        case CHAT_HEADER: {break;}
        case EXIT_HEADER:
            quit = 0;
            break;
        }
    }

    closesocket(clientSocket);

    WSACleanup();

    return EXIT_SUCCESS;
}
