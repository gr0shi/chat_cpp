#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>

#include "Client.h"
#include "Server.h"

using namespace std;

int main() {
  setlocale(LC_ALL, "ru");
  int check;
  cout << ">---------------------------------------<" << endl;
  cout << endl;
  cout << "Добро пожаловать в чат" << endl;
  cout << endl;
  cout << "Вызвать меню помощи: -help / -h" << endl;
  cout << endl;
  cout << "Отправьте пустое сообщение чтобы прекратить работу клиента" << endl;
  cout << endl;
  cout << "Выбирите: " << endl;
  cout << "1 - Для работы сервера | 2 - Для работы клиента" << endl;
  cout << endl;
  cout << ">---------------------------------------<" << endl;
  cout << endl;
  cin >> check;

  system("cls");

  string line;
  char* message;

  if (check == 1) {
    Server server;
    server.start();

    do {
      //	std::cout << "\nServer: ";
      cin >> line;
      if (line == "#stop")
        server.destroy();
      else {
        message = new char[line.length() + 1];
        strcpy(message, line.c_str());
        server.send_msg_from_server(message);
      }
    } while (line != "#stop");
  } else if(check == 2) {
    Client client;
    client.connect_to_server();

    do {
      //	std::cout << "\nClient: ";
      cin >> line;
      if (line == "#exit")
        client.destroy();
      else {
        message = new char[line.length() + 1];
        strcpy(message, line.c_str());
        client.send_msg(message);
      }
    } while (line != "#exit");
  } /*else if (check == "-h" ||) {*/

  cout << endl;
  return 0;
}