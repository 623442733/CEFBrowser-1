#pragma once

#include "include/cef_base.h"
#include "include/cef_browser.h"
#include <set>
#include "WebView.h"

// Interface for process message delegates. Do not perform work in the
// RenderDelegate constructor.
class WebMsgHandler : public virtual CefBase {
public:
	// Called when a process message is received. Return true if the message was
	// handled and should not be passed on to other handlers.
	// ProcessMessageDelegates should check for unique message names to avoid
	// interfering with each other.
	virtual bool OnProcessMessageReceived(
		CefRefPtr<WebView> handler,
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) {
		return false;
	}
};

typedef std::set < CefRefPtr<WebMsgHandler> > WebMsgHandlerSet;

class WebMsgHandlerMgr
{
private:
	WebMsgHandlerMgr();
	~WebMsgHandlerMgr();

public:
	static WebMsgHandlerMgr* GetInstancePtr();
	static void ReleaseInstance();
	static bool IsInstanceNull();

public:
	bool OnProcessMessageReceived(
		CefRefPtr<WebView> handler,
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message);

private:
	void InitMsgHanlders();

private:
	static WebMsgHandlerMgr* instance_;
	WebMsgHandlerSet msghandles_;
};

