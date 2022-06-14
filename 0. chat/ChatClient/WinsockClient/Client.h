#pragma once

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

const std::string CurrentTime();
void SetFontColor(size_t c);
void PrintError(std::string text, bool critical);
int CleanupList(bool status);

enum PACKET { 
	P_ChatMessage, 
	P_DirectMessage 
};

class Client {
public:
	Client(int argc, char **argv, int PORT);

	inline void StartSubRoutine() { 
		clientThread = std::thread(ClientHandler); 
	}
	
	bool Connect();
	bool SendString(const std::string& value);
	bool SendDirectMessage(const std::string &value);
	bool GetBool(bool &value);
	bool DM_Failed;

private:
	static void ClientHandler();
	
	bool ProcessPacket(PACKET packetType);
	bool SendInt(const int& value) const;
	bool GetInt(int& value);
	bool SendBool(bool value);
	bool SendPacketType(const PACKET& packetType);
	bool GetPacketType(PACKET& packetType);
	bool GetString(std::string& value);
	bool CloseConnection();

	int iResult;
	
	SOCKET ServerConnection;
	addrinfo *result = NULL;
	addrinfo *ptr = NULL;
	addrinfo hints;
	std::thread clientThread;
};

static Client* clientPtr;
