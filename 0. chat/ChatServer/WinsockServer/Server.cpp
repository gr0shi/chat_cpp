#include "Server.h"

int countusers = 0;

const std::string currentTime() {
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%X", &tstruct);

  return buf;
}

int listofusers(bool plusminus, std::string username, int countusers, int id) {
  std::string path = "..\\..\\list.txt";
  std::ofstream fout;
  fout.open(path, std::ofstream::app);

  if (!fout.is_open())
    std::cout << "Не получается открыть файл!" << std::endl;

	if (plusminus == true)
    fout << "+ | " << countusers << ". " << username << " ID = " << id << " | " << currentTime() << std::endl;
	else
		fout << "- | " << countusers << ". ID = " << id << " | " << currentTime() << std::endl;
  
	if (countusers == 0) {
		fopen("..\\..\\list.txt", "w");
	}

	fout.close();

  return 0;
}

void set_font_color(size_t c) {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, c);
}

void print_error(std::string text, bool critical) {
	set_font_color(8);
  std::cout << std::endl;
  std::cout << ">--------------------------- Ошибка ----------------------------<" << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "                        Описание ошибки:                         " << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << " " << text << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << ">---------------------------------------------------------------<" << std::endl;
  std::cout << std::endl;

  if (critical) {
    std::cout << "Работа приложения остановлена" << std::endl;
    std::cout << std::endl;
    system("pause");
  }
}



Server::Server(int PORT)
{
	WSADATA wsaData;

	ListenSocket = INVALID_SOCKET;

	// инициализация Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		print_error("Неудалось инициализировать Winsock", false);
		exit(0);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// проверка адреса сервера и порта
	iResult = getaddrinfo(NULL, std::to_string(PORT).c_str(), &hints, &result);
	if (iResult != 0)
	{
		print_error("Неудалось проверить адрес сервера и порта", false);
		WSACleanup();
		exit(0);
	}

	// создает сокет для подключения к серверу
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		print_error("Неудалось создать сокет", false);
		freeaddrinfo(result);
		WSACleanup();
		exit (0);
	}

	// настройка TCP сокета
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
    print_error("Неудалось связать сокет", false);
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit (0);
	}

	freeaddrinfo(result);

	//ожидание подключения
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		print_error("Неудалось установить подключение", false);
		closesocket(ListenSocket);
		WSACleanup();
		exit(0);
	}

	serverPtr = this;
}

bool Server::ListenForNewConnection()
{
	SOCKET ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET)
	{
		print_error("Неудалось принять подключение", false);
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	else // успешное соединение
	{
		set_font_color(2);
		std::cout << currentTime() << " | " << "Соединение установленно" << std::endl;
		Connections[ConnectionCounter] = ClientSocket;
		Getusername(ConnectionCounter);

		connectionThreads[ConnectionCounter] = std::thread(ClientHandler, ConnectionCounter);
		ConnectionCounter++;
		return true;
	}
}

bool Server::SendInt(int id, int value)
{
	int returnCheck = send(Connections[id], (char*)& value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::GetInt(int id, int& value)
{
	int returnCheck = recv(Connections[id], (char*)& value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::SendBool(int id, bool value)
{
	int returnCheck = send(Connections[id], (char*)& value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::GetBool(int id, bool& value)
{
	int returnCheck = recv(Connections[id], (char*)& value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::SendPacketType(int id, const PACKET& packetType)
{
	int returnCheck = send(Connections[id], (char*)& packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::GetPacketType(int id, PACKET& packetType)
{
	int returnCheck = recv(Connections[id], (char*)& packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::SendString(int id, const std::string& value)
{
	if (!SendPacketType(id, P_ChatMessage))
		return false;

	int bufferLength = value.size();
	if (!SendInt(id, bufferLength))
		return false;

	int returnCheck = send(Connections[id], value.c_str(), bufferLength, NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::GetString(int id, std::string& value)
{
	int bufferLength = 0;
	if (!GetInt(id, bufferLength))
		return false;

	char* buffer = new char[bufferLength + 1]; // +1 чтобы разрешить завершение '\0'

	int returnCheck = recv(Connections[id], buffer, bufferLength, NULL);
	buffer[bufferLength] = '\0';
	value = buffer;
	delete[] buffer;

	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::ProcessPacket(int index, PACKET packetType)
{
	switch (packetType)
	{
	case P_ChatMessage:
	{
		std::string message;
		if (!GetString(index, message))
			return false;
		for (int i = 0; i < ConnectionCounter; i++)
		{
			if (i == index)
				continue;
			//добавляет пользователя для написания сообщения

			std::string newMessage = currentTime() + " | " + usernames[index]+ ": " + message;
			if (!SendString(i, newMessage)) {
				set_font_color(7);
        std::cout << currentTime() << " | " << "Неудалось отправить сообщение от пользователя " << index << " к " << i << std::endl;
			}
				
		}

		set_font_color(11);
		std::cout << currentTime() << " | " << "Сообщение успешно доставленно. ID = " << index << std::endl;
		break;
	}

	case P_DirectMessage:
	{
		set_font_color(5);
		std::cout << currentTime() << " | " << "Личное сообщение" << std::endl;
		std::string user;
		std::string message;

		std::string value;

		int usernameIndex = -1;
		bool userExists = false;

		//получает пользователя
		if (!GetString(index, value))
			return false;

		int val = 0;
		//получает нужного пользователя
		while (value[val] != ' ')
		{
			user += value[val];
			val++;
		}
		
		//проверка на наличие пользователей
		for (int i = 0; i < ConnectionCounter; i++)
		{
			if (usernames[i] == user)
			{
				userExists = true;
				usernameIndex = i;
				break;
			}
		}

		if (userExists)
		{
			//получает сообщение
			for (int i = val; i < value.size(); i++)
			{
				message += value[i];
			}
		}

		SendPacketType(index, P_DirectMessage);
		SendBool(index, userExists);

		if (userExists)
		{
			std::string fullMessage = currentTime() + " | " + "ЛС от пользователя " + usernames[index] + ": " + message;

			SendString(usernameIndex, fullMessage);
		}

		break;
	}

	default:
		std::cout << currentTime() << " | " << "Нераспознанный покет: " << packetType << std::endl;
		break;
	}
	return true;
}

bool Server::CloseConnection(int index)
{
	if (closesocket(Connections[index]) == SOCKET_ERROR)
	{
    print_error("Неудалось закрыть сокет", false);
		return false;
	}

	return true;
}

void Server::Getusername(int index)
{
	serverPtr->usernames.push_back("");
	PACKET packetType;

	//получает имя пользователя
	bool usernameSaved = true;
	do
	{
		usernameSaved = true;
		if (!serverPtr->GetPacketType(index, packetType))
		{
			set_font_color(12);
			std::cout << currentTime() << " | " << "Неудалось получить имя пользователя" << std::endl;
			break;
		}

		if (!packetType == P_ChatMessage)
		{
			set_font_color(12);
			std::cout << currentTime() << " | " << "Получение имени пользователя это не отправка сообщения" << std::endl;
			break;
		}

		std::string userName;
		serverPtr->GetString(index, userName);
		for each (std::string var in serverPtr->usernames)
		{
			if (var == userName)
			{
				usernameSaved = false;
				break;
			}
		}
		if (usernameSaved)
		{
			serverPtr->usernames[index] = userName;
			set_font_color(10);
			std::cout << currentTime() << " | " << "Пользователь " << userName << " занял ячейку " << index << std::endl;
			countusers++;
			listofusers(true, userName, countusers, index);
		}
		serverPtr->SendBool(index, usernameSaved);

	} while (!usernameSaved);
}

//основная часть работы
void Server::ClientHandler(int index)
{
	PACKET packetType;
	while (true)
	{
		//получает сообщения
		if (!serverPtr->GetPacketType(index, packetType))
			break;
		if (!serverPtr->ProcessPacket(index, packetType))
			break;
	}

	set_font_color(12);
	std::cout << currentTime() << " | " << "Потеряно соединение с пользователем: ID = " << index << std::endl;
	countusers--;
  listofusers(false, "user", countusers, index);
	serverPtr->CloseConnection(index);
}
