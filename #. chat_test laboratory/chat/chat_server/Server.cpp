#include "Server.h"

#include <algorithm>

Server ServerEntity;

bool manualEnd = false;

void printError(string text, bool critical) {
  cout << endl;
  cout << ">--------------------------- ������ ----------------------------<"
       << endl;
  cout << endl;
  cout << boolalpha << "\t\t\t�������� ������:" << endl;
  cout << endl;
  cout << " " << text << endl;
  cout << endl;
  cout << ">---------------------------------------------------------------<"
       << endl;
  cout << endl;

  if (critical) {
    cout << "������ ������� �����������" << endl;
    cout << endl;
    system("pause");
  }
}

void setFontColor(size_t c) {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, c);
}

size_t getFontNumber(string text);
unsigned long __stdcall ServerRecThread(void* pParam);

bool Server::StartListening() {
  SOCKET currCliSocket;
  sockaddr_in incoming;
  int incominglen = sizeof(incoming);

  currCliSocket = accept(receiver, (struct sockaddr*)&incoming, &incominglen);

  char temp[4096];
  int locStatus;
  locStatus = recv(currCliSocket, temp, 4096, 0);
  if (locStatus == -1) {
    // printError("Cannot get user's id", false);
    return false;
  }
  string resID(temp);

  cout << "\b\b";
  string message = "#" + resID + " �����������";
  setFontColor(12);
  cout << message << endl;
  setFontColor(7);
  cout << "> ";
  message = "12$" + message;
  SendMessageToAll(message, "System");

  if (currCliSocket != INVALID_SOCKET) {
    clientsList.push_back(make_pair(currCliSocket, resID));
    cliColors.insert(make_pair(resID, getFontNumber(resID)));
  }

  DWORD threadId;
  CreateThread(NULL, NULL, ServerRecThread, (void*)currCliSocket, NULL, &threadId);
  return true;
}

Server::Server() {
  cout << "Server started." << endl;
  cout << endl;
  conn_stability = false;

  WSADATA wsaData;

  sockaddr_in local;

  int wsaret = WSAStartup(0x101, &wsaData);

  if (wsaret != 0) return;

  local.sin_family = AF_INET;
  local.sin_addr.s_addr = INADDR_ANY;
  local.sin_port = htons((u_short)10007);

  receiver = socket(AF_INET, SOCK_STREAM, 0);

  if (receiver == INVALID_SOCKET) {
    printError("�� ������� ���������� ����������� �����", false);
    return;
  }

  if (bind(receiver, (sockaddr*)&local, sizeof(local)) != 0) {
    printError("", false);
    return;
  }

  if (listen(receiver, 10) != 0) {
    printError("�� ������� ������ ������������� ����� �����", false);
    return;
  }

  conn_stability = true;
}

Server::~Server() {
  closesocket(receiver);
  WSACleanup();
}

void Server::destroy() {
  conn_stability = false;
  closesocket(receiver);
  for (auto it : clientsList) {
    closesocket(it.first);
    if (!clientsList.size()) break;
  }
}

bool Server::SendMessageToAll(string text, string id = "") {
  int locStatus = 0;
  if (!id.size())
    text = "Server: 9$" + text;
  else if (id == "System") {
    text = "System: " + text;
  } else
    text = "#" + id + ": " + text;
  if (clientsList.size() == 0) return true;

  for (auto it = clientsList.begin(); it != clientsList.end(); it++) {
    if (id != "" && id == it->second) continue;
    locStatus = send(it->first, text.c_str(), text.size() + 1, 0);
    if (locStatus == -1) {
      clientsList.erase(it);
    }
  }

  if (locStatus == -1) return false;
  return true;
}

bool Server::RecClient(SOCKET cliSocket) {
  if (!conn_stability || manualEnd) return false;
  char temp[4096];
  int locStatus;
  auto it = find_if(clientsList.begin(), clientsList.end(),
                    [=](const std::pair<SOCKET, string>& element) {
                      return element.first == cliSocket;
                    });

  locStatus = recv(cliSocket, temp, 4096, 0);

  if (!conn_stability || manualEnd) return false;
  if (locStatus == -1) {
    cout << "\b\b";
    string message = "#" + it->second + " ������� ���";
    setFontColor(12);
    cout << message << endl;
    setFontColor(7);
    cout << "> ";
    message = "12$" + message;
    clientsList.erase(it);
    SendMessageToAll(message, "System");
    return false;
  } else {
    string message(temp);
    cout << "\b\b";
    setFontColor(cliColors[it->second]);
    cout << "#" << it->second;
    setFontColor(7);
    cout << ": " << message << "\n";
    message = to_string(cliColors[it->second]) + "$" + message;
    SendMessageToAll(message, it->second);
    cout << "> ";
  }
  return true;
}

unsigned long __stdcall ServerRecThread(void* pParam) {
  SOCKET cliSocket = (SOCKET)pParam;
  while (1) {
    if (!ServerEntity.RecClient(cliSocket)) break;
  }
  return 0;
}

unsigned long __stdcall ServerListenThread(void* pParam) {
  while (ServerEntity.StartListening());
  return 0;
}

size_t getFontNumber(string text) {
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

void Server::sendTermination() {
  string message = "������ ����������, ��� ������������ ������������ ���� ���������";
  message = "12$" + message;
  SendMessageToAll(message, "System");
  destroy();
}

void main() {
  SetConsoleCP(1251);
  SetConsoleOutputCP(1251);

  if (!ServerEntity.IsConnected()) {
    printError("�� ������� ���������������� ��������� �����", true);
    return;
  }

  char buf[4096];

  cout << ">----------------------------------------------------------<"
       << endl;
  cout << endl;
  cout << "\t\t����� ���������� � ���" << endl;
  cout << "\t\t�� ��������� ��������" << endl;
  cout << endl;
  cout << "\t��������� �������: \tIP - 127.0.0.1" << endl;
  cout << "\t\t\t\t���� - 10007" << endl;
  cout << endl;
  cout << "           ������� ���� ������: -help / -h" << endl;
  cout << endl;
  cout << " ��������� ������ ��������� ����� ���������� ������ �������" << endl;
  cout << endl;
  cout << ">----------------------------------------------------------<"
       << endl;
  cout << endl;

  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, 7);

  DWORD threadId;
  CreateThread(NULL, NULL, ServerListenThread, NULL, NULL, &threadId);

  cout << "> ";
  while (gets_s(buf)) {
    if (strlen(buf) == 0) {
      cout << endl;
      cout << "�� ���������� ������ �������" << endl;
      manualEnd = true;
      ServerEntity.sendTermination();
      break;
    }
    if (!ServerEntity.SendMessageToAll(buf)) {
      printError("���������� ������������ � ��������", false);
      break;
    }
    cout << "> ";
  }

  cout << "������ ����������" << endl;
  cout << endl;
  system("pause");
}
