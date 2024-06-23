/* ffscene  - graphics scene to host productionline components */

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

#ifndef FAUDES_FFSCENE_H
#define FAUDES_FFSCENE_H

#include <QtGui>
#include <QtXml>
#include "ffstyle.h"
#include "ffsignal.h"
#include "ffcomponent.h"
#include "ffworkpiece.h"
#include "ffmonitor.h"


/*
************************************************
************************************************
  
Scene of Components

************************************************
************************************************
*/


class FfScene : public QGraphicsScene {

Q_OBJECT

public:

  // constructor 
  FfScene(QObject* parent);


  /*
  ********************************************************************
  ********************************************************************
  construct scenes programatically /from file 
  ********************************************************************
  ********************************************************************
  */


  // have a name
  void SetName(QString name);

  // clear all
  void Clear(void);
  void AdjustScene(void);

  // access components
  bool AddComponent(FfComponent* comp);
  void RemoveComponent(FfComponent* comp);
  FfComponent* Component(QPoint pos);
  FfComponent* Component(const QString& name);

  // compile component configuration
public slots:
  void ClearImage(void);
  void CompileImage(void);
  void ClearMonitor(void);
  void CompileMonitor(void);

public:
  // access workpieces
  void AddWorkpiece(const QPointF& pos, FfComponent::Orientation o=FfComponent::Ostd);
  QList<FfWorkpiece*> Workpieces(const QPainterPath& path);
  QList<FfWorkpiece*> Workpieces(const QPointF& pos);
  void RemoveWorkpiece(FfWorkpiece* wp);

  // file io
  QDomElement ToElement(QDomDocument& doc);
  void FromElement(QDomElement& elem);

  // export graphics
  int ExportSvg(const QString &svgfile="");

  // register fault monitor
  void SetMonitor(FfMonitorPanel* monitor);
  FfMonitorPanel* Monitor(void);


  /*
  ********************************************************************
  ********************************************************************
  edit scenes 
  ********************************************************************
  ********************************************************************
  */

  void UserSelectionUpdate() {};
  bool MoveSelection() {return false;};
  bool MoveComponent(FfComponent* comp, QPoint ipos);
  bool OrientComponent(FfComponent* comp, FfComponent::Orientation o);
  bool RotateComponent(FfComponent* comp, const QPoint& iorigin, bool cw);
  bool RenameComponent(FfComponent* comp, QString nname);
  bool RenameComponent(QString oldname, QString newname);
  bool ReconfigureComponent(FfComponent* comp, QList<QPoint>& narea);
  void OperatorPanelSize(int sz);
  void UserRotateSelection(const QPoint& iorigign, bool cw);
  void UserDelSelection(void);
  

  /*
  ********************************************************************
  ********************************************************************
  run simulation
  ********************************************************************
  ********************************************************************
  */

  // reset state
  void Reset(void);
  void Run(bool run);

  // advance time
  void AdvanceTime(qreal dt);

  // process image interface (merged signals over all components)
  int  ImageSize(void);
  void SetSignal(int address, bool val);
  bool Signal(int address);
  QString SignalName(int address);
  bool IsActuator(int address);
  bool IsSensor(int address);

  // event interface
  bool ExecuteActuatorEvent(const QString& actev);
  QStringList ActuatorEvents();
  QStringList SensorEvents();


signals:

  // notify changes
  void NotifySignal(int absaddr, bool val);
  void NotifySensorEvent(QString ev);
  void NotifyClearImage(void);

  // operator panel interface
public slots:
  void OperatorSwitch(int i, bool v);
signals:
  void OperatorLight(int i, bool v);
  
  // dispatch operator signals
protected slots:
  void EmitOperatorLight(void);

  /*
  ********************************************************************
  ********************************************************************
  misc
  ********************************************************************
  ********************************************************************
  */

 public:

  // process image as item model
  QStandardItemModel* ImageModel(void) { return mImage->Model(); };
  FfProcessImage* Image(void) { return mImage; };

  // poor men's color effect
  const QColor& EffectColor(int vcol);


 protected:

  // reimplement scene  
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
  virtual void keyPressEvent(QKeyEvent *keyEvent);
  virtual void drawBackground(QPainter * painter, const QRectF & rect);

  // name
  QString mName;

  // my components/workpieces
  QList<FfComponent*> mComponents;
  QList<FfWorkpiece*> mWorkpieces;

  // component maps
  QMap<QPoint,FfComponent*> mComponentPosMap;
  QMap<QString,FfComponent*> mComponentNameMap;

  // item model to represent process image
  FfProcessImage* mImage;

  // operator panel
  int mOperatorComponentSize;
  QList<FfSignal*> mOperatorSignals;
 
  // fault monitor
  FfMonitorPanel* pMonitor;

  // sim flag
  bool mRunning;

  // editing state
  bool mMoveSelection;
  bool mMovingSelection;

  // my context menu
  void ComponentContextMenu(const QPoint& screenpos, const QPointF& scenepos, FfComponent* comp);
  void SceneContextMenu(const QPoint& screenpos, const QPointF& scenepos);

};


#endif
