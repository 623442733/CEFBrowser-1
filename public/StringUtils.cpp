
#include "StringUtils.h"

#include <windows.h>
#include <atlcomcli.h>

std::string StringUtils::Utf8ToAnsi(std::string& str)
{
	int nwBytes = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
	wchar_t *pWBytes = new wchar_t[nwBytes];

	::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pWBytes, nwBytes);

	int nBytes = ::WideCharToMultiByte(CP_ACP, 0, pWBytes, nwBytes, NULL, 0, NULL, NULL);
	char* pBytes = new char[nBytes];

	::WideCharToMultiByte(CP_ACP, 0, pWBytes, nwBytes, pBytes, nBytes, NULL, NULL);

	str = "";
	str.append(pBytes, nBytes);

	delete[] pWBytes;
	delete[] pBytes;

	return str;
}

std::string StringUtils::AnsiToUtf8(std::string& str)
{
	int nwBytes = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0);
	wchar_t *pWBytes = new wchar_t[nwBytes];

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pWBytes, nwBytes);

	int nBytes = ::WideCharToMultiByte(CP_UTF8, 0, pWBytes, nwBytes, NULL, 0, NULL, NULL);
	char* pBytes = new char[nBytes];

	::WideCharToMultiByte(CP_UTF8, 0, pWBytes, nwBytes, pBytes, nBytes, NULL, NULL);

	str = "";
	str.append(pBytes, nBytes);

	delete[] pWBytes;
	delete[] pBytes;

	return str;
}

//一些unicode字符会丢失成???
std::string StringUtils::WcharToAnsi(const std::wstring& str)
{
	int nBytes = ::WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.length(), NULL, 0, NULL, NULL);
	char* pBytes = new char[nBytes];

	::WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.length(), pBytes, nBytes, NULL, NULL);

	std::string rs;
	rs.append(pBytes, nBytes);

	delete[] pBytes;

	return rs;
}

std::wstring StringUtils::AnsiToWchar(const std::string& str)
{
	int nwBytes = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0);
	wchar_t *pWBytes = new wchar_t[nwBytes];

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pWBytes, nwBytes);

	std::wstring rs;
	rs.append(pWBytes, nwBytes);

	delete[] pWBytes;

	return rs;
}

std::string StringUtils::WcharToUtf8(const std::wstring& str)
{
	int nBytes = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0, NULL, NULL);
	char* pBytes = new char[nBytes];

	::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), pBytes, nBytes, NULL, NULL);

	std::string rs;
	rs.append(pBytes, nBytes);

	delete[] pBytes;

	return rs;
}

std::wstring StringUtils::Utf8ToWchar(const std::string& str)
{
	int nwBytes = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
	wchar_t *pWBytes = new wchar_t[nwBytes];

	::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pWBytes, nwBytes);

	std::wstring rs;
	rs.append(pWBytes, nwBytes);

	delete[] pWBytes;

	return rs;
}

std::string StringUtils::ToUpperCase(std::string &str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] >= 'a' && str[i] <= 'z')
		{
			str[i] += ('A' - 'a');
		}
	}

	return str;
}

std::string StringUtils::ToLowerCase(std::string &str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
		{
			str[i] += ('a' - 'A');
		}
	}

	return str;
}

std::string StringUtils::ReplaceChar(std::string& str, char ch, const std::string& strReplace)
{
	std::string strNew = "";

	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == ch)
		{
			strNew += strReplace;
		}
		else
		{
			strNew += str[i];
		}
	}

	str = strNew;
	return str;
}

std::string StringUtils::ReplaceStr(std::string& str, const std::string& strFind, const std::string& strReplace)
{
	int p = str.find(strFind);
	while (p != std::string::npos)
	{
		str.replace(str.begin() + p, str.begin() + p+ strFind.length(), strReplace);
		p = str.find(strFind);
	}
	return str;
}

void StringUtils::SplitStringByChar(const std::string& str, std::vector<std::string>& arrStr, char ch /*= ' '*/)
{
	const char *pStr = str.c_str();

	char* cTmp = new char[str.length() + 1];
	memset(cTmp, 0, str.length() + 1);

	int j = 0;

	for (size_t i = 0; i < str.length(); i++)
	{
		if (pStr[i] != ch)
		{
			cTmp[j++] = pStr[i];
		}
		else
		{
			if (j > 0)
			{
				cTmp[j] = 0x00;
				arrStr.push_back(std::string(cTmp));
				j = 0;
			}
		}
	}

	if (j != 0)
	{
		cTmp[j] = 0x00;
		arrStr.push_back(std::string(cTmp));
	}

	delete[] cTmp;
}
// 截取包含汉字的字符串(utf8不支持),若发生截断的现象,往左取半个,最终可能的长度比len少1
std::string StringUtils::SubAnsiString(const std::string& str, size_t start, size_t len)
{
	if (len <= 0 || start >= str.length())
		return std::string();

	size_t end = start + len;
	if (end >= str.length())
	{
		end = str.length() - 1;
	}

	size_t validpos = 0;
	int nStart = -1;
	int nEnd = -1;

	for (size_t i = 0; i < str.length(); i++)
	{
		validpos = i;

		//如果当前位置是汉字等2个字节字符的开始,那么主动加1防止截断
		if (str[i] & 0x80)
		{
			i++;
		}

		//如果达到当前的开始位置,那么返回到未截断的开始位置,往前加半个字符
		if (nStart == -1 && i >= start)
		{
			nStart = validpos;
			end = nStart + len;
		}

		if (nEnd == -1 && i >= end)
		{
			nEnd = validpos;
			break;
		}
	}

	return str.substr(nStart, nEnd - nStart);
}

std::string StringUtils::FileSizeToString(DWORD64 filesize, int level)
{
	char cFileSize[256];
	switch (level)
	{
	case 1:
	{
		sprintf_s(cFileSize, 255, "%d B", filesize);
		break;
	}
	case 2:
	{
		sprintf_s(cFileSize, 255, "%.2lf KB", (double)filesize / 1024.0);
		break;
	}
	case 3:
	{
		sprintf_s(cFileSize, 255, "%.2lf MB", (double)filesize / (1024.0 * 1024.0));
		break;
	}
	default:
	{
		if (filesize < 1000)
		{
			sprintf_s(cFileSize, 255, "%d B", filesize);
		}
		else if (filesize < 1000 * 1024)
		{
			sprintf_s(cFileSize, 255, "%.2lf KB", (double)filesize / 1024.0);
		}
		else if (filesize < 1000 * 1024 * 1024)
		{
			sprintf_s(cFileSize, 255, "%.2lf MB", (double)filesize / (1024.0 * 1024.0));
		}
		else
		{
			double g = (double)filesize / (1024.0 * 1024.0 * 1024.0);
			if (g < 1000.0)
			{
				sprintf_s(cFileSize, 255, "%.2lf GB", g);
			}
			else
			{
				sprintf_s(cFileSize, 255, "%.2lf TB", g / 1024.0);
			}
		}
		break;
	}
	}
	
	return std::string(cFileSize);
}

std::string StringUtils::ExtractFileName(const std::string& filepath)
{
	int nPos1 = filepath.rfind('\\');
	int	nPos2 = filepath.rfind('/');
	int nPos = nPos1 > nPos2 ? nPos1 : nPos2;
	return filepath.substr(nPos + 1, MAX_PATH);
}

std::string StringUtils::ExtractFileExt(const std::string& filepath)
{
	int nPos = filepath.rfind('.');
	if (nPos != -1)
	{
		return filepath.substr(nPos + 1, MAX_PATH);
	}
	else
	{
		return std::string();
	}
}

std::string StringUtils::GetGuid(bool bracket)
{
	char buf[64] = { 0 };
	GUID guid;
	if (SUCCEEDED(CoCreateGuid(&guid)))
	{
		_snprintf_s(buf, 64, sizeof(buf)
			, (bracket ? "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}" : \
				"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X")
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
			, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
			, guid.Data4[6], guid.Data4[7]
			);
	}

	return std::string(buf);
}

std::string StringUtils::MakeSureFileNameValid(const std::string& fname)
{
	std::string str = fname;
	const char* szInValid = "/\\:?\"<>|";
	for (size_t i = 0; i < str.length(); i++)
	{
		if (strchr(szInValid, str[i]) != NULL)
		{
			str[i] = '_';
		}
	}
	return str;
}

std::string StringUtils::ExtractStr(const std::string& str, const std::string& sBegin, const std::string& sEnd)
{
	int p1 = str.find(sBegin);
	if (p1 == std::string::npos)
	{
		return "";
	}
	int p2 = str.find(sEnd, p1+1);
	return str.substr(p1 + sBegin.length(), p2 - p1 - sBegin.length());
}

void StringUtils::SplitStr(const char *buf, char ch, std::vector<std::string> &arOut, long size /*= -1*/)
{
	std::string sValue;

	if (size == -1)
		size = strlen(buf);

	for (int i = 0; i < size; i++)
	{
		if (buf[i] == ch && (i > 0 && buf[i - 1] != '\\'))
		{
			arOut.push_back(sValue);
			sValue = "";
		}
		else
		{
			if (buf[i] == 0)
				break;

			if ((buf[i] == '\\') && ((i + 1) < size && buf[i + 1] == ch))
			{
				i++;
				sValue += buf[i];
				continue;
			}
			sValue += buf[i];
		}
	}

	if (sValue.size() > 0)
	{
		arOut.push_back(sValue);
	}
}

void StringUtils::SplitStr(const char *buf, char ch, std::string &sDest, long nNO /*= 0*/)
{
	std::string sValue;

	int size = strlen(buf);
	int nIdx = 0;

	for (int i = 0; i < size; i++)
	{
		if (buf[i] == ch && (i == 0 || buf[i - 1] != '\\'))
		{
			if (nIdx == nNO)
			{
				sDest = sValue;
				return;
			}
			else
			{
				nIdx++;
				sValue = "";
			}
		}
		else
		{
			if (buf[i] == 0)
				break;

			if ((buf[i] == '\\') && ((i + 1) < size && buf[i + 1] == ch))
			{
				i++;
				sValue += buf[i];
				continue;
			}
			sValue += buf[i];
		}
	}

	if (nIdx == nNO)
	{
		sDest = sValue;
	}
}

