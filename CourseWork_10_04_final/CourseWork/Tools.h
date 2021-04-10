#pragma once
#define _WIN32_DCOM
#include "framework.h"

class Tools
{
public:
	/// <summary>
	/// string to wstring
	/// </summary>
	/// <returns></returns>
	static wstring StrToWstr(string str);

	/// <summary>
	/// wstring to string
	/// </summary>
	/// <returns></returns>
	static string WstrToStr(wstring wstr);

	/// <summary>
	/// Возвращает папку Автозагрузки
	/// </summary>
	/// <returns></returns>
	static wstring GetStartup();

	/// <summary>
	/// Возвращает имя файла по пути
	/// </summary>
	/// <param name="path">Путь файла, включая *.*</param>
	/// <returns></returns>
	static wstring GetFileName(LPCWSTR path);

	/// <summary>
	/// Диалог выбора файла, возвращает путь
	/// </summary>
	/// <param name="hWnd">для отображения диалогового окна</param>
	/// <returns></returns>
	static wstring SelectFile(HWND hWnd);
};

wstring Tools::StrToWstr(string str)
{
	wstring wstr = wstring(str.begin(), str.end());
	return wstr;
}

string Tools::WstrToStr(wstring wstr)
{
    const wstring ws = wstr;
    const string s(ws.begin(), ws.end());
    return s;
}

wstring Tools::GetStartup()
{
	char* userpath = getenv("USERPROFILE");
	string img = string(userpath) + "\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\";
	return StrToWstr(img);
}

wstring Tools::GetFileName(LPCWSTR path) {
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(path, &FindFileData);
	FindClose(hFind);
    wstring wstr = L"";
    int i = 0;
    while (FindFileData.cFileName[i] != L'.')
    {
        wstr += FindFileData.cFileName[i];
        i++;
    }
	return wstr;
}

wstring Tools::SelectFile(HWND hWnd) {
	OPENFILENAME ofn;
	wchar_t wtext[200];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = wtext;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 200;
	ofn.lpstrFilter = TEXT("All files\0*.*\0exe files\0*.exe");
	ofn.nFilterIndex = 1;
	GetOpenFileName(&ofn);
	return  wtext;
}
