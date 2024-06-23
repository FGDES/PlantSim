/* sink.h  - sink components */

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

#ifndef FAUDES_SINK_H
#define FAUDES_SINK_H

#include <QtGui>
#include "ffcomponent.h"


/*
************************************************
************************************************
  
sink component

************************************************
************************************************
*/


class FfSink : public FfComponent  {

Q_OBJECT

public:

  // constructor 
  FfSink(QObject* parent=0);

  // re-implement graphics item
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { ExitSlide = UserType+4};
  virtual int type(void) const { return ExitSlide;};

  // area covered by this component
  QList<QPoint> TestArea(const QPoint& pos,Orientation o);


protected:

  // update internal data structures 
  virtual void DoUpdateAll(void);
  virtual void DoAdvanceTime(qreal dt);
  virtual void DoReset(void);

  // initiate user command
  virtual void DoUserCommand(QString cmd);

  // dynamic parameters
  qreal mSlideVelocity;

  // my items
  FfRailItem* mAllRail;
  FfButtonItem* mButtonClear;
  FfButtonItem* mButtonClearAll;
  QPainterPath mBase;
  QLinearGradient mGrad;


private:


};



#endif
