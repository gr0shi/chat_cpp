#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <stdio.h>

#include "Server.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27015


int main(void)
{
  SetConsoleCP(1251);
  SetConsoleOutputCP(1251);

	Server server(DEFAULT_PORT);

	std::cout << ">---------------------------------------------------------------<" << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "                    ����� ���������� � ���                       " << std::endl;
  std::cout << "                    �� ��������� ��������                        " << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "         ��������� �������:          ���� - " << DEFAULT_PORT      << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "    � ��� ���� ����������� ���������� ���������� �������������,  " << std::endl;
  std::cout << "      �� �����, ����� ID ��� ����������, � ����� �����������     " << std::endl;
  std::cout << "           � ��������� ��������� ��� ��������� list.txt          " << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << ">---------------------------------------------------------------<" << std::endl;
  std::cout << "                                                                 " << std::endl;

	for (int i = 0; i < 100; i++)
	{
		server.ListenForNewConnection();
	}

	system("pause");

	return 0;
}