#include <tchar.h>
using namespace std;

static int Sum(int a, int b)
{
	return a + b;
}

/// <summary>
/// String to wstring
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
wstring StrToLP(string str)
{
	wstring stemp = wstring(str.begin(), str.end());
	return stemp;
}

/// <summary>
/// ���������� ������ � �������
/// </summary>
/// <returns></returns>
bool RegAdd() {
	HKEY hKey = NULL;
	if (RegOpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey) != ERROR_SUCCESS) return 1;
	_TCHAR path[] = _T("C:/usesd");
	LSTATUS status1 = RegSetValueEx(hKey, L"PARAM", NULL, REG_SZ, (BYTE*)path, sizeof(path));
}

/// <summary>
/// ������ �������� �� ����� � ���� �� ��������
/// </summary>
/// <param name="hWnd"></param>
/// <param name="str"></param>
/// <returns></returns>
bool RegRead(HWND hWnd,LPCWSTR str) {
	//�����:  RegRead(hWnd,L"Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	_TCHAR szBuf[MAX_PATH];
	DWORD dwBufLen = MAX_PATH;

	if (RegGetValue(HKEY_CURRENT_USER, str, _T("PARAM"), RRF_RT_REG_SZ, NULL, (BYTE*)szBuf, &dwBufLen) != ERROR_SUCCESS) {
		MessageBox(hWnd, TEXT("��� ������ ������ ��������� ������"), TEXT("���������"), 0);
		return 4;
	}
	MessageBox(hWnd, szBuf, TEXT("���������"), 0);
}

/// <summary>
/// ���������� ������ ������ �� ����� System32/Tasks
/// </summary>
/// <returns></returns>
wstring PlanRead(){
	WIN32_FIND_DATA FindFileData;
	wstring str;
	string img = "C:/Windows/System32/Tasks/*";
	HANDLE hFind = FindFirstFile(StrToLP(img).c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		str = L"";
	}
	else do {
		if (wstring(FindFileData.cFileName) != L"." && wstring(FindFileData.cFileName) != L"..") {
			str = str + FindFileData.cFileName + L"\n";
		}
		//MessageBox(hWnd, FindFileData.cFileName, TEXT("���������"), 0);
	} while (FindNextFile(hFind, &FindFileData));
	FindClose(hFind);
	str += L"\n";
	return str;
}

/// <summary>
/// ������ ������ �����, ���������� ����
/// </summary>
/// <param name="hWnd"></param>
/// <returns></returns>
wstring SelectFile(HWND hWnd) {
	OPENFILENAME ofn;
	wchar_t wtext[200];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = wtext;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 200;
	ofn.lpstrFilter = TEXT("All files\0*.*\0Source Files\0*.CPP\0Text Files\0*.TXT");
	ofn.nFilterIndex = 1;
	GetOpenFileName(&ofn);
	return  wtext;
}

/// <summary>
/// ���������� ������ ������ �� ����� Startup
/// </summary>
/// <param name="hWnd"></param>
/// <returns></returns>
wstring AutoRead() {
	WIN32_FIND_DATA FindFileData;
	wstring str;
	char* userpath = getenv("USERPROFILE");
	string img = string(userpath) + "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup/*";
	HANDLE hFind = FindFirstFile(StrToLP(img).c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		str = L"";
	}
	else do {
		if (wstring(FindFileData.cFileName) != L"." && wstring(FindFileData.cFileName) != L"..") {
			str = str + FindFileData.cFileName + L"\n";
		}
	} while (FindNextFile(hFind, &FindFileData));
	FindClose(hFind);
	str += L"\n";
	return str;
}