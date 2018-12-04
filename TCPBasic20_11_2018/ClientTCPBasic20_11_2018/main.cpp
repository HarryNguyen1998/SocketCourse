#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib") // Longer way is Project -> %Project% Properties -> Linker -> "Additional Dependencies"

#define PRINTERROR(s) printf("%s: %ld\n", s, WSAGetLastError())

int main(int argc, char **argv) {
	
	// Initialize Winsock.
	WSADATA WsaData;
	int CheckWinsockCode = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (CheckWinsockCode != 0) {
		printf("WSAStartup failed!! %d\n", CheckWinsockCode);
		return 1;
	}

	// Create TCP socket.
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		PRINTERROR("Client: Error at socket()");
		closesocket(ClientSocket); 
		return 1;
	}

	unsigned short Port = atoi(argv[2]);
	// Specify params to bind @ClientSocket.
	sockaddr_in addr_connect;
	addr_connect.sin_family = AF_INET;
	addr_connect.sin_addr.s_addr = inet_addr(argv[1]);
	addr_connect.sin_port = htons(Port);
	
	// Connecting to a server.
	if (connect(ClientSocket, (SOCKADDR*)&addr_connect, sizeof(addr_connect)) == SOCKET_ERROR) {
		PRINTERROR("Client: connect() Failed");
		closesocket(ClientSocket);
		return 1;
	}

	// Send and receive data.
	int BytesSent, BytesRecv = SOCKET_ERROR;
	char SendBuf[256] = { 0 };
	char RecvBuf[256] = { 0 };
	while (1) {
		memset(SendBuf, 0, sizeof(SendBuf));
		memset(RecvBuf, 0, sizeof(RecvBuf));

		printf("Client> ");
		//scanf_s("%[^\n]", sendbuf, sizeof(sendbuf));
		fgets(SendBuf, sizeof(SendBuf), stdin);

		// Trim "\n" when using stdin & make sure that the buffer is not empty before trimming.
		if (*SendBuf && SendBuf[strlen(SendBuf) - 1] == '\n') {
			SendBuf[strlen(SendBuf) - 1] = '\0';
		}
		BytesSent = send(ClientSocket, SendBuf, strlen(SendBuf), 0);
		
		// If user types "quit", exit program.
		if (strcmp(SendBuf, "quit") == 0) {
			break;
		}

		BytesRecv = recv(ClientSocket, RecvBuf, sizeof(RecvBuf), 0);
		if (BytesRecv == 0 || BytesRecv == WSAECONNRESET) {
			printf("Client: Connection Closed.\n");
			break;
		}
		else {
			// If server wishes to exit the program.
			if (strcmp(RecvBuf, "quit") == 0) {
				break;
			}

			RecvBuf[BytesRecv] = '\0';
			printf("Server> %s\n", RecvBuf);
		}
	}

	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}

