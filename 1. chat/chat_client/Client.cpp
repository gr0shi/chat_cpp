#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "Client.h"

Client client_entity;

int countus = 0;

const std::string currentTime() {
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%X", &tstruct);

  return buf;
}

void print_error(string text, bool critical) {
  cout << endl;
  cout << ">--------------------------- Ошибка ----------------------------<" << endl;
  cout << "                                                                 " << endl;
  cout << "                        Описание ошибки:                         " << endl;
  cout << "                                                                 " << endl;
  cout << " " << text << endl;
  cout << "                                                                 " << endl;
  cout << ">---------------------------------------------------------------<" << endl;
  cout << endl;

  if (critical) {
    cout << "Работа приложения остановлена" << endl;
    cout << endl;
    system("pause");
  }
}

void set_font_color(size_t c) {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, c);
}

unsigned long __stdcall MessageRecThread(void* pParam) {
  do {
    if (!client_entity.receive_msg()) {
      cout << "\b\b" << endl;
      break;
    }
  } while (client_entity.is_connected());
  return 0;
}

Client::Client() : conn_stability(false) {
  cout << "Client Started." << endl;
  cout << endl;
}

void Client::connect_to_server(string address, int port, string locID) {
  server_IP = address;
  server_port = port;

  struct hostent* host_info;
  unsigned int addr;
  struct sockaddr_in server;

  WSADATA wsaData;

  if (WSAStartup(0x101, &wsaData) != 0) {
    print_error("Инициация библиотеки WinSock не удалась", false);
    return;
  }

  connection = socket(AF_INET, SOCK_STREAM, 0);

  if (connection == INVALID_SOCKET) {
    print_error("Неправильный сокет", false);
    return;
  }

  addr = inet_addr(server_IP.c_str());

  host_info = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);

  if (host_info == NULL) {
    print_error("Не удалось получить информацию о сервере", false);
    closesocket(connection);
    return;
  }

  server.sin_addr.s_addr = *((unsigned long*)host_info->h_addr);
  server.sin_family = AF_INET;
  server.sin_port = htons(server_port);

  if (connect(connection, (struct sockaddr*)&server, sizeof(server))) {
    print_error("Не удается установить соединение с сервером", false);
    closesocket(connection);
    return;
  }
  send_msg(locID);
  conn_stability = true;
}

Client::~Client() {
  if (conn_stability) 
    closesocket(connection);
}

bool Client::send_msg(string text) {
  if (send(connection, text.c_str(), text.size() + 1, 0) == -1) 
    return false;
  return true;
}

bool Client::receive_msg() {
  char message[4096];
  if (recv(connection, message, 4096, 0) == -1) 
    return false;

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

void main() {
  SetConsoleCP(1251);
  SetConsoleOutputCP(1251);

  FILE* file;
  fopen_s(&file, "server.ini", "r");
  if (file == NULL) {
    print_error("Неудалось открыть server.ini", true);
    return;
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
    return;
  }

  cout << ">---------------------------------------------------------------<" << endl;
  cout << "                                                                 " << endl;
  cout << "                    Добро пожаловать в чат                       " << endl;
  cout << "                    Вы являетесь клиентом                        " << endl;
  cout << "                                                                 " << endl;
  cout << "         Параметры сервера:          IP - " << server_address      << endl;
  cout << "                                     Порт - 10007                " << endl;
  cout << "                                                                 " << endl;
  cout << "   Отправьте пустое сообщение чтобы прекратить работу сервера    " << endl;
  cout << "                                                                 " << endl;
  cout << ">---------------------------------------------------------------<" << endl;
  cout << "                                                                 " << endl;
  cout << "Введите имя пользователя: ";

  string locID;
  cin >> locID;

  system("cls");

  cin.ignore();
  client_entity.connect_to_server(server_address.c_str(), 10007, locID);

  if (!client_entity.is_connected()) {
    print_error("Неудалось подключиться к IP адресу, привязанному к server.ini", true);
    return;
  }

  cout << ">---------------------------------------------------------------<" << endl;
  cout << "                                                                 " << endl;
  cout << "                    Добро пожаловать в чат                       " << endl;
  cout << "                    Вы являетесь клиентом                        " << endl;
  cout << "                                                                 " << endl;
  cout << "         Параметры сервера:          IP - " << server_address      << endl;
  cout << "                                     Порт - 10007                " << endl;
  cout << "                                     Ваше имя - " << locID         << endl;
  cout << "                                                                 " << endl;
  cout << "   Отправьте пустое сообщение чтобы прекратить работу сервера    " << endl;
  cout << "                                                                 " << endl;
  cout << ">---------------------------------------------------------------<" << endl;
  cout << "                                                                 " << endl;

  DWORD threadId;
  CreateThread(NULL, NULL, MessageRecThread, NULL, NULL, &threadId);

  cout << "> ";
  while (gets_s(buf)) {
    if (strlen(buf) == 0) {
      cout << endl;
      cout << "Вы остановили работу клиента" << endl;
      client_entity.destroy();
      break;
    }
    if (!client_entity.send_msg(buf)) {
      print_error("Неудалось подключиться. Проверьте, запущен ли сервер", false);
      break;
    }
    cout << "> ";
  }

  cout << "Работа клиента остановлена" << endl;
  cout << endl;
  system("pause");
}
