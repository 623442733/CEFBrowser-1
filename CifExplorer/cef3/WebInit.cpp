
#include "WebInit.h"
#include "WebMsgHandlerMgr.h"
#include "WebApp.h"

#ifdef ENABLE_WEB_CEF3

#include "include/cef_app.h"


WebViewInit::WebViewInit(void* hInst, void* app, void* sbox)
{
	m_bInited = false;
	_inst = hInst;
	_app = app;
	_box = sbox;
}

void WebViewInit::Init()
{
	CefMainArgs main_args((HINSTANCE)_inst);

	CefSettings settings;
	settings.ignore_certificate_errors = 1;
	settings.multi_threaded_message_loop = 1;
	settings.remote_debugging_port = 8088;
	//settings.single_process = 1;
	//CefString(&settings.browser_subprocess_path) = "xtweb.dll";
	settings.no_sandbox = 1;

	CefRefPtr<WebApp>* pApp = (CefRefPtr<WebApp>*)_app;

	m_bInited = CefInitialize(main_args, settings, pApp->get(), _box);

	WebMsgHandlerMgr::GetInstancePtr();
}

WebViewInit::~WebViewInit()
{
	if (m_bInited)
	{
		WebMsgHandlerMgr::ReleaseInstance();
		CefShutdown();
		m_bInited = false;
	}
}

bool WebViewInit::ExitSubProcess()
{
	return m_bSubProcess;
}

#else // NOT ENABLE_WEB_CEF3 

WebViewInit::WebViewInit(void* hInst)
{

}

WebViewInit::~WebViewInit()
{

}

bool WebViewInit::ExitSubProcess()
{
	return false;
}

void WebViewInit::Init()
{

}

#endif
