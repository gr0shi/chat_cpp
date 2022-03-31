#include "Server.h"

void showMsg(char* message, int lenght) {
  std::cout << "\Client: ";
  for (int i = 0; i < lenght; i++) std::cout << message[i];
  std::cout << std::endl;
}

void printError(const char* c, bool critical) {
  std::cout << std::endl;
  std::cout << ">----------------- Ошибка ------------------<" << std::endl;
  std::cout << std::endl;
  std::cout << "критическая ошибка" << std::endl;
  std::cout << std::endl; 
  std::cout << c << std::endl;
  std::cout << std::endl;
  std::cout << ">-------------------------------------------<" << std::endl;
  std::cout << std::endl;
  ;
  if (critical) {
    std::cout << "Работа сервера остановлена" << std::endl;
    std::cout << std::endl;
    system("pause");
  }
}

Server::Server() {
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
}

Server::Server(const u_short port) {
  // this->port = port;
}

Server::~Server() {
  closesocket(listenSocket);
  WSACleanup();
}

int Server::start() {
  iResult = WSAStartup(MAKEWORD(2, 2), &WData);
  if (iResult != 0) {
    printError("Инициация библиотеки WinSock не удалась", false);
    return 1;
  }

  iResult = getaddrinfo(NULL, "27015", &hints, &result);
  if (iResult != 0) {
    printError("Не удалось получить addrinfo", false);
    WSACleanup();
    return 1;
  }

  listenSocket =
      socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (listenSocket == INVALID_SOCKET) {
    printError("Не удалось создать сокет сокет", false);
    freeaddrinfo(result);
    WSACleanup();
    return 1;
  }

  iResult = bind(listenSocket, result->ai_addr, result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    printError("Не удалось связать сокет", false);
    freeaddrinfo(result);
    closesocket(listenSocket);
    WSACleanup();
    return 1;
  }
  freeaddrinfo(result);

  if (StartListening() == 1) return 1;

  return 0;
}
int Server::StartListening() {
  std::cout << "\nОжидание подключиний..." << std::endl;
  std::cout << ">---------------------------------------<" << std::endl;
  std::cout << std::endl;
  std::cout << "Вы являетесь сервером" << std::endl;
  std::cout << std::endl;
  std::cout << "Вызвать меню помощи: -help / -h" << std::endl;
  std::cout << std::endl;
  std::cout << "Отправьте пустое сообщение чтобы прекратить работу клиента"
            << std::endl;
  
  std::cout << std::endl;
  std::cout << ">---------------------------------------<" << std::endl;
  std::cout << std::endl;


  if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
    printError("Прослушивание приостановленно.", false);
    closesocket(listenSocket);
    WSACleanup();
    return 1;
  }

  if (handleConn() == 1) return 1;

  std::thread th(&Server::handleConn, this);
  th.detach();

  return 0;
}

int Server::handleConn() {
  ClientSocket = INVALID_SOCKET;
  ClientSocket = accept(listenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
    printError("Неудалось принять данные.", false);
    closesocket(listenSocket);
    WSACleanup();
    return 1;
  }

  std::thread th(&Server::RecClient, this);
  th.detach();

  return 0;
}

int Server::RecClient() {
  char recvbuf[1024];
  do {
    iResult = recv(ClientSocket, recvbuf, 1024, 0);
    if (iResult > 0) {
      showMsg(recvbuf, iResult);
    } else {
      printError("Пользователь вышел из чата", false);
      closesocket(ClientSocket);
      WSACleanup();
      return -1;
    }
  } while (iResult > 0);

  return 0;
}

int Server::sendMessage(char* message) {
  iResult = send(ClientSocket, message, strlen(message), 0);
  if (iResult < 0) {
    printError("Неудалось отправить сообщение", false);
    return -1;
  }
  return 0;
}

void Server::destroy() { Server::~Server(); }