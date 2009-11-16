/*
 * Based on kleds-0.8.0, written by Hans Matzen (hans@hansmatzen.de)
 */
//
// class for watch the keyboard indicators
//
extern "C" {
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
}
#include "xkbwatch.h"

xkbwatch::xkbwatch(char *displaystr)
{
	int major, minor, error_code, status_code;
	displaystring = displaystr;

	// connect to the XServer
	// get version of XKB
	major = XkbMajorVersion;
	minor = XkbMinorVersion;
	// open connection
	display = XkbOpenDisplay(displaystring, &eventnum,
	                         &error_code, &major, &minor, &status_code);

	// check for errors
	if (status_code != XkbOD_Success) {
		if (status_code == XkbOD_BadServerVersion)
			printf("XServer on %s uses wrong XKB version %d.%d",
			       displaystring, major, minor);

		if (status_code == XkbOD_BadLibraryVersion)
			printf("KLeds is compiled with XKB version %d.%d",
			       XkbMajorVersion, XkbMinorVersion);

		if (status_code == XkbOD_ConnectionRefused)
			printf("Can't open display %s",displaystring);

		if (status_code == XkbOD_NonXkbServer)
			printf("XKB extension not present");
	}

	// get the current led state
	if (XkbGetIndicatorState(display, XkbUseCoreKbd, &ledstate) != Success)
		printf("Can't get indicator state from Xkb");

	// remeber the current state
	lednumlock   = ledstate & NUM_LOCK_MASK;
	ledcapslock  = ledstate & CAPS_LOCK_MASK;
	ledscrollock = ledstate & SCROLL_LOCK_MASK;

	int curr_state = lednumlock | ledcapslock | ledscrollock;

	// configure XKB for only send events on indicators
	XkbSelectEvents(display, XkbUseCoreKbd,
	                XkbIndicatorStateNotifyMask,
	                XkbIndicatorStateNotifyMask);

	// send initial state
	indicators_changed(curr_state);
}

xkbwatch::~xkbwatch() {}

void xkbwatch::run() {
	int ind_state=0;

	// lets loop until we are killed
	while (1) {
		// wait for next X event
		ind_state = waitEvent();
		// send it to the gui
		indicators_changed(ind_state);
	}
}

int xkbwatch::waitEvent()
{
	// wait for Event (blocking)
	XNextEvent(display, &xkbevent.core);
	if (xkbevent.core.type == eventnum + XkbEventCode) {
		if (xkbevent.any.xkb_type == XkbIndicatorStateNotify) {
			// store new state
			lednumlock   = xkbevent.indicators.state & NUM_LOCK_MASK;
			ledcapslock  = xkbevent.indicators.state & CAPS_LOCK_MASK;
			ledscrollock = xkbevent.indicators.state & SCROLL_LOCK_MASK;
			ledstate = lednumlock | ledcapslock | ledscrollock;
		}
	}
	return ledstate;
}

void xkbwatch::indicators_changed(int ledstate)
{
	printf("indicators_changed: 0x%X\n", ledstate);
/*
	// register with dcop
	DCOPClient *dc=new DCOPClient();
	QString appid;
	if ( !dc->isRegistered() )
		appid=dc->registerAs("kledsd",false);


	// send new state via dcop
	QByteArray params;
	QDataStream d(params, IO_WriteOnly);
	d << (int) ledstate;

	if (!dc->send("kleds", "kleds", "state_changed(int)",params))
		printf("error sending new state\n");

	// de-register
	dc->detach();
*/
}

