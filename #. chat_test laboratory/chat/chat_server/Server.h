#pragma once
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <Windows.h>

using namespace std;

class Server {
 public:
  Server();
  ~Server();
  bool IsConnected() { return conn_stability; }
  bool StartListening();
  bool SendMessageToAll(string, string);
  bool RecClient(SOCKET);
  void sendTermination();
  void destroy();

 private:
  bool conn_stability;
  list<pair<SOCKET, string>> clientsList;
  SOCKET receiver;
  map<string, size_t> cliColors;
};
