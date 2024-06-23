/*
this is a hack just for fun ...
... I'ld like to see simulations 
in osx lion fullscreen mode?
*/

#include <QtGui>
#include "osxfullscreen.h"

void OsxFullScreen(QWidget* mainwin) {
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 1070
  NSView *nsview = (NSView *) mainwin->winId();
  NSWindow *nswindow = [nsview window];
  [nswindow setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
#endif
}
