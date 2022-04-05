#include "Server.h"

void main() {
  SetConsoleCP(1251);
  SetConsoleOutputCP(1251);

  if (!server_entity.is_connected()) {
    print_error("Не удалось инициализировать серверный сокет", true);
    return;
  }

  char buf[4096];

  cout << ">----------------------------------------------------------<"
       << endl;
  cout << endl;
  cout << "\t\tДобро пожаловать в чат" << endl;
  cout << "\t\tВы являетесь сервером" << endl;
  cout << endl;
  cout << "\tПараметры сервера: \tIP - 127.0.0.1" << endl;
  cout << "\t\t\t\tПорт - 10007" << endl;
  cout << endl;
  cout << "           Вызвать меню помощи: -help / -h" << endl;
  cout << endl;
  cout << " Отправьте пустое сообщение чтобы прекратить работу сервера" << endl;
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