#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
#define _CRT_SECURE_NO_WARNINGS
#define RegWRun L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"
#define RegRun L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define  WM_USER_DESTROY_LIST  WM_APP
// Файлы заголовков Windows
#include <windows.h>
#include <tchar.h>
#include <string>
#include <commdlg.h>
#include <thread>
#include <comdef.h>
#include <wincred.h>
//  Include the task header file.
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")
#include <shobjidl_core.h>
#include "shlguid.h"
using namespace std;
