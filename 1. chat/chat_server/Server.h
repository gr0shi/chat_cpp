#pragma once
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <Windows.h>

#include <ctime>
#include <iostream>
#include <list>
#include <map>
#include <string>

using namespace std;

class Server {
 public:
  Server();
  ~Server();
  bool is_connected() { return conn_stability; }
  bool start_listening();
  bool send_msg_from_server(string, string);
  bool rec_client(SOCKET);
  void send_termination();
  void destroy();

 private:
  bool conn_stability;
  list<pair<SOCKET, string>> clients_list;
  SOCKET receiver;
  map<string, size_t> client_colors;
};