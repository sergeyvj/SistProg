#pragma once
#include "framework.h"
#include "Tools.h"

class Registry
{
public:
    /// <summary>
    /// Добавление строки в реестр
    /// </summary>
    /// <param name="hkeyPath">Раздел реестра</param>
    /// <param name="lpszPathObj">Путь в реестре</param>
    /// <param name="path">Путь файла для добалнеия в автозагрузку</param>
    /// <returns>true при успехе, иначе false</returns>
    static bool RegAdd(HKEY hkeyPath, LPCWSTR lpszPathObj, LPCWSTR path);

	/// <summary>
	/// Возвращает значения по имени и пути из реестра
	/// </summary>
	/// <param name="key">Раздел реестра</param>
	/// <param name="str">Путь в реестре</param>
	/// <returns></returns>
    static wstring RegRead(HKEY key, LPCWSTR str);

    /// <summary>
    /// Удаление программы из реестра автозагрузки
    /// </summary>
    /// <param name="key">Раздел реестра</param>
    /// <param name="str">Путь в реестре</param>
    /// <param name="name">Имя программы</param>
    /// <returns>true при успехе, иначе false</returns>
    static bool RegDel(HKEY key, LPCWSTR str, wstring name);
};

bool Registry::RegAdd(HKEY hkeyPath, LPCWSTR lpszPathObj, LPCWSTR path) {
	HKEY hKey = NULL;
	if (RegOpenKey(hkeyPath, path, &hKey) != ERROR_SUCCESS) return 0;
	else
	{
		LSTATUS status1 = RegSetValueEx(hKey, Tools::GetFileName(lpszPathObj).c_str(), NULL, REG_SZ, (BYTE*)lpszPathObj, 20 * sizeof(lpszPathObj));
		return 1;
	}
}

wstring Registry::RegRead(HKEY key, LPCWSTR str) {
    //this_thread::sleep_for(chrono::milliseconds(5000));
    _TCHAR szBuf[MAX_PATH];
    DWORD dwBufLen = MAX_PATH;
    HKEY hKey = NULL;
    wstring wstr = L"";
    wstring ident = L"";
    if (key == HKEY_CURRENT_USER)
    {
        ident = L"UserReg";
    }
    if (key == HKEY_LOCAL_MACHINE && str == RegWRun)
    {
        ident = L"MachRegW";
    }
    if (key == HKEY_LOCAL_MACHINE && str == RegRun)
    {
        ident = L"MachReg";
    }

    int count = 0;
    if (RegOpenKey(key, str, &hKey) != ERROR_SUCCESS) return L"";
    else
    {
        while (RegEnumValueW(hKey, count, szBuf, &dwBufLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
        {
            wstr = wstr + ident + L"\t" + szBuf + L"\t\n";
            count++;
            dwBufLen = MAX_PATH;
        }
    }
    return wstr;
}

bool Registry::RegDel(HKEY key, LPCWSTR str, wstring name)
{
    HKEY hKey = NULL;
    if (RegOpenKey(key, str, &hKey) == ERROR_SUCCESS) {
        if (RegDeleteValueA(hKey, Tools::WstrToStr(name).c_str()) != ERROR_SUCCESS)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
}