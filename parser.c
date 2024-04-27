#include "parser.h"
#include "console.h"

DRIVE_LAYOUT_INFORMATION_EX*
getPartitionInfo(
    HANDLE hConsole,
	PWCHAR drive
)
{
    HANDLE hDisk = CreateFile(drive, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDisk == INVALID_HANDLE_VALUE)
    {
        SendConsoleError(hConsole, "[-] Unable to open the drive for reading.\n");
        return NULL;
    }
    UCHAR ucBuffer[1024] = {0};
    BOOL bDevice = DeviceIoControl(hDisk, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, 0, 0, ucBuffer, sizeof(ucBuffer), 0, 0);
    if (!bDevice) return NULL;
    CloseHandle(hDisk);
    return (DRIVE_LAYOUT_INFORMATION_EX*)ucBuffer;
}

BOOL
ReadPartition(
    HANDLE hConsole,
    PWCHAR drive,
    LPVOID buffer,
    UINT sectorSize
)
{
    HANDLE hDisk = CreateFile(drive, GENERIC_READ, FILE_SHARE_VALID_FLAGS, 0, OPEN_EXISTING, 0, 0);
    DWORD dwRead;
    if (hDisk == INVALID_HANDLE_VALUE) {
        SendConsoleLastError(hConsole, "[-] Failed to open drive/file for reading.\n");
        CloseHandle(hDisk);
        return 0;
    }
    SetFilePointer(hDisk, sectorSize / 512, 0, FILE_BEGIN);
    BOOL result = ReadFile(hDisk, buffer, sectorSize, &dwRead, 0);
    if (result) SendConsoleOK(hConsole, "[+] Successfull read the content from the drive/file.\n");
    else SendConsoleLastError(hConsole, "[-] Failed read the content from the drive/file.");
    CloseHandle(hDisk);
    return result;
}

BOOL
WritePartition(
    HANDLE hConsole,
    PWCHAR drive,
    LPVOID buffer,
    UINT sectorSize,
    DWORD Flags
)
{
    HANDLE hDisk = CreateFile(drive, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, Flags, 0, 0);
    DWORD dwRead;
    if (hDisk == INVALID_HANDLE_VALUE) {
        SendConsoleLastError(hConsole, "[-] Failed to open drive/file for writing.\n");
        CloseHandle(hDisk);
        return 0;
    }
    SetFilePointer(hDisk, 0, 0, FILE_BEGIN);
    BOOL result = WriteFile(hDisk, buffer, sectorSize, &dwRead, 0);
    CloseHandle(hDisk);
    return result;
}

CONST LONG 
_GetFileSize(
    HANDLE hConsole,
    LPWSTR Filename
)
{
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesEx(Filename, GetFileExInfoStandard, &fad))
    {
        SendConsoleLastError(hConsole, "[-] Unable to get file size.");
    }
    LARGE_INTEGER size;
    size.HighPart = fad.nFileSizeHigh;
    size.LowPart = fad.nFileSizeLow;
    return size.QuadPart;
}