// replimanApp.cpp

#ifndef __REPLIMANAPP_H__
#include "replimanApp.h"
#endif

#ifndef __REPLIMANWINDOW_H__
#include "replimanWindow.h"
#endif

replimanApp::replimanApp()
	: BApplication("application/x-vnd.rxc-repliman")
{
	replimanWindow *repliman_w = new replimanWindow();
	
	repliman_w->Show();
}
