#include "Server.h"

void show_msg_server(char* message, int lenght) {
  std::cout << "\Client: ";
  for (int i = 0; i < lenght; i++) 
    std::cout << message[i];
  std::cout << std::endl;
}

void print_error_server(const char* c, bool critical) {
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
    print_error_server("Инициация библиотеки WinSock не удалась", true);
    return 1;
  }

  iResult = getaddrinfo(NULL, "27015", &hints, &result);
  if (iResult != 0) {
    print_error_server("Не удалось получить addrinfo", false);
    WSACleanup();
    return 1;
  }

  listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (listenSocket == INVALID_SOCKET) {
    print_error_server("Не удалось создать сокет", false);
    freeaddrinfo(result);
    WSACleanup();
    return 1;
  }

  iResult = bind(listenSocket, result->ai_addr, result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    print_error_server("Не удалось связать сокет", false);
    freeaddrinfo(result);
    closesocket(listenSocket);
    WSACleanup();
    return 1;
  }

  freeaddrinfo(result);

  if (start_listening() == 1) return 1;

  return 0;
}
int Server::start_listening() {
  std::cout << ">---------------------------------------<" << std::endl;
  std::cout << std::endl;
  std::cout << "Вы являетесь сервером" << std::endl;
  std::cout << std::endl;
  std::cout << "Вызвать меню помощи: -help / -h" << std::endl;
  std::cout << std::endl;
  std::cout << "Отправьте пустое сообщение чтобы прекратить работу клиента" << std::endl;
  std::cout << std::endl;
  std::cout << ">---------------------------------------<" << std::endl;
  std::cout << std::endl;


  if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
    print_error_server("Прослушивание приостановленно.", true);
    closesocket(listenSocket);
    WSACleanup();
    return 1;
  }

  if (handle_connection() == 1) 
    return 1;

  return 0;
}

int Server::handle_connection() {
  ClientSocket = INVALID_SOCKET;
  ClientSocket = accept(listenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
    print_error_server("Неудалось принять данные.", false);
    closesocket(listenSocket);
    WSACleanup();
    return 1;
  }

  std::thread th(&Server::rec_client, this);
  th.detach();

  return 0;
}

int Server::rec_client() {
  char recvbuf[1024];
  do {
    iResult = recv(ClientSocket, recvbuf, 1024, 0);
    if (iResult > 0) {
      show_msg_server(recvbuf, iResult);
    } else {
      std::cout << std::endl;
      std::cout << "Пользователь вышел из чата" << std::endl;
      closesocket(ClientSocket);
      WSACleanup();
      return -1;
    }
  } while (iResult > 0);

  return 0;
}

int Server::send_msg_from_server(char* message) {
  iResult = send(ClientSocket, message, strlen(message), 0);
  if (iResult < 0) {
    print_error_server("Неудалось отправить сообщение", false);
    return -1;
  }
  return 0;
}

void Server::destroy() { Server::~Server(); }