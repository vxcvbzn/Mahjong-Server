#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

void notify(std::string text)
{
	std::cout << text << "\n";
}

int main()
{
	// init
	notify("init");
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "Can't init winsock, quitting...\n";
		return 1;
	}

	// create socket
	notify("create socket");
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create a socket, quitting...\n";
		return 2;
	}

	// bind socket
	notify("bind socket");
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(34100);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// set as listening
	notify("set as listening");
	listen(listening, SOMAXCONN);

	// wait for connection
	notify("wait for connection");
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);
	
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << "\n";
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << "\n";
	}

	// close listening
	//notify("close listening");
	//closesocket(listening);

	// accept and echo message back
	notify("echo");
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);

		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in recv(), quitting...\n";
			break;
		}

		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected\n";
			break;
		}

		send(clientSocket, buf, bytesReceived + 1, 0);
	}

	// close socket
	notify("close client socket");
	closesocket(clientSocket);

	// cleanup
	notify("cleanup");
	WSACleanup();
}