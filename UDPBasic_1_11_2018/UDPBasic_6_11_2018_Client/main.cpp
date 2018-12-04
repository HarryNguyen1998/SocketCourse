#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#include <string.h>
#include <WS2tcpip.h>

#pragma comment (lib, "Ws2_32.lib") // Longer way is Project -> %Project% Properties -> Linker -> "Additional Dependencies"

#define PRINTERROR(s) printf("%s: %ld\n", s, WSAGetLastError())


void UDPClient(char *Server, unsigned short Port) 
{
	// Create a UDP socket.
	SOCKET Socket;
	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (Socket == INVALID_SOCKET) {
		PRINTERROR("Create socket failed");
		closesocket(Socket);
		return;
	}

	// Initialize params for getaddrinfo.
	addrinfo Hints;
	memset(&Hints, 0, sizeof(Hints));
	Hints.ai_family = AF_INET;
	Hints.ai_socktype = SOCK_DGRAM;
	Hints.ai_protocol = IPPROTO_UDP;

	// Convert port from short to char.
	// Port in string is 5 decimal-digits + 1 null terminator.
	char InPortStr[6] = { 0 };	
	sprintf_s(InPortStr, sizeof(InPortStr), "%d", Port);

	// Try to get addrinfo given params.
	addrinfo *ServerAddrInfo;
	memset(&ServerAddrInfo, 0, sizeof(ServerAddrInfo));
	if (getaddrinfo(Server, InPortStr, &Hints, &ServerAddrInfo) != 0) {
		PRINTERROR("getaddrinfo() failed");
		closesocket(Socket);
		return;
	}

	// UserName to address the Client instance connecting to listen Server.
	char UserName[100] = { 0 };

	printf("\nSimple.chat ver0.1\n");
	std::cout << "login: "; std::cin >> UserName;

	// Check if Winsock functions (gethostname, bind, sendto, recvfrom, etc) catch any error.
	int RetCheck = 0;

	char Buffer[256];							// contains message received from Server.
	char MessageToSend[256] = { 0 };			// contains input to be sent to Server.
	int ServerSockLength = sizeof(SOCKADDR);	// Socket address length of Server.
	
	// loop until user types "quit"
	while (strcmp(MessageToSend, "quit") != 0) {
		// Clear buffers.
		memset(Buffer, 0, sizeof(Buffer));
		memset(MessageToSend, 0, sizeof(MessageToSend));

		printf("\n %s> ", UserName);

		std::cin >> std::ws;					// clear whitespace.
		std::cin.getline(MessageToSend, 256);	// The message input Client wishes to send to Server.

		// Sends the message to Server.
		RetCheck = sendto(Socket, MessageToSend, sizeof(MessageToSend), 0, ServerAddrInfo->ai_addr, ServerSockLength);
		if (RetCheck == SOCKET_ERROR) {
			PRINTERROR("sendto() failed");
			closesocket(Socket);
			return;
		}

		// Don't have to wait for Server response if we want to quit.
		if (strcmp(MessageToSend, "quit") == 0) {
			closesocket(Socket);
			return;
		}

		// Wait & recieve response from Server.
		RetCheck = recvfrom(Socket, Buffer, sizeof(Buffer), 0, ServerAddrInfo->ai_addr, &ServerSockLength);
		if (RetCheck == SOCKET_ERROR) {
			PRINTERROR("recvfrom() failed");
			closesocket(Socket);
			return;
		}
		printf(" %s> %s", Server, Buffer);

		// Terminate if Server exits.
		if (strcmp(Buffer, "quit") == 0) {
			break;
		}

	}

	closesocket(Socket);
}


int main(int argc, char **argv) {
	WSADATA WsaData;

	if (argc != 3) {
		fprintf(stderr, "\nSyntax: udp-client ServerName PortNumber\n");
		return 0;
	}

	// port input from cmd.
	unsigned short Port = atoi(argv[2]);

	// Initialize Winsock.
	int CheckWinsockCode = WSAStartup(MAKEWORD(2,2), &WsaData);
	if (CheckWinsockCode != 0) {
		printf("WSAStartup failed!! %d\n", CheckWinsockCode);
		return 1;
	}

	UDPClient(argv[1], Port);

	WSACleanup();
	return 0;
}
