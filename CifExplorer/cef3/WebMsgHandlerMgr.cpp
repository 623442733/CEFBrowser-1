
#include "WebMsgHandlerMgr.h"
#include "WebMsgHandle.h"

WebMsgHandlerMgr* WebMsgHandlerMgr::instance_ = NULL;

WebMsgHandlerMgr::WebMsgHandlerMgr()
{
	CreateWebMsgHandle(msghandles_);
}


WebMsgHandlerMgr::~WebMsgHandlerMgr()
{
}

WebMsgHandlerMgr* WebMsgHandlerMgr::GetInstancePtr()
{
	if (instance_ == NULL)
	{
		instance_ = new WebMsgHandlerMgr();
	}

	return instance_;
}

void WebMsgHandlerMgr::ReleaseInstance()
{
	if (instance_)
	{
		delete instance_;
	}
}

bool WebMsgHandlerMgr::IsInstanceNull()
{
	return (instance_ == NULL);
}

void WebMsgHandlerMgr::InitMsgHanlders()
{

}

bool WebMsgHandlerMgr::OnProcessMessageReceived(CefRefPtr<WebView> handler, CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	bool handled = false;

	// Execute delegate callbacks.
	WebMsgHandlerSet::iterator it = msghandles_.begin();
	for (; it != msghandles_.end() && !handled; ++it)
	{
		handled = (*it)->OnProcessMessageReceived(handler, browser, source_process,	message);
	}

	return handled;
}
