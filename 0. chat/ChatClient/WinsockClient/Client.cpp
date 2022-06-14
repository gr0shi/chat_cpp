#include "Client.h"

bool status = true;

const std::string CurrentTime() { // показывает текущее врем€
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%X", &tstruct);

  return buf;
}

void SetFontColor(size_t c) { // мен€ет цвет отображени€ текста
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, c);
}

void PrintError(std::string text, bool critical) { // печатает ошибку если такова€ имеетс€
	SetFontColor(8);
  std::cout << std::endl;
  std::cout << ">--------------------------- ќшибка ----------------------------<" << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "                        ќписание ошибки:                         " << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << " " << text << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << ">---------------------------------------------------------------<" << std::endl;
  std::cout << std::endl;

  if (critical) {
    std::cout << "–абота приложени€ остановлена" << std::endl;
    std::cout << std::endl;
    system("pause");
  }
}

int CleanupList(bool status) { // очищает файл list.txt при потере соединени€ с сервером
	std::string path("..\\..\\list.txt");
	std::ofstream fout;
	fout.open(path, std::ofstream::app);

	if (!fout.is_open())
		std::cout << "Ќе получаетс€ открыть файл!" << std::endl;

	if (status == false) {
		fopen("..\\..\\list.txt", "w");
	}

	fout.close();

	return 0;
}

Client::Client(int argc, char **argv, int PORT) {
	WSADATA wsaData;

	// подтверждение параметров
	if (argc != 2) {
		PrintError("Ќужно указать IP-адрес", false);
	}

	// инициальзаци€ библиотеки WinSock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		PrintError("»нициаци€ библиотеки WinSock не удалась", false);
		exit(0);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// проверка адреса сервера и порта
	iResult = getaddrinfo(argv[1], std::to_string(PORT).c_str(), &hints, &result);
	if (iResult != 0) {
		PrintError("Ќе удалось получить информацию о сервере", false);
		WSACleanup();
	}

	clientPtr = this;
}

bool Client::Connect() { // осуществл€ет подключение к серверу
	bool connected = false;

	// пытаетс€ подключитьс€ к адресу, пока не удастс€
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		// создайте сокет дл€ подключени€ к серверу
		ServerConnection = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ServerConnection == INVALID_SOCKET) {
			PrintError("Ќеправильный сокет", false);
			WSACleanup();
			return false;
		}

		// подключение к серверу
		iResult = connect(ServerConnection, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			PrintError("Ќе удаетс€ установить соединение с сервером", false);
			closesocket(ServerConnection);
			ServerConnection = INVALID_SOCKET;
			continue;
		}
		else {
			connected = true;
			break;

		}
	}

	return connected;
}

bool Client::ProcessPacket(PACKET packetType) { // обрабатывает пакеты данных
	switch (packetType) {
		case P_ChatMessage: {
			std::string message;
			if (!GetString(message))
				return false;
			std::cout << message << std::endl;
			break;
		}

		case P_DirectMessage: {
			bool value;
			if (!GetBool(value))
				DM_Failed = value;
			break;
		}

		default:
			PrintError("Ќераспознанный пакет", false);
	}
	return true;
}

void Client::ClientHandler() { // обрабатывает клиентов
	PACKET packetType;
	while (true) {
		//получает тип пакета
		if (!clientPtr->GetPacketType(packetType))
			break;
		//обрабатывает тип пакета
		if (!clientPtr->ProcessPacket(packetType))
			break;
	}

	//ѕотер€но соединение или произошла ошибка
	PrintError("ѕотер€но соединенние с сервером", false);
	if (clientPtr->CloseConnection()) {
		PrintError("—окет успешно закрыт", false);
		status = false;
		CleanupList(status);
	}
}

bool Client::SendString(const std::string& value) { // отправл€ет строки дл€ обработки
	if (!SendPacketType(P_ChatMessage))
		return false;

	int bufferLength = value.size();
	if (!SendInt(bufferLength))
		return false;

	int returnCheck = send(ServerConnection, value.c_str(), bufferLength, NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::SendDirectMessage(const std::string& value) { // отвечает за отправку личных сообщений пользовател€м
	if (!SendPacketType(P_DirectMessage))
		return false;

	//отправить определенному пользователю
	int bufferLength = value.size();
	if (!SendInt(bufferLength))
		return false;

	int returnCheck = send(ServerConnection, value.c_str(), bufferLength, NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::SendInt(const int& value) const { // отправл€ет целочисленные значени€ дл€ обработки
	int returnCheck = send(ServerConnection, (char*)&value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::GetInt(int& value) { // получает целочисленные значени€ дл€ обработки
	int returnCheck = recv(ServerConnection, (char*)& value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::SendPacketType(const PACKET& packetType) { // отправл€ет тип пакета данных дл€ обработки
	int returnCheck = send(ServerConnection, (char*)& packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::GetPacketType(PACKET& packetType) { // получает тип пакета данных дл€ обработки
	int returnCheck = recv(ServerConnection, (char*)& packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::GetString(std::string& value) { // получает строки дл€ обработки
	int bufferLength = 0;
	if (!GetInt(bufferLength))
		return false;

	char* buffer = new char[bufferLength + 1]; // +1 чтобы разрешить завершение '/0'
	buffer[bufferLength] = '\0';

	int returnCheck = recv(ServerConnection, buffer, bufferLength, NULL);
	value = buffer;
	delete[] buffer;

	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::SendBool(bool value) { // отправл€ет булевые значени€ дл€ обработки
	int returnCheck = send(ServerConnection, (char*)& value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false; 

	return true; 
}

bool Client::GetBool(bool& value) { // получает булевые значени€ дл€ обработки
	int returnCheck = recv(ServerConnection, (char*)& value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::CloseConnection() { // закрывает соединение
	if (closesocket(ServerConnection == SOCKET_ERROR)) {
		if (WSAGetLastError() == WSAENOTSOCK)
			return true;

		PrintError("Ќеудалось закрыть сокет", false);
		return false;
	}

	return true;
}
