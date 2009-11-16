/*
 * Based on kleds-0.8.0, written by Hans Matzen (hans@hansmatzen.de)
 */
#ifndef xkbwatch_included
#define xkbwatch_included

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
};
#endif
