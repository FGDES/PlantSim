/* distributor.h  - wp distribution unit */

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


#ifndef FAUDES_DISTRIBUTOR_H
#define FAUDES_DISTRIBUTOR_H

#include <QtGui>
#include "ffcomponent.h"
#include "ffelements.h"



/*
************************************************
************************************************
  
wp distribution unit

************************************************
************************************************
*/


class FfDistributor : public FfComponent  {

Q_OBJECT

public:

  // constructor 
  FfDistributor(QObject* parent=0);

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
  enum { Distributor = UserType+6};
  virtual int type(void) const { return Distributor;};

  // propagate workpiece
  virtual void AdvanceWorkpiece(const qreal& dt, const QPointF& pos, const qreal& orient, 
				QPointF& npos, qreal& norient, bool& strictmove, bool& err);


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
  qreal mPusherVelocity;

  // dynamic state
  qreal mBeltPos;

  // pushers
  QList<FfPusherItem*> mPushers;

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

class FfDistributorEditor : public FfComponentEditor {

Q_OBJECT

public:

  // construct
  FfDistributorEditor(FfComponent* comp);

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
