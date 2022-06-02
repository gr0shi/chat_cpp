#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <WS2tcpip.h>
#include <WinSock2.h>

#include <iostream>
#include <thread>
#include <string>

class Client {
 public:
  Client();
  ~Client();
  int connect_to_server();
  void destroy();
  int send_msg(char* message);
  int receive_msg();

  private:
  WSADATA WData;
  char* buff;
  int iResult;
  SOCKET clientSock = INVALID_SOCKET;
};