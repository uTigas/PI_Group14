#include "ms_windows_console_output_common_20200819.h"

void ConsoleOutputCommon::clrLine() {
	printf("\x1B[2K");
}

void ConsoleOutputCommon::moveCursorTopLeft() {
	printf("\x1B[1;1H");
}

void ConsoleOutputCommon::replaceLine(std::string s) {
	printf("\x1B[2K");
	std::cout << s;
}

void ConsoleOutputCommon::clrRemainingScreen() {
	printf("\x1B[J");
}

void ConsoleOutputCommon::clrScreen() {
	printf("\033[2J");
}

void ConsoleOutputCommon::prepareWindowsConsole() {

	#ifdef _WIN32
	// Set output mode to handle virtual terminal sequences
	DWORD error;
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		error = GetLastError();
	}

	CONSOLE_CURSOR_INFO     cursorInfo;
	GetConsoleCursorInfo(hOut, &cursorInfo);
	cursorInfo.bVisible = false;				// hide the cursor
	SetConsoleCursorInfo(hOut, &cursorInfo);

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		error = GetLastError();
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
	{
		error = GetLastError();
	}
	#endif

	#ifdef __linux__
	bool showCursor {false}; // false = hide the cursor 
	std::cout << (showCursor ? "\033[?25h" : "\033[?25l");
	#endif

	ConsoleOutputCommon::clrScreen();
}
