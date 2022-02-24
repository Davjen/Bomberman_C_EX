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

#include "client.h"
#include <time.h>
#include <SDL.h>
#include <bomberman.h>

#define PUBLIC_SERVER "192.168.1.142"
// #define PUBLIC_SERVER "95.238.236.101"
#define SERVER_PORT 9999
#define ALIVE_CHECK_TIMER 3

#define PK_DISCONNECTED 'A'
#define PK_ALIVE 'B'
#define PK_POSITION 'C'
#define PK_HANDSHAKE 'D'

#define POSITION_DELTA 12

int new_socket;
struct sockaddr_in sk_in;
int _ID= -1;

int client_init(){

 #ifdef _WIN32
    // this part is only required on Windows: it initializes the Winsock2 dll
    WSADATA wsa_data;
    if (WSAStartup(0x0202, &wsa_data))
    {
        return -1;
    }
    #endif
    new_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (new_socket<0)
    {
        return -2;
    }
    inet_pton(AF_INET, PUBLIC_SERVER, &sk_in.sin_addr);
    sk_in.sin_family = AF_INET;
    sk_in.sin_port = htons(SERVER_PORT);

    _set_timeout();
    return 0;
}

int _set_timeout()
{
    unsigned int timeout = 5;
    if (setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(unsigned int)))
    {
        return -1;
    }

    int current_buffer_size = 0;
    int sockopt_len = sizeof(int);
    getsockopt(new_socket, SOL_SOCKET, SO_RCVBUF, (char*)&current_buffer_size, &sockopt_len);

    int buffer_size = 8192;
    if (setsockopt(new_socket, SOL_SOCKET, SO_RCVBUF, (char *)&buffer_size, sizeof(int)))
    {
        return -2;
    }

    if (setsockopt(new_socket, SOL_SOCKET, SO_SNDBUF, (char *)&buffer_size, sizeof(int)))
    {
        return -3;
    }

    getsockopt(new_socket, SOL_SOCKET, SO_RCVBUF, (char*)&current_buffer_size, &sockopt_len);

    return 0;
}

void send_packet(char *packet, int len){
    int sent_bytes = sendto(new_socket, packet, len, 0, (struct sockaddr *)&sk_in, sizeof(sk_in));
    char addr_as_string[64];
    inet_ntop(AF_INET, &sk_in.sin_addr, addr_as_string, 64);
}

void send_handshaking_req(){

  pk_handshake_t pk = {PK_HANDSHAKE,_ID};
  send_packet(&pk,sizeof(pk_handshake_t));
}

void send_alive_packet(clock_t *client_start){
  if(_alive_check_cd(client_start)){
            
    pk_check_alive_t data = {PK_ALIVE,_ID};
    send_packet(&data,sizeof(pk_check_alive_t));
  }
}

void send_position(float x_pos,float y_pos){
  pk_position_t data = {PK_POSITION,_ID,x_pos,y_pos};
  send_packet(&data,sizeof(pk_position_t));

}


bool handshaking_verifier(){
  char buffer[8192];
  int sender_in_size = sizeof(sk_in);
  int len = recvfrom(new_socket, buffer, 8191, 0, (struct sockaddr*)&sk_in, &sender_in_size);
  if (len > 0)
  {
      char addr_as_string[64];
      inet_ntop(AF_INET, &sk_in.sin_addr, addr_as_string, 64);
      printf("received %d bytes from %s:%d\n", len, addr_as_string, ntohs(sk_in.sin_port));

      char type;
      SDL_memcpy(&type, buffer, sizeof(char));

      if (PK_HANDSHAKE==type){

          pk_handshake_t handshake_pk;
          SDL_memcpy(&handshake_pk, buffer, sizeof(pk_handshake_t));
          _ID = handshake_pk.ID;
          return true;
      }
  }
  else
  {
        int socket_error = WSAGetLastError();
        if (socket_error == WSAETIMEDOUT)
        {
            return false;
        }
        else
        {
            printf("something went wrong: %d\n", socket_error);
            return false;
        }
    }
}

bool receive_data(bomberman_t *remote_player){
  char buffer[8192];
  int sender_in_size = sizeof(sk_in);
  int len = recvfrom(new_socket, buffer, 8191, 0, (struct sockaddr*)&sk_in, &sender_in_size);

  if (len > 0)
  {
      char addr_as_string[64];
      inet_ntop(AF_INET, &sk_in.sin_addr, addr_as_string, 64);
      // printf("received %d bytes from %s:%d\n", len, addr_as_string, ntohs(sk_in.sin_port));

      char type;
      SDL_memcpy(&type, buffer, sizeof(char));

      if (PK_HANDSHAKE==type){

          pk_handshake_t handshake_pk;
          SDL_memcpy(&handshake_pk, buffer, sizeof(pk_handshake_t));
          _ID = handshake_pk.ID;
          return true;
      }
      if(PK_POSITION==type){

          pk_position_t pos_packet;
          SDL_memcpy(&pos_packet, buffer, sizeof(pk_position_t));
          printf("posizione ricevuta %d \n",pos_packet.x_pos);
          remote_player->movable.x= pos_packet.x_pos;
          remote_player->movable.y = pos_packet.y_pos;
          remote_player->drawable.rect.x = pos_packet.x_pos;
          remote_player->drawable.rect.y = pos_packet.y_pos;
          
          return true;
      }
      return false;
    }
    else if (len <= 0)
    {
        int socket_error = WSAGetLastError();
        if (socket_error == WSAETIMEDOUT)
        {
            return false;
        }
        else
        {
            printf("something went wrong: %d\n", socket_error);
            return false;
        }
    }
    return false;
}



bool _alive_check_cd(clock_t *client_start){
  if(((clock()-*client_start)/CLOCKS_PER_SEC)>ALIVE_CHECK_TIMER){
    *client_start=clock();
    return true;
  }
  return false;
}

bool verify_handshacking(){
  printf("%d handshaking check\n",_ID);
  return _ID==1;
}