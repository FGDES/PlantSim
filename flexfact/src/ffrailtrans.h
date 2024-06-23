/* railtrans.h  - rail transport system */

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


#ifndef FAUDES_RAILTRANS_H
#define FAUDES_RAILTRANS_H

#include <QtGui>
#include "ffcomponent.h"
#include "ffelements.h"



/*
************************************************
************************************************
  
rail trsanpost system

************************************************
************************************************
*/


class FfRailTransport : public FfComponent  {

Q_OBJECT

public:

  // constructor 
  FfRailTransport(QObject* parent=0);

  // paremeter
  int Length(void) { return mLength; };
  void SetLength(int nlen);

  // file io
  void AppendToElement(QDomDocument& doc, QDomElement& cele);
  void ExtractFromElement(QDomElement& elem);

  // reimplement integer shape aka area
  QList<QPoint> TestArea(const QPoint& pos,Orientation o);
  QList<QPoint> TestArea(const QPoint& pos,Orientation o,int len);

  // propagate workpiece
  virtual void AdvanceWorkpiece(const qreal& dt, const QPointF& pos, const qreal& orient, 
				QPointF& npos, qreal& norient, bool& strictmove, bool& err);

  // re-implement editor factory
  virtual QWidget* NewEditor(void);

  // re-implement graphics item
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { RailTransport = UserType+7};
  virtual int type(void) const { return RailTransport;};



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
  qreal mCartVelocity;

  // dynamic state
  qreal mBeltPos;

  // my items
  QPainterPath mRails;
  QList<FfConveyorItem*> mConveyors;



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

class FfRailTransportEditor : public FfComponentEditor {

Q_OBJECT

public:

  // construct
  FfRailTransportEditor(FfComponent* comp);

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
