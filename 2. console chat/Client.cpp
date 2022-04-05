#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "Client.h"

void show_message_client(char* message, int lenght) {
  std::cout << "\nServer: ";
  for (int i = 0; i < lenght; i++) std::cout << message[i];
  std::cout << std::endl;
}

void print_error_client(const char* c, bool critical) {
  std::cout << std::endl;
  std::cout << ">----------------- Ошибка ------------------<" << std::endl;
  std::cout << std::endl;
  std::cout << " " << c << std::endl;
  std::cout << std::endl;
  std::cout << ">-------------------------------------------<" << std::endl;
  std::cout << std::endl;

  if (critical) {
    std::cout << "Работа сервера остановлена" << std::endl;
    std::cout << std::endl;
    system("pause");
  }
}

Client::Client() {
  iResult = WSAStartup(MAKEWORD(2, 2), &WData);
  if (iResult != 0) {
    std::cout << "\nWSASturtup: " << iResult;
    exit(1);
  }
  clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientSock == INVALID_SOCKET) {
    print_error_client("Ошибка сокета", false);
    WSACleanup();
    exit(1);
  }
}

Client::~Client() {
  closesocket(clientSock);
  WSACleanup();
}

int Client::connect_to_server() {
  sockaddr_in clientService;
  clientService.sin_family = AF_INET;
  clientService.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
  clientService.sin_port = htons(27015);
  iResult = connect(clientSock, reinterpret_cast<SOCKADDR*>(&clientService), sizeof(clientService));
  if (iResult != 0) {
    print_error_client("Ошибка подключения", false);
    WSACleanup();
    return -1;
  }

  std::thread th(&Client::receive_msg, this);
  th.detach();
  std::cout << ">---------------------------------------<" << std::endl;
  std::cout << std::endl;
  std::cout << "Вы являетесь клиентом" << std::endl;
  std::cout << std::endl;
  std::cout << "Вызвать меню помощи: -help / -h" << std::endl;
  std::cout << std::endl;
  std::cout << "Отправьте пустое сообщение чтобы прекратить работу клиента" << std::endl;
  std::cout << std::endl;
  std::cout << ">---------------------------------------<" << std::endl;
  std::cout << std::endl;
  return 0;
}

void Client::destroy() { Client::~Client(); }

int Client::send_msg(char* message) {
  iResult = send(clientSock, message, strlen(message), 0);
  if (iResult < 0) {
    print_error_client("Ошибка отправки", false);
    return -1;
  }
  return 0;
}

int Client::receive_msg() {
  char recvbuf[1024];
  do {
    iResult = recv(clientSock, recvbuf, 1024, 0);
    if (iResult > 0) {
      show_message_client(recvbuf, iResult);
    } else {
      print_error_client("Соединение закрыто", false);
      closesocket(clientSock);
      WSACleanup();
      return -1;
    }
  } while (iResult > 0);
  return 0;
}