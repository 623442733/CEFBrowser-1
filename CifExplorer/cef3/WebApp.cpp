
#include "WebApp.h"
#include "WebExtensionHandler.h"
#include "setlist.h"

WebApp::WebApp()
{
}


WebApp::~WebApp()
{
}

CefRefPtr<CefRenderProcessHandler> WebApp::GetRenderProcessHandler()
{
	return this;
}

void WebApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{

}

void WebApp::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
{

}

CefRefPtr<CefResourceBundleHandler> WebApp::GetResourceBundleHandler()
{
	return NULL;
}

CefRefPtr<CefBrowserProcessHandler> WebApp::GetBrowserProcessHandler()
{
	return this;
}

void WebApp::OnContextInitialized()
{

}

void WebApp::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info)
{

}

void WebApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{

}

void WebApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	// Remove any JavaScript callbacks registered for the context that has been
	// released.
	if (!callback_map_.empty()) {
		CallbackMap::iterator it = callback_map_.begin();
		for (; it != callback_map_.end();) {
			if (it->second.first->IsSame(context))
				callback_map_.erase(it++);
			else
				++it;
		}
	}
}

bool WebApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	//ASSERT(source_process == PID_BROWSER);

	bool handled = false;

	// Execute the registered JavaScript callback if any.
	if (!callback_map_.empty()) {
		CefString message_name = message->GetName();
		CallbackMap::const_iterator it = callback_map_.find(
			std::make_pair(message_name.ToString(),
			browser->GetIdentifier()));
		if (it != callback_map_.end()) {
			// Keep a local reference to the objects. The callback may remove itself
			// from the callback map.
			CefRefPtr<CefV8Context> context = it->second.first;
			CefRefPtr<CefV8Value> callback = it->second.second;

			// Enter the context.
			context->Enter();

			CefV8ValueList arguments;

			// First argument is the message name.
			arguments.push_back(CefV8Value::CreateString(message_name));

			// Second argument is the list of message arguments.
			CefRefPtr<CefListValue> list = message->GetArgumentList();
			CefRefPtr<CefV8Value> args =
				CefV8Value::CreateArray(static_cast<int>(list->GetSize()));
			SetList(list, args);
			arguments.push_back(args);

			// Execute the callback.
			CefRefPtr<CefV8Value> retval = callback->ExecuteFunction(NULL, arguments);
			if (retval.get()) {
				if (retval->IsBool())
					handled = retval->GetBoolValue();
			}

			// Exit the context.
			context->Exit();
		}
	}

	return handled;
}

void WebApp::OnWebKitInitialized()
{
	// Register the client_app extension.
	std::string app_code =
		"var app;"
		"if (!app)"
		"  app = {};"
		"(function() {"
		"  app.sendMessage = function(name, arguments) {"
		"    native function sendMessage();"
		"    return sendMessage(name, arguments);"
		"  };"
		"  app.setMessageCallback = function(name, callback) {"
		"    native function setMessageCallback();"
		"    return setMessageCallback(name, callback);"
		"  };"
		"  app.removeMessageCallback = function(name) {"
		"    native function removeMessageCallback();"
		"    return removeMessageCallback(name);"
		"  };"
	/*	"  var i = 0;"
		"  app.test22 = function() {"
		"    i++; "
		"    return i; "
		"  };"*/
		//"  var external;"
		//"  if (!external) external={};"
		//"  external = {};"
		//"  external.OnWebNotify = function(id, n1, n2, s1, s2){app.sendMessage('OnWebNotify', [id,n1,n2,s1,s2]);};"
		//"  window1 = {};"
		//"  window.navigate = function(s){ location.href = s;};"		
		//" function cb(name, args){OnHostNotify(args[0], args[1], args[2], args[3], args[4]);};"
		//
		//"  app.setMessageCallback('OnHostNotify', cb);"
		"})();"
		//"  function OnHostNotify(id, n1, n2, s1, s2){};"
		//"  app.setMessageCallback('OnHostNotify', function(name, args){"
		//"    OnHostNotify(args[0], args[1], args[2], args[3], args[4]);"
		//"  };"
		;

	if (!CefRegisterExtension("v8/app", app_code, new WebExtensionHandler(this)))
	{
		::MessageBox(NULL, L"sss", L"sss", 32 + 1);
	}
}

void WebApp::SetMessageCallback(const std::string& message_name, int browser_id, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Value> function)
{
	//ASSERT(CefCurrentlyOn(TID_RENDERER));

	callback_map_.insert(
		std::make_pair(std::make_pair(message_name, browser_id),
		std::make_pair(context, function)));
}

bool WebApp::RemoveMessageCallback(const std::string& message_name, int browser_id)
{
	//ASSERT(CefCurrentlyOn(TID_RENDERER));

	CallbackMap::iterator it =
		callback_map_.find(std::make_pair(message_name, browser_id));
	if (it != callback_map_.end()) {
		callback_map_.erase(it);
		return true;
	}

	return false;
}



