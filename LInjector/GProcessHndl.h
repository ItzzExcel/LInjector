#include <Windows.h>
#include <TlHelp32.h>

HANDLE GetProcessHandle(LPCWSTR processName)
{
    HANDLE processHandle = NULL;

    // Obtener un snapshot de todos los procesos en ejecución en el sistema
    HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshotHandle != INVALID_HANDLE_VALUE)
    {
        // Definir una estructura para almacenar la información de cada proceso
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        // Iterar sobre los procesos hasta encontrar el proceso con el nombre especificado
        if (Process32First(snapshotHandle, &processEntry))
        {
            do
            {
                if (wcscmp(processEntry.szExeFile, processName) == 0)
                {
                    // Si el nombre del proceso coincide, obtener un HANDLE al proceso
                    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
                    break;
                }
            } while (Process32Next(snapshotHandle, &processEntry));
        }

        // Cerrar el snapshot de procesos
        CloseHandle(snapshotHandle);
    }

    return processHandle;
}