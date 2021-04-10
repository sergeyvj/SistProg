#pragma once
#include "framework.h"
#include "Tools.h"

class Startup
{
public:

	/// <summary>
	/// ���������� ������ ������ �� ����� Startup
	/// </summary>
	/// <returns></returns>
	static wstring StartupRead();

	/// <summary>
	/// ������� ��������� �� ����� ������������
	/// </summary>
	/// <param name="wstr">��� ���������</param>
	/// <returns>true ��� ������, ����� false</returns>
	static bool StartupDel(wstring wstr);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="lpszPathObj">���� �����, ������� *.*</param>
	/// <param name="path">���� ��� ����������</param>
	/// <returns></returns>
	static bool CreateLink(LPCWSTR lpszPathObj, wstring path);
};

wstring Startup::StartupRead() {
	WIN32_FIND_DATA FindFileData;
	wstring str;
	HANDLE hFind = FindFirstFile((Tools::GetStartup() + L"*").c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		str = L"";
	}
	else do {
		if (wstring(FindFileData.cFileName) != L"." && wstring(FindFileData.cFileName) != L".." && wstring(FindFileData.cFileName) != L"desktop.ini") {
			str = str + L"Startup\t" + FindFileData.cFileName + L"\t\n";
		}
	} while (FindNextFile(hFind, &FindFileData));
	FindClose(hFind);
	return str;
}

bool Startup::StartupDel(wstring wstr)
{
	wstr = Tools::GetStartup() + wstr;
	_wremove(wstr.c_str());
	return true;
}

bool Startup::CreateLink(LPCWSTR lpszPathObj, wstring path)
{
	IShellLink* psl;
	HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);

	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf;
		psl->SetPath(lpszPathObj);
		
		//������� � ������� ������ ������� �����
		wstring WorkFolder = lpszPathObj;
		WorkFolder = WorkFolder.erase(WorkFolder.find(Tools::GetFileName(lpszPathObj))-1);
		psl->SetWorkingDirectory(WorkFolder.c_str());

		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

		if (SUCCEEDED(hres))
		{
			path += Tools::GetFileName(lpszPathObj) + L".lnk";
			hres = ppf->Save(path.c_str(), TRUE);
			ppf->Release();
			return true;
		}
		psl->Release();
	}
	return false;
}