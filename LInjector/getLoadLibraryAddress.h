#include <windows.h>

// Returns the address of the LoadLibrary function
LPVOID getLoadLibraryAddress()
{
    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    return GetProcAddress(kernel32, "LoadLibraryA");
}
