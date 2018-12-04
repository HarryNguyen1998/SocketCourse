#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib") // Longer way is Project -> %Project% Properties -> Linker -> "Additional Dependencies"

#define PRINTERROR(s) printf("%s: %ld\n", s, WSAGetLastError())

// Server should never actively terminate itself because the closed port will be inactive for 3 minutes.

int main(int argc, char **argv) {

	if (argc != 2) {
		fprintf(stderr, "\nSyntax: tcp-server PortNumber\n");
		return 0;
	}

	// Initialize Winsock.
	WSADATA WsaData;
	int CheckWinsockCode = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (CheckWinsockCode != 0) {
		printf("WSAStartup failed!! %d\n", CheckWinsockCode);
		return 1;
	}

	// Make a socket to listen for incoming connection requests.
	SOCKET ListenSocket;
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == SOCKET_ERROR) {
		PRINTERROR("Socket creation failed!");
		closesocket(ListenSocket);
		return 1;
	}

	unsigned short Port = atoi(argv[1]);
	// Specify params to bind Socket.
	sockaddr_in addr_listen;
	addr_listen.sin_family = AF_INET;
	addr_listen.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_listen.sin_port = htons(Port);

	// Trying to bind Socket to specified params.
	if (bind(ListenSocket, (SOCKADDR*)&addr_listen, sizeof(addr_listen)) == SOCKET_ERROR) {
		PRINTERROR("Server: bind() failed!");
		closesocket(ListenSocket);
		return 1;
	}

	// Listen for incoming connection requests on the created socket.
	if (listen(ListenSocket, 10) == SOCKET_ERROR) {
		printf("Server: Error listening on socket.\n");
	}

	// Create a SOCKET for accepting incoming requests.
	while (1) {
		SOCKET AcceptSocket = SOCKET_ERROR;
		sockaddr_in Client;
		int ClientLength = sizeof(Client);
		char RecvBuf[256] = { 0 };
		char SendBuf[256] = { 0 };

		printf("\nServer: Waiting for client to connect...\n");
		AcceptSocket = accept(ListenSocket, (sockaddr*)&Client, &ClientLength);

		printf("Server: accept() is OK.\n");
		printf("Server: Client connected...ready for communication.\n");

		while (strcmp(SendBuf, "quit") != 0) {
			memset(RecvBuf, 0, sizeof(RecvBuf));
			memset(SendBuf, 0, sizeof(SendBuf));
			int Rec = recv(AcceptSocket, RecvBuf, sizeof(RecvBuf), 0);
			if (Rec <= 0) { break; }

			// If client types "quit", exit the session.
			if (strcmp(RecvBuf, "quit") == 0) {
				printf("Client disconnected!!!");
				// Terminate session and listen for new connection.
				strncpy_s(SendBuf, RecvBuf, sizeof(SendBuf));
			} else {
				printf("Received %d bytes: %s\n", Rec, RecvBuf);
				printf("Server> ");
				// Scan the whole line the C way.
				//scanf_s("%[^\n]", SendBuf, sizeof(SendBuf));
				fgets(SendBuf, sizeof(SendBuf), stdin);

				// Trim "\n" when using stdin & make sure that the buffer is not empty before trimming.
				if (*SendBuf && SendBuf[strlen(SendBuf) - 1] == '\n') {
					SendBuf[strlen(SendBuf) - 1] = '\0';
				}
				int sec = send(AcceptSocket, SendBuf, strlen(SendBuf), 0);
				if (sec > 0) {
					printf("Number of bytes sent: %d\n", sec);
				}
			}

		}

	}

	closesocket(ListenSocket);
	WSACleanup();
	return 0;
}
