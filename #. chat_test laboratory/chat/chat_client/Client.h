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
  void ConnectToServer(string, int, string);
  bool SendMsg(string);
  bool ReceiveMsg();
  bool IsConnected() { return conn_stability; }
  void destroy();

 private:
  bool conn_stability;
  string serverIP;
  int serverPort;
  SOCKET connection;
};
