// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		  0x0500
#define _WIN32_WINNT  0x0501
#define _WIN32_IE	  0x0501
#define _RICHEDIT_VER 0x0500
#define _VERSION      1004
#include <algorithm> 
#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlcrack.h>
#include <atlmisc.h>
#include <atlddx.h>

#include <atlcom.h>
#include <atlhost.h>
#include <atlctl.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <Dbghelp.h>
#include "LogFile.h"

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define QUIT_DOWNDLG  WM_USER+10010
#define OPEN_UPDATE   WM_USER+10011
#define UPDATE_SYSINI WM_USER+10012

void c2w(wchar_t *pwstr, size_t len, const char *str);
char* w2c(const wchar_t* wp);
bool readINI(char* option, char* key, char* value, char* path);
bool readINI(char* option, char* key, int& value, char* path);
bool readINI(char* option, char* key, char* value);
bool readINI(char* option, char* key, int& value);
bool writeINI(char* option, char* key, char* value);
bool writeINI(char* option, char* key, int value);
void WriteLog(const std::string& str);
void WriteLog(const std::string& str,int value);
void WriteLog(const std::string& str, std::string value);