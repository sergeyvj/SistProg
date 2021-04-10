#include "framework.h"
#include "Tools.h"
#include "Startup.h"
#include "Tasks.h"
#include "Registry.h"
#include "CourseWork.h"

static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Управление автозагрузкой");

bool start = false;
wstring column1;
wstring column2;
wstring AllAutoStarts;
wstring buf;
int ident;
static HWND hList;
static HWND hWnd;
HINSTANCE hInst;
HANDLE thread1;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Wind2(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Wind3(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE hInstance, LPCTSTR szWindowClass);

/// <summary>
/// Записывает все автозагрузки в переменную AllAutoStarts
/// </summary>
void GetAllAutoStarts() {
	wstring wstr1, wstr2, wstr3, wstr4, wstr5;
	thread th1([&wstr1]() {wstr1 += Startup::StartupRead(); });
	thread th2([&wstr2]() {wstr2 += Tasks::TasksRead(); });
	thread th3([&wstr3]() {wstr3 += Registry::RegRead(HKEY_CURRENT_USER, RegRun); });
	thread th4([&wstr4]() {wstr4 += Registry::RegRead(HKEY_LOCAL_MACHINE, RegWRun); });
	thread th5([&wstr5]() {wstr5 += Registry::RegRead(HKEY_LOCAL_MACHINE, RegRun); });
	th1.join();
	th2.join();
	th3.join();
	th4.join();
	th5.join();
	AllAutoStarts = wstr1 + wstr2 + wstr3+ wstr4+ wstr5;
	PostMessage(hWnd, WM_USER_DESTROY_LIST, 0, 0);
}

/// <summary>
/// Создание и заполнение ListBox
/// </summary>
void CreateListBox()
{
	int nTabStops[3] = { 60, 250 , 10};
	DestroyWindow(hList);
	hList = CreateWindow(_T("listbox"), NULL,
		WS_CHILD | WS_VISIBLE | LBS_STANDARD |
		LBS_USETABSTOPS | LBS_WANTKEYBOARDINPUT,
		50, 50, 900, 500, hWnd, (HMENU)ID_LIST2, hInst, NULL);
	ShowWindow(hList, SW_RESTORE);
	SendMessage(hList, LB_SETTABSTOPS, 3, (LPARAM)&nTabStops);

	SendMessage(hList, LB_RESETCONTENT, 0, NULL);
	
	for (int i = 0; i < AllAutoStarts.length(); i++)
	{
		if (AllAutoStarts[i] == '\n')
		{
			SendMessage(hList, LB_ADDSTRING, 0, (WPARAM)buf.c_str());
			buf = L"";
		}
		else
		{
			buf += AllAutoStarts[i];
		}
	}
	UpdateWindow(hList);
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int  nCmdShow) 
{

	if (!RegMyWindowClass(hInstance, szWindowClass)) {
		MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Windows Desktop Guided Tour"), NULL);
		return 1;
	}
	
	hInst = hInstance;
	
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1020, 600, (HWND)NULL, (HMENU)NULL, hInstance, NULL);
	if (!hWnd) { MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Windows Desktop Guided Tour"), NULL); return 1; }
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
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

// функция обработки сообщений главного окна
LRESULT CALLBACK WndProc(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam) 
{
	PAINTSTRUCT ps;    
	HDC hdc;  
	char szText[] = "";
	static HWND hBtn; // дескриптор кнопки
	static HWND hBtn2; // дескриптор кнопки
	static HWND hBtn3; // дескриптор кнопки
	
	if (start == false) {
		thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetAllAutoStarts, NULL, 0, NULL);
		start = true;
	}

	switch (message) {
	case WM_PAINT:     
		hdc = BeginPaint(hWnd, &ps);
		
		hBtn = CreateWindow(_T("button"), _T("Удалить"),
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			390, 5, 120, 30, hWnd, (HMENU)ID_Btn1, hInst, NULL);
		ShowWindow(hBtn, SW_SHOWNORMAL);
		UpdateWindow(hBtn);

		hBtn2 = CreateWindow(_T("button"), _T("Добавить"),
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			220, 5, 120, 30, hWnd, (HMENU)ID_Btn2, hInst, NULL);
		ShowWindow(hBtn2, SW_SHOWNORMAL);
		UpdateWindow(hBtn2);

		hBtn3 = CreateWindow(_T("button"), _T("Обновить"),
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			50, 5, 120, 30, hWnd, (HMENU)ID_Btn3, hInst, NULL);
		ShowWindow(hBtn3, SW_SHOWNORMAL);
		UpdateWindow(hBtn3);

		CreateListBox();
		UpdateWindow(hWnd);
		EndPaint(hWnd, &ps);    
		break;   
	case WM_DESTROY:   
		PostQuitMessage(0);
		break;
	case WM_USER_DESTROY_LIST:
	{
		DestroyWindow(hList);
	}
		break;
	case WM_KEYDOWN:
		if (wParam == 116)//F5
		{
			thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetAllAutoStarts, NULL, 0, NULL);
		}
		break;
	case WM_VKEYTOITEM://получаем кнопки нажатые в ListBox
		if (LOWORD(wParam) == 116)//F5
		{
			thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetAllAutoStarts, NULL, 0, NULL);
		}
		if (LOWORD(wParam) == 46)//delete
		{
			goto metka;
		}
		if (LOWORD(wParam) == 38 && HIWORD(wParam) > 0)//up
		{
			SendMessage(hList, LB_SETCURSEL, HIWORD(wParam)-1, 0);
		}
		if (LOWORD(wParam) == 40 && HIWORD(wParam) < SendMessage(hList, LB_GETCOUNT, 0, 0)-1)//down
		{
			SendMessage(hList, LB_SETCURSEL, HIWORD(wParam)+1, 0);	
		}
		return -2;
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_LIST2:
		{
			if (HIWORD(wParam) == LBN_DBLCLK)
			{
				metka:
				wchar_t str_2[255];
				ident = SendMessage(hList, LB_GETCURSEL, 0, 0);
				SendMessage(hList, LB_GETTEXT, ident, (LPARAM)str_2);

				//Получим 1ый столбец
				column1 = str_2;
				column1 = column1.erase(column1.find(L"\t"));

				//Получим 2ой столбец
				column2 = str_2;
				column2 = column2.substr(column2.find(L"\t") + 1);
				column2 = column2.erase(column2.find(L"\t"));

				if (column1==L"Startup")
				{
					Startup::StartupDel(column2);
					SendMessage(hList, LB_DELETESTRING, ident, NULL);
				}
				if (column1==L"Tasks")
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, Wind3);
				}
				if (column1 == L"UserReg")
				{
					if (Registry::RegDel(HKEY_CURRENT_USER, RegRun, column2) == true)
						SendMessage(hList, LB_DELETESTRING, ident, NULL);
				}
				if (column1 == L"MachRegW")
				{
					if (Registry::RegDel(HKEY_LOCAL_MACHINE, RegWRun, column2)==true)
						SendMessage(hList, LB_DELETESTRING, ident, NULL);
				}
				if (column1 == L"MachReg")
				{
					if (Registry::RegDel(HKEY_LOCAL_MACHINE, RegRun, column2)==true)
						SendMessage(hList, LB_DELETESTRING, ident, NULL);
				}
			}
		}
		break;
		case ID_Btn1:
		{
			if (SendMessage(hList, LB_GETCURSEL, 0, 0) != -1) goto metka;//проверка на выбор поля
		}
			break;
		case ID_Btn2:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, Wind2);
			break;
		case ID_Btn3:
		{
			thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetAllAutoStarts, NULL, 0, NULL);
		}
			break;
		default:
			break;
		}
		break;
	default:      return DefWindowProc(hWnd, message, wParam, lParam);      break;
	}
	return 0;
};

// функция обработки сообщений окна добавления в автозагрузку
LRESULT CALLBACK Wind2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	wstring strin;
	static int radio = 0;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RADIO1:
			radio = 1;
			break;
		case IDC_RADIO2:
			radio = 2;
			break;
		case IDC_RADIO3:
			radio = 3;
			break;
		case IDC_RADIO4:
			radio = 4;
			break;
		case IDC_BUTTON1:
		{
			SetDlgItemText(hDlg, IDC_EDIT1, Tools::SelectFile(hDlg).c_str());
		}
			break;
		case IDC_BUTTON2:
			TCHAR text[256];
			
			GetDlgItemText(hDlg, IDC_EDIT1, text, 256);
			if (GetFileAttributes(text) != DWORD(-1))
			{
				switch (radio)
				{
				case 1:
				{
					bool result = false;
					thread th1([&result, &text]() {result = Startup::CreateLink(text, Tools::GetStartup()); });
					th1.join();
					if (result == true)
						MessageBox(hDlg, _T("Файл добален в папку автозагрузки"), _T("Сообщение"), NULL);
					else MessageBox(hDlg, _T("Ошибка, презапустите программу"), _T("Сообщение"), NULL);
				}
					break;
				case 2:
				{
					bool result = false;
					thread th1([&result, &text]() {result = Tasks::CreateTask(text); });
					th1.join();
					if (result == true)
						MessageBox(hWnd, L"Файл успешно добавлен в автозагрузку с помощью планировщика", L"Сообщение", 0);
					else MessageBox(hDlg, _T("Ошибка"), _T("Сообщение"), NULL);			
				}
					break;
				case 3:
				{
					bool result = false;
					thread th1([&result,&text]() {result = Registry::RegAdd(HKEY_CURRENT_USER, text, RegRun); });
					th1.join();
					if (result == true)
						MessageBox(hDlg, _T("Файл добавлен в автозагрузку с помощью реестра учетной записи"), _T("Сообщение"), NULL);
					else MessageBox(hDlg, _T("Ошибка"), _T("Сообщение"), NULL);
				}	
					break;
				case 4:
				{
					bool result = false;
					thread th1([&result, &text]() {result = Registry::RegAdd(HKEY_LOCAL_MACHINE, text, RegWRun); });
					th1.join();
					if (result == true)
						MessageBox(hDlg, _T("Файл добавлен в автозагрузку с помощью общего реестра"), _T("Сообщение"), NULL);
					else MessageBox(hDlg, _T("Ошибка"), _T("Сообщение"), NULL);
				}
					break;
				default:
					MessageBox(hDlg, _T("Выберите тип"), _T("Сообщение"), NULL);
					break;
				}
			}
			else
			{
				MessageBox(hDlg, _T("Файл не существует"), _T("Сообщение"), NULL);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			radio = 0;
			break;
		case IDOK:
			EndDialog(hDlg, LOWORD(wParam));
			radio = 0;
			break;
		default:
			break;
		}
		break;
	}
	return 0;
}

// функция обработки сообщений окна выбора действия с задачей автозапуска программы
LRESULT CALLBACK Wind3(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
		{
			bool result = false;
			thread th1([&result, &hDlg]() {result = Tasks::On_Off_Task(column2.c_str(),0); });
			th1.join();
			if (result == true) {
				SendMessage(hList, LB_DELETESTRING, ident, NULL);
				buf = column1 + L"\t" + column2 + L"\tDisable\n";
				SendMessage(hList, LB_ADDSTRING, 0, (WPARAM)buf.c_str());
				MessageBox(hWnd, L"Автозапуск программы отключён", L"Сообщение", 0);
			}
			else MessageBox(hWnd, L"Ошибка", L"Сообщение", 0);
			EndDialog(hDlg, LOWORD(wParam));
		}
			break;
		case IDC_BUTTON2:
		{
			bool result = false;
			thread th1([&result, &hDlg]() {result = Tasks::On_Off_Task(column2.c_str(), -1); });
			th1.join();
			if (result == true) {
				SendMessage(hList, LB_DELETESTRING, ident, NULL);
				buf = column1 + L"\t" + column2 + L"\tEnable\n";
				SendMessage(hList, LB_ADDSTRING, 0, (WPARAM)buf.c_str());
				MessageBox(hWnd, L"Автозапуск программы включён", L"Сообщение", 0);
			}
			else MessageBox(hWnd, L"Ошибка", L"Сообщение", 0);
			EndDialog(hDlg, LOWORD(wParam));
		}
			break;
		case IDC_BUTTON3:
		{
			bool result = false;
			thread th1([&result,&hDlg]() {result = Tasks::DeleteTask(column2.c_str()); });
			th1.join();
			if (result == true) {
				SendMessage(hList, LB_DELETESTRING, ident, NULL);
				MessageBox(hWnd, L"Файл успешно удалён", L"Сообщение", 0);
			}
			else MessageBox(hWnd, L"Ошибка", L"Сообщение", 0);
			EndDialog(hDlg, LOWORD(wParam));
		}
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		case IDOK:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		default:
			break;
		}
		break;
	}
	return 0;
}
