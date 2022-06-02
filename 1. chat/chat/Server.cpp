#define _CRT_SECURE_NO_WARNINGS
#include "Server.h"
#include "inc.h"

Server::Server() {
  cout << "Client Started." << endl;
  cout << endl;
}

Server::~Server() {
  closesocket(receiver);
  WSACleanup();
}

bool Server::start_listening() {
  SOCKET client_socket;
  sockaddr_in incoming;
  int incominglen = sizeof(incoming);

  client_socket =
      accept(receiver, (struct sockaddr*)&incoming, &incominglen);

  char temp[4096];
  int locStatus;
  locStatus = recv(client_socket, temp, 4096, 0);
  if (locStatus == -1) {
    return false;
  }
  string resID(temp);

  cout << "\b\b";
  string message = "#" + resID + " подключился";
  set_font_color(12);
  cout << message << endl;
  set_font_color(7);
  cout << "> ";
  message = "12$" + message;
  send_msg_from_server(message, "System");

  if (client_socket != INVALID_SOCKET) {
    clients_list.push_back(make_pair(client_socket, resID));
    client_colors.insert(make_pair(resID, get_font_number(resID)));
  }

  DWORD threadId;
  CreateThread(NULL, NULL, rec_thread, (void*)client_socket,
               NULL, &threadId);
  return true;
}

int Server::start() { 
  conn_stability = false;

  WSADATA wsaData;

  sockaddr_in local;
  
  int wsa_start = WSAStartup(0x101, &wsaData);

  if (wsa_start != 0) 
    return -1;

  local.sin_family = AF_INET;
  local.sin_addr.s_addr = INADDR_ANY;
  local.sin_port = htons((u_short)10007);
  
  receiver = socket(AF_INET, SOCK_STREAM, 0);

  if (receiver == INVALID_SOCKET) {
    print_error("Не удалось установить принимающий сокет", false);
    return -1;
  }

  if (bind(receiver, (sockaddr*)&local, sizeof(local)) != 0) {
    print_error("Не удалось связать сокет", false);
    return -1;
  }

  if (listen(receiver, 10) != 0) {
    print_error("Не удалось начать прослушивание через сокет", false);
    return -1;
  }

  conn_stability = true;

}

bool Server::send_msg_from_server(string text, string id = "") {
  int locStatus = 0;

  if (!id.size())
    text = "Server: 9$" + text;

  else if (id == "System") {
    text = "System: " + text;
  } else
    text = "#" + id + ": " + text;

  if (clients_list.size() == 0) return true;

  for (auto it = clients_list.begin(); it != clients_list.end(); it++) {
    if (id != "" && id == it->second) continue;

    locStatus = send(it->first, text.c_str(), text.size() + 1, 0);

    if (locStatus == -1) clients_list.erase(it);
  }

  if (locStatus == -1) return false;
  return true;
}

bool Server::rec_client(SOCKET cliSocket) {
  if (!conn_stability /*|| manualend*/) return false;

  char temp[4096];
  int locStatus;

  auto it = find_if(clients_list.begin(), clients_list.end(),
                    [=](const std::pair<SOCKET, string>& element) {
                      return element.first == cliSocket;
                    });

  locStatus = recv(cliSocket, temp, 4096, 0);

  if (!conn_stability /*|| manualend*/) return false;

  if (locStatus == -1) {
    cout << "\b\b";

    string message = "#" + it->second + " покинул чат";
    set_font_color(12);
    cout << message << endl;
    set_font_color(7);
    cout << "> ";

    message = "12$" + message;
    clients_list.erase(it);

    send_msg_from_server(message, "System");
    return false;
  } else {
    string message(temp);
    cout << "\b\b";

    set_font_color(client_colors[it->second]);
    cout << "#" << it->second;
    set_font_color(7);
    cout << ": " << message << endl;
    message = to_string(client_colors[it->second]) + "$" + message;
    send_msg_from_server(message, it->second);
    cout << "> ";
  }
  return true;
}

void Server::send_termination() {
  string message = "Сервер остановлен, все подключенные пользователи были выброшены";
  message = "12$" + message;
  send_msg_from_server(message, "System");
  destroy();
}

void Server::destroy() {
  conn_stability = false;
  closesocket(receiver);
  for (auto it : clients_list) {
    closesocket(it.first);
    if (!clients_list.size()) break;
  }
}

//void start_server() {
//  SetConsoleCP(1251);
//  SetConsoleOutputCP(1251);
//
//  if (!server.is_connected()) {
//    print_error("Не удалось инициализировать серверный сокет", true);
//    return;
//  }
//
//  char buf[4096];
//
//  cout << ">----------------------------------------------------------<"
//       << endl;
//  cout << endl;
//  cout << "\t\tДобро пожаловать в чат" << endl;
//  cout << "\t\tВы являетесь сервером" << endl;
//  cout << endl;
//  cout << "\tПараметры сервера: \tIP - 127.0.0.1" << endl;
//  cout << "\t\t\t\tПорт - 10007" << endl;
//  cout << endl;
//  cout << "           Вызвать меню помощи: -help / -h" << endl;
//  cout << endl;
//  cout << " Отправьте пустое сообщение чтобы прекратить работу сервера" << endl;
//  cout << endl;
//  cout << ">----------------------------------------------------------<"
//       << endl;
//  cout << endl;
//
//  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//  SetConsoleTextAttribute(hConsole, 7);
//
//  DWORD threadId;
//  CreateThread(NULL, NULL, listen_thread, NULL, NULL, &threadId);
//
//  cout << "> ";
//  while (gets_s(buf)) {
//    if (strlen(buf) == 0) {
//      cout << endl;
//      cout << "Вы остановили работу сервера" << endl;
//      manual_end = true;
//      server.send_termination();
//      break;
//    }
//    if (!server.send_msg_from_server(buf)) {
//      print_error("Невозможно подключиться к клиентам", false);
//      break;
//    }
//    cout << "> ";
//  }
//
//  cout << "Сервер остановлен" << endl;
//  cout << endl;
//  system("pause");
//}
