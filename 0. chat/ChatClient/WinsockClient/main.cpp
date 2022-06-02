#include "Client.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

Client* myClient;

#define DEFAULT_PORT 27015

std::string username;

void SecureUsername()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	bool usernameAccepted = false;
	do
	{
		std::cout << "������� ��� ������������: " << std::endl;

		std::getline(std::cin, username);
		myClient->SendString(username);

		myClient->GetBool(usernameAccepted);

		if (!usernameAccepted)
			std::cout << "��� ������������ ��� ������, ���������� �����." << std::endl;

	} while (!usernameAccepted);
}

int main(int argc, char **argv)
{
	SetConsoleCP(1251);
  SetConsoleOutputCP(1251);

	myClient = new Client(argc, argv, DEFAULT_PORT);

	if (!myClient->Connect())
	{
		system("pause");
		return 1;
	}

	std::cout << ">---------------------------------------------------------------<" << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "                    ����� ���������� � ���                       " << std::endl;
  std::cout << "                    �� ��������� ��������                        " << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "         ��������� �������:          IP - 127.0.0.1              " << std::endl;
  std::cout << "                                     ���� - 27015                " << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << ">---------------------------------------------------------------<" << std::endl;
  std::cout << "                                                                 " << std::endl;
	
	SecureUsername();

	system("cls");

	std::cout << ">---------------------------------------------------------------<" << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "                    �� ��������� ��������                        " << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "         ��������� �������:          IP - 127.0.0.1              " << std::endl;
  std::cout << "                                     ���� - 27015                " << std::endl;
	std::cout << "                                     ���� ��� - " << username      << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << ">---------------------------------------------------------------<" << std::endl;
  std::cout << "                                                                 " << std::endl;

	myClient->StartSubRoutine();

	//�������� � ��������� ���������
	std::string buffer;
	while (true)
	{
		std::getline(std::cin, buffer); // ����� �������, ��������� � �������

		if (buffer[0] == '@')
		{
			buffer.erase(0, 1);
			if (!myClient->SendDirectMessage(buffer))
				break;
    } 
		else
		{
			if (!myClient->SendString(buffer))
				break; 
		}
	}

	// �������
	system("pause");

	WSACleanup();

	system("pause");

	return 0;
}