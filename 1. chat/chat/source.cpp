#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "Client.h"
#include "Server.h"
#include "inc.h"

using namespace std;

Server server;
Client client;
bool manualend = false;

unsigned long __stdcall listen_thread(void* pParam) {
  while (server.start_listening());
  return 0;
}

unsigned long __stdcall message_rec_thread(void* pParam) {
  do {
    if (!client.receive_msg()) break;
  } while (client.is_connected());
  return 0;
}

int main() {
  SetConsoleCP(1251);
  SetConsoleOutputCP(1251);

  system("cls");
  
  cout << "hello" << endl;
  int abc;
  cin >> abc;

  if (abc == 1) {
    system("cls");

    server.start();
    if (!server.is_connected()) {
      print_error("Не удалось инициализировать серверный сокет", true);
      return -1;
    }

    char buf[4096];

    cout << ">---------------------------------------------------------------<" << endl;
    cout << "                                                                 " << endl;
    cout << "                    Добро пожаловать в чат                       " << endl;
    cout << "                    Вы являетесь сервером                        " << endl;
    cout << "                                                                 " << endl;
    cout << "         Параметры сервера:          IP - 127.0.0.1              " << endl;
    cout << "                                     Порт - 10007                " << endl;
    cout << "                                                                 " << endl;
    cout << "               Вызвать меню помощи: -help / -h                   " << endl;
    cout << "                                                                 " << endl;
    cout << "   Отправьте пустое сообщение чтобы прекратить работу сервера    " << endl;
    cout << "                                                                 " << endl;
    cout << ">---------------------------------------------------------------<" << endl;
    cout << "                                                                 " << endl;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);

    DWORD threadId;
    CreateThread(NULL, NULL, listen_thread, NULL, NULL, &threadId);

    cout << "> ";
    while (gets_s(buf)) {
      if (strlen(buf) == 0) {
        cout << endl;
        cout << "Вы остановили работу сервера" << endl;
        manualend = true;
        server.send_termination();
        break;
      }
      if (!server.send_msg_from_server(buf, "System")) {
        print_error("Невозможно подключиться к клиентам", false);
        break;
      }
      cout << "> ";
    }

    cout << "Сервер остановлен" << endl;
    cout << endl;
    system("pause");
  } else if (abc == 2) {
    system("cls");

    FILE* file;
    fopen_s(&file, "server.ini", "r");
    if (file == NULL) {
      print_error("Неудалось открыть server.ini", true);
      return -1;
    }

    char buf[4096];
    string server_address;

    while ((fgets(buf, 4096, file)) != NULL) {
      if (buf[0] == '#') continue;
      server_address = buf;
    }
    fclose(file);

    if (server_address.size() == 0) {
      print_error("В server.ini нет IP адреса", true);
      return -1;
    }

    cout << ">----------------------------------------------------------<"
         << endl;
    cout << endl;
    cout << "\t\tДобро пожаловать в чат" << endl;
    cout << "\t\tВы являетесь клиентом" << endl;
    cout << endl;
    cout << "\tПараметры сервера: \tIP - " << server_address << endl;
    cout << "\t\t\t\tПорт - 10007" << endl;
    cout << endl;
    cout << "           Вызвать меню помощи: -help / -h" << endl;
    cout << endl;
    cout << " Отправьте пустое сообщение чтобы прекратить работу клиента"
         << endl;
    cout << endl;
    cout << ">----------------------------------------------------------<"
         << endl;
    cout << endl;
    cout << "Введите имя пользователя: ";

    string locID;
    cin >> locID;

    system("cls");

    cin.ignore();
    client.connect_to_server(server_address.c_str(), 10007, locID);

    if (!client.is_connected()) {
      print_error(
          "Неудалось подключиться к IP адресу, привязанному к server.ini",
          true);
      return -1;
    }

    cout << ">----------------------------------------------------------<"
         << endl;
    cout << endl;
    cout << "\tПараметры сервера: \tIP - " << server_address << endl;
    cout << "\t\t\t\tПорт - 10007" << endl;
    cout << "\t\t\t\tВаше имя - " << locID << endl;
    cout << endl;
    cout << "           Вызвать меню помощи: -help / -h" << endl;
    cout << endl;
    cout << " Отправьте пустое сообщение чтобы прекратить работу клиента"
         << endl;
    cout << endl;
    cout << ">----------------------------------------------------------<"
         << endl;
    cout << endl;

    DWORD threadId;
    CreateThread(NULL, NULL, message_rec_thread, NULL, NULL, &threadId);

    cout << "> ";
    while (gets_s(buf)) {
      if (strlen(buf) == 0) {
        cout << endl;
        cout << "Вы остановили работу клиента" << endl;
        client.destroy();
        break;
      }
      if (!client.send_msg(buf)) {
        print_error(
            "Неудалось подключиться. Проверьте, запущен ли сервер", false);
        break;
      }
      cout << "> ";
    }

    cout << "Работа клиента остановлена" << endl;
    cout << endl;
    system("pause");
  }
  return 0;
}
