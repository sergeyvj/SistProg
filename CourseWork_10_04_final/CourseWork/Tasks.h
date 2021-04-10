#pragma once
#include "framework.h"
#include "Tools.h"

class Tasks
{
public:
	/// <summary>
	/// Создаёт задачу в планировщике заданий
	/// </summary>
	/// <param name="lpszPathObj">Путь файла, включая *.*</param>
	/// <returns>true при успехе, иначе false</returns>
	static bool CreateTask(LPCWSTR lpszPathObj);

    /// <summary>
    /// Удаляет задачу из планировщика
    /// </summary>
    /// <param name="lpszPathObj">Имя задачи</param>
    /// <returns>true при успехе, иначе false</returns>
    static bool DeleteTask(LPCWSTR lpszPathObj);

    /// <summary>
    /// Возвращает список задач планировщика
    /// </summary>
    /// <returns></returns>
    static wstring TasksRead();

    /// <summary>
    /// Включает и выключает задачу в планировщике
    /// </summary>
    /// <param name="lpszPathObj">Имя задачи</param>
    /// <param name="varbool"> 0 = выключить, -1 = включить</param>
    /// <returns>true при успехе, иначе false</returns>
    static bool On_Off_Task(LPCWSTR lpszPathObj, VARIANT_BOOL varbool);
};

bool Tasks::CreateTask(LPCWSTR lpszPathObj)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        return false;
    }

    if (FAILED(hr))
    {
        CoUninitialize();
        return false;
    }

    //  Create a name for the task.
    wstring Name = (Tools::GetFileName(lpszPathObj));

    //  Create an instance of the Task Service. 
    ITaskService* pService = NULL;
    hr = CoCreateInstance(CLSID_TaskScheduler,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_ITaskService,
        (void**)&pService);
    if (FAILED(hr))
    {
        CoUninitialize();
        return false;
    }

    //  Connect to the task service.
    hr = pService->Connect(_variant_t(), _variant_t(),
        _variant_t(), _variant_t());
    if (FAILED(hr))
    {
        pService->Release();
        CoUninitialize();
        return false;
    }

    //  Get the pointer to the root task folder.  This folder will hold the
    //  new task that is registered.
    ITaskFolder* pRootFolder = NULL;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr))
    {
        pService->Release();
        CoUninitialize();
        return false;
    }

    //  If the same task exists, remove it.
    hr = pRootFolder->DeleteTask(_bstr_t(Name.c_str()), 0);

    //  Create the task builder object to create the task.
    ITaskDefinition* pTask = NULL;
    hr = pService->NewTask(0, &pTask);

    pService->Release();  // COM clean up.  Pointer is no longer used.
    if (FAILED(hr))
    {
        pRootFolder->Release();
        CoUninitialize();
        return false;
    }

    //  Get the registration info for setting the identification.
    IRegistrationInfo* pRegInfo = NULL;
    hr = pTask->get_RegistrationInfo(&pRegInfo);
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }

    //  Create the principal for the task
    IPrincipal* pPrincipal = NULL;
    hr = pTask->get_Principal(&pPrincipal);
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }

    //  Set up principal information: 
    hr = pPrincipal->put_Id(_bstr_t(L"Principal1"));

    hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);

    //  Run the task with the least privileges (LUA) 
    hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_LUA);
    pPrincipal->Release();
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }

    //  Create the settings for the task
    ITaskSettings* pSettings = NULL;
    hr = pTask->get_Settings(&pSettings);
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }

    //  Set setting values for the task.
    hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
    pSettings->Release();
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }

    //  Get the trigger collection to insert the registration trigger.
    ITriggerCollection* pTriggerCollection = NULL;
    hr = pTask->get_Triggers(&pTriggerCollection);
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }

    //  Add the registration trigger to the task.
    ITrigger* pTrigger = NULL;
    hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
    pTriggerCollection->Release();
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }


    ILogonTrigger* pLoginTrigger = NULL;
    hr = pTrigger->QueryInterface(
        IID_ILogonTrigger, (void**)&pLoginTrigger);
    pTrigger->Release();
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }

    hr = pLoginTrigger->put_Id(_bstr_t(L"Trigger1"));
 
    IActionCollection* pActionCollection = NULL;

    //  Get the task action collection pointer.
    hr = pTask->get_Actions(&pActionCollection);
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }

    //  Create the action, specifying that it is an executable action.
    IAction* pAction = NULL;
    hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
    pActionCollection->Release();
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }

    IExecAction* pExecAction = NULL;

    //  QI for the executable task pointer.
    hr = pAction->QueryInterface(
        IID_IExecAction, (void**)&pExecAction);
    pAction->Release();
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }

    //Отключим параметры электропитания
    hr = pSettings->put_DisallowStartIfOnBatteries(0);
    hr = pSettings->put_StopIfGoingOnBatteries(0);

    //  Set the path of the executable to notepad.exe.
    hr = pExecAction->put_Path(_bstr_t(lpszPathObj));
    pExecAction->Release();
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }

    //  Save the task in the root folder.
    IRegisteredTask* pRegisteredTask = NULL;
    hr = pRootFolder->RegisterTaskDefinition(
        //_bstr_t(wszTaskName),
        _bstr_t(Name.c_str()),
        pTask,
        TASK_CREATE_OR_UPDATE,
        _variant_t(),
        _variant_t(),
        TASK_LOGON_INTERACTIVE_TOKEN,
        _variant_t(L""),
        &pRegisteredTask);
    if (FAILED(hr))
    {
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return false;
    }

    //  Clean up.
    pRootFolder->Release();
    pTask->Release();
    pRegisteredTask->Release();
    CoUninitialize();
    return true;
}

bool Tasks::DeleteTask(LPCWSTR lpszPathObj)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        return false;
    }

    if (FAILED(hr))
    {
        CoUninitialize();
        return false;
    }

    //  Create a name for the task.
    wstring Name = (lpszPathObj);

    //  Create an instance of the Task Service. 
    ITaskService* pService = NULL;
    hr = CoCreateInstance(CLSID_TaskScheduler,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_ITaskService,
        (void**)&pService);
    if (FAILED(hr))
    {
        CoUninitialize();
        return false;
    }

    //  Connect to the task service.
    hr = pService->Connect(_variant_t(), _variant_t(),
        _variant_t(), _variant_t());
    if (FAILED(hr))
    {
        pService->Release();
        CoUninitialize();
        return false;
    }

    //  Get the pointer to the root task folder.  This folder will hold the
    //  new task that is registered.
    ITaskFolder* pRootFolder = NULL;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr))
    {
        pService->Release();
        CoUninitialize();
        return false;
    }

    //  If the same task exists, remove it.
    hr = pRootFolder->DeleteTask(_bstr_t(Name.c_str()), 0);

    //  Clean up.
    pRootFolder->Release();
    CoUninitialize();
    return true;
}

wstring Tasks::TasksRead() //LPCWSTR lpszPathObj, wstring wstr
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        return L"";
    }

    if (FAILED(hr))
    {
        CoUninitialize();
        return L"";
    }

    //  Create an instance of the Task Service. 
    ITaskService* pService = NULL;
    hr = CoCreateInstance(CLSID_TaskScheduler,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_ITaskService,
        (void**)&pService);
    if (FAILED(hr))
    {
        CoUninitialize();
        return L"";
    }

    //  Connect to the task service.
    hr = pService->Connect(_variant_t(), _variant_t(),
        _variant_t(), _variant_t());
    if (FAILED(hr))
    {
        pService->Release();
        CoUninitialize();
        return L"";
    }

    //  Get the pointer to the root task folder.  This folder will hold the
    //  new task that is registered.
    ITaskFolder* pRootFolder = NULL;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr))
    {
        pService->Release();
        CoUninitialize();
        return L"";
    }
    IRegisteredTask* pTask = NULL;

    if (FAILED(hr))
    {
        CoUninitialize();
        return L"";
    }

    VARIANT_BOOL boo = NULL;

    WIN32_FIND_DATA FindFileData;
    wstring str;
    string img = "C:/Windows/System32/Tasks/*";
    HANDLE hFind = FindFirstFile(Tools::StrToWstr(img).c_str(), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        str = L"";
    }
    else do {
        if (wstring(FindFileData.cFileName) != L"." && wstring(FindFileData.cFileName) != L".." && wstring(FindFileData.cFileName) != L"Microsoft") {
            hr = pRootFolder->GetTask(_bstr_t(FindFileData.cFileName), &pTask);
            hr = pTask->get_Enabled(&boo);
            if (boo == -1)
            {
                str = str + L"Tasks\t" + FindFileData.cFileName + L"\tEnable\n";
            }
            else
            {
                str = str + L"Tasks\t" + FindFileData.cFileName + L"\tDisable\n";
            }
        }
    } while (FindNextFile(hFind, &FindFileData));
    FindClose(hFind);

    //  Clean up.
    pRootFolder->Release();
    pTask->Release();
    CoUninitialize();

    return str;
}

bool Tasks::On_Off_Task(LPCWSTR lpszPathObj, VARIANT_BOOL varbool) //LPCWSTR lpszPathObj, wstring wstr
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        return false;
    }

    if (FAILED(hr))
    {
        CoUninitialize();
        return false;
    }

    //  Create a name for the task.
    wstring Name = (lpszPathObj);

    //  Create an instance of the Task Service. 
    ITaskService* pService = NULL;
    hr = CoCreateInstance(CLSID_TaskScheduler,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_ITaskService,
        (void**)&pService);
    if (FAILED(hr))
    {
        CoUninitialize();
        return false;
    }

    //  Connect to the task service.
    hr = pService->Connect(_variant_t(), _variant_t(),
        _variant_t(), _variant_t());
    if (FAILED(hr))
    {
        pService->Release();
        CoUninitialize();
        return false;
    }

    //  Get the pointer to the root task folder.  This folder will hold the
    //  new task that is registered.
    ITaskFolder* pRootFolder = NULL;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr))
    {
        pService->Release();
        CoUninitialize();
        return false;
    }

    IRegisteredTask* pTask = NULL;
    hr = pRootFolder->GetTask(_bstr_t(Name.c_str()), &pTask);

    pRootFolder->Release();
    if (FAILED(hr))
    {
        CoUninitialize();
        return false;
    }

    hr = pTask->put_Enabled(varbool);

    pTask->Release();
    CoUninitialize();
    return true;
}