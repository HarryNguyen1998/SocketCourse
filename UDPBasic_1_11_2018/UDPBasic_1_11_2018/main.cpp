#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#pragma comment (lib, "Ws2_32.lib") // Longer way is Project -> %Project% Properties -> Linker -> "Additional Dependencies"

#define PRINTERROR(s) printf("%s: %ld\n", s, WSAGetLastError())

void UDPServer(unsigned short Port) 
{
	// Create a UDP socket.
	SOCKET Socket;
	Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (Socket == INVALID_SOCKET) {
		PRINTERROR("Socket creation failed");
		closesocket(Socket);
		return;
	}

	// Check if Winsock functions (gethostname, bind, sendto, recvfrom, etc) catch any error.
	int RetCheck = 0;

	// Initialize params (port, ip addr) to bind to Server Socket.
	SOCKADDR_IN ServerService;
	memset(&ServerService, 0, sizeof(SOCKADDR_IN));
	ServerService.sin_family = AF_INET;
	ServerService.sin_addr.S_un.S_addr = INADDR_ANY;
	ServerService.sin_port = htons(Port);
	
	// Bind Socket to specified params.
	RetCheck = bind(Socket, (SOCKADDR*)&ServerService, sizeof(SOCKADDR));
	if (RetCheck == INVALID_SOCKET) {
		PRINTERROR("Bind failed");
		closesocket(Socket);
		return;
	}

	do {
		// Clear screen.
		system("cls");

		// Get Server hostname.
		char ServerHostName[256] = { 0 };
		RetCheck = gethostname(ServerHostName, sizeof(ServerHostName));
		if (RetCheck == SOCKET_ERROR) {
			PRINTERROR("gethostname() of Server failed");
			closesocket(Socket);
			return;
		}
		printf("\n\n Simple Chat Server v0.2\n");
		printf("\n Server named %s waiting on port %d\n", ServerHostName, Port);

		int ClientSockLength = sizeof(SOCKADDR);	// Client's socket addr length.
		char MessageToSend[256] = { 0 };			// contains inputs to be sent to Client.
		char Buffer[256] = { 0 };					// contains message received from Client.
		int BytesReceived = 0;
		int BytesSent = 0;

		// Loop until Client terminate chat session.
		while (strcmp(Buffer, "quit") != 0) {
			// Clear buffers.
			memset(Buffer, 0, sizeof(Buffer));
			memset(MessageToSend, 0, sizeof(MessageToSend));

			// Try to receive incoming request.
			SOCKADDR_IN ClientSockAddr;
			RetCheck = recvfrom(Socket, Buffer, sizeof(Buffer), 0, (SOCKADDR*)&ClientSockAddr, &ClientSockLength);
			if (RetCheck == SOCKET_ERROR) {
				PRINTERROR("recvfrom() failed");
				closesocket(Socket);
				return;
			}
			BytesReceived += RetCheck;

			// Print out what the Client sent.
			printf(" user> %s ", Buffer);

			// End the session if Client quit.
			if (strcmp(Buffer, "quit") == 0) {
				break;
			}

			// Prepare to send a message back to Client.
			printf("\n %s> ", ServerHostName);

			// Clear the keyboard buffer.
			std::cin >> std::ws;

			// Send a message back to Client.
			std::cin.getline(MessageToSend, 256);
			RetCheck = sendto(Socket, MessageToSend, sizeof(MessageToSend), 0, (SOCKADDR*)&ClientSockAddr, ClientSockLength);
			if (RetCheck == SOCKET_ERROR) {
				PRINTERROR("sendto() failed");
				closesocket(Socket);
				return;
			}
			BytesSent += RetCheck;

			// Terminate if Server wants to quit.
			if (strcmp(MessageToSend, "quit") == 0) {
				closesocket(Socket);
				return;
			}

		}
		
		printf("\nSummary of session:\n");
		printf("Bytes received: %d\n", BytesReceived);
		printf("Bytes received: %d\n", BytesSent);
		printf("\nPress \"Enter\" to start a new session");

		// Press enter to start new session.
		char c[256] = { 0 };
		std::cin.getline(c, 256);
		while (c[0] != '\0') {
			printf("Key pressed is invalid. Please try again!");
			std::cin.getline(c, 256);
		}

		// NOTE(Quan): How can i easily capture the ESC key?
		//if (c == 27) { // This couldn't capture ESC key.
		//	break;
		//}

	} while (true);
	

	closesocket(Socket);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "\nSyntax: udp-server PortNumber\n");
		return 0;
	}

	// port input from cmd.
	unsigned short Port = atoi(argv[1]);
	
	// Initialize Winsock.
	WSADATA WsaData;
	int CheckWinsockCode = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (CheckWinsockCode != 0) {
		printf("WSAStartup failed!! %d\n", CheckWinsockCode);
		return 1;
	}

	UDPServer(Port);

	WSACleanup();
	return 0;
}
