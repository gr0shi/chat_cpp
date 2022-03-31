#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Client.h"

Client ClientEntity;

void printError(string text, bool critical) {
  cout << endl;
  cout << ">--------------------------- Ошибка ----------------------------<" << endl;
  cout << endl;
  cout << boolalpha << "\t\t\tОписание ошибки:" << endl;
  cout << endl;
  cout << " " << text << endl;
  cout << endl;
  cout << ">---------------------------------------------------------------<" << endl;
  cout << endl;
  ;
  if (critical) {
    cout << "Работа клиента остановлена" << endl;
    cout << endl;
    system("pause");
  }
}

void setFontColor(size_t c) {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, c);
}

unsigned long __stdcall MessageRecThread(void* pParam);

Client::Client() : conn_stability(false) {
  cout << "Client Started." << endl;
  cout << endl;
}

void Client::ConnectToServer(string address, int port, string locID) {
  serverIP = address;
  serverPort = port;

  struct hostent* hostInfo;
  unsigned int addr;
  struct sockaddr_in server;

  WSADATA wsaData;

  if (WSAStartup(0x101, &wsaData) != 0) {
    printError("Инициация библиотеки WinSock не удалась", false);
    return;
  }

  connection = socket(AF_INET, SOCK_STREAM, 0);

  if (connection == INVALID_SOCKET) {
    printError("Неправильный сокет", false);
    return;
  }

  addr = inet_addr(serverIP.c_str());

  hostInfo = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);

  if (hostInfo == NULL) {
    printError("Не удалось получить информацию о сервере", false);
    closesocket(connection);
    return;
  }

  server.sin_addr.s_addr = *((unsigned long*)hostInfo->h_addr);
  server.sin_family = AF_INET;
  server.sin_port = htons(serverPort);

  if (connect(connection, (struct sockaddr*)&server, sizeof(server))) {
    printError("Не удается установить соединение с сервером", false);
    closesocket(connection);
    return;
  }
  SendMsg(locID);
  conn_stability = true;
}

Client::~Client() {
  if (conn_stability) 
    closesocket(connection);
}

bool Client::SendMsg(string text) {
  if (send(connection, text.c_str(), text.size() + 1, 0) == -1) 
    return false;
  return true;
}

bool Client::ReceiveMsg() {
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

  setFontColor(fontNum);
  cout << name;
  setFontColor(7);
  cout << ": " << text << endl;
  cout << "> ";
  return true;
}

unsigned long __stdcall MessageRecThread(void* pParam) {
  do {
    if (!ClientEntity.ReceiveMsg()) {
      cout << "\b\b" << endl;
      break;
    }
  } while (ClientEntity.IsConnected());
  return 0;
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
    printError("Неудалось открыть server.ini", true);
    return;
  }

  char buf[4096];
  string serverAddress;
  while ((fgets(buf, 4096, file)) != NULL) {
    if (buf[0] == '#') continue;
    serverAddress = buf;
  }
  fclose(file);

  if (serverAddress.size() == 0) {
    printError("В server.ini нет IP адреса", true);
    return;
  }
  cout << ">----------------------------------------------------------<" << endl;
  cout << endl;
  cout << "\t\tДобро пожаловать в чат" << endl;
  cout << "\t\tВы являетесь клиентом" << endl;
  cout << endl;
  cout << "\tПараметры сервера: \tIP - " << serverAddress << endl;
  cout << "\t\t\t\tПорт - 10007" << endl;
  cout << endl;
  cout << "           Вызвать меню помощи: -help / -h" << endl;
  cout << endl;
  cout << " Отправьте пустое сообщение чтобы прекратить работу клиента" << endl;
  cout << endl;
  cout << ">----------------------------------------------------------<" << endl;
  cout << endl;
  cout << "Введите имя пользователя: ";

  string locID;
  cin >> locID;

  system("cls");

  cin.ignore();
  ClientEntity.ConnectToServer(serverAddress.c_str(), 10007, locID);
  if (!ClientEntity.IsConnected()) {
    printError("Неудалось подключиться к IP адресу, привязанному к server.ini", true);
    return;
  }

  cout << ">----------------------------------------------------------<" << endl;
  cout << endl;
  cout << "\tПараметры сервера: \tIP - " << serverAddress << endl;
  cout << "\t\t\t\tПорт - 10007" << endl;
  cout << "\t\t\t\tВаше имя - " << locID << endl;
  cout << endl;
  cout << "           Вызвать меню помощи: -help / -h" << endl;
  cout << endl;
  cout << " Отправьте пустое сообщение чтобы прекратить работу клиента" << endl;
  cout << endl;
  cout << ">----------------------------------------------------------<" << endl;
  cout << endl;

  DWORD threadId;
  CreateThread(NULL, NULL, MessageRecThread, NULL, NULL, &threadId);
  cout << "> ";
  while (gets_s(buf)) {
    if (strlen(buf) == 0) {
      cout << endl;
      cout << "Вы остановили работу клиента" << endl;
      ClientEntity.destroy();
      break;
    }
    if (!ClientEntity.SendMsg(buf)) {
      printError("Неудалось подключиться. Проверьте, запущен ли сервер", false);
      break;
    }
    cout << "> ";
  }

  cout << "Работа клиента остановлена" << endl;
  cout << endl;
  system("pause");
}
