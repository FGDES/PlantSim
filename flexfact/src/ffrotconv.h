/* rotconv.h  - rotary table incl conveyor belt component */

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


#ifndef FAUDES_ROTCONV_H
#define FAUDES_ROTCONV_H

#include <QtGui>
#include "ffcomponent.h"
#include "ffelements.h"



/*
************************************************
************************************************
  
rotary table

************************************************
************************************************
*/


class FfRotaryTable : public FfComponent  {

Q_OBJECT

public:

  // constructor 
  FfRotaryTable(QObject* parent=0);

  // reimplement transport
  void AdvanceWorkpiece(const qreal& dt, const QPointF& pos, const qreal& orient, 
			QPointF& npos, qreal& norient, bool& strict, bool& err);

  // re-implement graphics item
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { RotaryTable = UserType+5};
  virtual int type(void) const { return RotaryTable;};


protected:

  // update internal data structures 
  virtual void DoUpdateAll(void);
  virtual void DoAdvanceTime(qreal dt);
  virtual void DoWorkpiece(FfWorkpiece* wp);
  virtual void DoReset(void);

  // initiate user command
  virtual void DoUserCommand(QString cmd);

  // dynamic parameters
  qreal mBeltVelocity;
  qreal mTableVelocity;

  // dynamic state
  qreal mRTVelocity;
  qreal mBeltPos;
  qreal mAngle;
  qreal mAutoRotate;
  QTransform mTransDynamic;

  // my items
  FfBeltItem*   mBelt;
  FfRailItem*   mUpperRail;
  FfRailItem*   mLowerRail;
  FfButtonItem* mButtonToA;
  FfButtonItem* mButtonToB;
  FfButtonItem* mButtonStop;
  FfButtonItem* mButtonRotate;

private:


};



#endif
