#include "Client.h"

bool status = true;

const std::string currentTime() {
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%X", &tstruct);

  return buf;
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

int cleanuplist(bool status) {
	std::string path("..\\..\\list.txt");
	std::ofstream fout;
	fout.open(path, std::ofstream::app);

	if (!fout.is_open())
		std::cout << "�� ���������� ������� ����!" << std::endl;

	if (status == false) {
		fopen("..\\..\\list.txt", "w");
	}

	fout.close();

	return 0;
}

Client::Client(int argc, char **argv, int PORT)
{
	WSADATA wsaData;

	// ������������� ����������
	if (argc != 2) {
		print_error("����� ������� ����", false);
	}

	// ������������� ���������� WinSock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		print_error("��������� ���������� WinSock �� �������", false);
		exit(0);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// �������� ������ ������� � �����
	iResult = getaddrinfo(argv[1], std::to_string(PORT).c_str(), &hints, &result);
	if (iResult != 0) {
		print_error("�� ������� �������� ���������� � �������", false);
		WSACleanup();
	}

	clientPtr = this;
}

bool Client::Connect()
{
	bool connected = false;

	// �������� ������������ � ������, ���� �� �������
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{

		// �������� ����� ��� ����������� � �������
		ServerConnection = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ServerConnection == INVALID_SOCKET) {
			print_error("������������ �����", false);
			WSACleanup();
			return false;
		}

		// ����������� � �������
		iResult = connect(ServerConnection, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			print_error("�� ������� ���������� ���������� � ��������", false);
			closesocket(ServerConnection);
			ServerConnection = INVALID_SOCKET;
			continue;
		}
		else
		{
			connected = true;
			break;

		}
	}

	return connected;
}

bool Client::ProcessPacket(PACKET packetType)
{
	switch (packetType)
	{
	case P_ChatMessage:
	{
		std::string message;
		if (!GetString(message))
			return false;
		std::cout << message << std::endl;
		break;
	}

	case P_DirectMessage:
	{
		bool value;
		if (!GetBool(value))
		DM_Failed = value;
		break;
	}
	default:
		print_error("�������������� �����", false);
	}
	return true;
}

void Client::ClientHandler()
{
	PACKET packetType;
	while (true)
	{
		//get type
		if (!clientPtr->GetPacketType(packetType))
			break;
		//procees package type
		if (!clientPtr->ProcessPacket(packetType))
			break;
	}

	//�������� ���������� ��� ��������� ������

	print_error("�������� ����������� � ��������", false);
	if (clientPtr->CloseConnection())
	{
		print_error("����� ������� ������", false);
		status = false;
		cleanuplist(status);
	}
}

bool Client::SendString(const std::string& value)
{
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

bool Client::SendDirectMessage(const std::string& value)
{
	if (!SendPacketType(P_DirectMessage))
		return false;

	//��������� ������������� ������������
	int bufferLength = value.size();
	if (!SendInt(bufferLength))
		return false;

	int returnCheck = send(ServerConnection, value.c_str(), bufferLength, NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::SendInt(const int& value) const
{
	int returnCheck = send(ServerConnection, (char*)&value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::GetInt(int& value)
{
	int returnCheck = recv(ServerConnection, (char*)& value, sizeof(int), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::SendPacketType(const PACKET& packetType)
{
	int returnCheck = send(ServerConnection, (char*)& packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::GetPacketType(PACKET& packetType)
{
	int returnCheck = recv(ServerConnection, (char*)& packetType, sizeof(PACKET), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::GetString(std::string& value)
{
	int bufferLength = 0;
	if (!GetInt(bufferLength))
		return false;

	char* buffer = new char[bufferLength + 1]; // +1 ����� ��������� ���������� '/ 0'
	buffer[bufferLength] = '\0';

	int returnCheck = recv(ServerConnection, buffer, bufferLength, NULL);
	value = buffer;
	delete[] buffer;

	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}
bool Client::SendBool(bool value)
{
	int returnCheck = send(ServerConnection, (char*)& value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false; 

	return true; 
}

bool Client::GetBool(bool& value)
{
	int returnCheck = recv(ServerConnection, (char*)& value, sizeof(bool), NULL);
	if (returnCheck == SOCKET_ERROR)
		return false;

	return true;
}

bool Client::CloseConnection()
{
	if (closesocket(ServerConnection == SOCKET_ERROR))
	{
		if (WSAGetLastError() == WSAENOTSOCK)
			return true;

		print_error("��������� ������� �����", false);
		return false;
	}

	return true;
}
