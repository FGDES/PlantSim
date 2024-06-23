/* conveyor.h  - conveyor belt components */

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


#ifndef FAUDES_CONVEYOR_H
#define FAUDES_CONVEYOR_H

#include <QtGui>
#include "ffcomponent.h"
#include "ffelements.h"



/*
************************************************
************************************************
  
conveyor belt component

************************************************
************************************************
*/


class FfConveyor : public FfComponent  {

Q_OBJECT

public:

  // constructor 
  FfConveyor(QObject* parent=0);

  // paremeter
  int Length(void) { return mLength; };
  void SetLength(int nlen);

  // file io
  void AppendToElement(QDomDocument& doc, QDomElement& cele);
  void ExtractFromElement(QDomElement& elem);

  // reimplement integer shape aka area
  QList<QPoint> TestArea(const QPoint& pos,Orientation o);
  QList<QPoint> TestArea(const QPoint& pos,Orientation o,int len);

  // re-implement editor factory
  virtual QWidget* NewEditor(void);

  // re-implement graphics item
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { ConveyorBelt = UserType+3};
  virtual int type(void) const { return ConveyorBelt;};



protected:

  // configuration parameter
  int mLength;

  // update internal data structures 
  virtual void DoUpdateAll(void);
  virtual void DoAdvanceTime(qreal dt);
  virtual void DoWorkpiece(FfWorkpiece* wp);
  virtual void DoReset(void);

  // initiate user command
  virtual void DoUserCommand(QString cmd);

  // dynamic parameters
  qreal mBeltVelocity;

  // dynamic state
  qreal mBeltPos;

  // my items
  FfBeltItem*   mBelt;
  FfRailItem*   mUpperRail;
  FfRailItem*   mLowerRail;
  FfButtonItem* mButtonToA;
  FfButtonItem* mButtonToB;
  FfButtonItem* mButtonStop;

private:


};


/*
*********************************************************
*********************************************************
*********************************************************

FfParameterEditor

*********************************************************
*********************************************************
*********************************************************
*/

class FfConveyorEditor : public FfComponentEditor {

Q_OBJECT

public:

  // construct
  FfConveyorEditor(FfComponent* comp);

  // my component
  FfComponent* Component(void);

protected:
  

  // my gui items
  QLineEdit* mLengthEdit;

  // reimplement hooks
  virtual void DoUpdateFromComponent(FfComponent* comp);
  virtual void DoUpdateFromEditor(FfComponent* comp);

};



#endif
