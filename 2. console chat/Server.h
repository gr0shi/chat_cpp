#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <WS2tcpip.h>
#include <WinSock2.h>

#include <iostream>
#include <thread>

class Server {
 public:
  Server();
  Server(const u_short port);
  ~Server();
  int start();
  void destroy();
  int start_listening();
  int handle_connection();
  int rec_client();
  int send_msg_from_server(char* message);

 private: 
  WSADATA WData;
  struct addrinfo *result = NULL,
                  *ptr = NULL,
                  hints;
  SOCKET listenSocket = INVALID_SOCKET;
  SOCKET ClientSocket;
  int iResult;
};