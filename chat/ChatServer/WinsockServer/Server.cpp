#include "Server.h"

int countusers = 0;

const std::string CurrentTime() { // показывает текущее время
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%X", &tstruct);

  return buf;
}

int ListOfUsers(bool statuspm, std::string username, int countusers, int id) { // создает файл list.txt, в котором ведет лог
  std::string path = "..\\..\\list.txt";
  std::ofstream fout;
  fout.open(path, std::ofstream::app);

  if (!fout.is_open())
    std::cout << "Не получается открыть файл!" << std::endl;

	if (statuspm == true)
    fout << "+ | " << countusers << ". " << username << " ID = " << id << " | " << CurrentTime() << std::endl;
	else
		fout << "- | " << countusers << ". ID = " << id << " | " << CurrentTime() << std::endl;
  
	if (countusers == 0) {
		fopen("..\\..\\list.txt", "w");
	}

	fout.close();

  return 0;
}

void SetFontColor(size_t c) { // меняет цвет отображения текста
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, c);
}

void PrintError(std::string text, bool critical) { // печатает ошибку если таковая имеется
	SetFontColor(8);
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

Server::Server(int PORT) {
	WSADATA wsaData;

	ListenSocket = INVALID_SOCKET;

	// инициализация Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		PrintError("Неудалось инициализировать Winsock", false);
		exit(0);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// проверка адреса сервера и порта
	iResult = getaddrinfo(NULL, std::to_string(PORT).c_str(), &hints, &result);
	if (iResult != 0) {
		PrintError("Неудалось проверить адрес сервера и порта", false);
		WSACleanup();
		exit(0);
	}

	// создает сокет для подключения к серверу
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		PrintError("Неудалось создать сокет", false);
		freeaddrinfo(result);
		WSACleanup();
		exit (0);
	}

	// настройка TCP сокета
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
    PrintError("Неудалось настроить сокет", false);
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit (0);
	}

	freeaddrinfo(result);

	//ожидание подключения
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		PrintError("Неудалось установить подключение", false);
		closesocket(ListenSocket);
		WSACleanup();
		exit(0);
	}

	serverPtr = this;
}

bool Server::ListenForNewConnection() { // отвечает за подключения
	SOCKET ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		PrintError("Неудалось принять подключение", false);
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	else { // успешное соединение 
		SetFontColor(2);
		std::cout << CurrentTime() << " | " << "Соединение установленно" << std::endl;
		Connections[ConnectionCounter] = ClientSocket;
		Getusername(ConnectionCounter);

		connectionThreads[ConnectionCounter] = std::thread(ClientHandler, ConnectionCounter);
		ConnectionCounter++;
		return true;
	}
}

bool Server::SendInt(int id, int value) { // отправляет целочисленные значения для обработки
	int returnCheck = send(Connections[id], (char*)& value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::GetInt(int id, int& value) { // получает целочисленные значения для обработки
	int returnCheck = recv(Connections[id], (char*)& value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::SendBool(int id, bool value) { // отправляет булевые значения для обработки
	int returnCheck = send(Connections[id], (char*)& value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::GetBool(int id, bool& value) { // получает булевые значения для обработки
	int returnCheck = recv(Connections[id], (char*)& value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::SendPacketType(int id, const PACKET& packetType) { // отправляет тип пакета данных для обработки
	int returnCheck = send(Connections[id], (char*)& packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::GetPacketType(int id, PACKET& packetType) { // получает тип пакета данных для обработки
	int returnCheck = recv(Connections[id], (char*)& packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Server::SendString(int id, const std::string& value) { // отправляет строки для обработки
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

bool Server::GetString(int id, std::string& value) { // получает строки для обработки
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

bool Server::ProcessPacket(int index, PACKET packetType) { // обрабатывает пакеты данных
	switch (packetType) {
		case P_ChatMessage: {
			std::string message;
			if (!GetString(index, message))
				return false;
			for (int i = 0; i < ConnectionCounter; i++) {
				if (i == index)
					continue;
					//отображает пользователя и написанное сообщение

				std::string newMessage = CurrentTime() + " | " + "#" + usernames[index] + ": " + message;
				if (!SendString(i, newMessage)) {
					SetFontColor(7);
					std::cout << CurrentTime() << " | " << "Неудалось отправить сообщение от пользователя " << index << " к " << i << std::endl;
				}
				
			}

			SetFontColor(11);
			std::cout << CurrentTime() << " | " << "Сообщение успешно доставленно. ID = " << index << std::endl;
			break;
		}

		case P_DirectMessage: {
			SetFontColor(5);
			std::cout << CurrentTime() << " | " << "Личное сообщение" << std::endl;
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
			while (value[val] != ' ') {
				user += value[val];
				val++;
			}
		
			//проверка на наличие пользователей
			for (int i = 0; i < ConnectionCounter; i++) {
				if (usernames[i] == user) {
					userExists = true;
					usernameIndex = i;
					break;
				}
			}

			if (userExists) {
				//получает сообщение
				for (int i = val; i < value.size(); i++) {
				message += value[i];
				}
			}

			SendPacketType(index, P_DirectMessage);
			SendBool(index, userExists);

			if (userExists) {
				std::string fullMessage = CurrentTime() + " | " + "ЛС от пользователя " + usernames[index] + ": " + message;

				SendString(usernameIndex, fullMessage);
			}

			break;
		}

		default:
			std::cout << CurrentTime() << " | " << "Нераспознанный пакет: " << packetType << std::endl;
			break;
	}
	return true;
}

bool Server::CloseConnection(int index)	{ // закрывает соединение
	if (closesocket(Connections[index]) == SOCKET_ERROR) {
    PrintError("Неудалось закрыть сокет", false);
		return false;
	}

	return true;
}

void Server::Getusername(int index) {	//получает имя пользователя
	serverPtr->usernames.push_back("");
	PACKET packetType;

	bool usernameSaved = true;
	do {
		usernameSaved = true;
		if (!serverPtr->GetPacketType(index, packetType)) {
			SetFontColor(12);
			std::cout << CurrentTime() << " | " << "Неудалось получить имя пользователя" << std::endl;
			break;
		}

		if (!packetType == P_ChatMessage) {
			SetFontColor(12);
			std::cout << CurrentTime() << " | " << "Получение имени пользователя это не отправка сообщения" << std::endl;
			break;
		}

		std::string userName;
		serverPtr->GetString(index, userName);
		for each (std::string var in serverPtr->usernames) {
			if (var == userName) {
				usernameSaved = false;
				break;
			}
		}
		if (usernameSaved) {
			serverPtr->usernames[index] = userName;
			SetFontColor(10);
			std::cout << CurrentTime() << " | " << "Пользователь " << "#" << userName << " занял ячейку " << index << std::endl;
			countusers++;
			ListOfUsers(true, userName, countusers, index);
		}
		serverPtr->SendBool(index, usernameSaved);

	} while (!usernameSaved);
}

//основная часть работы
void Server::ClientHandler(int index) { // обрабатывает клиентов
	PACKET packetType;
	while (true) {
		//получает сообщения
		if (!serverPtr->GetPacketType(index, packetType))
			break;
		if (!serverPtr->ProcessPacket(index, packetType))
			break;
	}

	SetFontColor(12);
	std::cout << CurrentTime() << " | " << "Потеряно соединение с пользователем: ID = " << index << std::endl;
	countusers--;
  ListOfUsers(false, "user", countusers, index);
	serverPtr->CloseConnection(index);
}
