/* process.h  - processing machine */

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


#ifndef FAUDES_PROCESS_H
#define FAUDES_PROCESS_H

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


class FfProcess : public FfComponent  {

Q_OBJECT

public:

  // constructor 
  FfProcess(QObject* parent=0);

  // paremeter
  const QString& Process(void) { return mProcess; };
  void SetProcess(const QString& proc);

  // file io
  void AppendToElement(QDomDocument& doc, QDomElement& cele);
  void ExtractFromElement(QDomElement& elem);

  // reimplement integer shape aka area
  QList<QPoint> TestArea(const QPoint& pos,Orientation o);

  // re-implement editor factory
  virtual QWidget* NewEditor(void);

  // re-implement graphics item
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Machine = UserType+8};
  virtual int type(void) const { return Machine;};



protected:

  // configuration parameter
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

  // dynamic state
  qreal mBeltPos;
  int mAutoProcess;

  // processsed workpiece 
  QPointer<FfWorkpiece> pProcWorkpiece;

  // my items
  FfBeltItem*   mBelt;
  FfRailItem*   mUpperRail;
  FfRailItem*   mLowerRail;
  FfProcessorItem*  mProcessor;
  FfButtonItem* mButtonToA;
  FfButtonItem* mButtonToB;
  FfButtonItem* mButtonStop;
  FfButtonItem* mButtonProc;
  QPainterPath mBasePlate;
  QPainterPath mBaseCirc;

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

class FfProcessEditor : public FfComponentEditor {

Q_OBJECT

public:

  // construct
  FfProcessEditor(FfComponent* comp);

  // my component
  FfComponent* Component(void);

protected:
  

  // my gui items
  QLineEdit* mTypeEdit;

  // reimplement hooks
  virtual void DoUpdateFromComponent(FfComponent* comp);
  virtual void DoUpdateFromEditor(FfComponent* comp);

};



#endif
