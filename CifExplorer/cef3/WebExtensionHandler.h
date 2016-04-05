#pragma once

#include "include/cef_v8.h"

class WebApp;

class WebExtensionHandler : public CefV8Handler
{
public:
	explicit WebExtensionHandler(CefRefPtr<WebApp> app);
	~WebExtensionHandler();

public:
	virtual bool Execute(const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception);

private:
	CefRefPtr<WebApp> webapp_;

	IMPLEMENT_REFCOUNTING(WebExtensionHandler);
};

