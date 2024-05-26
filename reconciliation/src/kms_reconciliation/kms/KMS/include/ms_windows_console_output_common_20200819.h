#ifndef CONSOLE_OUTPUT_COMMON_H
#define CONSOLE_OUTPUT_COMMON_H

#include <string>
#include <iostream>
#include <stdio.h>

#ifdef _WIN32
#define NOMINMAX // max/min has been made a macro inside windows.h. define NOMINMAX prior to including to stop windows.h from doing that.
#define _WINSOCKAPI_
#include <windows.h>
#endif

//#ifdef __linux__
//typedef unsigned long DWORD;
//typedef void * HANDLE;
//#endif

namespace ConsoleOutputCommon {
	void clrLine();
	void moveCursorTopLeft();
	void replaceLine(std::string s);
	void clrScreen();
	void prepareWindowsConsole();
	void clrRemainingScreen();
}

#endif // !CONSOLE_OUTPUT_COMMON_H
