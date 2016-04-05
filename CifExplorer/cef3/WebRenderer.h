#pragma once

#ifndef __X_CEF3_WEBRENDERER_H__
#define __X_CEF3_WEBRENDERER_H__

#ifdef ENABLE_WEB_CEF3
#include "include/cef_base.h"
#endif

namespace WebRenderer {

// Message sent when the focused node changes.
extern const char kFocusedNodeChangedMessage[];

}  // namespace WebRenderer

#endif  // __X_CEF3_WEBRENDERER_H__
