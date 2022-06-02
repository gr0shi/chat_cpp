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
		std::cout << "Введите имя пользователя: " << std::endl;

		std::getline(std::cin, username);
		myClient->SendString(username);

		myClient->GetBool(usernameAccepted);

		if (!usernameAccepted)
			std::cout << "Имя пользователя уже занято, попробуйте снова." << std::endl;

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
  std::cout << "                    Добро пожаловать в чат                       " << std::endl;
  std::cout << "                    Вы являетесь клиентом                        " << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "         Параметры сервера:          IP - 127.0.0.1              " << std::endl;
  std::cout << "                                     Порт - 27015                " << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << ">---------------------------------------------------------------<" << std::endl;
  std::cout << "                                                                 " << std::endl;
	
	SecureUsername();

	system("cls");

	std::cout << ">---------------------------------------------------------------<" << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "                    Вы являетесь клиентом                        " << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "         Параметры сервера:          IP - 127.0.0.1              " << std::endl;
  std::cout << "                                     Порт - 27015                " << std::endl;
	std::cout << "                                     Ваше имя - " << username      << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << ">---------------------------------------------------------------<" << std::endl;
  std::cout << "                                                                 " << std::endl;

	myClient->StartSubRoutine();

	//Отправка и получение сообщений
	std::string buffer;
	while (true)
	{
		std::getline(std::cin, buffer); // берет символы, введенные в консоли

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

	// очистка
	system("pause");

	WSACleanup();

	system("pause");

	return 0;
}