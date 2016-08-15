#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT "29898"

SOCKET create_listen_socket(char* server_port)
{
	struct addrinfo *result = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	int iResult = getaddrinfo(NULL, server_port, &hints, &result);
	if (iResult != 0)
		return INVALID_SOCKET;

	SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listen_socket == INVALID_SOCKET)
	{
		freeaddrinfo(result);
		return INVALID_SOCKET;
	}

	iResult = bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) 
	{
		freeaddrinfo(result);
		closesocket(listen_socket);
		return INVALID_SOCKET;
	}
	freeaddrinfo(result);

	iResult = listen(listen_socket, SOMAXCONN_HINT(1));
	if (iResult == SOCKET_ERROR)
	{
		closesocket(listen_socket);
		return INVALID_SOCKET;
	}
	return listen_socket;
}


SOCKET accept_client_socket(SOCKET listen_socket)
{
	
}

int main()
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0)
	{
		printf("WSAStartup error\n");
		system("PAUSE");
		return 1;
	}

	SOCKET listen_socket = create_listen_socket(PORT);
	if (listen_socket == INVALID_SOCKET)
	{
		WSACleanup();
		printf("Error: can't create listen socket\n");
		system("PAUSE");
		return 1;
	}

	SOCKET master_socket = accept(listen_socket, NULL, NULL);
	closesocket(listen_socket);
	if (master_socket == INVALID_SOCKET)
	{	
		printf("Error: can't accept client socket, %d\n", WSAGetLastError());
		WSACleanup();
		system("PAUSE");
		return 1;
	}
	

	
	HDC hScreen = GetDC(NULL);
	int w = GetDeviceCaps(hScreen, HORZRES);
	int h = GetDeviceCaps(hScreen, VERTRES);
	int caps[] = { w, h };
	iResult = send(master_socket, (char*)caps, sizeof(int) * 2, 0);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(master_socket);
		WSACleanup();
		return 0;
	}

	HDC hMemory = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
	SelectObject(hMemory, hBitmap);

	BITMAPINFOHEADER bmi;
	bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 24;
	bmi.biWidth = w;
	bmi.biHeight = h;
	bmi.biSizeImage = 3 * w * h;
	bmi.biCompression = BI_RGB;

	int size = 3 * w * h;
	char* screen_data = (char*) malloc(size);

	do
	{
		BitBlt(hMemory, 0, 0, w, h, hScreen, 0, 0, SRCCOPY);
		GetDIBits(hMemory, hBitmap, 0, h, screen_data, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
		iResult = send(master_socket, screen_data, size, 0);
	} while (iResult > 0);

	free(screen_data);
	closesocket(master_socket);
	WSACleanup();
	return 0;
}
