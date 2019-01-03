#include "SDK.h"
#include <time.h>

char g_logFile[MAX_PATH];
char g_debugLogFile[MAX_PATH];

void Log::Init(HMODULE hModule) {
	memset(g_logFile, 0, sizeof(g_logFile));

	if (GetModuleFileNameA(hModule, g_logFile, MAX_PATH) != 0) {
		size_t slash = -1;

		for (size_t i = 0; i < strlen(g_logFile); i++) {
			if (g_logFile[i] == '/' || g_logFile[i] == '\\') {
				slash = i;
			}
		}

		if (slash != -1) {
			g_logFile[slash + 1] = '\0';
			strcpy_s(g_debugLogFile, g_logFile);
			strcat_s(g_debugLogFile, "debug.log");
			strcat_s(g_logFile, "hook.log");
		}
		else {
			// Shitty manual mapper detected.
			MessageBoxA(NULL, "Unable to parse target module path", "ERROR", MB_OK);
			//ExitProcess(0);
		}
	}
	else {
		// Shitty manual mapper detected.
		MessageBoxA(NULL, "GetModuleFileNameA failed", "ERROR", MB_OK);
		//ExitProcess(0);
	}
}

void Log::Debug(const char* fmt, ...) 
{
	va_list va_alist;
	char szLogbuf[4096];
	char szParameters[4066];
	char szTimestamp[30];
	struct tm current_tm;
	time_t current_time = time(NULL);
	FILE* file;

	localtime_s(&current_tm, &current_time);
	sprintf_s(szTimestamp, "[%02d:%02d:%02d]{DEBUG} %%s\n", current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec);

	va_start(va_alist, fmt);
	_vsnprintf_s(szParameters, sizeof(szParameters), fmt, va_alist);
	va_end(va_alist);
	sprintf_s(szLogbuf, szTimestamp, szParameters);
	if ((fopen_s(&file, g_debugLogFile, "a")) == 0)
	{
		fprintf_s(file, "%s", szLogbuf);
		fclose(file);
	}
}

void Log::Msg(const char* fmt, ...) {
	va_list va_alist;
	char szLogbuf[4096];
	char szParameters[4066];
	char szTimestamp[30];
	struct tm current_tm;
	time_t current_time = time(NULL);
	FILE* file;

	localtime_s(&current_tm, &current_time);
	sprintf_s(szTimestamp, "[%02d:%02d:%02d]{MSG} %%s\n", current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec);

	va_start(va_alist, fmt);
	_vsnprintf_s(szParameters, sizeof(szParameters), fmt, va_alist);
	va_end(va_alist);
	sprintf_s(szLogbuf, szTimestamp, szParameters);
	if ((fopen_s(&file, g_logFile, "a")) == 0)
	{
		fprintf_s(file, "%s", szLogbuf);
		fclose(file);
	}
}

void Log::Error(const char* fmt, ...) {
	va_list va_alist;
	char szLogbuf[4096];
	char szParameters[4066];
	char szTimestamp[30];
	struct tm current_tm;
	time_t current_time = time(NULL);
	FILE* file;

	localtime_s(&current_tm, &current_time);
	sprintf_s(szTimestamp, "[%02d:%02d:%02d]{ERROR} %%s\n", current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec);

	va_start(va_alist, fmt);
	_vsnprintf_s(szParameters, sizeof(szParameters), fmt, va_alist);
	va_end(va_alist);
	sprintf_s(szLogbuf, szTimestamp, szParameters);
	if ((fopen_s(&file, g_debugLogFile, "a")) == 0)
	{
		fprintf_s(file, "%s", szLogbuf);
		fclose(file);
	}

	MessageBoxA(NULL, szLogbuf, "ERROR", MB_ICONERROR);
}

void Log::Fatal(const char* fmt, ...) {
	va_list va_alist;
	char szLogbuf[4096];
	char szParameters[4066];
	char szTimestamp[30];
	struct tm current_tm;
	time_t current_time = time(NULL);
	FILE* file;

	localtime_s(&current_tm, &current_time);
	sprintf_s(szTimestamp, "[%02d:%02d:%02d]{FATAL} %%s\n", current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec);

	va_start(va_alist, fmt);
	_vsnprintf_s(szParameters, sizeof(szParameters), fmt, va_alist);
	va_end(va_alist);
	sprintf_s(szLogbuf, szTimestamp, szParameters);
	if ((fopen_s(&file, g_debugLogFile, "a")) == 0)
	{
		fprintf_s(file, "%s", szLogbuf);
		fclose(file);
	}

	MessageBoxA(NULL, szLogbuf, "FATAL ERROR", MB_ICONERROR);

	ExitProcess(0);
}