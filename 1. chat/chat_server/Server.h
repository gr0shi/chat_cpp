#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <WS2tcpip.h>
#include <WinSock2.h>

#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <ctime>
#include <string>
#include <Windows.h>
#include <stdio.h>
#include <time.h>

using namespace std;

class Server {
 public:
  Server();
  Server(const u_short port);
  ~Server();
  void destroy();
  void send_termination();
  bool start_listening();
  bool rec_client(SOCKET);
  bool send_msg_from_server(string, string);
  bool is_connected() { return conn_stability; }

 private:
  bool conn_stability;
  list<pair<SOCKET, string>> clients_list;
  SOCKET receiver;
  map<string, size_t> client_colors;
};