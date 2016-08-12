#include <windows.h>
#include <stdio.h>

int main(int argc, char** argv)
{

	HDC hScreenDC = GetDC(NULL);
	int w = GetDeviceCaps(hScreenDC, HORZRES);
	int h = GetDeviceCaps(hScreenDC, VERTRES);
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

	

	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, w, h);
	HGDIOBJ old_obj = SelectObject(hMemoryDC, hBitmap);
	BitBlt(hMemoryDC, 0, 0, w, h, hScreenDC, 0, 0, SRCCOPY);

//	BITMAP bitmap;
//	GetObject(hBitmap, sizeof(bitmap), &bitmap);

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
			printf("%d ", GetPixel(hMemoryDC, x, y));
		putchar('\n');
	}

	SelectObject(hMemoryDC, old_obj);
	DeleteDC(hMemoryDC);
	DeleteDC(hScreenDC);

	return 0;
}