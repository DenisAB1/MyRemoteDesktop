#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"

void CursorPosToString(char* sendbuf);

int main() 
{
	WSADATA wsaData;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	int iResult;
	SOCKET ListenSocket;
	SOCKET ClientSocket;

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;			// ����� �������������� IPv4
	hints.ai_socktype = SOCK_STREAM;	// ���������������� ������������ ����� ������
	hints.ai_protocol = IPPROTO_TCP;	// �������� - Transmission Control Protocol
	hints.ai_flags = AI_PASSIVE;		// ������ ������ ������������ � ����� �������

	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0)
	{
		cout << "WSAStartup failed: "<< iResult << endl;
		return 0;
	}

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);	// ������� ��������� ������ ������
	if (iResult != 0) 
	{
		cout <<"getaddrinfo failed: " << iResult << endl;
		WSACleanup();
		return 0;
	}

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);	// ������� �������� ����� ����������
	
	if (ListenSocket == INVALID_SOCKET) 
	{
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();			// ��� �������� WS2_32 DLL
		return 0;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen); // ����������� ��� � ������
    if (iResult == SOCKET_ERROR) 
	{
        cout << "Bind() failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 0;
    }

		// ������������� ���������� ��������� �������� ���������� � ������ ������ �������
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) 
	{
		cout << "Listen failed with error: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 0;
	}

	ClientSocket = accept(ListenSocket, NULL, NULL);	// ���� ����������� � �������
	if (ClientSocket == INVALID_SOCKET) 
	{
		cout << "accept failed: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 0;
	}

	cout << "Client has connected." << endl;

	char sendbuf[128];							//
	char xPos[5];
	char yPos[5];
	int iSendResult;
	int sendbuflen = 128;						//
	POINT pt;

	do 
	{
		CursorPosToString(sendbuf);
	
		sendbuflen = strlen(sendbuf);
		sendbuf[sendbuflen] = '\0';
		iSendResult = send(ClientSocket, sendbuf, sendbuflen+1, 0);
		if (iSendResult == SOCKET_ERROR) 
		{
			cout << "Send failed: " << WSAGetLastError() << endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 0;
		}
		//printf("Bytes sent: %d\n", iSendResult);
		Sleep(10);
	} 
	while (sendbuf[0] != 'q');

	freeaddrinfo(result);
    closesocket(ListenSocket);
    WSACleanup();
	cout << "end of all";
	_getch();
	return 0;
}

void CursorPosToString(char* sendbuf)
{
	POINT pt;
	char xPos[5];
	char yPos[5];
	GetCursorPos(&pt);
	sendbuf[0] = '\0';
	itoa(pt.x, xPos, 10);
	itoa(pt.y, yPos, 10);
	strcat(sendbuf, xPos);
	strcat(sendbuf, "_");
	strcat(sendbuf, yPos);
}