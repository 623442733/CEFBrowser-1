#pragma once

#ifndef __X_CEF3_WEBVIEW_H__
#define __X_CEF3_WEBVIEW_H__

#include "include/cef_client.h"
#include "include/cef_v8.h"
#include "include/base/cef_lock.h"
#include <list>
#include <set>
using namespace std;
#define WM_DOWNLOAD   WM_USER+10001      //下载文件消息
#define WM_POPWEBURL  WM_USER+10002      //弹出的web.url地址
#define WM_WBETITLE   WM_USER+10003      //打开web的title
#define WM_LOADWEBSUC WM_USER+10004      //加载web成功
#define WM_KEYDOWNMSG WM_USER+10005      //网页截获的按键消息

enum KeyDownMsg
{
	GoForward = 1,
	GoNext,
	GoCreate,
	GoClose
};

struct DownMsg
{
	int    idty;          //下载标识
	int    iprogress;     //下载进度
	int    ispeed;        //下载速度
	int    icurrent;      //已下载大小
	int    itotal;        //总大小
	string strPath;       //保存地址
};

typedef void(*WEBCALLBACK)(void* data, const char*s, int n1, int n2, const char* s1, const char* s2);

const UINT WM_CEF3_BROWSER_CREATED = (WM_USER + 27001);

typedef std::set < CefRefPtr<CefProcessMessage> > ProcessMsgSet;

class WebView
	: public CefClient
	, public CefDisplayHandler
	, public CefLifeSpanHandler
	, public CefLoadHandler
	//, public CefV8Handler
	, public CefKeyboardHandler
	, public CefRequestHandler
	, public CefContextMenuHandler
	,public CefDownloadHandler
	//, public CefDragHandler // unknow issue in release mode
{
public:
	WebView();
	virtual ~WebView();

	// CefClient methods:
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE{
		return this;
	}
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE{
		return this;
	}
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE{
		return this;
	}
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefDownloadHandler>		GetDownloadHandler() OVERRIDE { 
		return this; 
	}
	/*virtual CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE {
		return this;
	}*/
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) OVERRIDE;

	// CefDisplayHandler methods:
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;

	// CefLifeSpanHandler methods:
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& target_url,
		const CefString& target_frame_name,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings,
		bool* no_javascript_access) OVERRIDE;

	// CefLoadHandler methods:
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) OVERRIDE;
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int httpStatusCode);
	virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
		bool isLoading,
		bool canGoBack,
		bool canGoForward); 
	// CefV8Handler
	/*virtual bool Execute(const CefString& name,	CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval,
		CefString& exception) OVERRIDE;*/

	// CefKeyboardHandler
	virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
		const CefKeyEvent& event,
		CefEventHandle os_event,
		bool* is_keyboard_shortcut) OVERRIDE;

	virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
		const CefKeyEvent& event,
		CefEventHandle os_event) OVERRIDE; 

	// CefRequestHandler
	virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool is_redirect) OVERRIDE;
	/*virtual bool OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request) OVERRIDE;*/

	// CefContextMenuHandler
	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model) OVERRIDE;

	virtual bool OnContextMenuCommand(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		int command_id,
		EventFlags event_flags) OVERRIDE;

	//下载处理
	virtual void OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, \
		const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback);
	virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, \
		CefRefPtr<CefDownloadItemCallback> callback);


	bool CloseAllBrowsers(bool force_close);

	bool IsClosing() const { return is_closing_; }

	void OnWebNotify(const std::string& sEvent, int p1, int p2, const std::string& s1, const std::string& s2);


public:
	CefRefPtr<CefBrowser> GetBrowser();
	int GetBrowserId();
	void BrowserSetFocus(CefRefPtr<CefBrowser> browser);
	void BrowserReSize(CefRefPtr<CefBrowser> browser);

	//开发者工具
	void ShowDevTools(CefRefPtr<CefBrowser> browser,
		const CefPoint& inspect_element_at);
	void CloseDevTools(CefRefPtr<CefBrowser> browser);

	//下载图片
	void DownImage(std::string& strURL);

	//复制网址
	void CopyUrl(std::string& strURL);
	// COWORK 
public:
	HWND GetWebHwnd();
	HRESULT CreateWebHwnd(HWND hParent, RECT& rect, const std::string &sURL);	
	bool Navigate(const std::string& url);
	void SetHostNotify(const std::string& sEvent, int p1, int p2, const std::string& s1, const std::string& s2);
	void Refresh(); 
	void SetCallback(WEBCALLBACK func, void* data);
	void SetNeedJs(bool v);
	void SetPenderSize(RECT& rc);

private:
	// List of existing browser windows. Only accessed on the CEF UI thread.
	typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
	BrowserList browser_list_;
	CefRefPtr<CefBrowser> m_Browser;
	int browser_id_;
	bool is_closing_;
	bool is_created_;
	bool do_close_;
	bool is_need_js_;
	RECT pender_rc_;
	HWND hParent_;
	ProcessMsgSet m_pendingJs;
	WEBCALLBACK cb_func_;
	void* cb_data_;
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(WebView);
	// Include the default locking implementation. cef1650
	//IMPLEMENT_LOCKING(WebView);
	mutable base::Lock lock_;
};

#endif  // __X_CEF3_WEBVIEW_H__
