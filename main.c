#include "console.h"
#include "parser.h"

#define WCHAR_EXIST(x, y) wcscmp(x, y) == 0

BOOL 
isWideNumber(
    PWCHAR str
)
{
    PWCHAR ptr = str;
    while (*ptr != L'\0') {
        if(!iswdigit(*ptr)) {
            return 0;
        }
        ++ptr;
    }
    return 1;
}

typedef struct _options
{
    BOOL infoOption;
    BOOL extractOption;
    BOOL injectOption;
} MenuOptions;

INT 
main(
    HINSTANCE hInstance, 
    HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, 
    INT nCmdShow
)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    INT argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    PWCHAR diskName = {0};
    PWCHAR fileName = { 0 };
    MenuOptions MenuOptions = { FALSE, FALSE, FALSE };
    UINT sectorSize = 512;
    SendConsoleHelp(hConsole, argv[0]);
    for (INT i = 1; i < argc; ++i)
    {
        if(WCHAR_EXIST(argv[i], L"--info") || WCHAR_EXIST(argv[i], L"--i"))
        {
            MenuOptions.infoOption = TRUE;
            if (i + 1 == argc) diskName = L"\\\\.\\PhysicalDrive0";
            else if (wcsncmp(argv[i + 1], L"-", 1)) diskName = argv[i + 1];
            else
            {
                SendConsoleError(hConsole, "[-] Disk name is invalid.\n");
                goto EXIT;
            }
        }
        else if (WCHAR_EXIST(argv[i], L"--extract") || WCHAR_EXIST(argv[i], L"--ex"))
        {
            MenuOptions.extractOption = TRUE;
            if (i + 1 == argc) diskName = L"\\\\.\\PhysicalDrive0";
            else if (wcsncmp(argv[i + 1], L"-", 1))
            {
                diskName = argv[i + 1];
                if (i + 2 != argc)
                {
                    if (isWideNumber(argv[i + 2])) sectorSize = _wtoi(argv[i + 2]);
                }
            }
            else
            {
                SendConsoleError(hConsole, "[-] Disk name or size is invalid.\n");
                goto EXIT;
            }
        }
        else if (WCHAR_EXIST(argv[i], L"--inject"))
        {
			MenuOptions.injectOption = TRUE;
            if (i + 1 == argc)
            {
                SendConsoleError(hConsole, "[-] The file name can't be empty.\n");
                goto EXIT;
            }
            else if (wcsncmp(argv[i + 1], L"-", 1))
            {
                fileName = argv[i + 1];
                if (i + 2 == argc)
                {
                    diskName = L"\\\\.\\PhysicalDrive0";
                    sectorSize = _GetFileSize(hConsole, fileName);
                }
                else if (wcsncmp(argv[i + 2], L"-", 1))
                {
                    diskName = argv[i + 2];
                    if (i + 3 == argc) sectorSize = _GetFileSize(hConsole, fileName);
                    else if (isWideNumber(argv[i + 3]))
                    {
                        if(GetFileAttributes(fileName) == INVALID_FILE_ATTRIBUTES)
                        {
                            SendConsoleLastError(hConsole, "[-] Unable to get file size.");
                            goto EXIT;
                        }
                        sectorSize = _wtoi(argv[i + 3]);
                    }
                }
            }
        }
    }
    if (MenuOptions.infoOption)
    {
        if (diskName != NULL)
        {
            DRIVE_LAYOUT_INFORMATION_EX* info;
            info = getPartitionInfo(hConsole, diskName);
            if (info)
            {
                DWORD style = info->PartitionStyle;
                printf("[+] Partition Type --> %s\n", style == 0 ? "MBR (Bios)" : "GPT (EFI)");
                if (style == PARTITION_STYLE_MBR)
                {
                    printf("[+] MBR Signature --> %lu\n", info->Mbr.Signature);
                    printf("[+] MBR Checksum --> %lu\n", info->Mbr.CheckSum);
                }
                else if (style == PARTITION_STYLE_GPT)
                {
                    printf("[+] EFI Disk ID (Data1) --> %lu\n", info->Gpt.DiskId.Data1);
                    printf("[+] EFI Disk ID (Data2) --> %lu\n", info->Gpt.DiskId.Data2);
                    printf("[+] EFI Disk ID (Data3) --> %lu\n", info->Gpt.DiskId.Data3);
                    printf("[+] EFI Disk ID (Data4) --> %p\n", info->Gpt.DiskId.Data4);
                    printf("[+] EFI Partition Count --> %d sectors\n", info->Gpt.MaxPartitionCount);
                    printf("[+] EFI Usable Length --> %lld (High: %d | Low: %ld)\n", info->Gpt.UsableLength.QuadPart, info->Gpt.UsableLength.HighPart, info->Gpt.UsableLength.LowPart);
                    printf("[+] EFI Starting Usable Offset --> %lld (High: %d | Low: %ld)\n", info->Gpt.StartingUsableOffset.QuadPart, info->Gpt.StartingUsableOffset.HighPart, info->Gpt.StartingUsableOffset.LowPart);
                }
            }
            else
            {
                SendConsoleError(hConsole, "[-] Unable to send information about given drive.\n");
            }
            goto EXIT;
        }
    }

    else if (MenuOptions.extractOption)
    {
        if (sectorSize % 512 != 0)
        {
            SendConsoleError(hConsole, "[-] Sector size is invalid.\n");
            goto EXIT;
        }
        LPVOID buffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sectorSize * sizeof(buffer));
        if (ReadPartition(hConsole, diskName, buffer, sectorSize))
        {
            WritePartition(hConsole, L"extracted.bin", buffer, sectorSize, CREATE_ALWAYS);
            SendConsoleOK(hConsole, "[+] Successfull extracted the content from the drive.\n");
        }
        else
        {
            SendConsoleError(hConsole, "[-] Unable to extract the given content from the drive.\n");
        }
        HeapFree(GetProcessHeap(), 0, buffer);
        goto EXIT;
    }

    else if (MenuOptions.injectOption)
    {
        LPVOID buffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sectorSize * sizeof(buffer));
        if (ReadPartition(hConsole, fileName, buffer, sectorSize))
        {
            if (sectorSize % 512 != 0)
            {
                UINT sectorSize2 = (512 * (INT)((sectorSize / 512) + 1));
                LPVOID buffer2 = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sectorSize2 * sizeof(buffer));
                if (ReadPartition(hConsole, diskName, buffer2, sectorSize2))
                {
                    memcpy(buffer2, buffer, sectorSize);
                    WritePartition(hConsole, diskName, buffer2, sectorSize2, OPEN_EXISTING);
                }
                HeapFree(GetProcessHeap(), 0, buffer2);
                SendConsoleOK(hConsole, "[+] Successfull wrote data in the drive.\n");
            }
            else
            {
                WritePartition(hConsole, diskName, buffer, sectorSize, OPEN_EXISTING);
                SendConsoleOK(hConsole, "[+] Successfull wrote data in the drive.\n");
            }
        }
        else
        {
            SendConsoleError(hConsole, "[-] Unable to write the given content in the drive.\n");
        }
        HeapFree(GetProcessHeap(), 0, buffer);
        goto EXIT;
    }
EXIT:
    SET_CONSOLE_ATTRIBUTE(hConsole, DEFAULT_CONSOLE_ATTRIBUTE);
    CloseHandle(hConsole);
    return EXIT_SUCCESS;
}