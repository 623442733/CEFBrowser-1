#pragma once

// CSetting dialog
#include "resource.h"
#include "stdafx.h"

using namespace std;

class CDownloadDlg : public CDialogImpl<CDownloadDlg>
{
public:
	enum { IDD = IDD_DLDIALOG };
	BEGIN_MSG_MAP(CDownloadDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DOWNLOAD, OnDownload)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		NOTIFY_HANDLER(IDC_LIST1, NM_CLICK, OnNMClick)
		NOTIFY_HANDLER(IDC_LIST1, NM_DBLCLK, OnNMDblclk)
		NOTIFY_HANDLER(IDC_LIST1, NM_CUSTOMDRAW, OnNMCustomdrawList)
		//COMMAND_HANDLER_EX(IDC_COMCHOS, CBN_SELCHANGE, OnSelchangeComchos)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	CDownloadDlg()
	{
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		RECT rc;
		GetDlgItem(IDOK).ShowWindow(FALSE);
		m_downlist.Attach(GetDlgItem(IDC_LIST1));
		m_downlist.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
		m_downlist.GetClientRect(&rc);
		int ix = (rc.right - rc.left) / 10;
		m_downlist.AddColumn(L"文件名", 0);
		m_downlist.SetColumnWidth(0, 2 * ix);
		m_downlist.AddColumn(L"进度", 1);
		m_downlist.SetColumnWidth(1, ix);
		m_downlist.AddColumn(L"速度", 2);
		m_downlist.SetColumnWidth(2, ix);
		m_downlist.AddColumn(L"已下载(kb)", 3);
		m_downlist.SetColumnWidth(3, 2 * ix);
		m_downlist.AddColumn(L"总大小(kb)", 4);
		m_downlist.SetColumnWidth(4, 2 * ix);
		m_downlist.AddColumn(L"保存路径", 5);
		m_downlist.SetColumnWidth(5, 2 * ix);
		return TRUE;
	}

	LRESULT OnDownload(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		DownMsg *precv = (DownMsg *)wParam;
		WCHAR wName[MAX_PATH], wPath[MAX_PATH], wProgress[64], wSpeed[64], wCurrent[64], wTotal[64];
		string name;
		name.append(precv->strPath, precv->strPath.find_last_of("\\") + 1, precv->strPath.size());
		c2w(wName, MAX_PATH, name.c_str());
		c2w(wPath, MAX_PATH, precv->strPath.c_str());
		swprintf(wProgress, L"%d%%", precv->iprogress);
		swprintf(wSpeed, L"%d", precv->ispeed);
		swprintf(wCurrent, L"%d", precv->icurrent);
		swprintf(wTotal, L"%d", precv->itotal);
		if (m_downlist.GetItemCount() >= precv->idty)
		{
			m_downlist.SetItemText(precv->idty - 1, 0, wName);
			m_downlist.SetItemText(precv->idty - 1, 1, wProgress);
			m_downlist.SetItemText(precv->idty - 1, 2, wSpeed);
			m_downlist.SetItemText(precv->idty - 1, 3, wCurrent);
			m_downlist.SetItemText(precv->idty - 1, 4, wTotal);
			m_downlist.SetItemText(precv->idty - 1, 5, wPath);
		}
		else
		{
			m_downlist.AddItem(precv->idty - 1, 0, wName);
			m_downlist.AddItem(precv->idty - 1, 1, wProgress);
			m_downlist.AddItem(precv->idty - 1, 2, wSpeed);
			m_downlist.AddItem(precv->idty - 1, 3, wCurrent);
			m_downlist.AddItem(precv->idty - 1, 4, wTotal);
			m_downlist.AddItem(precv->idty - 1, 5, wPath);
		}
		delete precv;
		return 0;
	}

	LRESULT OnNMClick(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
	{
		LPNMLVCUSTOMDRAW pLVNMCD = reinterpret_cast< LPNMLVCUSTOMDRAW>(pNMHDR);
		NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;
		if (-1 != pNMListView->iItem)        // 如果iItem不是-1，就说明有列表项被选择   
		{
			SetDlgItemText(IDC_STAMSG, L"双击打开所在文件夹");
		}
		else
		{
			SetDlgItemText(IDC_STAMSG, L"");
		}
		return 0;
	}

	LRESULT OnNMDblclk(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
	{
		LPNMLVCUSTOMDRAW pLVNMCD = reinterpret_cast< LPNMLVCUSTOMDRAW>(pNMHDR);
		NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;
		if (-1 != pNMListView->iItem)        // 如果iItem不是-1，就说明有列表项被选择   
		{
			CString str;
			int inow = m_downlist.GetSelectionMark();
			if (-1 != m_downlist.GetSelectionMark())
			{
				m_downlist.GetItemText(inow, 5, str);
				ShellExecute(NULL, NULL, _T("explorer"), _T("/select, ") + str, NULL, SW_SHOW);
			}
			/*SetDlgItemText(IDC_STAMSG, L"");*/
		}
		return 0;
	}

	LRESULT OnNMCustomdrawList(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
	{
		LPNMLVCUSTOMDRAW pLVNMCD = reinterpret_cast< LPNMLVCUSTOMDRAW>(pNMHDR);
		int nResult = CDRF_DODEFAULT;
		if (CDDS_PREPAINT == pLVNMCD->nmcd.dwDrawStage)
		{
			nResult = CDRF_NOTIFYITEMDRAW;
		}
		else if (CDDS_ITEMPREPAINT == pLVNMCD->nmcd.dwDrawStage)
		{
			nResult = CDRF_NOTIFYSUBITEMDRAW;
		}
		else if (pLVNMCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
		{
			//单双行不同颜色
			if (pLVNMCD->nmcd.dwItemSpec % 2 == 0)
			{
				pLVNMCD->clrText = RGB(60, 128, 160);
			}
			else
			{
				pLVNMCD->clrText = RGB(80, 160, 80);
			}
		}
		return nResult;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString str;
		int inow = m_downlist.GetSelectionMark();
		if (-1 != m_downlist.GetSelectionMark())
		{
			m_downlist.GetItemText(inow, 5, str);
			ShellExecute(NULL, NULL, _T("explorer"), _T("/select, ") + str, NULL, SW_SHOW);
		}
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
	{
		GetParent().PostMessage(QUIT_DOWNDLG, 1, 0);
		DestroyWindow();
		return 0;
	}

private:
	CListViewCtrl   m_downlist;
};