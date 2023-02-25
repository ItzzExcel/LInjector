#include <iostream>
#include <Windows.h>
#include <string>
#include <memoryapi.h>
#include <cstring>
#include <stdlib.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <libloaderapi.h>

void Reload() {
    system("LInjector.exe");
};

int main(int argc, char const* argv[]) {
    MessageBox(NULL, L"This application was made by @ItzzzExcel as an open-source project under the MIT License.", L"LInjector | Welcome", NULL);

    const char* DLLPath = "injector.dll";
    
    int wstrLength = MultiByteToWideChar(CP_ACP, 0, DLLPath, -1, NULL, 0);
    wchar_t* WDLLPath = new wchar_t[wstrLength];
    MultiByteToWideChar(CP_ACP, 0, DLLPath, -1, WDLLPath, wstrLength);
    HMODULE HModule = LoadLibrary(WDLLPath);
    if (HModule == NULL)
    {
        std::cerr << "An unexpected error loading the injector.";
        return 1;
    }

    // Cleaning Memory
    delete[] WDLLPath;

    // Injection

    const char* TargetRBLX = "RobloxPlayerBeta.exe";
    const wchar_t* Targett = L"RobloxPlayerBeta.exe";
    DWORD TargetProcessID = 0;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &entry))
    {
        do
        {
            if (wcsicmp(entry.szExeFile, Targett) == NULL) {
                TargetProcessID = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);

    if (TargetProcessID == 0)
    {
        std::cerr << "Couldn't find process : " << Targett << std::endl;
        return 1;
    }

    // Getting a Handle 🗿🗿🗿

    HANDLE TargetProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, TargetProcessID);
    if (TargetProcessHandle == NULL)
    {
        std::cerr << "Couldn't get a handle for the target process." << std::endl;
        return 1;
    }

    // Get base module path

    HMODULE TargetModuleBase = NULL;
    DWORD cbNeeded = 0;
    if (!EnumProcessModules(TargetProcessHandle, &TargetModuleBase, sizeof(TargetModuleBase), &cbNeeded))
    {
        std::cerr << "Couldn't get the base direction of the module of the process." << std::endl;
        return 1;
    }

    // Reserve memory block in the process
    LPVOID RemoteMemory = VirtualAllocEx(TargetProcessHandle, NULL, strlen(DLLPath) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (RemoteMemory == NULL)
    {
        std::cerr << "Couldn't reserve a memory block in the process." << std::endl;
        return 1;
    }

    // Write in the DLL Path to inject to the memory block.
    if (!WriteProcessMemory(TargetProcessHandle, RemoteMemory, DLLPath, strlen(DLLPath) + 1, NULL)) {
        std::cerr << "Couldn't write in the path of the Dynamic Link-Library in the memory block." << std::endl;
        return 1;
    }

    // Get the path of the function LoadLibrary

    FARPROC loadLibraryAddress = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");

    // Get sure that it fond the path

    if (!loadLibraryAddress)
    {
        std::cout << "Error: Couldn't get the path of LoadLibraryA." << std::endl;
        return 1;
    }

    // Get the Roblox Process ID

    DWORD RblxProcessId = 0;
    HANDLE Snapshot_ = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 Entry;
    Entry.dwSize = sizeof(PROCESSENTRY32);
    while (Process32Next(Snapshot_, &Entry) == TRUE)
    {
        if (wcsicmp(Entry.szExeFile, Targett) == NULL)
        {
            RblxProcessId = Entry.th32ProcessID;
            break;
        }
    }
    CloseHandle(Snapshot_);
    if (!RblxProcessId) {
        std::cout << "Couldn't find the Roblox process." << std::endl;
        return 1;
    }

    HANDLE RobloxProcessHandle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, RblxProcessId);
    if (!RobloxProcessHandle) {
        std::cout << "Couldn't open the Roblox process." << std::endl;
        return 1;
    }

    const char* dllPath = "injector.dll";
    LPVOID dllPathAdress = VirtualAllocEx(RobloxProcessHandle, NULL, strlen(dllPath) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    std::cout << "Couldn't assign memory in the Roblox Process." << std::endl;
    CloseHandle(RobloxProcessHandle);
    return 1;

    // Write DLL Filename in the assigned memory.
    if (!WriteProcessMemory (RobloxProcessHandle, dllPathAdress, dllPath, strlen (dllPath) + 1, NULL)) {
        std::cout << "Couldn't write the DLL Filename in the Roblox Process." << std::endl;
        VirtualFreeEx(RobloxProcessHandle, dllPathAdress, strlen(dllPath) + 1, MEM_RELEASE);
        CloseHandle(RobloxProcessHandle);
        return 1;
    }

    // Create a thread into the Roblox Process to load the Dynamic Link-Library file.
    HANDLE RemoteThread = CreateRemoteThread(RobloxProcessHandle, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddress, dllPathAdress, 0, NULL);
    if (!RemoteThread) {
        std::cout << "Couldn't create a threat into the Roblox Process." << std::endl;
        VirtualFreeEx(RobloxProcessHandle, dllPathAdress, strlen(dllPath) + 1, MEM_RELEASE);
        CloseHandle(RobloxProcessHandle);
        return 1;
    }

    // Wait for the thread to finish.
    WaitForSingleObject(RemoteThread, INFINITE);

    // Free the assigned memory in the Roblox Process.
    VirtualFreeEx(RobloxProcessHandle, dllPathAdress, strlen (dllPath) + 1, MEM_RELEASE);

    // Close the handles.
    CloseHandle(RemoteThread);
    CloseHandle(RobloxProcessHandle);

    printf("\x1B[32mLooks like everything is OK, LInjector has been injecte\033[0m\t\t");
}