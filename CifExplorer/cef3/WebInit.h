#pragma once

#ifndef __X_CEF3_WEBINIT_H__
#define __X_CEF3_WEBINIT_H__

class WebViewInit
{
public:
	WebViewInit(void* hInst, void* app, void* sbox);
	~WebViewInit();
	bool ExitSubProcess();
	void Init();
private:
	bool m_bInited;
	bool m_bSubProcess;
	void* _inst;
	void* _app;
	void* _box;
};

#endif  // __X_CEF3_WEBINIT_H__
