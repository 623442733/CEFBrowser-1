#pragma once

// CSetting dialog
#include "resource.h"
#include "stdafx.h"

class CUpdateDlg : public CDialogImpl<CUpdateDlg>
{
public:
	enum { IDD = IDD_UPDIALOG };
	BEGIN_MSG_MAP(CUpdateDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	CUpdateDlg()
	{
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		GetDownfile();
		ShowUpdateText();
		return TRUE;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		//删除ini,打开并运行setup.exe,然后关闭整个程序
		if (MessageBox(L"安装新版本,当前程序会退出!!!", L"提示", MB_OKCANCEL) == IDOK)
		{
			HANDLE handle = ShellExecute(NULL, L"open", L"setup.exe", NULL, wcExe, SW_SHOW);
			DeleteFile(wcIni);
			GetParent().PostMessage(WM_CLOSE, 0, 0);
			DestroyWindow();
		}
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
	{
		DestroyWindow();
		return 0;
	}

	//下载的update不能超过2kb,update数目不超过30个,若要改变，调整下面wcTitle和for循环的参数
	void ShowUpdateText()
	{
		WCHAR wcTitle[2048] = { 0 }, wcKey[64], wcValue[MAX_PATH];
		int version = GetPrivateProfileInt(L"CifExplorer", L"VERSION", 0, wcIni);
		swprintf(wcTitle, L"Cif浏览器%d版本更新日志\r\n", version);

		for (int i = 0; i < 30; i++)
		{
			memset(wcKey, 0, 64);
			memset(wcValue, 0, MAX_PATH);
			swprintf(wcKey, L"UPDATE%d", i + 1);
			GetPrivateProfileString(L"ReadMe", wcKey, wcValue, wcValue, MAX_PATH, wcIni);
			if (wcslen(wcValue))
			{
				wcscat(wcValue, L"\r\n");
				wcscat(wcTitle, wcValue);
			}
			else
			{
				break;
			}
		}
		SetDlgItemText(IDC_EDITMSG, wcTitle);
	}

	void GetDownfile()
	{
		WCHAR wcPath[MAX_PATH] = { 0 };
		SHGetSpecialFolderPath(NULL, wcPath, CSIDL_PERSONAL, FALSE);
		wcscpy(wcIni, wcPath);
		wcscpy(wcExe, wcPath);
		wcscat(wcIni, L"\\CifExplorer\\update.ini");
		wcscat(wcExe, L"\\CifExplorer");
	}

private:
	WCHAR wcIni[MAX_PATH];
	WCHAR wcExe[MAX_PATH];
};