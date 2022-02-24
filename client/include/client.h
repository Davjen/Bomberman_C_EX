#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "bomberman.h"


typedef struct pk_handshake{
   char pk_type;
   int ID;
} pk_handshake_t;

typedef struct pk_position{
    char pk_type;
    int ID;
    float x_pos;
    float y_pos;
} pk_position_t;

typedef struct pk_disconnect{
    char pk_type;
    int ID;
}pk_disconnect_t;

typedef struct pk_check_alive{
    char pk_type;
    int ID;
} pk_check_alive_t;

int client_init();
int _set_timeout();
void send_packet(char *packet, int len);
void send_handshaking_req();
void send_alive_packet(clock_t *client_start);
void send_position(float x_pos,float y_pos);
bool receive_data(bomberman_t *remote_player);
bool _alive_check_cd(clock_t *client_start);
bool verify_handshacking();
bool handshaking_verifier();