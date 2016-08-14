#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
	case WM_CREATE:
		// Initialize the window. 
		return 0;

	case WM_PAINT:
		// Paint the window's client area. 
		return 0;

	case WM_SIZE:
		// Set the size and position of the window. 
		return 0;

	case WM_DESTROY:
		// Clean up window-specific data objects. 
		return 0;

		// 
		// Process other messages. 
		// 

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, int show_code)
{
	int w = 500;
	int h = 500;
	
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

	HDC hScreen = GetDC(NULL);
	HDC hWin = GetDC(win);

	ShowWindow(win, show_code);
	MSG msg;
	RECT rect;
	while (GetMessage(&msg, win, 0, 0) > 0)
	{
		GetWindowRect(win, &rect);
		w = rect.right - rect.left;
		h = rect.bottom - rect.top;
		BitBlt(hWin, 0, 0, w, h, hScreen, 0, 0, SRCCOPY);
		DispatchMessage(&msg);
	}
}