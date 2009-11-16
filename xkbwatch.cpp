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
//#include <dcopclient.h>
//#include <klocale.h>
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

	// register Keys with X for logitech keys

	// This is buggy... Don't know why.
	/*register_key(KLEDS_KEY_STANDBY,XF86XK_Standby);
	register_key(KLEDS_KEY_MUTE   ,XF86XK_AudioMute);
	register_key(KLEDS_KEY_LOWER  ,XF86XK_AudioLowerVolume);
	register_key(KLEDS_KEY_RAISE  ,XF86XK_AudioRaiseVolume);
	register_key(KLEDS_KEY_PLAY   ,XF86XK_AudioPlay);
	register_key(KLEDS_KEY_STOP   ,XF86XK_AudioStop);
	register_key(KLEDS_KEY_PREV   ,XF86XK_AudioPrev);
	register_key(KLEDS_KEY_NEXT   ,XF86XK_AudioNext);
	register_key(KLEDS_KEY_MAIL   ,XF86XK_Mail);
	register_key(KLEDS_KEY_SEARCH ,XF86XK_Search);
	register_key(KLEDS_KEY_START  ,XF86XK_Start);
	register_key(KLEDS_KEY_HOME   ,XF86XK_HomePage);*/

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
	                XkbActionMessageMask,
	                XkbActionMessageMask);
	XkbSelectEvents(display, XkbUseCoreKbd,
	                XkbIndicatorStateNotifyMask,
	                XkbIndicatorStateNotifyMask);

	// send pid
	send_pid();

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
	int keycode = 0;

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
		if (xkbevent.any.xkb_type == XkbActionMessage) {
			keycode = xkbevent.message.keycode;
			specialkey_pressed(keycode);
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

void xkbwatch::send_pid()
{
	printf("send_pid() called\n");
/*
	// register with dcop
	DCOPClient *dc=new DCOPClient();
	QString appid;
	if ( !dc->isRegistered() )
		appid=dc->registerAs("kledsd",false);

	// get pid
	uint pid=getpid();

	// send new pid via dcop
	QByteArray params;
	QDataStream d(params, IO_WriteOnly);
	d << (int) pid;

	if (!dc->send("kleds", "kleds", "get_pid(int)",params))
		printf("error sending pid\n");

	// de-register
	dc->detach();
*/
}

void xkbwatch::register_key(int tcode, KeySym newKS)
{
	// this piece of code is from the hotkeys program
	// it is great and works perfectly, I modified it a bit to
	// fit into my code
	// HOTKEYS - use keys on your multimedia keyboard to control your computer
	// Copyright (C) 2000-2002  Anthony Y P Wong <ypwong@ypwong.org>
	// published under the GPL

	XkbMessageAction    xma;
	XkbMapChangesRec    mapChangeRec;
	int                 types[1];
	XkbDescPtr  xkb     = NULL;

	// Construct the Message Action struct
	xma.type = XkbSA_ActionMessage;
	xma.flags = XkbSA_MessageOnPress;
	strcpy((char *) xma.message," ");

	xkb = XkbGetMap(display, XkbAllMapComponentsMask, XkbUseCoreKbd);
	if (!xkb)
		printf("XkbGetMap failed\n");

	// Assign a group to the key code
	types[0] = XkbOneLevelIndex;
	if ( XkbChangeTypesOfKey(xkb, tcode, 1, XkbGroup1Mask, types, NULL)
	     != Success )
		printf("XkbChangeTypesOfKey failed");


	// Change their Keysyms
	if ( XkbResizeKeySyms( xkb, tcode, 1 ) == NULL )
		printf("resize keysym failed\n");

	// Assign a new keysym to the key code.
	*XkbKeySymsPtr(xkb,tcode) = newKS;

	// Add one key action to it
	if ( XkbResizeKeyActions( xkb, tcode, 1 ) == NULL )
		printf("resize key action failed\n");

	// Commit the change back to the server two times
	memset(&mapChangeRec, 0, sizeof(mapChangeRec));
	mapChangeRec.changed = XkbKeySymsMask | XkbKeyTypesMask;
	mapChangeRec.first_key_sym = tcode;
	mapChangeRec.num_key_syms = 1;
	mapChangeRec.first_type = 0;
	mapChangeRec.num_types = xkb->map->num_types;
	if (! XkbChangeMap(display, xkb, &mapChangeRec) )
		printf("map changed failed\n");

	memset(&mapChangeRec, 0, sizeof(mapChangeRec));
	mapChangeRec.changed = XkbKeySymsMask | XkbKeyTypesMask;
	mapChangeRec.first_key_sym = tcode;
	mapChangeRec.num_key_syms = 1;
	mapChangeRec.first_type = 0;
	mapChangeRec.num_types = xkb->map->num_types;
	if (! XkbChangeMap(display, xkb, &mapChangeRec) )
		printf("map changed failed\n");


	//* Assign the Message Action to the key code
	(&(xkb->server->acts[ xkb->server->key_acts[tcode] ]))[0].msg=xma;

	/* Commit the change back to the server. Yeah we need to do it
	 * here instead of in commit XKBChanges(). Strange, eh?  But
	 * you just can't, I wonder what the fsck X is doing.  I get
	 * this just by lots of trial-and-error and many nights of no
	 * sleeping to trace X with gdb. */
	memset(&mapChangeRec, 0, sizeof(mapChangeRec));
	mapChangeRec.changed = XkbKeyActionsMask;
	mapChangeRec.first_key_act = tcode;
	mapChangeRec.num_key_acts = 1;
	if (! XkbChangeMap(display, xkb, &mapChangeRec) )
		printf("map changed failed\n");
}

void xkbwatch::specialkey_pressed(int keycode)
{
	printf("specialkey_pressed: %d\n", keycode);
/*
	// register with dcop
	DCOPClient *dc=new DCOPClient();
	QString appid;
	if ( !dc->isRegistered() )
		appid=dc->registerAs("kledsd",false);

	// send new state via dcop
	QByteArray params;
	QDataStream d(params, IO_WriteOnly);
	d << (int) keycode;

	if (!dc->send("kleds", "kleds", "specialkey_pressed(int)",params))
		printf("error sending keycode\n");

	// de-register
	dc->detach();
*/
}






