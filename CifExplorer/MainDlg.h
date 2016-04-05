// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "resource.h"
#include "ChildDlg.h"
#include "DownDlg.h"
#include "SettingDlg.h"
#include "UpdateDlg.h"
#include "MyTooltips.h"
#include "CustomButton.h"
#include <vector>
using namespace std;

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->message == WM_LBUTTONDBLCLK)
		{
			if (m_tabTitle.m_hWnd == pMsg->hwnd)
			{
				CPoint pos(pMsg->pt);
				CloseWebItem(pos);
			}
		}
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateChildWindows();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_DOWNLOAD, OnDownload)
		MESSAGE_HANDLER(WM_POPWEBURL, OnPopWeburl)
		MESSAGE_HANDLER(WM_WBETITLE, OnWebTitle) 
		MESSAGE_HANDLER(WM_LOADWEBSUC, OnLoadWebsuc)
		MESSAGE_HANDLER(WM_KEYDOWNMSG, OnKeyDownMsg)
		MESSAGE_HANDLER(QUIT_DOWNDLG, OnQuitdowndlg)
		MESSAGE_HANDLER(OPEN_UPDATE, OnOpenUpdate)
		MESSAGE_HANDLER(UPDATE_SYSINI, OnUpdateSysini)
		COMMAND_ID_HANDLER(ID_BTNHOMEPAGE, OnbtnMain)
		COMMAND_ID_HANDLER(ID_BTNBACK, OnbtnBack)
		COMMAND_ID_HANDLER(ID_BTNFORWARD, OnbtnForward)
		COMMAND_ID_HANDLER(ID_BTNREFRESH, OnbtnRefresh)
		COMMAND_ID_HANDLER(ID_BTNSETTING, OnbtnSetting)
		COMMAND_ID_HANDLER(ID_MUDOWN, OnMudown)
		COMMAND_ID_HANDLER(ID_MUHOME, OnMuhome)
		COMMAND_ID_HANDLER(ID_MUUPDATE, OnMupdate)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		NOTIFY_HANDLER(IDC_TABTITLE, TCN_SELCHANGE, OnTcnSelchangeTab) 
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDownload(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnPopWeburl(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnWebTitle(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLoadWebsuc(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKeyDownMsg(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnQuitdowndlg(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOpenUpdate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUpdateSysini(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnbtnMain(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnbtnBack(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnbtnForward(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnbtnRefresh(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnbtnSetting(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMudown(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMuhome(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMupdate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTcnSelchangeTab(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);

public:
	void DoPaint(Graphics& g);
	void CloseDialog(int nVal);

private:
	int                m_labelNum;         //��ǰtab������
	HWND               m_hDownload;        //���ش��ڵı�ʶ
	string             m_strCustomUrl;     //�Զ�����ҳ
	string             m_strDefaultUrl;    //Ĭ����ҳ��url
	string             m_strTitle;         //�������
	string             m_strUpdate;        //�����µ�ַ
	vector<CChildDlg*> m_vetdlgs;          //��������ҳ��dlg��ָ��

	CTabCtrl           m_tabCtrl;          //tabctrl�ؼ�
	CTabCtrl           m_tabTitle;         //��ҳ����
	CCustomButton	   m_homepage;
	CCustomButton	   m_back;
	CCustomButton	   m_forward; 
	CCustomButton      m_refresh;
	CCustomButton      m_setting;
	CMyTooltip         m_homeTip;
	CMyTooltip         m_backTip;
	CMyTooltip         m_fowdTip;
	CMyTooltip         m_refsTip;
	CMyTooltip         m_setgTip;

private:
	void AddWebDlg(std::string strUrl);
	void ShowLog(std::string strUrl, int idty);
	void InitControl();
	void ReadSysIni();
	void SetButtonIco();
	void CloseWebItem(CPoint pos);
	void CloseItem(int item);
	bool CanGoBack();
	bool CanGoForward();
	int  UpdateExe();
	static DWORD WINAPI CreateUpdateThread(void *pParam);
};
