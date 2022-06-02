#define _CRT_SECURE_NO_WARNINGS
#include "Server.h"

Server server_entity;

int countus = 0;

bool manual_end = false;

const std::string currentTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%X", &tstruct);

    return buf;
}

int listofusers(string resID, int countus) {
  string path = "list.txt";
  ofstream fout;
  fopen("list.txt", "w");
  fout.open(path, ofstream::app);

  if (!fout.is_open())
    cout << "Не получается открыть файл!" << endl;

  else
    fout << countus << ". " << resID << " - " << currentTime() << endl;

  fout.close();

  return 0;
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

unsigned long __stdcall server_rec_thread(void* pParam) {
  SOCKET client_socket = (SOCKET)pParam;
  while (1) {
    if (!server_entity.rec_client(client_socket)) break;
  }
  return 0;
}

unsigned long __stdcall ServerListenThread(void* pParam) {
  while (server_entity.start_listening());
  return 0;
}

size_t get_font_number(string text) {
  hash<string> hash_fn;
  size_t str_hash = hash_fn(text);
  size_t temp = 0;
  do {
    temp = 0;
    while (str_hash > 0) {
      temp += str_hash % 10;
      str_hash /= 10;
    }
    str_hash = temp;
  } while (temp > 10);
  temp += 5;
  if (temp == 9 || temp == 12) --temp;
  return temp;
}

bool Server::start_listening() {
  SOCKET current_client_socket;
  sockaddr_in incoming;
  int incominglen = sizeof(incoming);

  current_client_socket = accept(receiver, (struct sockaddr*)&incoming, &incominglen);

  char temp[4096];
  int locStatus;
  locStatus = recv(current_client_socket, temp, 4096, 0);
  if (locStatus == -1) {
    return false;
  }
  string resID(temp);

  cout << "\b\b";
  string message = currentTime() + " " + "#" + resID + " подключился";
  countus++;
  set_font_color(11);
  cout << "Количество пользователей: " << countus << endl;
  listofusers(resID, countus);
  set_font_color(12);
  cout << message << endl;
  set_font_color(7);
  cout << "> ";
  message = "12$" + message;
  send_msg_from_server(message, "System");

  if (current_client_socket != INVALID_SOCKET) {
    clients_list.push_back(make_pair(current_client_socket, resID));
    client_colors.insert(make_pair(resID, get_font_number(resID)));
  }

  DWORD threadId;
  CreateThread(NULL, NULL, server_rec_thread, (void*)current_client_socket, NULL, &threadId);
  return true;
}

Server::Server() {
  cout << "Server started." << endl;
  cout << endl;
  conn_stability = false;

  WSADATA wsaData;

  sockaddr_in local;

  int wsa_start = WSAStartup(0x101, &wsaData);

  if (wsa_start != 0) return;

  local.sin_family = AF_INET;
  local.sin_addr.s_addr = INADDR_ANY;
  local.sin_port = htons((u_short)10007);

  receiver = socket(AF_INET, SOCK_STREAM, 0);

  if (receiver == INVALID_SOCKET) {
    print_error("Не удалось установить принимающий сокет", false);
    return;
  }

  if (bind(receiver, (sockaddr*)&local, sizeof(local)) != 0) {
    print_error("Не удалось связать сокет", false);
    return;
  }

  if (listen(receiver, 10) != 0) {
    print_error("Не удалось начать прослушивание через сокет", false);
    return;
  }

  conn_stability = true;
}

Server::Server(const u_short port) {
  // this->port = port;
}

Server::~Server() {
  closesocket(receiver);
  WSACleanup();
}

void Server::destroy() {
  conn_stability = false;
  closesocket(receiver);
  for (auto it : clients_list) {
    closesocket(it.first);
    if (!clients_list.size()) break;
  }
}

bool Server::send_msg_from_server(string text, string id = "") {
  int locStatus = 0;

  if (!id.size())
    text = currentTime() + " " + "Server: 9$" + text;

  else if (id == "System") {
    text = "System: " + text;
  } else
    text = currentTime() + " " + "#" + id + ": " + text;

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
  if (!conn_stability || manual_end) return false;

  char temp[4096];
  int locStatus;

  auto it = find_if(clients_list.begin(), clients_list.end(),
                    [=](const std::pair<SOCKET, string>& element) {
                      return element.first == cliSocket;
                    });

  locStatus = recv(cliSocket, temp, 4096, 0);

  if (!conn_stability || manual_end) return false;

  if (locStatus == -1) {
    cout << "\b\b";

    string message = currentTime() + " " + "#" + it->second + " покинул чат";
    countus--;
    set_font_color(11);
    cout << "Количество пользователей: " << countus << endl;
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
    cout << currentTime() << " " << "#" << it->second;
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

void main() {
  SetConsoleCP(1251);
  SetConsoleOutputCP(1251);

  if (!server_entity.is_connected()) {
    print_error("Не удалось инициализировать серверный сокет", true);
    return;
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
  cout << "   Отправьте пустое сообщение чтобы прекратить работу сервера    " << endl;
  cout << "                                                                 " << endl;
  cout << ">---------------------------------------------------------------<" << endl;
  cout << "                                                                 " << endl;

  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, 7);

  DWORD threadId;
  CreateThread(NULL, NULL, ServerListenThread, NULL, NULL, &threadId);

  cout << "> ";
  while (gets_s(buf)) {
    if (strlen(buf) == 0) {
      cout << endl;
      cout << "Вы остановили работу сервера" << endl;
      manual_end = true;
      server_entity.send_termination();
      break;
    }
    if (!server_entity.send_msg_from_server(buf)) {
      print_error("Невозможно подключиться к клиентам", false);
      break;
    }
    cout << "> ";
  }

  cout << "Сервер остановлен" << endl;
  cout << endl;
  system("pause");
}
