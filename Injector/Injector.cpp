#include <Windows.h>

#include <iostream>

#include <fstream>

#include <TlHelp32.h>

#include <string>

DWORD GetProcId(const char* procName)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                if (!_stricmp(procEntry.szExeFile, procName))
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procId;
}

int main()
{
    const char* dllPath = "";

    const char* procName = "";
    DWORD procId = 0;

    while (!procId)
    {
        printf("Find Proc Id\n");
        procId = GetProcId(procName);
        Sleep(30);
    }
    printf("Proc Id:\n",procId);
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {
        void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);

        auto _adresslla = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        if (!_adresslla)
        {
            printf("Error _LoadLibraryA:\n", _adresslla);
        }
        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)_adresslla, loc, 0, 0);
        
        if (hThread)
        {
            CloseHandle(hThread);
        }
    }

    if (hProc)
    {
        CloseHandle(hProc);
    }
    
    std::cin.get();
}