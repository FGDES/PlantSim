/*
this is a hack just for fun ...
... I'ld like to see simulations 
in osx lion fullscreen mode?
*/

#include <QtGui>
#include "osxfullscreen.h"

void OsxFullScreen(QWidget* mainwin) {
  NSView *nsview = (NSView *) mainwin->winId();
  NSWindow *nswindow = [nsview window];
  [nswindow setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
}
