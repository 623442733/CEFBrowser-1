#include "stdafx.h"
#include "MainDlg.h"

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	//读配置必须在最开始
	ReadSysIni();
	InitControl();
	SetButtonIco();
	UpdateExe();
	return TRUE;
}

LRESULT CMainDlg::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 1;
}

LRESULT CMainDlg::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// wParam not NULL means some sub window want get its background brush.
	if (wParam != NULL)
	{
		Graphics g((HDC)wParam);
		DoPaint(g);
	}
	else
	{
		CPaintDC dc(m_hWnd);
		Graphics g(dc.m_hDC);
		DoPaint(g);
	}
	return 0;
}

LRESULT CMainDlg::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	RECT rc, rc1, rc2, rc3, rc4, rc5, rc6, rc7, rc8;
	GetClientRect(&rc);
	{
		if (m_homepage.m_hWnd)
		{
			rc1.left = rc.left + 5;
			rc1.right = rc1.left + 20;
			rc1.top = rc.top + 3;
			rc1.bottom = rc1.top + 20;
			m_homepage.MoveWindow(&rc1);
		}
		if (m_back.m_hWnd)
		{
			rc2.left = rc1.right + 10;
			rc2.right = rc2.left + 20;
			rc2.top = rc.top + 3;
			rc2.bottom = rc2.top + 20;
			m_back.MoveWindow(&rc2);
		}
		if (m_forward.m_hWnd)
		{
			rc3.left = rc2.right + 10;
			rc3.right = rc3.left + 20;
			rc3.top = rc.top + 3;
			rc3.bottom = rc3.top + 20;
			m_forward.MoveWindow(&rc3);
		}
		if (m_refresh.m_hWnd)
		{
			rc4.left = rc3.right + 10;
			rc4.right = rc4.left + 20;
			rc4.top = rc.top + 3;
			rc4.bottom = rc4.top + 20;
			m_refresh.MoveWindow(&rc4);
		}
		if (GetDlgItem(IDC_TABTITLE) != NULL)
		{
			rc8.left = rc4.right + 5;
			rc8.right = rc.right - 30;
			rc8.top = rc.top;
			rc8.bottom = rc4.top + 20;
			GetDlgItem(IDC_TABTITLE).MoveWindow(&rc8, TRUE);
		}
		if (m_setting.m_hWnd)
		{
			rc5.left = rc.right - 25;
			rc5.right = rc.right - 5;
			rc5.top = rc.top + 3;
			rc5.bottom = rc4.top + 20;
			m_setting.MoveWindow(&rc5);
		}
		if (GetDlgItem(IDC_TABCEF) != NULL)
		{
			rc6.left = rc.left + 1;
			rc6.right = rc.right - 1;
			rc6.top = rc5.bottom;
			rc6.bottom = rc.bottom - 1;
			GetDlgItem(IDC_TABCEF).MoveWindow(&rc6, TRUE);
			for (int i = 0; i < m_vetdlgs.size(); i++)
			{
				::PostMessage(m_vetdlgs[i]->m_hWnd, WM_SIZE, 0, 0);
			}
		}
		if (GetDlgItem(IDC_STALOG) != NULL)
		{
			rc7.left = rc.left + 10;
			rc7.right = rc.right - 10;
			rc7.top = rc6.bottom;
			rc7.bottom = rc.bottom;
			GetDlgItem(IDC_STALOG).MoveWindow(&rc7, TRUE);
		}
	}
	return 0;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	for (int i = 0; i < m_vetdlgs.size(); i++)
	{
		m_vetdlgs[i]->DestroyWindow();
		delete m_vetdlgs[i];
	}

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

//
LRESULT CMainDlg::OnDownload(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	WCHAR wcPath[MAX_PATH] = { 0 }, wcData[MAX_PATH] = { 0 };
	int itype = lParam;
	if (itype)
	{
		//下载文件
		if (m_hDownload != NULL)
		{
			::PostMessage(m_hDownload, WM_DOWNLOAD, wParam, lParam);
		}
	}
	else
	{
		std::string *precv = (std::string *)wParam;
		std::string str;
		int is = precv->find_last_of('/');
		str.append(*precv, is + 1, precv->size());
		c2w(wcData, MAX_PATH, str.c_str());
		CFileDialog savedlg(FALSE, NULL, wcData, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"文件 (*.jpg; *.bmp; *.png)", m_hWnd);
		if (IDOK == savedlg.DoModal(*this))
		{
			memset(wcData, 0, MAX_PATH);
			c2w(wcData, MAX_PATH, precv->c_str());
			wcscpy(wcPath, savedlg.m_szFileName);
			URLDownloadToFile(0, wcData, wcPath, 0, NULL);
		}
		delete precv;
	}
	return 0;
}

//要打开的新的页面url消息
LRESULT CMainDlg::OnPopWeburl(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	std::string strUrl;
	std::string *precv = (std::string *)wParam;
	strUrl = *precv;
	AddWebDlg(strUrl);
	delete precv;
	return 0;
}

//返回的网页标题消息
LRESULT CMainDlg::OnWebTitle(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	std::wstring *precv = (std::wstring *)wParam;
	int titleId = (int)lParam;
	WCHAR wcTitle[MAX_PATH] = { 0 };
	wcscat(wcTitle, precv->c_str());
	TCITEM item = { 0 };
	item.mask = TCIF_TEXT;
	item.pszText = wcTitle;
	m_tabTitle.SetItem(titleId, &item);
	m_tabTitle.SetCurSel(titleId);
	m_back.EnableWindow(CanGoBack() ? TRUE : FALSE);
	m_forward.EnableWindow(CanGoForward() ? TRUE : FALSE);
	delete precv;
	return 0;
}

LRESULT CMainDlg::OnLoadWebsuc(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	std::string strUrl;
	std::string *precv = (std::string *)wParam;
	strUrl = *precv;
	ShowLog(strUrl, 2);
	delete precv;
	return 0;
}

LRESULT CMainDlg::OnKeyDownMsg(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	int inow = m_tabTitle.GetCurSel();
	int isum = m_tabTitle.GetItemCount();
	switch (wParam)
	{
	case KeyDownMsg::GoForward:
		m_vetdlgs[inow]->ShowWindow(SW_HIDE);
		inow = inow == 0 ? (isum - 1) : (inow - 1);
		m_tabTitle.SetCurSel(inow);
		m_vetdlgs[inow]->ShowWindow(SW_SHOW);
		m_back.EnableWindow(CanGoBack() ? TRUE : FALSE);
		m_forward.EnableWindow(CanGoForward() ? TRUE : FALSE);
		break;
	case KeyDownMsg::GoNext:
		m_vetdlgs[inow]->ShowWindow(SW_HIDE);
		inow = inow == isum - 1 ? 0 : (inow + 1);
		m_tabTitle.SetCurSel(inow);
		m_vetdlgs[inow]->ShowWindow(SW_SHOW);
		m_back.EnableWindow(CanGoBack() ? TRUE : FALSE);
		m_forward.EnableWindow(CanGoForward() ? TRUE : FALSE);
		break;
	case KeyDownMsg::GoCreate:
		AddWebDlg(m_strDefaultUrl);
		break;
	case KeyDownMsg::GoClose:
		CloseItem(inow);
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CMainDlg::OnQuitdowndlg(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	int idty = (int)wParam;
	if (idty)
		m_hDownload = NULL;
	return 0;
}

LRESULT CMainDlg::OnOpenUpdate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CUpdateDlg *updlg = new CUpdateDlg;
	updlg->Create(m_hWnd);
	updlg->ShowWindow(SW_SHOW);
	return 0;
}

LRESULT CMainDlg::OnUpdateSysini(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	ReadSysIni();
	return 0;
}

LRESULT CMainDlg::OnbtnMain(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	AddWebDlg(m_strDefaultUrl);
	return 0;
}

LRESULT CMainDlg::OnbtnBack(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nItem = m_tabTitle.GetCurSel();
	if (m_vetdlgs.size() > nItem)
	    m_vetdlgs[nItem]->GoBack();
	return 0;
}

LRESULT CMainDlg::OnbtnForward(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nItem = m_tabTitle.GetCurSel();
	if (m_vetdlgs.size() > nItem)
	    m_vetdlgs[nItem]->GoForward();
	return 0;
}

LRESULT CMainDlg::OnbtnRefresh(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nItem = m_tabTitle.GetCurSel();
	if (m_vetdlgs.size() > nItem)
	    m_vetdlgs[nItem]->Refresh();
	return 0;
}

LRESULT CMainDlg::OnbtnSetting(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CRect rect;
	CMenu menu;
	if (IDR_MENU1 == 0)
		return 0;
	if (m_setting.GetCheck() == FALSE)
		m_setting.SetCheck(TRUE);
	m_setting.GetWindowRect(&rect);
	if (!menu.LoadMenu(IDR_MENU1))
		return 0;
	CMenuHandle menuPopup = menu.GetSubMenu(0);
	if (menuPopup == NULL)
		return 0;
	bool rs = menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left, rect.bottom, m_hWnd);
	m_setting.SetCheck(FALSE);
	return 0;
}

LRESULT CMainDlg::OnMudown(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CDownloadDlg *downdlg = new CDownloadDlg;
	m_hDownload = downdlg->Create(m_hWnd);
	downdlg->ShowWindow(SW_SHOW);
	return 0;
}

LRESULT CMainDlg::OnMuhome(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CSettingDlg setdlg;
	setdlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnMupdate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int rs = UpdateExe();
	switch (rs)
	{
	case -1:
		MessageBox(L"访问下载服务器失败");
		break;
	case 0:
		MessageBox(L"当前版本无需更新");
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::DoPaint(Graphics& g)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	Rect rc(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height());

	SolidBrush br(Color::WhiteSmoke);
	g.FillRectangle(&br, rc);
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

void CMainDlg::AddWebDlg(std::string strUrl)
{
	CChildDlg *dlg = new CChildDlg(m_hWnd, strUrl, m_labelNum);
	dlg->Create(m_tabCtrl.m_hWnd);
	m_vetdlgs.push_back(dlg);

	for (int i = 0; i < m_vetdlgs.size(); i++)
	{
		if (i != (m_vetdlgs.size() - 1))
		{
			m_vetdlgs[i]->ShowWindow(SW_HIDE);
		}
	}
	m_vetdlgs[(m_vetdlgs.size() - 1)]->ShowWindow(SW_SHOW);
	m_tabTitle.AddItem(L"                  ");
	m_labelNum++;
	ShowLog(strUrl, 1);
}

void CMainDlg::InitControl()
{
	RECT rc;
	m_labelNum = 0;
	m_hDownload = NULL;
	m_tabCtrl.Attach(GetDlgItem(IDC_TABCEF).m_hWnd);
	m_tabTitle.Attach(GetDlgItem(IDC_TABTITLE).m_hWnd);
	HFONT hfont;
	hfont = CreateFont(14, 10, 0, 0, 100, false, false, false, CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, PROOF_QUALITY, FF_MODERN, L"黑体");
	m_tabTitle.SetFont(hfont);
	m_tabTitle.SetItemSize(20, 22);
	m_tabTitle.SetMinTabWidth(200);

	GetDlgItem(IDC_STALOG).GetClientRect(&rc);
	::SetWindowPos(GetDlgItem(IDC_STALOG).m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SW_SHOW);

	AddWebDlg(m_strDefaultUrl);
}

void CMainDlg::ReadSysIni()
{
	int icustom = 0;
	char szTemp[MAX_PATH] = { 0 };
	readINI("system", "custom_choose", icustom);
	readINI("system", "auto_update", szTemp);
	m_strUpdate = szTemp;
	memset(szTemp, 0, MAX_PATH);
	readINI("system", "software_title", szTemp);
	m_strTitle = szTemp;
	SetWindowTextA(m_hWnd, m_strTitle.c_str());
	memset(szTemp, 0, MAX_PATH);
	readINI("system", "default_homepage", szTemp);
	m_strDefaultUrl = szTemp;
	if (m_strDefaultUrl.size())
	{
		WriteLog("ok: 读取到默认主页地址:%s", m_strDefaultUrl);
	}
	else
	{
		m_strDefaultUrl = "http://baidu.com";
		MessageBox(L"error: 读取到默认主页地址为空,请设置...");
		WriteLog("error: 读取到默认主页地址为空");
	}
	if (icustom)
	{
		memset(szTemp, 0, MAX_PATH);
		readINI("system", "custom_homepage", szTemp);
		m_strCustomUrl = szTemp;
		if (m_strCustomUrl.size())
		{
			m_strDefaultUrl = m_strCustomUrl;
		}
	}
}

void CMainDlg::SetButtonIco()
{
	RECT rc{ 0, 0, 21, 21 };
	m_homepage.Create(m_hWnd, rc, NULL, WS_CHILD | WS_VISIBLE, 0, ID_BTNHOMEPAGE);
	m_homepage.SetThemeParent(m_hWnd);
	m_homepage.LoadBitmap(IDB_PNG1, 2);
	m_homepage.SetImages(0, 2, 1, 1);

	m_back.Create(m_hWnd, rc, NULL, WS_CHILD | WS_VISIBLE, 0, ID_BTNBACK);
	m_back.SetThemeParent(m_hWnd);
	m_back.LoadBitmap(IDB_PNG2, 3);
	m_back.SetImages(0, 2, 1, 2);
	m_back.EnableWindow(FALSE);

	m_forward.Create(m_hWnd, rc, NULL, WS_CHILD | WS_VISIBLE, 0, ID_BTNFORWARD);
	m_forward.SetThemeParent(m_hWnd);
	m_forward.LoadBitmap(IDB_PNG3, 3);
	m_forward.SetImages(0, 2, 1, 2);
	m_forward.EnableWindow(FALSE);

	m_refresh.Create(m_hWnd, rc, NULL, WS_CHILD | WS_VISIBLE, 0, ID_BTNREFRESH);
	m_refresh.SetThemeParent(m_hWnd);
	m_refresh.LoadBitmap(IDB_PNG4, 2);
	m_refresh.SetImages(0, 2, 1, 1);

	m_setting.Create(m_hWnd, rc, NULL, WS_CHILD | WS_VISIBLE, 0, ID_BTNSETTING);
	m_setting.SetThemeParent(m_hWnd);
	m_setting.LoadBitmap(IDB_PNG5, 2);
	m_setting.SetImages(0, 2, 1, 1);

	m_homeTip.Init(m_homepage.m_hWnd, L"主页");
	m_backTip.Init(m_back.m_hWnd, L"后退");
	m_fowdTip.Init(m_forward.m_hWnd, L"前进");
	m_refsTip.Init(m_refresh.m_hWnd, L"刷新");
	m_setgTip.Init(m_setting.m_hWnd, L"设置");
}

void CMainDlg::CloseWebItem(CPoint pos)
{
	m_tabTitle.ScreenToClient(&pos);
	int sum = m_tabTitle.GetItemCount() - 1;
	for (int i = sum; i >= 0; i--)
	{
		RECT rc;
		m_tabTitle.GetItemRect(i, &rc);
		if (pos.x > rc.left && pos.x < rc.right)
		{
			CloseItem(i);
		}
	}
}

void CMainDlg::CloseItem(int item)
{
	m_vetdlgs[item]->DestroyWindow();
	delete m_vetdlgs[item];
	vector<CChildDlg*>::iterator itr = m_vetdlgs.begin();
	for (int i = 0; itr != m_vetdlgs.end(); i++)
	{
		if (i == item)
		{
			itr = m_vetdlgs.erase(itr);
		}
		else
		{
			itr++;
		}
	}
	m_tabTitle.DeleteItem(item);
	m_labelNum--;
	if (item)
	{
		m_tabTitle.SetCurSel(item - 1);
		m_vetdlgs[item - 1]->ShowWindow(SW_SHOW);
		m_back.EnableWindow(CanGoBack() ? TRUE : FALSE);
		m_forward.EnableWindow(CanGoForward() ? TRUE : FALSE);
	}
	else if (!item && !m_labelNum)
	{
		AddWebDlg(m_strDefaultUrl);
	}
	else if(!item && m_labelNum)
	{
		m_tabTitle.SetCurSel(item);
		m_vetdlgs[m_labelNum - 1]->ShowWindow(SW_SHOW);
		m_back.EnableWindow(CanGoBack() ? TRUE : FALSE);
		m_forward.EnableWindow(CanGoForward() ? TRUE : FALSE);
	}
}

bool CMainDlg::CanGoBack()
{
	bool rs = false;
	int nItem = m_tabTitle.GetCurSel();
	if (m_vetdlgs.size() > nItem)
		rs = m_vetdlgs[nItem]->CanGoBack();
	return rs;
}

bool CMainDlg::CanGoForward()
{
	bool rs = false;
	int nItem = m_tabTitle.GetCurSel();
	if (m_vetdlgs.size() > nItem)
		rs = m_vetdlgs[nItem]->CanGoForward();
	return rs;
}

int CMainDlg::UpdateExe()
{
	int iVersion = 0, rs;
	char cPath[MAX_PATH] = { 0 };
	SHGetSpecialFolderPathA(NULL, cPath, CSIDL_PERSONAL, FALSE);
	strcat(cPath, "\\CifExplorer\\update.ini");

	HRESULT hr = URLDownloadToFileA(0, m_strUpdate.c_str(), cPath, 0, NULL);
	if (hr == S_OK)
	{
		readINI("CifExplorer", "VERSION", iVersion, cPath);
		if (iVersion > _VERSION)
		{
			rs = 1;
			HANDLE m_handle = CreateThread(NULL, 0, CreateUpdateThread, this, 0, 0);
			CloseHandle(m_handle);
		}
		else
		{
			rs = 0;
		}
	}
	else
	{
		rs = -1;
	}
	return rs;
}

DWORD WINAPI CMainDlg::CreateUpdateThread(void *pParam)
{
	CMainDlg* _pthis = (CMainDlg*)pParam;
	if (_pthis == NULL)
	{
		return 0;
	}

	WCHAR wcUrl[MAX_PATH] = { 0 }, wcPath[MAX_PATH] = { 0 }, wcFile[MAX_PATH] = {0};
	SHGetSpecialFolderPath(NULL, wcFile, CSIDL_PERSONAL, FALSE);
	wcscpy(wcPath, wcFile);
	wcscat(wcPath, L"\\CifExplorer\\setup.exe");
	wcscat(wcFile, L"\\CifExplorer\\update.ini");
	GetPrivateProfileString(L"CifExplorer", L"FILE", wcUrl, wcUrl, MAX_PATH, wcFile);
	HRESULT hr = URLDownloadToFile(0, wcUrl, wcPath, 0, NULL);
	hr == S_OK ? _pthis->PostMessage(OPEN_UPDATE) : _pthis->MessageBox(L"新版本下载失败");
	return 0;
}

void CMainDlg::ShowLog(std::string strUrl, int idty)
{
	std::string str;
	WCHAR wc[MAX_PATH], wlog[MAX_PATH];
	if (strUrl.size() >= 100)
	{
		str.append(strUrl.c_str(), 100);
	}
	else
	{
		str = strUrl;
	}
	c2w(wc, MAX_PATH, str.c_str());
	switch (idty)
	{
	case 1:
		swprintf(wlog, L"正在连接网页:%s 中", wc);
		break;
	case 2:
		swprintf(wlog, L"打开网页:%s 成功", wc);
		break;
	case 3:
		swprintf(wlog, L"打开网页:%s 失败", wc);
		break;
	default:
		break;
	}
}

LRESULT CMainDlg::OnTcnSelchangeTab(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	int nItem = m_tabTitle.GetCurSel();
	for (int i = 0; i < m_vetdlgs.size(); i++)
	{
		if (i != nItem)
		{
			m_vetdlgs[i]->ShowWindow(SW_HIDE);
		}
	}
	m_vetdlgs[nItem]->ShowWindow(SW_SHOW);
	m_back.EnableWindow(CanGoBack() ? TRUE : FALSE);
	m_forward.EnableWindow(CanGoForward() ? TRUE : FALSE);
	return 0;
}
