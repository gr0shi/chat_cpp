#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Client.h"
#include "inc.h"

Client::Client() : conn_stability(false) {
  cout << "Client Started." << endl;
  cout << endl;
}

Client::~Client() {
  if (conn_stability) closesocket(connection);
}

void Client::connect_to_server(string address, int port, string locID) {
  server_IP = address;
  server_port = port;

  struct hostent* host_info;
  unsigned int addr;
  struct sockaddr_in server;

  WSADATA wsaData;

  if (WSAStartup(0x101, &wsaData) != 0) {
    print_error("��������� ���������� WinSock �� �������", false);
    return;
  }

  connection = socket(AF_INET, SOCK_STREAM, 0);

  if (connection == INVALID_SOCKET) {
    print_error("������������ �����", false);
    return;
  }

  addr = inet_addr(server_IP.c_str());

  host_info = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);

  if (host_info == NULL) {
    print_error("�� ������� �������� ���������� � �������", false);
    closesocket(connection);
    return;
  }

  server.sin_addr.s_addr = *((unsigned long*)host_info->h_addr);
  server.sin_family = AF_INET;
  server.sin_port = htons(server_port);

  if (connect(connection, (struct sockaddr*)&server, sizeof(server))) {
    print_error("�� ������� ���������� ���������� � ��������", false);
    closesocket(connection);
    return;
  }
  send_msg(locID);
  conn_stability = true;
}



bool Client::send_msg(string text) {
  if (send(connection, text.c_str(), text.size() + 1, 0) == -1) return false;
  return true;
}

bool Client::receive_msg() {
  char message[4096];
  if (recv(connection, message, 4096, 0) == -1) return false;

  cout << "\b\b";

  string msg(message);
  size_t pos1 = msg.find("$");
  size_t pos2 = msg.find(": ");
  size_t fontNum = stoi(msg.substr(pos2 + 2, pos1 - pos2));
  string name = msg.substr(0, pos2);
  string text = msg.substr(pos1 + 1);

  set_font_color(fontNum);
  cout << name;
  set_font_color(7);
  cout << ": " << text << endl;
  cout << "> ";
  return true;
}

void Client::destroy() {
  conn_stability = false;
  closesocket(connection);
}

//void start_client() {
//  SetConsoleCP(1251);
//  SetConsoleOutputCP(1251);
//
//  FILE* file;
//  fopen_s(&file, "server.ini", "r");
//  if (file == NULL) {
//    print_error("��������� ������� server.ini", true);
//    return;
//  }
//
//  char buf[4096];
//  string server_address;
//
//  while ((fgets(buf, 4096, file)) != NULL) {
//    if (buf[0] == '#') continue;
//    server_address = buf;
//  }
//  fclose(file);
//
//  if (server_address.size() == 0) {
//    print_error("� server.ini ��� IP ������", true);
//    return;
//  }
//
//  cout << ">----------------------------------------------------------<"
//       << endl;
//  cout << endl;
//  cout << "\t\t����� ���������� � ���" << endl;
//  cout << "\t\t�� ��������� ��������" << endl;
//  cout << endl;
//  cout << "\t��������� �������: \tIP - " << server_address << endl;
//  cout << "\t\t\t\t���� - 10007" << endl;
//  cout << endl;
//  cout << "           ������� ���� ������: -help / -h" << endl;
//  cout << endl;
//  cout << " ��������� ������ ��������� ����� ���������� ������ �������" << endl;
//  cout << endl;
//  cout << ">----------------------------------------------------------<"
//       << endl;
//  cout << endl;
//  cout << "������� ��� ������������: ";
//
//  string locID;
//  cin >> locID;
//
//  system("cls");
//
//  cin.ignore();
//  client.connect_to_server(server_address.c_str(), 10007, locID);
//
//  if (!client.is_connected()) {
//    print_error("��������� ������������ � IP ������, ������������ � server.ini",
//                true);
//    return;
//  }
//
//  cout << ">----------------------------------------------------------<"
//       << endl;
//  cout << endl;
//  cout << "\t��������� �������: \tIP - " << server_address << endl;
//  cout << "\t\t\t\t���� - 10007" << endl;
//  cout << "\t\t\t\t���� ��� - " << locID << endl;
//  cout << endl;
//  cout << "           ������� ���� ������: -help / -h" << endl;
//  cout << endl;
//  cout << " ��������� ������ ��������� ����� ���������� ������ �������" << endl;
//  cout << endl;
//  cout << ">----------------------------------------------------------<"
//       << endl;
//  cout << endl;
//
//  DWORD threadId;
//  CreateThread(NULL, NULL, message_rec_thread, NULL, NULL, &threadId);
//
//  cout << "> ";
//  while (gets_s(buf)) {
//    if (strlen(buf) == 0) {
//      cout << endl;
//      cout << "�� ���������� ������ �������" << endl;
//      client.destroy();
//      break;
//    }
//    if (!client.send_msg(buf)) {
//      print_error("��������� ������������. ���������, ������� �� ������",
//                  false);
//      break;
//    }
//    cout << "> ";
//  }
//
//  cout << "������ ������� �����������" << endl;
//  cout << endl;
//  system("pause");
//}
