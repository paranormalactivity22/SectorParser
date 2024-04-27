#ifndef __PARSER_H
#define __PARSER_H
#include<windows.h>

#define FILE_SHARE_VALID_FLAGS 0x00000007

DRIVE_LAYOUT_INFORMATION_EX*
getPartitionInfo(
	HANDLE hConsole,
	PWCHAR drive
);

BOOL
ReadPartition(
	HANDLE hConsole,
	PWCHAR drive,
	LPVOID buffer,
	UINT sectorSize
);

BOOL
WritePartition(
	HANDLE hConsole,
	PWCHAR drive,
	LPVOID buffer,
	UINT sectorSize,
	DWORD Flags
);

CONST LONG
_GetFileSize(
	HANDLE hConsole,
	LPWSTR Filename
);

#endif // __PARSER_H