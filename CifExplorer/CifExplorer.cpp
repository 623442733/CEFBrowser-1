// CifExplorer.cpp : main source file for CifExplorer.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"

#include "include/cef_sandbox_win.h"
#include "cef3/WebInit.h"
#include "cef3/WebApp.h"
#include "cef3/WebView.h"
CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;

	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(SW_SHOWMAXIMIZED);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

bool checkfile(wchar_t* file)
{
	WIN32_FIND_DATA p;
	if (FindFirstFile(file, &p) == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	return true;
}

bool OpenUpdate()
{
	WCHAR czPath[MAX_PATH] = { 0 }, czFile[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, czPath);
	swprintf(czFile, L"%s\\update.exe", czPath);
	if (checkfile(czFile))
	{
		ShellExecute(NULL, NULL, L"update.exe", L"CifExplorer 1.0.0.2 http://192.168.3.202:8080/download_gzl/update.ini",
			czPath, SW_SHOW);
		return true;
	}
	return false;
}

void OpenCompaty(HINSTANCE hInstance)
{
	HKEY hKey;
	WCHAR czPath[MAX_PATH];
	GetModuleFileName(hInstance, czPath, MAX_PATH);
	LPCTSTR strSubKey = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers";
	long lRet = ::RegOpenKeyEx(HKEY_CURRENT_USER, strSubKey, 0, KEY_WRITE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		//if (RegQueryValueEx(hKey, czPath, 0, &dwtype, (BYTE*)reBuff, &sizeBuff) == ERROR_SUCCESS)
		TCHAR achValue[] = { _T("WIN7RTM") };
		lRet = ::RegSetValueEx(hKey, czPath, NULL, REG_SZ, (LPBYTE)&achValue, sizeof(achValue));
		RegCloseKey(hKey);
	}
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	void* sandbox_info = NULL;
	CefMainArgs main_args((HINSTANCE)hInstance);
	CefRefPtr<WebApp> app(new WebApp());
	int exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
	if (exit_code >= 0)
	{
		return 0;
	}
	WebViewInit* p = new WebViewInit(hInstance, &app, sandbox_info);
	p->Init();

	WriteLog("ok: --- CIF EXPLORER BEGIN ---");
	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));
	::DefWindowProc(NULL, 0, 0, 0L);
	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls
	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));
	int nRet = Run(lpstrCmdLine, nCmdShow);
	_Module.Term();
	::CoUninitialize();
	if (p)
	{
		delete (WebViewInit *)p;
	}
	GdiplusShutdown(gdiplusToken);
	WriteLog("ok: --- CIF EXPLORER END ---");
	return nRet;
}
