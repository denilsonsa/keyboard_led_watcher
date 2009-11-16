/* 
    kleds for KDE2/KDE3

    Copyright (C) 2001 Hans Matzen (hans@hansmatzen.de)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef xkbwatch_included
#define xkbwatch_included

#include <kapp.h> 
#include <dcopclient.h>

extern "C" {
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/XF86keysym.h>
#include <X11/extensions/XKBstr.h>
}

#define NUM_LOCK_MASK    0x00000002
#define CAPS_LOCK_MASK   0x00000001
#define SCROLL_LOCK_MASK 0x00000004

/* special keys from logitech keyboard */
#define KLEDS_KEY_STANDBY 223
#define KLEDS_KEY_MUTE    160
#define KLEDS_KEY_LOWER   174
#define KLEDS_KEY_RAISE   176
#define KLEDS_KEY_PLAY    162
#define KLEDS_KEY_STOP    164
#define KLEDS_KEY_PREV    144
#define KLEDS_KEY_NEXT    153
#define KLEDS_KEY_MAIL    236
#define KLEDS_KEY_SEARCH  229
#define KLEDS_KEY_START   230
#define KLEDS_KEY_HOME    178

/* class xkbwatch the indicator watcher class */

class xkbwatch
{
public:
  xkbwatch(char *displaystr);
  ~xkbwatch();

  int waitEvent();
  void run();

private:
  int lednumlock;
  int ledcapslock;
  int ledscrollock;
  unsigned int ledstate;
  Display  *display;
  char *displaystring;
  XkbEvent  xkbevent;
  int eventnum;

protected:
  void indicators_changed(int ledstate);
  void send_pid();
  void register_key(int tcode, KeySym newKS);
  void specialkey_pressed(int keycode);
};
#endif
