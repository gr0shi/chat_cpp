#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <WS2tcpip.h>
#include <WinSock2.h>

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <stdio.h>
#include <time.h>

using namespace std;

class Client {
 public:
  Client();
  ~Client();
  void connect_to_server(string, int, string);
  void destroy();
  bool send_msg(string);
  bool receive_msg();
  bool is_connected() { return conn_stability; }

 private:
  bool conn_stability;
  string server_IP;
  int server_port;
  SOCKET connection;
};
