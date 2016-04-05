// stdafx.cpp : source file that includes just the standard includes
//	CifExplorer.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)

void c2w(wchar_t *pwstr, size_t len, const char *str)
{
	if (str)
	{
		size_t nu = strlen(str);
		size_t n = (size_t)MultiByteToWideChar(CP_ACP, 0, (const char *)str, (int)nu, NULL, 0);
		if (n >= len)n = len - 1;
		MultiByteToWideChar(CP_ACP, 0, (const char *)str, (int)nu, pwstr, (int)n);
		pwstr[n] = 0;
	}
}

char* w2c(const wchar_t* wp)
{
	char *m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}

bool readINI(char* option, char* key, char* value, char* path)
{
	WCHAR wcOption[MAX_PATH] = { 0 }, wcKey[MAX_PATH] = { 0 }, wcValue[MAX_PATH] = { 0 }, wcPath[MAX_PATH] = { 0 };
	c2w(wcOption, MAX_PATH, option);
	c2w(wcKey, MAX_PATH, key);
	c2w(wcPath, MAX_PATH, path);
	bool rs = GetPrivateProfileString(wcOption, wcKey, wcValue, wcValue, MAX_PATH, wcPath);
	strcpy(value, w2c(wcValue));
	return rs;
}

bool readINI(char* option, char* key, int& value, char* path)
{
	WCHAR wcOption[MAX_PATH] = { 0 }, wcKey[MAX_PATH] = { 0 }, wcPath[MAX_PATH] = { 0 };
	c2w(wcOption, MAX_PATH, option);
	c2w(wcKey, MAX_PATH, key);
	c2w(wcPath, MAX_PATH, path);
	value = GetPrivateProfileInt(wcOption, wcKey, 0, wcPath);
	return true;
}

bool readINI(char* option, char* key, char* value)
{
	WCHAR czPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, czPath);
	wcscat(czPath, L"\\config.ini");

	WCHAR wcOption[MAX_PATH] = { 0 }, wcKey[MAX_PATH] = { 0 }, wcValue[MAX_PATH] = { 0 };
	c2w(wcOption, MAX_PATH, option);
	c2w(wcKey, MAX_PATH, key);
	bool rs = GetPrivateProfileString(wcOption, wcKey, wcValue, wcValue, MAX_PATH, czPath);
	strcpy(value, w2c(wcValue));
	return rs;
}

bool readINI(char* option, char* key, int& value)
{
	WCHAR czPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, czPath);
	wcscat(czPath, L"\\config.ini");

	WCHAR wcOption[MAX_PATH] = { 0 }, wcKey[MAX_PATH] = { 0 };
	c2w(wcOption, MAX_PATH, option);
	c2w(wcKey, MAX_PATH, key);
	value = GetPrivateProfileInt(wcOption, wcKey, 0, czPath);
	return true;
}

bool writeINI(char* option, char* key, char* value)
{
	WCHAR czPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, czPath);
	wcscat(czPath, L"\\config.ini");

	WCHAR wcOption[MAX_PATH] = { 0 }, wcKey[MAX_PATH] = { 0 }, wcValue[MAX_PATH] = { 0 };
	c2w(wcOption, MAX_PATH, option);
	c2w(wcKey, MAX_PATH, key);
	c2w(wcValue, MAX_PATH, value);
	bool rs = ::WritePrivateProfileString(wcOption, wcKey, wcValue, czPath);
	return rs;
}

bool writeINI(char* option, char* key, int value)
{
	WCHAR czPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, czPath);
	wcscat(czPath, L"\\config.ini");

	WCHAR wcOption[MAX_PATH] = { 0 }, wcKey[MAX_PATH] = { 0 }, wcValue[MAX_PATH] = { 0 };
	c2w(wcOption, MAX_PATH, option);
	c2w(wcKey, MAX_PATH, key);
	swprintf(wcValue, L"%d", value);
	bool rs = ::WritePrivateProfileString(wcOption, wcKey, wcValue, czPath);
	return rs;
}

std::string GetLogPath()
{
	char cPersonalFolderPath[MAX_PATH];
	SHGetSpecialFolderPathA(NULL, cPersonalFolderPath, CSIDL_PERSONAL, FALSE);
	string sdir = string(cPersonalFolderPath) + "\\CifExplorer\\CifExplorerLog";
	MakeSureDirectoryPathExists(sdir.c_str());
	return sdir;
}
LogFileEx* g_pLogAll = new LogFileEx(GetLogPath().c_str(), LogFileEx::DAY);

void WriteLog(const std::string& str)
{
	g_pLogAll->Log(str.c_str());
}

void WriteLog(const std::string& str, int value)
{
	char szlog[MAX_PATH] = { 0 };
	sprintf(szlog, str.c_str(), value);
	g_pLogAll->Log(szlog);
}

void WriteLog(const std::string& str, std::string value)
{
	char szlog[MAX_PATH] = { 0 };
	sprintf(szlog, str.c_str(), value.c_str());
	g_pLogAll->Log(szlog);
}