#include "inc.h"

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

void create_thread(LPTHREAD_START_ROUTINE text) {
  DWORD threadId;
  CreateThread(NULL, NULL, text, NULL, NULL, &threadId);
}

unsigned long __stdcall rec_thread(void* pParam) {
  SOCKET client_socket = (SOCKET)pParam;
  while (1) {
    if (!server.rec_client(client_socket)) 
      break;
  }
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