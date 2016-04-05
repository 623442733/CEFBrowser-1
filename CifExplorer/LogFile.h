#ifndef _LOGFILE_H
#define _LOGFILE_H

#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <windows.h>
#include <string>
using namespace std;
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

class LogFile
{
protected:

CRITICAL_SECTION _csLock;
string	sFileName;
HANDLE _hFile;

bool OpenFile()//���ļ��� ָ�뵽�ļ�β
{
   if(IsOpen())
    return true;

   if(sFileName.empty ())
    return false;

   _hFile = CreateFileA(
    sFileName.c_str (), 
    GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL 
   );

   if(!IsOpen() && GetLastError() == 2)//�򿪲��ɹ��� ����Ϊ�ļ������ڣ� �����ļ�
    _hFile = CreateFileA(
     sFileName.c_str (), 
     GENERIC_WRITE,
     FILE_SHARE_READ | FILE_SHARE_WRITE,
     NULL,
     OPEN_ALWAYS,
     FILE_ATTRIBUTE_NORMAL,
     NULL 
    );  

   if(IsOpen())
    SetFilePointer(_hFile, 0, NULL, FILE_END);

   return IsOpen();
}

DWORD Write(LPCVOID lpBuffer, DWORD dwLength)
{
   DWORD dwWriteLength = 0;

   if(IsOpen())
    WriteFile(_hFile, lpBuffer, dwLength, &dwWriteLength, NULL);

   return dwWriteLength;
}

virtual void WriteLog( LPCVOID lpBuffer, DWORD dwLength)//д��־, ������չ�޸�
{
   DWORD dwWriteLength;

   if(IsOpen())
   {
	   SYSTEMTIME st;
	   GetLocalTime(&st);
	   char buf[20];
	   sprintf_s(buf, 20, "\r\n%02d:%02d:%02d.%03d  ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	   WriteFile(_hFile, buf, strlen(buf), &dwWriteLength, NULL);
	   WriteFile(_hFile, lpBuffer, dwLength, &dwWriteLength, NULL);
	   FlushFileBuffers(_hFile);
   }
}

void Lock()   { ::EnterCriticalSection(&_csLock); }
void Unlock() { ::LeaveCriticalSection(&_csLock); }

public:
  
LogFile(const char *szFileName = "Log.log")//�趨��־�ļ���
{
   sFileName = "";
   _hFile = INVALID_HANDLE_VALUE;
   ::InitializeCriticalSection(&_csLock);

   SetFileName(szFileName);
}

virtual ~LogFile()
{
   ::DeleteCriticalSection(&_csLock);
   Close();
}

const char * GetFileName()
{
   return sFileName.c_str ();
}

void SetFileName(const char *szName)//�޸��ļ����� ͬʱ�ر���һ����־�ļ�
{
   assert(szName);

   Close();

   sFileName = szName;
 
}

bool IsOpen()
{
   return _hFile != INVALID_HANDLE_VALUE;
}

void Close()
{
   if(IsOpen())
   {
    CloseHandle(_hFile);
    _hFile = INVALID_HANDLE_VALUE;
   }
}

void Log(LPCVOID lpBuffer, DWORD dwLength)//׷����־����
{
   assert(lpBuffer);
   __try
   {
    Lock();

    if(!OpenFile())
     return;

    WriteLog(lpBuffer, dwLength);
   }
   __finally
   {
    Unlock();
   } 
}

void Log(const char *szText)
{
   Log(szText, strlen(szText));
}

private://���κ���

LogFile(const LogFile&);
LogFile&operator = (const LogFile&);
};

class LogFileEx : public LogFile
{
protected:

string sPath;
char _szLastDate[9];
int _iType;

void SetPath(const char *szPath)
{
   assert(szPath);

   WIN32_FIND_DATAA wfd;
   char temp[MAX_PATH + 1] = {0};

   if(FindFirstFileA(szPath, &wfd) == INVALID_HANDLE_VALUE && CreateDirectoryA(szPath, NULL) == 0)
   {
    strcat(strcpy(temp, szPath), " ��־�ļ�����ʧ�ܣ������Ƿ���Ȩ��д��!������ID:");
    //ltoa(GetLastError(), temp + strlen(temp), 10);
    MessageBoxA(NULL, temp, "��־д��ʧ��", MB_OK);
    exit(1);
   }
   else
   {
    GetFullPathNameA(szPath, MAX_PATH, temp, NULL);
    sPath = temp;
   }
}

public:

enum LOG_TYPE{YEAR = 0, MONTH = 1, DAY = 2};

LogFileEx(const char *szPath = ".", LOG_TYPE iType = MONTH)
{
	if (szPath == "" || szPath == ".")
	{
		char cPath[MAX_PATH] = {0}; 	
		::GetModuleFileNameA ((HINSTANCE)&__ImageBase, cPath, MAX_PATH);
		string ssPath = cPath;
		int nPos = ssPath.rfind ('\\');
		ssPath = ssPath.substr (0, nPos);
		ssPath = ssPath + "\\Log";
		
		SetPath(ssPath.c_str ());
	}
	else
	{
		SetPath (szPath);
	}
   _iType = iType;
   memset(_szLastDate, 0, 9);
}

~LogFileEx()
{

}

const char * GetPath()
{
   return sPath.c_str ();
}

void Log(LPCVOID lpBuffer, DWORD dwLength)
{
   assert(lpBuffer);

   char temp[10];
   static const char format[3][10] = {"%Y", "%Y-%m", "%Y%m%d"};
  
 
    time_t now = time(NULL);

    strftime(temp, 9, format[_iType], localtime(&now));

    if(strstr(_szLastDate, temp) == NULL)//�����ļ���
    {
		//DWORD pid = GetCurrentProcessId ();
		//char buf[50];
		//sprintf_s(buf, "-%d", pid);

		sFileName = sPath + "\\";
		sFileName += temp;
		//sFileName += buf;
		sFileName += ".log";    
		strcpy(_szLastDate, temp);
		Close();
    }

    if(!OpenFile())
     return;
   
    WriteLog(lpBuffer, dwLength);
 
}

void Log(const char *szText)
{
   Log(szText, strlen(szText));
}

private://���κ���

LogFileEx(const LogFileEx&);
LogFileEx&operator = (const LogFileEx&);

};

#endif

