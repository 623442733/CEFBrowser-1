#include "WebRenderer.h"

#ifdef ENABLE_WEB_CEF3
#include "include/cef_dom.h"
//#include "include/wrapper/cef_message_router.h"
#include "util.h"
#endif

namespace WebRenderer
{
	const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";
}
