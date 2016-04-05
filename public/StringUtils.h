#pragma once

#ifndef __XXX_STRINGUTILS_H__
#define __XXX_STRINGUTILS_H__

#include <string>
#include <vector>
#include <windows.h>

class StringUtils
{
public:
	static std::string Utf8ToAnsi(std::string& str);
	static std::string AnsiToUtf8(std::string& str);
	static std::string WcharToAnsi(const std::wstring& str);//WARN: some unicode data will lost
	static std::wstring AnsiToWchar(const std::string& str);
	static std::string WcharToUtf8(const std::wstring& str);
	static std::wstring Utf8ToWchar(const std::string& str);

	static std::string ToUpperCase(std::string &str);
	static std::string ToLowerCase(std::string &str);
	static std::string SubAnsiString(const std::string& str, size_t start, size_t len);
	static std::string ReplaceChar(std::string& str, char ch, const std::string& strReplace);
	static std::string ReplaceStr(std::string& str, const std::string& strFind, const std::string& strReplace);
	static void SplitStringByChar(const std::string& str, std::vector<std::string>& arrStr, char ch = ' ');
	static std::string ExtractStr(const std::string& str, const std::string& sBegin, const std::string& sEnd);
	static std::string GetGuid(bool bracket = false);

	static std::string FileSizeToString(DWORD64 filesize, int level = 0);
	static std::string ExtractFileName(const std::string& filepath);
	static std::string ExtractFileExt(const std::string& filepath);
	static std::string MakeSureFileNameValid(const std::string& fname);

	static void SplitStr(const char *buf, char ch, std::vector<std::string> &arOut, long size = -1);
	static void SplitStr(const char *buf, char ch, std::string &sDest, long nNO = 0);
};


#endif // __XXX_STRINGUTILS_H__

// Something reference from cowork