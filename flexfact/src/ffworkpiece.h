/* workpiece.h  - workpiece item  */

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


#ifndef FAUDES_FFWORKPIECE_H
#define FAUDES_FFWORKPIECE_H

#include <QtGui>
#include "ffstyle.h"
#include "ffelements.h"

// forward
class FfComponent;
class FfScene;

/*
************************************************
************************************************
  
FfWorkpiece

************************************************
************************************************
*/


class FfWorkpiece : public QObject, public QGraphicsItem {

Q_OBJECT
//Q_INTERFACES(QGraphicsItem)

public:

  // construct/destruct
  FfWorkpiece(QObject* parent=0);
  virtual ~FfWorkpiece(void);

  // figure scene
  FfScene* Scene(void);

  // set/get my position/orientation
  void SetPosition(const QPointF& pos, const qreal& orient);
  void SetPosition(const QPointF& pos);

  // set/get Name
  void SetName(const QString& name);
  const QString& Name(void); 

  // figure my components
  FfComponent* Component(void);
  const QSet<FfComponent*>& TransComponents(void);

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Workpiece = UserType+50};
  virtual int type(void) const { return Workpiece;};

  // enable simulation
  virtual void Run(bool run);
  void SetError(bool err);

  // advance time
  void AdvanceTimeA(qreal dt);
  void AdvanceTimeB(qreal dt);
  void AdvanceTimeC(qreal dt);

  // record process
  void Process(const QString& proc); 

  // reset counter
  static void ResetId(void);

signals:

  void NotifyError(void);

protected:

  // recognize scene change
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

  // track mouse
  virtual void mousePressEvent(QGraphicsSceneMouseEvent * ev);
  virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);


  // name
  QString mName;

  // counter
  static int mLastId;

  // position
  QPointF mPos;
  qreal mOrient;
  QPointF mNorm;
  QPointF mOrth;

  // shape 
  QRectF mOuterRect;
  QPainterPath  mCoreShape;
  QPainterPath  mOuterShape;

  // nominal geometry
  QPointF mNominalNorm;
  QPointF mNominalOrth;
  QPainterPath  mNominalCoreShape;
  QPainterPath  mNominalOuterShape;
  QRectF mNominalOuterRect;

  // responsible components
  FfComponent* mComponent;
  QSet<FfComponent*> mTransComponents;

  // simulation enabled
  bool mRunning;
  bool mError;

  // advance time state vars
  QPointF mNextPos;
  qreal mNextOrient;
  QPainterPath mNextShape;
  bool mNextStrict;
  bool mBlocked;

  // process history
  QList<QString> mHistory;

  // companion
  FfErrIndicatorItem* mErrIndicator;
  FfButtonItem* mErrButton;

private:


};




#endif
