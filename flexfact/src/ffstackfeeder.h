/* component.h  - base calss for productionline components */

/*
FlexFact --- a configurable factory simulator
Copyright (C) 2011 Thomas Moor

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef FAUDES_STACKFEEDER_H
#define FAUDES_STACKFEEDER_H

#include <QtGui>
#include "ffcomponent.h"



/*
************************************************
************************************************
  
Stackfeeder component

************************************************
************************************************
*/


class FfStackfeeder : public FfComponent  {

Q_OBJECT

public:

  // constructor 
  FfStackfeeder(QObject* parent=0);

  // re-implement graphics item
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { StackFeeder = UserType+2};
  virtual int type(void) const { return StackFeeder;};

protected:

  // update internal data structures 
  virtual void DoUpdateAll(void);
  virtual void DoAdvanceTime(qreal dt);
  virtual void DoReset(void);

  // initiate user command
  virtual void DoUserCommand(QString cmd);

  // dynamic parameters
  qreal mFeedVelocity;

  // dynamic state
  qreal mFeedPos;
  bool  mIsFeeding;
  bool  mIsAutoFeeding;
  int   mStack;

  // my items
  FfRailItem* mAllRail;
  FfDigitItem* mStackDigit;
  FfPushBeltItem* mBelt;
  FfButtonItem* mButtonFeed;
  FfButtonItem* mButtonInsert;

private:


};



#endif
