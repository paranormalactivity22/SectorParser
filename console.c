#include "console.h"

WCHAR* 
GetConsoleExeName(
    WCHAR* path
)
{
    BOOL flag = 0;
    for (size_t i = wcslen(path) - 1; i > 0; i--)
    {
        if (path[i] == '\\' || path[i] == '//' || path[i] == '/')
        {
            flag = 1;
            path = path + i + 1;
            break;
        }
    }
    return path;
}

BOOL 
runnedAsAdmin(
) 
{
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) fRet = Elevation.TokenIsElevated;
    }
    if (hToken) CloseHandle(hToken);
    return fRet;
}

VOID 
SendConsoleError(
    HANDLE hConsole, 
    CONST PCHAR msg
)
{
    SET_CONSOLE_ATTRIBUTE(hConsole, ERROR_OPERATION_ATTRIBUTE);
    printf("%s", msg);
}

VOID 
SendConsoleLastError(
    HANDLE hConsole, 
    CONST PCHAR msg
)
{
    SET_CONSOLE_ATTRIBUTE(hConsole, ERROR_OPERATION_ATTRIBUTE);
    printf("%s (Last Error --> 0x%X)\n", msg, GetLastError());
}

VOID 
SendConsoleOK(
    HANDLE hConsole, 
    CONST PCHAR msg
)
{
    SET_CONSOLE_ATTRIBUTE(hConsole, OK_OPERATION_ATTRIBUTE);
    printf("%s", msg);
}

VOID 
SendConsoleHelp(
    HANDLE hConsole, 
    WCHAR* path
)
{
    WCHAR* getExecutableName = GetConsoleExeName(path);
    printf("%ws - Simple disk parser made by P.A. Valid options are extract, info and inject.\nUsage: %ws [--i disk] [--ex disk (size)] [--inject fileName (disk) (size)].\n", getExecutableName, getExecutableName);
    if(!runnedAsAdmin())
    {
        SendConsoleError(hConsole, "[!] The program won't work because you are not using admin rights.\n");
        SET_CONSOLE_ATTRIBUTE(hConsole, DEFAULT_CONSOLE_ATTRIBUTE);
        exit(0);
    }
}
