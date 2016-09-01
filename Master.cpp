#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>


#pragma comment(lib, "Ws2_32.lib")

#define PORT "29898"
#define ADDR "192.168.1.103"

SOCKET create_client_socket(char* server_addr, char* server_port)
{
	struct addrinfo  *result = NULL;
	int iResult = getaddrinfo(server_addr, server_port, NULL, &result);
	if (iResult != 0) 
		return INVALID_SOCKET;

	SOCKET client_socket = INVALID_SOCKET;
	client_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (client_socket == INVALID_SOCKET)
	{
		freeaddrinfo(result);
		return INVALID_SOCKET;
	}

	iResult = connect(client_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) 
	{
		closesocket(client_socket);
		freeaddrinfo(result);
		return INVALID_SOCKET;
	}
	freeaddrinfo(result);
	return client_socket;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


inline bool recv(SOCKET client_socket, char* data, int size)
{
	int recivied;
	while (size > 0)
	{
		recivied = recv(client_socket, data, size, 0);
		if (0 == recivied)
			return false;
		data += recivied;
		size -= recivied;
	}
	return true;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, int show_code)
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0)
	{
		printf("WSAStartup error\n");
		system("PAUSE");
		return 1;
	}

	SOCKET slave_socket = create_client_socket(ADDR, PORT);
	if ( INVALID_SOCKET == slave_socket )
	{
		WSACleanup();
		printf("Error\n");
		system("PAUSE");
		return 1;
	}
	
	int* buf = (int*)malloc(sizeof(int) * 2);
	iResult = recv(slave_socket, (char*)buf, sizeof(int) * 2, 0);
	if (iResult <= 0)
	{
		closesocket(slave_socket);
		WSACleanup();
		system("PAUSE");
		return 0;
	}
	int w = *buf;
	int h = *(buf + 1);

	LPCTSTR class_name = L"SPY_window_class";
	WNDCLASS window_class;
	window_class.style = CS_OWNDC;
	window_class.lpfnWndProc = WindowProc;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = instance;
	window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	window_class.hbrBackground = NULL;
	window_class.lpszMenuName = 0;
	window_class.lpszClassName = class_name;
	RegisterClass(&window_class);
	HWND win = CreateWindowW(class_name, L"SPY", WS_OVERLAPPEDWINDOW, 50, 50, w, h, NULL, NULL, instance, 0);

	HDC hWin = GetDC(win);

	BITMAPINFOHEADER bmi;
	bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 24;
	bmi.biWidth = w;
	bmi.biHeight = h;
	bmi.biSizeImage = 3 * w * h;
	bmi.biCompression = BI_RGB;
	
	ShowWindow(win, show_code);
	MSG msg;
	FILE* f = fopen("log.txt", "w");
	fprintf(f, "%d, %d\n", w, h);
	fclose(f);
	int size = 3 * w * h;
	char* screen_data = (char*) malloc( size );

	while ( recv( slave_socket, screen_data, size ) )
	{
		if (PeekMessage(&msg, win, 0, 0, PM_REMOVE))
			DispatchMessage(&msg);
		StretchDIBits(hWin, 0, 0, w, h, 0, 0, w, h, screen_data, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, SRCCOPY);
	}


	closesocket(slave_socket);
	WSACleanup();
	system("PAUSE");
	return 0;
}
