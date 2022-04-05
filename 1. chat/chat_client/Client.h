 #pragma once
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

class Client {
 public:
  Client();
  ~Client();
  void connect_to_server(string, int, string);
  bool send_msg(string);
  bool receive_msg();
  bool is_connected() { return conn_stability; }
  void destroy();

 private:
  bool conn_stability;
  string server_IP;
  int server_port;
  SOCKET connection;
};
