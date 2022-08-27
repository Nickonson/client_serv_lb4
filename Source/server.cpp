#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define usePort 34353
#define SERVER_IP "192.168.100.4"

#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int WSASTARTUP(WORD wVersionRequired, LPWSADATA lpWSAData);
SOCKET Socket(int domain, int type, int protocol);
void Bind(SOCKET sockfd, const struct sockaddr* addr, int addrlen);
SOCKET Accept(SOCKET s, sockaddr * addr, int * addrlen);
void Listen(SOCKET srvLstn, int backlog);
int delFl(SOCKET s);
SOCKET Send(SOCKET s, const char* buf, int len, int flags);
SOCKET Recv(SOCKET s, char* buf, int len, int flags);

int main()
{
	WSAData wsadata;
	WORD DLLVersion = MAKEWORD(2, 2);
	WSASTARTUP(DLLVersion, &wsadata);

	sockaddr_in adr = { 0 };
	int sizeOfAdr = sizeof(adr);
	adr.sin_family = AF_INET;														// family of protocols
	adr.sin_port = htons(usePort);													//choosin port
	adr.sin_addr.s_addr = inet_addr(SERVER_IP);										// ip addr of server
	
	SOCKET sListen = Socket(AF_INET, SOCK_STREAM, NULL);		
	Bind(sListen, (SOCKADDR * )&adr, sizeof(adr));								//bindin addr to the port
	Listen(sListen, SOMAXCONN);													//listenin port
	SOCKET WSAAPI servConnect = accept(sListen, (SOCKADDR*)&adr, &sizeOfAdr);			//acceptin && connect an incommint connect. attemp	
	
	if (servConnect != -1)
		cout << "client connected\n";
	else
		while (servConnect == -1)
		{
			servConnect = accept(sListen, (SOCKADDR*)&adr, &sizeOfAdr);
			if (servConnect == 0)
				cout << "client connected\n";
			else
				cout << "awaiting for the connection\n";
			Sleep(100);
		}
	while (1)
	{
		int res = delFl(servConnect);
		if (res == -1)
			cout << "client disconnected\n";
		else if (res == 1)
			cout << "not possible to delete\n";
		else if (res == 0)
			cout << "deleted successfully\n";
		else
			cout << "unknown error\n";
		char usrAns;
		cout << "wanna try again with deleting?\n(y/n)\n";
		cin >> usrAns;
		if (usrAns == 'n')
			break;
		cin.ignore(256, '\n');

	}
	closesocket(servConnect);
	closesocket(sListen);
	WSACleanup();

	system("pause");
	return 0;
}

int WSASTARTUP(WORD wVersionRequired, LPWSADATA lpWSAData)
{
	int res = WSAStartup(wVersionRequired, lpWSAData);
	if (res != 0)
	{
		perror("Error with WINSOCK DLL");
		exit(EXIT_FAILURE);
	}
	return res;
}

SOCKET Socket(int domain, int type, int protocol)
{
	SOCKET res = socket(domain, type, protocol);
	// af inet - ipv4
	//tcp - sock_stream
	//udp - sock_dgram
	//0 - prtocol of lower lvl protocol
	if (res == -1)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	return res;
} 

void Bind(SOCKET sockfd, const struct sockaddr * addr, int addrlen)
{
	SOCKET res = bind(sockfd, addr, addrlen);
	if (res == -1)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
}

SOCKET Accept(SOCKET s, sockaddr* addr, int * addrlen)
{
	SOCKET res = accept(s, addr, addrlen);
	if (res == -1)
	{
		perror("acceptin failed");
		exit(EXIT_FAILURE);
	}
	return res;
}

void Listen(SOCKET srvLstn, int backlog)
{
	SOCKET res = listen(srvLstn, backlog);
	if (res == -1)
	{
		perror("listenin port failed\n");
		exit(EXIT_FAILURE);
	}
}

int delFl(SOCKET s)
{
	char flToDel[256], result[2], funcRes = 0;

	cout << "Enter the path of file to del : ";
	cin.getline(flToDel, 256);

	funcRes = Send(s, flToDel, 256, NULL);
	if (funcRes == -1)
		return -1;
	funcRes = Recv(s, result, sizeof(result), NULL);
	if (funcRes == -1)
		return -1;

	cout << "Result status: " << (int)(result[0] - '0') << endl;
	if ((int)(result[0] - '0') == 0)
		return 0;
	else
		return 1;
}

SOCKET Send(SOCKET s, const char * buf, int len, int flags)
{
	SOCKET res = send(s, buf, len, flags);
	if(res == -1)
	{
		perror("sending to client failed\n");
		exit(EXIT_FAILURE);
	}
	return res;
}

SOCKET Recv(SOCKET s, char * buf, int len, int flags)
{
	SOCKET res = recv(s, buf, len, flags);
	if (res == -1)
	{
		perror("problem with receivin client message\n");
		exit(EXIT_FAILURE);
	}
	return res;
}