#define _CRT_SECURE_NO_WARNINGS
#include <windows.h> 
#include <stdlib.h> 
#include <string.h> 
#include <tchar.h>
#include <fstream>
#include <iostream>
#include <string>
#include <commdlg.h>
#include "Tools.h"


using namespace std;

static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");
wchar_t buffer[256];
WCHAR* xxx;
ifstream fin;
ofstream fout;
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE hInstance, LPCTSTR szWindowClass);

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int  nCmdShow) 
{

	if (!RegMyWindowClass(hInstance, szWindowClass)) {
		MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Windows Desktop Guided Tour"), NULL);
		return 1;
	}
	
	hInst = hInstance;
	
	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, hInstance, NULL);
	if (!hWnd) { MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Windows Desktop Guided Tour"), NULL); return 1; }
	ShowWindow(hWnd, nCmdShow);   UpdateWindow(hWnd);
	// Main message loop:  
	MSG msg; 
	while (GetMessage(&msg, NULL, 0, 0))   
	{      
		TranslateMessage(&msg);      
		DispatchMessage(&msg);   
	}
	return (int)msg.wParam;
};

// функция регистрации класса окон
ATOM RegMyWindowClass(HINSTANCE hInstance, LPCTSTR szWindowClass) {
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	return RegisterClassEx(&wcex);
};


// функция обработки сообщений
LRESULT CALLBACK WndProc(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam) 
{
	PAINTSTRUCT ps;    
	HDC hdc;  
	static HWND hBtn; // дескриптор кнопки
	static HWND hStat; // дескриптор статического текста
	TCHAR greeting[] = _T("Hello, Windows desktop!");
	switch (message) {
	case WM_PAINT:     
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 5, 5, greeting, _tcslen(greeting));

		// Создаем и показываем кнопку
		hBtn = CreateWindow(_T("button"), _T("Рассчитать"),
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			200, 5, 120, 30, hWnd, 0, hInst, NULL);
		ShowWindow(hBtn, SW_SHOWNORMAL);

		// Создаем и показываем поле текста для результата
		hStat = CreateWindow(TEXT("static"), TEXT("0"), WS_CHILD | WS_VISIBLE,
			5, 40, 500, 400, hWnd, 0, hInst, NULL);
		ShowWindow(hStat, SW_SHOWNORMAL);
    
		EndPaint(hWnd, &ps);    
		break;   
	case WM_DESTROY:   
		PostQuitMessage(0);
		break; 
	case WM_LBUTTONUP:
		// реакция на сообщение
		MessageBox(hWnd, TEXT("ЛКМ"), TEXT("Сообщение"), 0);
		break;
	case WM_COMMAND:
		if (lParam == (LPARAM)hBtn) {
			//MessageBox(hWnd, _T("Кнопка нажата!"), _T("Сообщение"), NULL);
			//wsprintfW(buffer, L"%d", Sum(1, 5));
			
			wstring  strin = AutoRead() + PlanRead();
			SetWindowText(hStat, strin.c_str());
			
			//MessageBox(hWnd, SelectFile(hWnd).c_str(), _T("Сообщение"), NULL);
		}
		break;
	default:      return DefWindowProc(hWnd, message, wParam, lParam);      break;
	}
	return 0;
};