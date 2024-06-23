/* component.h  - base class for productionline components */

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

#ifndef FAUDES_COMPONENT_H
#define FAUDES_COMPONENT_H

#include <QtGui>
#include <QtXml>
#include "ffstyle.h"
#include "ffelements.h"
#include "ffsignal.h"
#include "fffault.h"

// forward 
class FfScene;
class FfWorkpiece;
class FfComponentEditor;

/*
************************************************
************************************************
  
FfComponent base class

We avoid QGrahicsObject since it was introduced
with Qt 4.6 and we still use Qt 4.4 for deployment.

************************************************
************************************************
*/


class FfComponent : public QObject, public QGraphicsItem {

Q_OBJECT
//Q_INTERFACES(QGraphicsItem)

public:

  // constructor 
  // components must be -- in this order --
  // 1) constructed (instantiates items)
  // 2) named and configured by all other parameters, incl. orientation and position
  // 3) updated by Update() (DoUpdateAll to configure graphics items and
  //      calls virtual DoReset for sim. vars                 
  // 4) placed in the scene (by the scene method AddComponent)
  FfComponent(QObject* parent=0);

  // take care about overlay/controls
  virtual ~FfComponent(void);

  // primitive rtti/factory mechanism
  // you must "register" your derived type manually in the implementation
  // of this two functions
  static QString Type(const FfComponent* comp);
  static QStringList AvailableTypes(void);
  static FfComponent* New(const QString& type);

  // figure scene
  FfScene* Scene(void);

  // orientation enum
  typedef enum {Ostd, O90, O180, O270} Orientation;

  // set/get my position/orientation
  void SetOrient(Orientation o);
  void SetPosition(const QPoint& pos);
  const QPoint& Position(void) { return mPos;};
  Orientation Orient(void) { return mOrient;};
  const QList<QPoint>& Area(void);

  // test the occupied area without placing the component (defaults to one square)
  virtual QList<QPoint> TestArea(const QPoint& pos,Orientation o);
  // test actual position during drag
  bool GoodPosition();

  // do the update from position, orientation and all other parameters
  void Update(void);

  // set/get Name
  void SetName(const QString& name);
  const QString& Name(void); 

  // file io
  QDomElement ToElement(QDomDocument& doc);
  static FfComponent* FromElement(QDomElement& elem);

  // transport geometry
  const QLineF& Rail(void);
  const QPointF& Velocity(void);
  const QPainterPath&  Block(void);
  void RefuseAdvance(void) { mRefuseAdvance=true; };

  // reset to initial state
  void Reset(void);

  // signal interface (wrt rel. address)
  void SetSignal(int reladdress, bool val);
  bool Signal(int reladdress);
  int  ImageSize(void);
  QString SignalName(int reladdress);
  QString SignalShortName(int reladdress);
  bool IsActuator(int reladdress);
  bool IsSensor(int reladdress);
  FfSignal* SignalCfg(int i);


  // access fault (e.g. from component editor)
  FfFault* FaultCfg(int i);


  // add/take a workpiece to/from scene
  void AddWorkpiece(const QPointF& pos, Orientation o=Ostd);

  // receive report from a workpiece 
  void Workpiece(FfWorkpiece* wp);

  // propagate workpiece
  virtual void AdvanceWorkpiece(const qreal& dt, const QPointF& pos, const qreal& orient, 
				QPointF& npos, qreal& norient, bool& strictmove, bool& err);

  // get new simulation context menu
  virtual QMenu* NewSimulationContextMenu(void);

  // get new parameter editor
  virtual QWidget* NewEditor(void);

  // controls
  FfSimControlsItem* SimControls(void) { return mSimControls; } ; 
  QGraphicsItem*   SimOverlay(void) { return mSimOverlay; } ; 

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Base = UserType+1};
  virtual int type(void) const { return Base;};


  // enable simulation
  virtual void Run(bool run);

  // advance time
  void AdvanceTime(qreal dt);


signals:

  // editing
  void NotifyParameterChanged(void);



public slots:
 
  // update from context menu
  void UpdateSignal(void);

protected:

  // derived class hook for file io
  virtual void AppendToElement(QDomDocument& doc, QDomElement& cele) { (void) doc; (void) cele; };
  virtual void ExtractFromElement(QDomElement& cele) { (void) cele;};

  // catch mouse
  virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* ev);
  virtual void mousePressEvent (QGraphicsSceneMouseEvent * ev);
  /*
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * ev);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * ev);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * ev);
  */

  // recognize scene change
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

  // update internal data structures 
  virtual void DoUpdateAll(void);
  virtual void DoReset(void);
  virtual void DoAdvanceTime(qreal dt);
  virtual void DoWorkpiece(FfWorkpiece* wp);

  // initiate user command
  virtual void DoUserCommand(QString cmd);

  // transopr helper: magnetic rail
  virtual void SnapOnRail(qreal dt, QPointF& pos, qreal& o);
  virtual void SnapOnPosition(qreal dt, qreal& pos);
  virtual void SnapOnOrientation(qreal dt, qreal& orient);

  // local signal conf
  QString mName;
  QList<FfSignal*> mSignals;
  QList<FfFault*> mFaults;

  // position
  QPoint mPos;
  Orientation mOrient;
  QList<QPoint> mArea;
  QTransform mTransOrient;

  // shape 
  QRectF mOuterRect;
  QPainterPath  mCoreShape;
  QPainterPath  mOuterShape;

  // default transport
  QPainterPath mBlock;
  QLineF mRail;
  QPointF mNormRail;
  QPointF mVelocity;
  qreal mSnapRate;
  bool mStrictAdvance;
  bool mRefuseAdvance;

  // wp sensors
  QList<FfSensorItem*> mSensors;

  // controls/overlay/selected
  FfSimControlsItem* mSimControls;  // not my child, only in simulation
  QGraphicsItem* mSimOverlay;       // not my child, only in simulation
  FfSelectedItem* mSelected;        // my child, only in editing mode

  // copy of the nominal geometry
  QRectF mNominalOuterRect;
  QPainterPath mNominalOuterShape;
  QPainterPath mNominalCoreShape;
  QLineF mNominalRail;
  QPointF mNominalNormRail;
  QPainterPath mNominalBlock;

  // simulation enabled
  bool mRunning;

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

class FfComponentEditor : public QWidget {
Q_OBJECT

public:

  // construct
  FfComponentEditor(FfComponent* comp, bool insfault=true);

  // my component
  FfComponent* Component(void);

public slots:
  
  // rename my component
  void SetName(QString newname="");

  // update visual from/to component
  virtual void UpdateFromComponent(FfComponent* comp=0);
  virtual void UpdateFromEditor(FfComponent* comp=0);

protected:
  
  // current component
  QString mName;  
  FfScene* mScene;

  // my gui items
  QLineEdit* mNameEdit;

  // gbox for derived classes to extend
  QGridLayout* mGbox;

  // hooks for derived classes
  virtual void DoUpdateFromComponent(FfComponent* comp);
  virtual void DoUpdateFromEditor(FfComponent* comp);

  // insert fault editors
  void InsertFaultEditor(FfComponent* comp);
};




#endif
