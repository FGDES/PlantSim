/* ftcitems.h  - items for bottling line simulator */


#ifndef FAUDES_FTCITEMS_H
#define FAUDES_FTCITEMS_H

#include <QApplication>
#include <QtGui>
#include "libfaudes.h"
#include "ffstyle.h"



/*
************************************************
************************************************
  
Conveyor-belt graphics item

************************************************
************************************************
*/


class ConveyorItem : public QGraphicsItem {

public:

  //constructor 
  ConveyorItem(void);

  // bounding rect and shape
  virtual QRectF boundingRect() const { return mOuterRect; };

  // paint myself
  void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);

  // rtti
  enum { Type = UserType+1};
  virtual int type(void) const { return Type;};

  // set/get my state: motor
  typedef enum { MOff, MLeft} MState;
  void Motor(MState s);
  MState Motor(void);

  // get/set my state: sensor
  bool SensorS1(void);
  bool SensorS2(void);
  bool SensorS3(void);
  void SensorS1(bool s);
  void SensorS2(bool s);
  void SensorS3(bool s);

  // get velocity
  qreal Velocity(void);


  // advance time
  void TimeStep(void);

 protected:

  // shape 
  QRectF mOuterRect;
  QRectF mBeltRect;
  QRectF mSensorS1Rect;
  QRectF mSensorS2Rect;
  QRectF mSensorS3Rect;

  // discrete state
  MState mMotor;
  bool mSensorS1;
  bool mSensorS2;
  bool mSensorS3;

  // continuous dynamics
  qreal mVelocity;
  qreal mPosition;


private:


};



/*
************************************************
************************************************
  
WorkPiece graphics item

************************************************
************************************************
*/


class WorkPieceItem : public QGraphicsItem {

public:

  //constructor 
  WorkPieceItem(void);

  // set/get my position
  void SetPosition(qreal x, qreal y, qreal o);
  const qreal& X(void) { return mX; }
  const qreal& Y(void) { return mY; }
  const qreal& O(void) { return mO; }

  // bounding rect and shape
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mMainPath; };

  // paint myself
  void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);

  // rtti
  enum { Type = UserType+2};
  virtual int type(void) const { return Type;};

  // set/get my state
  typedef enum { WPlain, WFailed, WJammed} EState;
  void State(EState s);
  EState State(void);

  // collision
  /*
  bool Sensor(int x, int y);
  bool Within(int x, int y);
  bool Overlaps(int x, int y);
  */

  // set/get process
  void ApplyA(void);
  void ApplyB(void);
  bool Complete(void);

  // read sensor
  typedef enum { Aktive, Passive} ESensor;
  ESensor Sensor(void);

  // advance time
  void TimeStep(void);

 protected:

  // position
  qreal mX;
  qreal mY;
  qreal mO;

  // shape 
  QRectF mOuterRect;
  QRectF mInnerRect;
  QPainterPath mMainPath;
  QPainterPath mP1APath;
  QPainterPath mP1BPath;
  QPainterPath mP2APath;
  QPainterPath mP2BPath;
  QRectF mP1Rect;
  QRectF mP2Rect;

  // discrete state
  EState mState;

  // continuous dynamics
  qreal mProcess1A;
  qreal mProcess1B;
  qreal mProcess2A;
  qreal mProcess2B;


private:


};




/*
************************************************
************************************************
  
Distributor graphics item

************************************************
************************************************
*/



class DistributorItem : public QObject, public QGraphicsItem {

Q_OBJECT
Q_INTERFACES(QGraphicsItem)

public:

  //constructor 
  DistributorItem(QObject* parent=0);

  // set/get my position
  void SetOrientation(qreal o);
  const qreal& O(void) { return mO; }

  // bounding rect and shape
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mMainPath; };

  // paint myself
  void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);

  // rtti
  enum { Type = UserType+3};
  virtual int type(void) const { return Type;};

  // set/get my state: motor
  typedef enum { MOff, MRight} MState;
  void Motor(MState s);
  MState Motor(void);

  // get my state: ready
  bool Ready(void) { return mReady; };

  // get velocity
  qreal Velocity(void);

  // advance time
  void TimeStep(void);

signals:

  // signale click
  void OperatorEvent(QString ev);


 protected:

  // position
  qreal mO;

  // shape 
  QRectF mOuterRect;
  QRectF mInnerRect;
  QPainterPath mMainPath;

  // discrete state
  MState mMotor;
  bool mReady;
  int mAutoStep;

  // cont state
  qreal mVelocity;

  // ui state
  bool mHovered;

  // re-impelement
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * ev);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * ev);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * ev);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent * ev);

private:


};



/*
************************************************
************************************************
  
Stopper graphics item

************************************************
************************************************
*/


class StopperItem : public QObject, public QGraphicsItem {

Q_OBJECT
Q_INTERFACES(QGraphicsItem)

public:

  //constructor 
  StopperItem(QObject *parent=0);

  // set/get my position
  void SetPositionL(qreal yl);
  const qreal& YL(void) { return mYL; }
  void SetPositionR(qreal yr);
  const qreal& YR(void) { return mYR; }

  // bounding rect and shape
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mMainPath; };

  // paint myself
  void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);

  // rtti
  enum { Type = UserType+4};
  virtual int type(void) const { return Type;};

  // set/get my state: motor
  typedef enum { MOff, MOpen, MClose, MJammed} MState;
  void MotorL(MState s);
  MState MotorL(void);
  void MotorR(MState s);
  MState MotorR(void);

  // get position
  bool OpenL(void)  { return mYL==0;}
  bool ClosedL(void){ return mYL==25;}
  bool OpenR(void)  { return mYR==0;}
  bool ClosedR(void){ return mYR==25;}

  // advance time
  void TimeStep(void);

 signals:

  // signale click
  void OperatorEvent(QString ev);

 protected:

  // position
  qreal mYL;
  qreal mYR;

  // shape 
  QRectF mOuterRect;
  QPainterPath mMainPath;
  QPainterPath mTriangleU;
  QPainterPath mTriangleL;

  // discrete state
  MState mMotorL;
  MState mMotorR;

  // cont state
  qreal mVelocityL;
  qreal mVelocityR;

  // ui state
  bool mHovered;


  // re-impelement
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * ev);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * ev);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * ev);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent * ev);


private:


};


/*
************************************************
************************************************
  
Process graphics item

************************************************
************************************************
*/


class ProcessItem : public QObject, public QGraphicsItem {

Q_OBJECT
Q_INTERFACES(QGraphicsItem)

public:

  //constructor 
  ProcessItem(QObject* parent=0);


  // bounding rect and shape
  virtual QRectF boundingRect() const { return mOuterRect; };

  // paint myself
  void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);

  // rtti
  enum { Type = UserType+5};
  virtual int type(void) const { return Type;};

  // set/get my state: moachine
  typedef enum { MOff, MProcA, MProcB, MJammed} MState;
  void Machine(MState s);
  MState Machine(void);

  // get/set defect
  bool DefectB(void) { return mDefectB; };
  void DefectB(bool f) { mDefectB=f; };

  // set workpiece
  void WorkPiece(WorkPieceItem* wpi);

  // advance time
  void TimeStep(void);


signals:

  // signale click
  void OperatorEvent(QString ev);

 protected:

  // overall shape
  QRectF mOuterRect;
  QRectF mInnerRect;

  // A and B
  QRectF mARect;
  QRectF mBRect;
  QPainterPath mAPath;
  QPainterPath mBPath;

  // discrete state
  MState mMachine;
  bool mDefectB;

  // my workpiece
  WorkPieceItem* pWorkPiece;

  // ui state
  bool mHovered;
  bool mAHovered;
  bool mBHovered;


  // re-impelement
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * ev);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * ev);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * ev);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent * ev);


private:


};


#endif
