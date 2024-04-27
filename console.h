#ifndef __CONSOLE_H
#define __CONSOLE_H
#include<windows.h>
#include<stdio.h>

#define SET_CONSOLE_ATTRIBUTE(console, attrib) SetConsoleTextAttribute(console, attrib)

/*
Color Attributes
*/
#define DEFAULT_CONSOLE_ATTRIBUTE 0x00007
#define OK_OPERATION_ATTRIBUTE 0x00002
#define NOTE_OPERATION_ATTRIBUTE 0x00005
#define ERROR_OPERATION_ATTRIBUTE 12
#define INFO_OPERATION_ATTRIBUTE 15

VOID 
SendConsoleError(
	HANDLE hConsole, 
	CONST PCHAR msg
);

VOID 
SendConsoleLastError(
	HANDLE hConsole, 
	CONST PCHAR msg
);

VOID 
SendConsoleOK(
	HANDLE hConsole, 
	CONST PCHAR msg
);

VOID 
SendConsoleHelp(
	HANDLE hConsole,
	WCHAR* path
);

/*
Utilities
*/
WCHAR* 
GetConsoleExeName(WCHAR* path);

BOOL runnedAsAdmin();

#endif // __CONSOLE_H