#pragma once

// CChildDlg dialog
#include "resource.h"
#include "stdafx.h"
#include "cef3\WebView.h"

class CChildDlg : public CWindowImpl<CChildDlg>
{
public:
	enum { IDD = IDD_CDDIALOG };
	BEGIN_MSG_MAP(CChildDlg)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_DOWNLOAD, OnDownload)
		MESSAGE_HANDLER(WM_POPWEBURL, OnPopWeburl)
		MESSAGE_HANDLER(WM_WBETITLE, OnWebTitle)
		MESSAGE_HANDLER(WM_LOADWEBSUC, OnLoadWebsuc)
		MESSAGE_HANDLER(WM_KEYDOWNMSG, OnKeyDownMsg)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	CChildDlg(HWND hparent,std::string strUrl, int titleId)
	{
		m_hParent = hparent;
		m_sUrl = strUrl;
		m_titleId = titleId;
		m_pWeb = new WebView();
		m_pWeb->AddRef();
	}
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		RECT rc;
		GetParent().GetClientRect(&rc);
		
		WebView* web = (WebView*)m_pWeb;
		if (m_hWnd)
		{
			WebView* web = (WebView*)m_pWeb;
			web->CreateWebHwnd(m_hWnd, rc, m_sUrl);
		    SetWindowPos(HWND_TOP, &rc, SWP_SHOWWINDOW);
		}
		return TRUE;
	}
	
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RECT rc;
		GetParent().GetClientRect(&rc);
		WebView* web = (WebView*)m_pWeb;
		//if (web->GetWebHwnd())
		{
			web->SetPenderSize(rc);
			web->BrowserReSize(web->GetBrowser());
			SetWindowPos(HWND_TOP, &rc, SWP_SHOWWINDOW);
		}
		return 0;
	}

	LRESULT OnDownload(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		::PostMessage(m_hParent, WM_DOWNLOAD, wParam, lParam);
		return 0;
	}

	LRESULT OnPopWeburl(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		::PostMessage(m_hParent, WM_POPWEBURL, wParam, 0);
		return 0;
	}

	LRESULT OnWebTitle(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		::PostMessage(m_hParent, WM_WBETITLE, wParam, m_titleId);
		return 0;
	}

	LRESULT OnLoadWebsuc(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		::PostMessage(m_hParent, WM_LOADWEBSUC, wParam, 0);
		return 0;
	}

	LRESULT OnKeyDownMsg(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		::PostMessage(m_hParent, WM_KEYDOWNMSG, wParam, 0);
		return 0;
	}

public:
	bool CanGoForward()
	{
		WebView* web = (WebView*)m_pWeb;
		if (web->GetBrowser()->CanGoForward())
		{
			return true;
		}
		return false;
	}

	bool CanGoBack()
	{
		WebView* web = (WebView*)m_pWeb;
		if (web->GetBrowser()->CanGoBack())
		{
			return true;
		}
		return false;
	}

	bool GoForward()
	{
		WebView* web = (WebView*)m_pWeb;
		if (web->GetBrowser()->CanGoForward())
		{
			web->GetBrowser()->GoForward();
		}
		return true;
	}
	bool GoBack()
	{
		WebView* web = (WebView*)m_pWeb;
		if (web->GetBrowser()->CanGoBack())
		{
			web->GetBrowser()->GoBack();
		}
		return true;
	}
	bool Refresh()
	{
		WebView* web = (WebView*)m_pWeb;
		web->GetBrowser()->ReloadIgnoreCache();
		return true;
	}

private:
	int           m_titleId;  //标识当前页面的标签号
	WebView      *m_pWeb;
	HWND          m_hParent;
	std::string   m_sUrl;
};