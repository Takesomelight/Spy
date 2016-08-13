#include <GL\freeglut.h>
#include <windows.h>
#include <stdio.h>


HDC hScreenDC, hMemoryDC;
HBITMAP hBitmap;
BITMAPINFOHEADER bmi;
HGDIOBJ old;

char* ScreenData;
int w, h;

void display()
{

}

void timer(int value)
{
	BitBlt(hMemoryDC, 0, 0, w, h, hScreenDC, 0, 0, SRCCOPY);
	GetDIBits(hMemoryDC, hBitmap, 0, h, ScreenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
	char buff;
	for (int i = 0; i < 3 * w*h; i += 3)
	{
		buff = ScreenData[i];
		ScreenData[i] = ScreenData[i + 2];
		ScreenData[i + 2] = buff;
	}

	glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, ScreenData);
	glFlush();
	glutTimerFunc(1, timer, 1);
}

int main(int argc, char** argv)
{

	hScreenDC = GetDC(NULL);
	w = GetDeviceCaps(hScreenDC, HORZRES);
	h = GetDeviceCaps(hScreenDC, VERTRES);
	hMemoryDC = CreateCompatibleDC(hScreenDC);
	hBitmap = CreateCompatibleBitmap(hScreenDC, w, h);
	SelectObject(hMemoryDC, hBitmap);

	bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 24;
	bmi.biWidth = w;
	bmi.biHeight = h;
	bmi.biSizeImage = 3 * w * h;
	bmi.biCompression = BI_RGB;

	ScreenData = (char*)malloc(3 * w * h);

	glutInit(&argc, argv);
	glutInitWindowSize(w, h);
	glutCreateWindow("Screen");
	glutDisplayFunc(display);
	glutTimerFunc(10, timer, 0);
	glutMainLoop();
	return 0;
}
