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
    std::cout << "�� ���������� ������� ����!" << std::endl;

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
  std::cout << ">--------------------------- ������ ----------------------------<" << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << "                        �������� ������:                         " << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << " " << text << std::endl;
  std::cout << "                                                                 " << std::endl;
  std::cout << ">---------------------------------------------------------------<" << std::endl;
  std::cout << std::endl;

  if (critical) {
    std::cout << "������ ���������� �����������" << std::endl;
    std::cout << std::endl;
    system("pause");
  }
}



Server::Server(int PORT)
{
	WSADATA wsaData;

	ListenSocket = INVALID_SOCKET;

	// ������������� Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		print_error("��������� ���������������� Winsock", false);
		exit(0);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// �������� ������ ������� � �����
	iResult = getaddrinfo(NULL, std::to_string(PORT).c_str(), &hints, &result);
	if (iResult != 0)
	{
		print_error("��������� ��������� ����� ������� � �����", false);
		WSACleanup();
		exit(0);
	}

	// ������� ����� ��� ����������� � �������
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		print_error("��������� ������� �����", false);
		freeaddrinfo(result);
		WSACleanup();
		exit (0);
	}

	// ��������� TCP ������
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
    print_error("��������� ������� �����", false);
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit (0);
	}

	freeaddrinfo(result);

	//�������� �����������
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		print_error("��������� ���������� �����������", false);
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
		print_error("��������� ������� �����������", false);
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	else // �������� ����������
	{
		set_font_color(2);
		std::cout << currentTime() << " | " << "���������� ������������" << std::endl;
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

	char* buffer = new char[bufferLength + 1]; // +1 ����� ��������� ���������� '\0'

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
			//��������� ������������ ��� ��������� ���������

			std::string newMessage = currentTime() + " | " + usernames[index]+ ": " + message;
			if (!SendString(i, newMessage)) {
				set_font_color(7);
        std::cout << currentTime() << " | " << "��������� ��������� ��������� �� ������������ " << index << " � " << i << std::endl;
			}
				
		}

		set_font_color(11);
		std::cout << currentTime() << " | " << "��������� ������� �����������. ID = " << index << std::endl;
		break;
	}

	case P_DirectMessage:
	{
		set_font_color(5);
		std::cout << currentTime() << " | " << "������ ���������" << std::endl;
		std::string user;
		std::string message;

		std::string value;

		int usernameIndex = -1;
		bool userExists = false;

		//�������� ������������
		if (!GetString(index, value))
			return false;

		int val = 0;
		//�������� ������� ������������
		while (value[val] != ' ')
		{
			user += value[val];
			val++;
		}
		
		//�������� �� ������� �������������
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
			//�������� ���������
			for (int i = val; i < value.size(); i++)
			{
				message += value[i];
			}
		}

		SendPacketType(index, P_DirectMessage);
		SendBool(index, userExists);

		if (userExists)
		{
			std::string fullMessage = currentTime() + " | " + "�� �� ������������ " + usernames[index] + ": " + message;

			SendString(usernameIndex, fullMessage);
		}

		break;
	}

	default:
		std::cout << currentTime() << " | " << "�������������� �����: " << packetType << std::endl;
		break;
	}
	return true;
}

bool Server::CloseConnection(int index)
{
	if (closesocket(Connections[index]) == SOCKET_ERROR)
	{
    print_error("��������� ������� �����", false);
		return false;
	}

	return true;
}

void Server::Getusername(int index)
{
	serverPtr->usernames.push_back("");
	PACKET packetType;

	//�������� ��� ������������
	bool usernameSaved = true;
	do
	{
		usernameSaved = true;
		if (!serverPtr->GetPacketType(index, packetType))
		{
			set_font_color(12);
			std::cout << currentTime() << " | " << "��������� �������� ��� ������������" << std::endl;
			break;
		}

		if (!packetType == P_ChatMessage)
		{
			set_font_color(12);
			std::cout << currentTime() << " | " << "��������� ����� ������������ ��� �� �������� ���������" << std::endl;
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
			std::cout << currentTime() << " | " << "������������ " << userName << " ����� ������ " << index << std::endl;
			countusers++;
			listofusers(true, userName, countusers, index);
		}
		serverPtr->SendBool(index, usernameSaved);

	} while (!usernameSaved);
}

//�������� ����� ������
void Server::ClientHandler(int index)
{
	PACKET packetType;
	while (true)
	{
		//�������� ���������
		if (!serverPtr->GetPacketType(index, packetType))
			break;
		if (!serverPtr->ProcessPacket(index, packetType))
			break;
	}

	set_font_color(12);
	std::cout << currentTime() << " | " << "�������� ���������� � �������������: ID = " << index << std::endl;
	countusers--;
  listofusers(false, "user", countusers, index);
	serverPtr->CloseConnection(index);
}
