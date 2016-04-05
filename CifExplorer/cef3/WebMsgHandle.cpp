
#include "WebMsgHandle.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "WebView.h"
#include <algorithm>
#include <string>
#include "public/StringUtils.h"


const char* kmsgOnWebNotify = "OnWebNotify";

// Handle messages in the browser process.
class SimpleMsgHandler : public WebMsgHandler
{
public:
	 SimpleMsgHandler() {}

public:
	virtual bool OnProcessMessageReceived(
		CefRefPtr<WebView> handler,
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) OVERRIDE
	{
		std::string message_name = message->GetName();
		if (message_name == kmsgOnWebNotify)
		{
			CefRefPtr<CefListValue> args = message->GetArgumentList();
			if (args->GetSize() >= 5)
			{
				std::string sid, s1, s2;
				int n1 = 0, n2 = 0;
				if (args->GetType(0) == VTYPE_STRING)
				{
					sid = args->GetString(0);
					StringUtils::Utf8ToAnsi(sid);
				}
				if (args->GetType(1) == VTYPE_INT)
				{
					n1 = args->GetInt(1);
				}
				if (args->GetType(2) == VTYPE_INT)
				{
					n2 = args->GetInt(2);
				}
				if (args->GetType(3) == VTYPE_STRING)
				{
					s1 = args->GetString(3);
					StringUtils::Utf8ToAnsi(s1);
				}
				if (args->GetType(4) == VTYPE_STRING)
				{
					s2 = args->GetString(4);
					StringUtils::Utf8ToAnsi(s2);
				}

				if (!sid.empty())
				{
					handler->OnWebNotify(sid, n1, n2, s1, s2);
					return true;
				}				
			}			
		}
		
		return false;
	}
	
	IMPLEMENT_REFCOUNTING(SimpleMsgHandler);
};

void CreateWebMsgHandle(WebMsgHandlerSet& handles)
{
	handles.insert(new SimpleMsgHandler());
}
