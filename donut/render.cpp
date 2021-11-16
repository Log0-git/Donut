#include <Windows.h>
#include "render.h"
#include <Math.h>

HDC backDC;
bool isDrawing = true;

float A = 1.f, B = 1.f;

float R1 = 1.f;
float R2 = 2.f;
float K1 = 150.f;
float K2 = 5.f;

// Math taken from https://www.a1k0n.net/2011/07/20/donut-math.html

void RenderDonut(HDC hdc, int width, int height)
{
	RECT rc = { 0 };
	rc.right = width; // right is a width
	rc.bottom = height; // bottom is a height

	FillRect(hdc, &rc, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH))); // clear screen

	A += 0.07; // for rotation by the x-axis
	B += 0.03; // rotation by the z-axis

	float cA = cos(A), sA = sin(A), 
		cB = cos(B), sB = sin(B);

	for (float theta = 0; theta < 6.28; theta += 0.3) // theta
	{
		float ct = cos(theta), st = sin(theta); 
		for (float phi = 0; phi < 6.28; phi += 0.1) // phi
		{
			float sp = sin(phi), cp = cos(phi);  
			float ox = R1 + R2 * ct, // object x
				oy = R1 * st; // object y

			float x = ox * (cB * cp + sA * sB * sp) - oy * cA * sB;
			float y = ox * (sB * cp - sA * cB * sp) + oy * cA * cB;
			float ooz = 1 / (K2 + cA * ox * sp + sA * oy); // "One over Z" from original code

			float xp = (150 + K1 * ooz * x);
			float yp = (120 - K1 * ooz * y);
			
			float L = 0.7 * (cp * ct * sB - cA * ct * sp - sA * st + cB * (cA * st - ct * sA * sp)); 

			if (L > 0) // this IF condition taken from Javascript version of "Spinning Donut"
			{
				PatBlt(hdc, xp, yp, 1, 1, PATCOPY);
			}
		}
	}
}

// render thread implemented with Win32 API
unsigned int RenderThread(void* lpThreadParameter) // lpThreadParameter is hWnd
{
	HWND hWnd = (HWND)lpThreadParameter;
	RECT rc;
	GetClientRect(hWnd, &rc);

	HDC dc = GetDC(hWnd);
	backDC = CreateCompatibleDC(dc);
	HBITMAP backBitmap = CreateCompatibleBitmap(backDC, rc.right, rc.bottom);

	SelectObject(backDC, backBitmap);
	ReleaseDC(hWnd, dc);

	while (isDrawing)
	{
		HGDIOBJ oldBrush = SelectObject(backDC, GetStockObject(WHITE_BRUSH));

		RenderDonut(backDC, rc.right, rc.bottom);

		SelectObject(backDC, oldBrush);

		RedrawWindow(hWnd, nullptr, NULL, RDW_INVALIDATE);
		Sleep(50);
	}
	
	DeleteDC(backDC);
	return DeleteObject(backBitmap);
}

void Draw(HDC hdc, RECT rc)
{
	BitBlt(hdc, 0, 0, rc.right, rc.bottom, backDC, 0, 0, SRCCOPY);
}

void StopDrawing()
{
	isDrawing = false;
}