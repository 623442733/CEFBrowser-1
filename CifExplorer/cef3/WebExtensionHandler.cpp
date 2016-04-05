
#include "WebExtensionHandler.h"
#include "WebApp.h"
#include "setlist.h"

WebExtensionHandler::WebExtensionHandler(CefRefPtr<WebApp> app)
	: webapp_(app)
{

}

WebExtensionHandler::~WebExtensionHandler()
{
}

bool WebExtensionHandler::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)
{
	bool handled = false;

	if (name == "sendMessage") {
		// Send a message to the browser process.
		if ((arguments.size() == 1 || arguments.size() == 2) &&
			arguments[0]->IsString()) {
			CefRefPtr<CefBrowser> browser =
				CefV8Context::GetCurrentContext()->GetBrowser();
			//ASSERT(browser.get());

			CefString name = arguments[0]->GetStringValue();
			if (!name.empty()) {
				CefRefPtr<CefProcessMessage> message =
					CefProcessMessage::Create(name);

				// Translate the arguments, if any.
				if (arguments.size() == 2 && arguments[1]->IsArray())
					SetList(arguments[1], message->GetArgumentList());

				browser->SendProcessMessage(PID_BROWSER, message);
				handled = true;
			}
		}
	}
	else if (name == "setMessageCallback") {
		// Set a message callback.
		if (arguments.size() == 2 && arguments[0]->IsString() &&
			arguments[1]->IsFunction()) {
			std::string name = arguments[0]->GetStringValue();
			CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
			int browser_id = context->GetBrowser()->GetIdentifier();
			webapp_->SetMessageCallback(name, browser_id, context,
				arguments[1]);
			handled = true;
		}
	}
	else if (name == "removeMessageCallback") {
		// Remove a message callback.
		if (arguments.size() == 1 && arguments[0]->IsString()) {
			std::string name = arguments[0]->GetStringValue();
			CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
			int browser_id = context->GetBrowser()->GetIdentifier();
			bool removed = webapp_->RemoveMessageCallback(name, browser_id);
			retval = CefV8Value::CreateBool(removed);
			handled = true;
		}
	}

	if (!handled)
		exception = "Invalid method arguments";

	return true;
}
