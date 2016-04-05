#pragma   once 
#include "stdafx.h"
class CMyTooltip : public CWindowImpl<CMyTooltip>
{
public:
	DECLARE_WND_CLASS_EX(NULL, 0, COLOR_3DFACE)

	CMyTooltip():m_lpstrInfo(NULL)
	{  }

	void Init(HWND hWnd, LPCTSTR lpstrName)
	{
		ATLASSERT(::IsWindow(hWnd));
		SubclassWindow(hWnd);
		m_tooltip.Create(m_hWnd);
		ATLASSERT(m_tooltip.IsWindow());
		RECT   rect;
		GetClientRect(&rect);
		CToolInfo  ti(0, m_hWnd, m_nToolTipID, &rect, NULL);
		m_tooltip.AddTool(&ti);
		m_lpstrInfo = lpstrName;
		if (m_lpstrInfo == NULL)
			return;
		m_tooltip.UpdateTipText(m_lpstrInfo, m_hWnd, m_nToolTipID);
		m_tooltip.Activate(true);
	}

	BEGIN_MSG_MAP(CMyTooltip)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
	END_MSG_MAP()

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_tooltip.IsWindow())
		{
			MSG msg = { m_hWnd, uMsg, wParam, lParam };
			m_tooltip.RelayEvent(&msg);
		}
		bHandled = FALSE;
		return 1;
	}

private:
	LPCTSTR   m_lpstrInfo;
	CToolTipCtrl m_tooltip;
	enum   { m_nToolTipID = 1313 };
};