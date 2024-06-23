/* multiproc.h  - universal machine */

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


#ifndef FAUDES_MULTIMACH_H
#define FAUDES_MULTIMACH_H

#include <QtGui>
#include "ffcomponent.h"
#include "ffelements.h"



/*
************************************************
************************************************
  
processig machine

************************************************
************************************************
*/


class FfMultimach : public FfComponent  {

Q_OBJECT

public:

  // constructor 
  FfMultimach(QObject* parent=0);

  // paremeter
  const QString& ProcessA(void) { return mProcessA; };
  const QString& ProcessB(void) { return mProcessB; };
  const QString& ProcessC(void) { return mProcessC; };
  void SetProcessA(const QString& proc);
  void SetProcessB(const QString& proc);
  void SetProcessC(const QString& proc);

  // file io
  void AppendToElement(QDomDocument& doc, QDomElement& cele);
  void ExtractFromElement(QDomElement& elem);

  // reimplement integer shape aka area
  QList<QPoint> TestArea(const QPoint& pos,Orientation o);

  // re-implement editor factory
  virtual QWidget* NewEditor(void);

  // re-implement graphics item
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Multimach = UserType+9};
  virtual int type(void) const { return Multimach;};



protected:

  // configuration parameter
  QString mProcessA;
  QString mProcessB;
  QString mProcessC;
  QString mProcess;

  // update internal data structures 
  virtual void DoUpdateAll(void);
  virtual void DoAdvanceTime(qreal dt);
  virtual void DoWorkpiece(FfWorkpiece* wp);
  virtual void DoReset(void);

  // initiate user command
  virtual void DoUserCommand(QString cmd);

  // dynamic parameters
  qreal mBeltVelocity;
  qreal mPositionVelocity;
  qreal mProcessVelocity;
  qreal mToolselVelocity;

  // dynamic state
  qreal mBeltPos;
  int mAutoProcess;
  int mAutoTool;

  // processsed workpiece 
  QPointer<FfWorkpiece> pProcWorkpiece;

  // my items
  FfBeltItem*   mBelt;
  FfRailItem*   mUpperRail;
  FfRailItem*   mLowerRail;
  FfUnivprocItem*  mProcessor;
  FfButtonItem* mButtonToA;
  FfButtonItem* mButtonToB;
  FfButtonItem* mButtonStop;
  FfButtonItem* mButtonTool;
  FfButtonItem* mButtonProc;
  QPainterPath mBasePlate;
  QPainterPath mBaseCirc;
  QPainterPath mInnerCirc;

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

class FfMultimachEditor : public FfComponentEditor {

Q_OBJECT

public:

  // construct
  FfMultimachEditor(FfComponent* comp);

  // my component
  FfComponent* Component(void);

protected:
  

  // my gui items
  QLineEdit* mProcAEdit;
  QLineEdit* mProcBEdit;
  QLineEdit* mProcCEdit;

  // reimplement hooks
  virtual void DoUpdateFromComponent(FfComponent* comp);
  virtual void DoUpdateFromEditor(FfComponent* comp);

};



#endif
