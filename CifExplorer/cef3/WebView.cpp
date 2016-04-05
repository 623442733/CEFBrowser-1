//#include "WebView.h"
#include "WebView.h"
//#include "WebView.h"
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_path_util.h"
#include "include/cef_process_util.h"
#include "include/cef_runnable.h"
#include "include/cef_trace.h"
#include "include/cef_url.h"
#include "util.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "WebRenderer.h"
#include "WebMsgHandlerMgr.h"

#include "public/StringUtils.h"
using namespace std;

enum client_menu_ids
{
	CLIENT_ID_SHOW_DEVTOOLS = MENU_ID_USER_FIRST,
	CLIENT_ID_CLOSE_DEVTOOLS,
	CLIENT_ID_INSPECT_ELEMENT,
	CLIENT_ID_DOWNLOAD_IMAGE,
	CLIENT_ID_COPY_URL
};

WebView::WebView()
	: is_created_(false)
	, is_closing_(false)
	, do_close_(false)
	, is_need_js_(false)
	, browser_id_(0)
	, hParent_(NULL)
	, cb_func_(NULL)
	, cb_data_(NULL)
{
}

WebView::~WebView() {
	
}

void WebView::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	//REQUIRE_UI_THREAD();

	if (!GetBrowser())
	{
		//AutoLock lock(this);
		base::AutoLock lock_scope(lock_);
		m_Browser = browser;
		browser_id_ = browser->GetIdentifier();
		
		//::PostMessage(hParent_, WM_CEF3_BROWSER_CREATED, NULL, NULL);
		CefPostTask(TID_UI, NewCefRunnableMethod(this, &WebView::BrowserReSize, m_Browser));
	}
	else if (browser->IsPopup())
	{
		base::AutoLock lock_scope(lock_);
		browser_list_.push_back(browser);
		CefPostTask(TID_UI, NewCefRunnableMethod(this, &WebView::BrowserSetFocus, browser));
	}
}

bool WebView::DoClose(CefRefPtr<CefBrowser> browser)
{
	//REQUIRE_UI_THREAD();
	
	if (browser_id_ == browser->GetIdentifier())
	{
		//browser->GetHost()->ParentWindowWillClose();
		return true;
	}
	
	return false;
}

void WebView::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	//REQUIRE_UI_THREAD();
	if (browser_id_ == browser->GetIdentifier())
	{
		m_Browser = NULL;
	}
	else if (browser->IsPopup())
	{
		if (!browser_list_.empty())
		{
			base::AutoLock lock_scope(lock_);
			// Remove from the browser popup list.
			BrowserList::iterator bit = browser_list_.begin();
			for (; bit != browser_list_.end(); ++bit)
			{
				if ((*bit) && (*bit)->IsSame(browser))
				{
					browser_list_.erase(bit);
					break;
				}
			}
		}
	}
}

void WebView::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl) {
	//REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message.
	// in release mode may be cause dead loop
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>无法访问 URL " << std::string(failedUrl) <<
		" ，错误信息： " << std::string(errorText) << " (" << errorCode <<
		").</h2></body></html>";
	std::string u8 = ss.str();
	StringUtils::AnsiToUtf8(u8);
	frame->LoadString(u8, failedUrl);
}

bool WebView::CloseAllBrowsers(bool force_close)
{
	base::AutoLock lock_scope(lock_);

	if (!browser_list_.empty())
	{
		BrowserList::const_iterator it = browser_list_.begin();
		for (; it != browser_list_.end(); ++it)
		{
			if (*it && (*it)->GetHost())
			{
				(*it)->GetHost()->CloseBrowser(force_close);
			}			
		}
			
		browser_list_.clear();
	}

	if (m_Browser)
	{
		m_Browser = NULL;
	}

	return true;
}

HWND WebView::GetWebHwnd()
{
	if (m_Browser && m_Browser->GetHost())
	{
		return m_Browser->GetHost()->GetWindowHandle();
	}
	return NULL;
}

HRESULT WebView::CreateWebHwnd(HWND hParent, RECT& rect, const std::string &sURL)
{
	//AutoLock lock(this);
	//base::AutoLock lock_scope(lock_);

	if (is_created_)
	{
		return S_OK;
	}

	do_close_ = true;

	CefWindowInfo wi;
	wi.SetAsChild(hParent, rect);

	CefBrowserSettings settings;
	std::string str = sURL;
	StringUtils::AnsiToUtf8(str);
	
	is_created_ = CefBrowserHost::CreateBrowser(wi, this, str, settings, NULL);
	//m_Browser = CefBrowserHost::CreateBrowserSync(wi, this, sURL, settings, NULL);

	if (is_created_)
	{
		hParent_ = hParent;
	}
	return is_created_ ? S_OK : S_FALSE;
}

bool WebView::Navigate(const std::string& url)
{
	//TEST: need reset m_browser object ?
	if (m_Browser.get() && !url.empty())
	{
		string str = url;
		StringUtils::AnsiToUtf8(str);
		
		m_Browser->GetMainFrame()->LoadURL(str);

		return true;
	}
	
	return false;
}

void WebView::SetHostNotify(const std::string& sEvent, int p1, int p2, const std::string& s1, const std::string& s2)
{	
	std::string ss1 = s1, ss2 = s2;
	// Send the result back to the render process.
	CefRefPtr<CefProcessMessage> response = CefProcessMessage::Create("OnHostNotify");
	response->GetArgumentList()->SetString(0, sEvent);
	response->GetArgumentList()->SetInt(1, p1);
	response->GetArgumentList()->SetInt(2, p2);
	response->GetArgumentList()->SetString(3, StringUtils::AnsiToUtf8(ss1));
	response->GetArgumentList()->SetString(4, StringUtils::AnsiToUtf8(ss2));

	if (!m_Browser.get())
	{
		m_pendingJs.insert(response);
		return;
	}
	else
	{
		m_Browser->SendProcessMessage(PID_RENDERER, response);
	}	
}

void WebView::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	string str = title;
	StringUtils::Utf8ToAnsi(str);
	std::wstring *strs = new wstring;
	if (str.size() > 20)
	{
		string str1;
		str1.append(str, 0, 20);
		std::wstring wstr = StringUtils::AnsiToWchar(str1);
		*strs = wstr;
	}
	else
	{
		std::wstring wstr = StringUtils::AnsiToWchar(str);
		*strs = wstr;
	}
	PostMessage(hParent_, WM_WBETITLE, (WPARAM)strs, 0);
}

bool WebView::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	return WebMsgHandlerMgr::GetInstancePtr()->OnProcessMessageReceived(this, browser, source_process, message);
}

void WebView::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	if (httpStatusCode)
	{
		string str = frame->GetURL();
		StringUtils::Utf8ToAnsi(str);
		std::string *strs = new string;
		*strs = str;
		PostMessage(hParent_, WM_LOADWEBSUC, (WPARAM)strs, 0);
	}
}

void WebView::OnWebNotify(const std::string& sEvent, int p1, int p2, const std::string& s1, const std::string& s2)
{
	if (cb_func_)
	{
		(*cb_func_)(cb_data_, sEvent.c_str(), p1, p2, s1.c_str(), s2.c_str());
	}	
}

void WebView::Refresh()
{
	if (m_Browser)
	{
		m_Browser->ReloadIgnoreCache();
	}
}

bool WebView::OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut)
{
	if (os_event && os_event->message == WM_KEYDOWN && os_event->wParam == VK_F5)
	{
		browser->ReloadIgnoreCache();
		return true;
	}
	return false;
}

bool WebView::OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event)
{
	if (os_event && os_event->message == WM_KEYDOWN && GetAsyncKeyState(VK_CONTROL)) //ctrl + tab 上一个标签
	{
		if (GetAsyncKeyState(VK_SHIFT))
		{
			if (GetAsyncKeyState(VK_TAB))
			{
				PostMessage(hParent_, WM_KEYDOWNMSG, KeyDownMsg::GoForward, 0);
				return true;
			}
		}
	}
	if (os_event && os_event->message == WM_KEYDOWN && GetAsyncKeyState(VK_CONTROL)) //ctrl + tab 下一个标签
	{
		if (GetAsyncKeyState(VK_TAB))
		{
			PostMessage(hParent_, WM_KEYDOWNMSG, KeyDownMsg::GoNext, 0);
			return true;
		}
	}
	if (os_event && os_event->message == WM_KEYDOWN && GetAsyncKeyState(VK_CONTROL)) //ctrl + t 新建标签
	{
		if (GetAsyncKeyState(84))
		{
			PostMessage(hParent_, WM_KEYDOWNMSG, KeyDownMsg::GoCreate, 0);
			return true;
		}
	}
	if (os_event && os_event->message == WM_KEYDOWN && GetAsyncKeyState(VK_CONTROL)) //ctrl + w 关闭标签
	{
		if (GetAsyncKeyState(87))
		{
			PostMessage(hParent_, WM_KEYDOWNMSG, KeyDownMsg::GoClose, 0);
			return true;
		}
	}

	if (os_event && os_event->message == WM_KEYDOWN && GetAsyncKeyState(VK_CONTROL)) //ctrl + F5 强制刷新
	{
		if (GetAsyncKeyState(VK_F5))
		{
			browser->Reload();
			return true;
		}
	}
	if (os_event && os_event->message == WM_KEYDOWN && os_event->wParam == VK_F5)   //F5 刷新
	{
		browser->ReloadIgnoreCache();
		return true;
	}
	if (os_event && os_event->message == WM_KEYDOWN && os_event->wParam == VK_F12)  //F12 开发者工具
	{
		ShowDevTools(browser, CefPoint());
		return true;
	}
	return false;
}

void WebView::SetCallback(WEBCALLBACK func, void* data)
{
	cb_func_ = func;
	cb_data_ = data;
}

bool WebView::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect)
{
	string strUrl = std::string(request->GetURL()).c_str();
	if (strUrl.substr(0, 10) == "apexapp://")
	{
		strUrl = strUrl.substr(10, strUrl.length());
		int p = strUrl.find('/');
		if (p != string::npos)
		{
			string func, arg;
			func = strUrl.substr(0, p);
			arg = strUrl.substr(p + 1, strUrl.length());
			OnWebNotify("apexapp", 0, 0, func, arg);
			return true;
		}
	}

	if (browser.get() && browser->GetHost().get() && browser->IsPopup())
	{
		browser->GetHost().get()->SetFocus(true);
		HWND hWnd = browser->GetHost().get()->GetWindowHandle();
		SetForegroundWindow(hWnd);
	}

	return false;
}

bool WebView::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, 
	const CefString& target_frame_name, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, 
	CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access)
{
	//把要打开的网页post回去打开，别弹出来
	string *pstr = new string;
	*pstr = std::string(target_url).c_str();
	PostMessage(hParent_, WM_POPWEBURL, (WPARAM)pstr, 0);
	return true;
}

CefRefPtr<CefBrowser> WebView::GetBrowser()
{
	base::AutoLock lock_scope(lock_);
	return m_Browser;
}

int WebView::GetBrowserId()
{
	base::AutoLock lock_scope(lock_);
	return browser_id_;
}

void WebView::BrowserSetFocus(CefRefPtr<CefBrowser> browser)
{
	if (browser.get() && browser->GetHost().get())
	{
		browser->GetHost().get()->SetFocus(true);
		HWND hWnd = browser->GetHost().get()->GetWindowHandle();
		SetForegroundWindow(hWnd);
	}	
}

void WebView::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	model->Clear();
	cef_context_menu_type_flags_t flag = params->GetTypeFlags();
	model->AddItem(CLIENT_ID_COPY_URL, L"复制网址");
	model->AddItem(MENU_ID_RELOAD, L"重新加载(&R)");
	model->AddSeparator();
	if (flag & CM_TYPEFLAG_MEDIA)
	{
		cef_context_menu_media_type_t mediaFlag = params->GetMediaType();
		if (mediaFlag == CM_MEDIATYPE_IMAGE)
		{
			model->AddItem(CLIENT_ID_DOWNLOAD_IMAGE, L"另存为(&A)...");
		}
	}
	model->AddItem(MENU_ID_PRINT, L"打印(&P)...");
	if ((flag & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) != 0) 
	{
		model->AddSeparator();
		model->AddItem(CLIENT_ID_INSPECT_ELEMENT, L"审查元素(&S)...");
	}
}
 
bool WebView::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,CefRefPtr<CefFrame> frame,CefRefPtr<CefContextMenuParams> params,
	int command_id,EventFlags event_flags)
{
	std::string str;
	switch (command_id) {
	case CLIENT_ID_SHOW_DEVTOOLS:
		ShowDevTools(browser, CefPoint());
		return true;
	case CLIENT_ID_CLOSE_DEVTOOLS:
		CloseDevTools(browser);
		return true;
	case CLIENT_ID_INSPECT_ELEMENT:
		ShowDevTools(browser, CefPoint(params->GetXCoord(), params->GetYCoord()));
		return true;
	case CLIENT_ID_DOWNLOAD_IMAGE:
		str = params->GetSourceUrl();
		StringUtils::Utf8ToAnsi(str);
		DownImage(str);
		return true;
	case CLIENT_ID_COPY_URL:
		str = params->GetPageUrl();
		StringUtils::Utf8ToAnsi(str);
		CopyUrl(str);
		return true;
	default:  // Allow default handling, if any.
		return false;
	}
}

void WebView::ShowDevTools(CefRefPtr<CefBrowser> browser,const CefPoint& inspect_element_at) 
{
	CefWindowInfo windowInfo;
	CefBrowserSettings settings;

#if defined(OS_WIN)
	windowInfo.SetAsPopup(browser->GetHost()->GetWindowHandle(), "DevTools");
#endif
	browser->GetHost()->ShowDevTools(windowInfo, this, settings,
		inspect_element_at);
}

void WebView::CloseDevTools(CefRefPtr<CefBrowser> browser) 
{
	browser->GetHost()->CloseDevTools();
}

void WebView::OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, \
	const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback)
{
	CefString str;
	callback->Continue(str, true);
}

void WebView::OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, \
	CefRefPtr<CefDownloadItemCallback> callback)
{
	CefString cefPath = download_item->GetFullPath();
	string path;
	path = cefPath;
	StringUtils::Utf8ToAnsi(path);
	if (path.size())
	{
		DownMsg *psend = new DownMsg;
		psend->strPath = path;
		psend->idty = download_item->GetId();
		psend->iprogress = download_item->GetPercentComplete();
		psend->ispeed = download_item->GetCurrentSpeed() / 1024;
		psend->icurrent = download_item->GetReceivedBytes() / 1024;
		psend->itotal = download_item->GetTotalBytes() / 1024;
		PostMessage(hParent_, WM_DOWNLOAD, (WPARAM)psend, 1);
	}
}

void WebView::DownImage(std::string& strURL)
{
	std::string *strs = new string;
	*strs = strURL;
	PostMessage(hParent_, WM_DOWNLOAD, (WPARAM)strs, 0);
}

void WebView::CopyUrl(std::string& strURL)
{
	if (::OpenClipboard(NULL))
	{
		::EmptyClipboard();
		HGLOBAL clipbuffer;
		char *buffer;
		clipbuffer = ::GlobalAlloc(GMEM_DDESHARE, strURL.size() + 1);
		buffer = (char *)::GlobalLock(clipbuffer);
		strcpy(buffer, strURL.c_str());
		::GlobalUnlock(clipbuffer);
		::SetClipboardData(CF_TEXT, clipbuffer);
		::CloseClipboard();
	}
}

void WebView::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
{
}

void WebView::SetNeedJs(bool v)
{
	is_need_js_ = v;
}

void WebView::SetPenderSize(RECT& rc)
{
	pender_rc_ = rc;
}

void WebView::BrowserReSize(CefRefPtr<CefBrowser> browser)
{
	if (pender_rc_.left == pender_rc_.right || pender_rc_.top == pender_rc_.bottom)
	{
		return;
	}
	
	if (browser && browser->GetHost())
	{
		HWND hWeb = browser->GetHost()->GetWindowHandle();
		if (hWeb && ::IsWindow(hWeb))
		{
			::MoveWindow(hWeb, pender_rc_.left, pender_rc_.top,
				pender_rc_.right, pender_rc_.bottom, TRUE);
		}
	}
}