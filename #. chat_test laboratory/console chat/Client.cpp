#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "Client.h"

void showMessageC(char* message, int lenght) {
  std::cout << "\nServer: ";
  for (int i = 0; i < lenght; i++) std::cout << message[i];
  std::cout << "\n";
}

Client::Client() {
  iResult = WSAStartup(MAKEWORD(2, 2), &WData);
  if (iResult != 0) {
    std::cout << "\nWSASturtup: " << iResult;
    exit(1);
  }
  clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientSock == INVALID_SOCKET) {
    std::cout << "\nОшибка сокета: " << WSAGetLastError();
    WSACleanup();
    exit(1);
  }
}
Client::~Client() {
  closesocket(clientSock);
  WSACleanup();
}
int Client::connectToServer() {
  sockaddr_in clientService;
  clientService.sin_family = AF_INET;
  clientService.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
  clientService.sin_port = htons(27015);
  iResult = connect(clientSock, reinterpret_cast<SOCKADDR*>(&clientService),
                    sizeof(clientService));
  if (iResult != 0) {
    std::cout << "Ошибка подключения: " << WSAGetLastError() << std::endl;
    WSACleanup();
    return -1;
  }

  std::thread th(&Client::ReceiveMsg, this);
  th.detach();

    std::cout << "\nПодключение к серверу..." << std::endl;
  std::cout << ">---------------------------------------<" << std::endl;
  std::cout << std::endl;
  std::cout << "Вы являетесь клиентом" << std::endl;
  std::cout << std::endl;
  std::cout << "Вызвать меню помощи: -help / -h" << std::endl;
  std::cout << std::endl;
  std::cout << "Отправьте пустое сообщение чтобы прекратить работу клиента"
            << std::endl;

  std::cout << std::endl;
  std::cout << ">---------------------------------------<" << std::endl;
  std::cout << std::endl;
  return 0;
}
void Client::destroy() { Client::~Client(); }
int Client::SendMsg(char* message) {
  iResult = send(clientSock, message, strlen(message), 0);
  if (iResult < 0) {
    std::cout << "\nОшибка отправки:" << WSAGetLastError();
    return -1;
  }
  return 0;
}

int Client::ReceiveMsg() {
  char recvbuf[1024];
  do {
    iResult = recv(clientSock, recvbuf, 1024, 0);
    if (iResult > 0) {
      showMessageC(recvbuf, iResult);
    } else {
      std::cout << "\nСоединение закрыто " << WSAGetLastError();
      closesocket(clientSock);
      WSACleanup();
      return -1;
    }
  } while (iResult > 0);
  return 0;
}