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

		// UGLY DEBUG BEGIN
		XkbStateRec state;
		XkbGetState(display, XkbUseCoreKbd, &state);
		// X.h defines ShiftMask, LockMask, ControlMask, Mod1Mask, ...
		// See also:
		// http://www.x.org/releases/X11R7.6/doc/libX11/specs/XKB/xkblib.html#tracking_keyboard_state
		// http://stackoverflow.com/questions/1859049/check-if-key-is-pressed-using-python-a-daemon-in-the-background
		printf(
			"group: 0x%X "
			"locked_group: 0x%X "
			"base_group: 0x%X "
			"latched_group: 0x%X\n"
			"mods: 0x%X "
			"base_mods: 0x%X "
			"latched_mods: 0x%X "
			"locked_mods: 0x%X "
			"compat_state: 0x%X "
			"grab_mods: 0x%X "
			"compat_grab_mods: 0x%X "
			"lookup_mods: 0x%X "
			"compat_lookup_mods: 0x%X "
			"ptr_buttons: 0x%X\n"
		,
			state.group,
			state.locked_group,
			state.base_group,
			state.latched_group,
			state.mods,
			state.base_mods,
			state.latched_mods,
			state.locked_mods,
			state.compat_state,
			state.grab_mods,
			state.compat_grab_mods,
			state.lookup_mods,
			state.compat_lookup_mods,
			state.ptr_buttons
		);
		// UGLY DEBUG END

		if (xkbevent.any.xkb_type == XkbIndicatorStateNotify) {
			printf("changed: 0x%X state: 0x%X\n", xkbevent.indicators.changed, xkbevent.indicators.state);
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

