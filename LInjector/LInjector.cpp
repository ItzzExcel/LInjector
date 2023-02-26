#include <iostream>
#include <Windows.h>
#include <string>
#include <memoryapi.h>
#include <cstring>
#include <string.h>
#include <stdlib.h>
#include <processthreadsapi.h>
#include <Psapi.h>
#include <sal.h>
#include <TlHelp32.h>
#include <fstream>
#include <cstdio>
#include <libloaderapi.h>
#include <filesystem>
#include <vector>
#include "resource.h"
#pragma comment (lib, "kernel32.lib")
#define GetProcessHandle

namespace fs = std::filesystem;

int main(int argc, char const* argv[]) {
    MessageBox(NULL, L"This application was made by @ItzzzExcel as an open-source project under the MIT License.", L"LInjector | Welcome", NULL);

    const char* DLLPath = "libs/injector.dll";

    int wstrLength = MultiByteToWideChar(CP_ACP, 0, DLLPath, -1, NULL, 0);
    wchar_t* WDLLPath = new wchar_t[wstrLength];
    MultiByteToWideChar(CP_ACP, 0, DLLPath, -1, WDLLPath, wstrLength);
    HMODULE HModule = LoadLibrary(WDLLPath);
    if (HModule == NULL)
    {
        std::cerr << "An unexpected error has ocurred while loading the injector.\nMust check /libs folder includes the 'injector.dll' file.\n";
        system("pause > nul");
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
            if (_wcsicmp(entry.szExeFile, Targett) == NULL) {
                TargetProcessID = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);

    if (TargetProcessID == 0)
    {
        std::cerr << "Couldn't find the Roblox process : " << Targett << std::endl;
        system("pause");
        return 1;
        
    }

    // Getting a Handle 🗿🗿🗿

    HANDLE TargetProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, TargetProcessID);
    if (TargetProcessHandle == NULL)
    {
        std::cerr << "Couldn't get a handle for the Roblox process." << std::endl;
        system("pause");
        return 1;
        
    }

    // Get base module path

    HMODULE TargetModuleBase = NULL;
    DWORD cbNeeded = 0;
    if (!EnumProcessModules(TargetProcessHandle, &TargetModuleBase, sizeof(TargetModuleBase), &cbNeeded))
    {
        std::cerr << "Couldn't get the base direction of the module of the process." << std::endl;
        system("pause");
        return 1;
        
    }

    // Reserve memory block in the process
    LPVOID RemoteMemory = VirtualAllocEx(TargetProcessHandle, NULL, strlen(DLLPath) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (RemoteMemory == NULL)
    {
        std::cerr << "Couldn't reserve a memory block in the process." << std::endl;
        system("pause");
        return 1;
        
    }

    // Write in the DLL Path to inject to the memory block.
    if (!WriteProcessMemory(TargetProcessHandle, RemoteMemory, DLLPath, strlen(DLLPath) + 1, NULL)) {
        std::cerr << "Couldn't write in the path of the Dynamic Link-Library in the memory block." << std::endl;
        return 1;
        system("pause");
    }

    // Get the path of the function LoadLibrary

    FARPROC loadLibraryAddress = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");

    // Get sure that it fond the path

    if (!loadLibraryAddress)
    {
        std::cout << "Error: Couldn't get the path of LoadLibraryA." << std::endl;
        system("pause");
        return 1;
       
    }

    // Get the Roblox Process ID

    DWORD RblxProcessId = 0;
    HANDLE Snapshot_ = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 Entry;
    Entry.dwSize = sizeof(PROCESSENTRY32);
    while (Process32Next(Snapshot_, &Entry) == TRUE)
    {
        if (_wcsicmp(Entry.szExeFile, Targett) == NULL)
        {
            RblxProcessId = Entry.th32ProcessID;
            break;
        }
    }
    CloseHandle(Snapshot_);
    if (!RblxProcessId) {
        std::cout << "Couldn't find the Roblox process." << std::endl;
        system("pause");
        return 1;
        

    }

    HANDLE RobloxProcessHandle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, RblxProcessId);
    if (!RobloxProcessHandle) {
        std::cout << "Couldn't open the Roblox process." << std::endl;
        system("pause");
        return 1;
        
    }

    const char* dllPath = "libs/injector.dll";
    LPVOID dllPathAdress = VirtualAllocEx(RobloxProcessHandle, NULL, strlen(dllPath) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!dllPathAdress)
    {
        std::cout << "Couldn't assign memory in the Roblox Process." << std::endl;
        CloseHandle(RobloxProcessHandle);
        system("pause");
        return 1;
    }


    // Write DLL Filename in the assigned memory.
    if (!WriteProcessMemory(RobloxProcessHandle, dllPathAdress, dllPath, strlen(dllPath) + 1, NULL)) {
        std::cout << "Couldn't write the DLL Filename in the Roblox Process." << std::endl;
        VirtualFreeEx(RobloxProcessHandle, dllPathAdress, strlen(dllPath) + 1, MEM_RELEASE);
        CloseHandle(RobloxProcessHandle);
        system("pause");
        return 1;
        
    }

    // Create a thread into the Roblox Process to load the Dynamic Link-Library file.
    HANDLE RemoteThread = CreateRemoteThread(RobloxProcessHandle, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddress, dllPathAdress, 0, NULL);
    if (!RemoteThread) {
        std::cout << "Couldn't create a threat into the Roblox Process." << std::endl;
        VirtualFreeEx(RobloxProcessHandle, dllPathAdress, strlen(dllPath) + 1, MEM_RELEASE);
        CloseHandle(RobloxProcessHandle);
        system("pause");
        return 1;
        
    }

    // Wait for the thread to finish.
    WaitForSingleObject(RemoteThread, INFINITE);

    // Free the assigned memory in the Roblox Process.
    VirtualFreeEx(RobloxProcessHandle, dllPathAdress, strlen(dllPath) + 1, MEM_RELEASE);

    // Close the handles.
    CloseHandle(RemoteThread);
    CloseHandle(RobloxProcessHandle);

    printf("\x1B[32mLooks like everything is OK,\nLInjector has been injected\033[0m\t\t");
    std::cout << std::endl << std::endl;

    std::string LocationScripts;
    std::vector<std::string> ScriptFilesoepe;

    std::cout << std::endl << std::endl << "Enter the directory of the scripts." << std::endl <<
        "Example: C:/Desktop/Scripts (MUST USE SLASH (NOT INVERTED ONE))" << std::endl << std::endl << " < ";
    std::cin >> LocationScripts;
    std::cout << std::endl << std::endl << "Intializing..." << std::endl << std::endl;
    int i = 1;
    for (const auto& Entry : fs::directory_iterator (LocationScripts)) {
        if (Entry.is_regular_file ()) {
            std::cout << i << ". | " << Entry.path().filename().string() << std::endl;
            ScriptFilesoepe.push_back(Entry.path().string());
            ++ i;
        }
    }

    // Get user selection

    int Selectn;
    std::cout << std::endl << std::endl << "Enter the number of the script to execute." << std::endl << std::endl << " < ";
    std::cin >> Selectn;
    if (Selectn > 0 && Selectn <= ScriptFilesoepe.size()) {
        wchar_t* dllPathWide = new wchar_t[strlen(dllPath) + 1];
        // Copy the narrow string to the wide string
        size_t convertedChars = 0;
        HMODULE hModule = LoadLibraryA("libs/injector.dll");
        if (hModule == NULL) {
            std::cout << "Failed to load library." << std::endl;
            system("pause");
            return 1;
        }

        /*
        FARPROC Injectt = GetProcAddress(hModule, "injector.dll");
        std::string selectedScriptPath = std::filesystem::absolute(ScriptFilesoepe[Selectn - 1]).string();
        HANDLE RobloxProcessHandlee = GetProcessHandle(L"RobloxPlayerBeta.exe");
        Injectt(TargetRBLX, selectedScriptPath.c_str());
        CloseHandle(RobloxProcessHandle);

        FreeLibrary(hModule);
        */
    }
    else {
        std::cout << "Invalid selection." << std::endl;
        system("pause");
        return 1;
    }

    system("pause > nul"); 
    return 0;
}