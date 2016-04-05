#pragma once

#include "include/cef_app.h"

class WebApp
	: public CefApp
	, public CefRenderProcessHandler
	, public CefBrowserProcessHandler
{
public:
	WebApp();
	virtual ~WebApp();

	// CefApp
public:
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler();
	virtual void OnBeforeCommandLineProcessing(
		const CefString& process_type,
		CefRefPtr<CefCommandLine> command_line);
	virtual void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar);
	virtual CefRefPtr<CefResourceBundleHandler> GetResourceBundleHandler();
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler();

	// CefBrowserProcessHandler
public:
	virtual void OnContextInitialized();
	virtual void OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info);

	// CefRenderProcessHandler
public:
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context);
	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context);
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message);
	virtual void OnWebKitInitialized();

public:
	// Set a JavaScript callback for the specified |message_name| and |browser_id|
	// combination. Will automatically be removed when the associated context is
	// released. Callbacks can also be set in JavaScript using the
	// app.setMessageCallback function.
	void SetMessageCallback(const std::string& message_name,
		int browser_id,
		CefRefPtr<CefV8Context> context,
		CefRefPtr<CefV8Value> function);

	// Removes the JavaScript callback for the specified |message_name| and
	// |browser_id| combination. Returns true if a callback was removed. Callbacks
	// can also be removed in JavaScript using the app.removeMessageCallback
	// function.
	bool RemoveMessageCallback(const std::string& message_name,
		int browser_id);

private:
	// Map of message callbacks.
	typedef std::map < std::pair<std::string, int>,
		std::pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value> > >
		CallbackMap;
	CallbackMap callback_map_;

	IMPLEMENT_REFCOUNTING(WebApp);
};
