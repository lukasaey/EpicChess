#ifndef NET_STRUCT
#define NET_STRUCT

#include <sys/types.h>
#include <WS2tcpip.h>

#include "game.h"

#define PORT 6969
#define IP "127.0.0.1"

/* the compiler warning was annoying, so i defined it myself */
INT WSAAPI inet_pton(
    INT   Family,
    PCSTR pszAddrString,
    PVOID pAddrBuf
);

enum EDataType {
    EXIT_HEADER,
    GAME_HEADER,
    CHAT_HEADER,
};

typedef struct {
    enum EDataType type;
} Header;

typedef struct {
    int x1, y1, x2, y2;
} GameData;

SOCKET init_socket();
int send_move(SOCKET sock, game_t *game, Header *head, GameData *data);

#endif /* NET_STRUCT */
