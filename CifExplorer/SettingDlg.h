#pragma once

// CSetting dialog
#include "resource.h"
#include "stdafx.h"

using namespace std;

class CSettingDlg : public CDialogImpl<CSettingDlg>
{
public:
	enum { IDD = IDD_STDIALOG };
	BEGIN_MSG_MAP(CSettingDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_CHECKCS, OnCheckcs)
		COMMAND_HANDLER_EX(IDC_EDITDEURL, EN_CHANGE, OnChangeEditdeurl)
		COMMAND_HANDLER_EX(IDC_EDITCSURL, EN_CHANGE, OnChangeEditcsurl)
		//COMMAND_HANDLER_EX(IDC_COMCHOS, CBN_SELCHANGE, OnSelchangeComchos)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	CSettingDlg()
	{
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		InitControl();
		SetControlValue();
		return TRUE;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		bool rsde = false, rscs = false;
		char szText[MAX_PATH] = { 0 };
		GetDlgItemTextA(m_hWnd, IDC_EDITDEURL, szText, MAX_PATH);
		string str = szText;
		if(CheckHttp(str, 0))
			rsde = writeINI("system", "default_homepage", szText);
		memset(szText, 0, MAX_PATH);
		GetDlgItemTextA(m_hWnd, IDC_EDITCSURL, szText, MAX_PATH);
		str = szText;
		if (CheckHttp(str, 0))
			rscs = writeINI("system", "custom_homepage", szText);
		writeINI("system", "custom_choose", m_checkbox.GetCheck() ? 1 : 0);
		SetDlgItemText(IDC_STATICLOG, rsde&&rscs ? L"写入配置成功" : L"写入配置失败");
		GetParent().PostMessage(UPDATE_SYSINI, 0, 0);
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnCheckcs(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
	{
		int isCheck = m_checkbox.GetCheck();
		if (isCheck)
		{
			GetDlgItem(IDC_EDITCSURL).EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_EDITCSURL).EnableWindow(FALSE);
		}
		return 0;
	}

	LRESULT OnChangeEditdeurl(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		string strUrl;
		char szText[MAX_PATH] = { 0 };
		GetDlgItemTextA(m_hWnd, IDC_EDITDEURL, szText, MAX_PATH);
		strUrl = szText;
		bool rs = CheckHttp(strUrl, 0);
		return 0;
	}

	LRESULT OnChangeEditcsurl(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		string strUrl;
		char szText[MAX_PATH] = { 0 };
		GetDlgItemTextA(m_hWnd, IDC_EDITCSURL, szText, MAX_PATH);
		strUrl = szText;
		bool rs = CheckHttp(strUrl, 1);
		return 0;
	}

	/*LRESULT OnSelchangeComchos(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		int is = m_combox.GetCurSel();
		if (is)
		{
			SetDlgItemTextA(m_hWnd, IDC_EDITURL, m_vetOptionUrl[is - 1].c_str());
		}
		else
		{
			SetDlgItemTextA(m_hWnd, IDC_EDITURL, m_strHomeUrl.c_str());
		}
		return 0;
	}*/

	/*LRESULT OnChangeEditnum(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		WCHAR wc[MAX_PATH] = { 0 };
		int is = GetDlgItemInt(IDC_EDITNUM);
		m_combox.ResetContent();
		m_combox.InsertString(0, L"主页");
		for (int i = 1; i <= is; i++)
		{
			memset(wc, 0, MAX_PATH);
			swprintf(wc, L"可选页面%d", i);
			m_combox.InsertString(i, wc);
		}
		m_combox.SetCurSel(0);
		return 0;
	}*/

private:
	void InitControl()
	{
		int icustom = 0;
		m_checkbox.Attach(GetDlgItem(IDC_CHECKCS).m_hWnd);
		GetDlgItem(IDC_EDITDEURL).EnableWindow(FALSE);
		GetDlgItem(IDC_EDITCSURL).EnableWindow(FALSE);
		GetDlgItem(IDC_STATICNM).ShowWindow(FALSE);
		GetDlgItem(IDC_EDITNUM).ShowWindow(FALSE);
		GetDlgItem(IDC_COMCHOS).ShowWindow(FALSE);
		char szTemp[MAX_PATH] = { 0 };
		readINI("system", "default_homepage", szTemp);
		m_strDefaultUrl = szTemp;
		memset(szTemp, 0, MAX_PATH);
		readINI("system", "custom_homepage", szTemp);
		m_strCustomUrl = szTemp;
		
		readINI("system", "custom_choose", icustom);
		if (icustom)
		{
			m_checkbox.SetCheck(TRUE);
			GetDlgItem(IDC_EDITCSURL).EnableWindow(TRUE);
		}
		/*m_combox.Attach(GetDlgItem(IDC_COMCHOS).m_hWnd);
		char szTemp[MAX_PATH] = { 0 }, szOption[52] = { 0 };
		readINI("system", "child_web_number", m_childNum);
		readINI("system", "main_home_page", szTemp);
		m_strHomeUrl = szTemp;
		for (int i = 0; i < m_childNum; i++)
		{
			memset(szOption, 0, 52);
			memset(szTemp, 0, MAX_PATH);
			sprintf(szOption, "child_web_page%d", i + 1);
			readINI("option", szOption, szTemp);
			m_vetOptionUrl.push_back(szTemp);
		}*/
	}

	void SetControlValue()
	{
		WCHAR wc[MAX_PATH] = { 0 };
		c2w(wc, MAX_PATH, m_strDefaultUrl.c_str());
		SetDlgItemText(IDC_EDITDEURL, wc);
		memset(wc, 0, MAX_PATH);
		c2w(wc, MAX_PATH, m_strCustomUrl.c_str());
		SetDlgItemText(IDC_EDITCSURL, wc);
		if (!m_strCustomUrl.size())
		{
			SetDlgItemText(IDC_STATICLOG, L"当前自定义主页为空");
		}
		/*WCHAR wc[MAX_PATH] = { 0 };
		c2w(wc, MAX_PATH, m_strHomeUrl.c_str());
		SetDlgItemInt(IDC_EDITNUM, m_childNum);
		SetDlgItemText(IDC_EDITURL, wc);
		m_combox.ResetContent();
		m_combox.InsertString(0, L"主页");
		for (int i = 1; i <= m_childNum; i++)
		{
			memset(wc, 0, MAX_PATH);
			swprintf(wc, L"可选页面%d", i);
			m_combox.InsertString(i, wc);
		}
		m_combox.SetCurSel(0);*/
	}

	bool CheckHttp(string& strUrl, int itype)
	{
		int ipos = strUrl.find(":");
		if (ipos == -1)
		{
			if (!itype)
			{
				SetDlgItemText(IDC_STATICLOG, L"默认主页URL不合法，应该以http或https开头");
			}
			else
			{
				SetDlgItemText(IDC_STATICLOG, L"自定义主页URL不合法，应该以http或https开头");
			}
			return false;
		}

		string str;
		str.append(strUrl, 0, ipos);
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		if (str != "http" && str != "https")
		{
			if (!itype)
			{
				SetDlgItemText(IDC_STATICLOG, L"默认主页URL不合法，应该以http或https开头");
			}
			else
			{
				SetDlgItemText(IDC_STATICLOG, L"自定义主页URL不合法，应该以http或https开头");
			}
			return false;
		}
		SetDlgItemText(IDC_STATICLOG, L"");
		return true;
	}

private:
	string             m_strCustomUrl;     //自定义主页
	string             m_strDefaultUrl;    //默认主页的url
	CButton            m_checkbox;
	//int                m_childNum;         //可选打开页面的数量
	//vector<string>     m_vetOptionUrl;     //包含所有的子页面url
	//CComboBox          m_combox;           //下拉框
};