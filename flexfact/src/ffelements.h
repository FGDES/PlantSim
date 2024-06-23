/* ffelemens.h  - lightweight aux items */

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


#ifndef FAUDES_ELEMENTS_H
#define FAUDES_ELEMENTS_H

#include <QtGui>
#include "ffstyle.h"


/*
************************************************
************************************************

sensor item

************************************************
************************************************
*/

// forward
class FfWorkpiece;

class FfSensorItem : public QGraphicsItem {

public:

  // construct 
  FfSensorItem(QGraphicsItem* parent=0);

  // configuration
  QPainterPath mOffPath;
  QPainterPath mOnPath;
  QPainterPath mIndicatorPath;
  bool         mValue;
  bool         mMute;
  bool         mWpSensor;
  QPointer<FfWorkpiece> mWpReference;
  int mRelAddress; 

  // my update
  void Update(void);
  void SetValue(bool val);

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mIndicatorPath; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Sensor = UserType+100};
  virtual int type(void) const { return Sensor;};

protected:

  // derived data
  QRectF mOuterRect;


};


/*
************************************************
************************************************

std rails

************************************************
************************************************
*/

class FfRailItem : public QGraphicsItem {

public:

  // construct 
  FfRailItem(QGraphicsItem* parent=0);

  // my update
  void SetLower(int len=1);
  void SetLowerX(int len=1);
  void SetLowerP(int len=1);
  void SetUpper(int len=1);
  void SetUpperC(int len=1);
  void SetUpperX(int len=1);
  void SetUpperP(int len=1);
  void SetLeft(int len=1);
  void SetRight(int len=1);
  void SetLeftHorseShoe(int len=1);
  void SetRightHorseShoe(int len=1);
  void SetRightOpen(int len=1);

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mRailPath; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Rail = UserType+101};
  virtual int type(void) const { return Rail;};

protected:

  // derived data
  QRectF mOuterRect;
  QPainterPath mRailPath;


};


/*
************************************************
************************************************

sim controls item

************************************************
************************************************
*/

class FfSimControlsItem : public QGraphicsItem {

public:

  // construct 
  FfSimControlsItem(QGraphicsItem* parent=0);

  // configuration
  QPainterPath mCoreShape;

  // my update
  void Update(void);

  // my version if visible
  bool IsVisible(void) { return mVisible; };
  void SetVisible(bool vis);

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { SimControls = UserType+102};
  virtual int type(void) const { return SimControls;};

protected:

  // derived date
  QRectF mOuterRect;
  QPainterPath mOuterShape;

  // catch mouse
 void hoverEnterEvent(QGraphicsSceneHoverEvent * ev);
 void hoverLeaveEvent(QGraphicsSceneHoverEvent * ev);
 void hoverMoveEvent(QGraphicsSceneHoverEvent * ev);

 // state
 bool mVisible;


};


/*
************************************************
************************************************

knobs item

************************************************
************************************************
*/

class FfButtonItem : public QGraphicsItem {

public:

  // construct 
  FfButtonItem(QGraphicsItem* parent=0);

  // configuration
  QPainterPath mCoreShape;

  // pragmatic click
  bool UnderMouse(void) { return mUnderMouse; };
  const QString& Text(void) { return mText; };
  void SetText(const QString& text) { mText=text; };
  void SetColorRed(void);
  void SetColorGreen(void);

  // my update
  void Update(void);

  // my presets
  void SetLeft(const QPointF& pos=QPointF(0,0));
  void SetRight(const QPointF& pos=QPointF(0,0));
  void SetUp(const QPointF& pos=QPointF(0,0));
  void SetDown(const QPointF& pos=QPointF(0,0));
  void SetStop(const QPointF& pos=QPointF(0,0));
  void SetLargeCircle(const QPointF& pos=QPointF(0,0));
  void SetCircle(const QPointF& pos=QPointF(0,0));
  void SetLeftSkip(const QPointF& pos=QPointF(0,0));
  void SetRightSkip(const QPointF& pos=QPointF(0,0));
  void SetDownSkip(const QPointF& pos=QPointF(0,0));
  void SetPlus(const QPointF& pos=QPointF(0,0));
  void SetMinus(const QPointF& pos=QPointF(0,0));

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Button = UserType+103};
  virtual int type(void) const { return Button;};

protected:

  // re-impelement
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * ev);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * ev);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * ev);

  // derived date
  QRectF mOuterRect;
  QPainterPath mOuterShape;
  bool mUnderMouse;
  QString mText;
  QColor mNormColor;
  QColor mLightColor;
  QColor mDarkColor;

};


/*
************************************************
************************************************

selector item

************************************************
************************************************
*/

class FfSelectedItem : public QGraphicsItem {

public:

  // construct 
  FfSelectedItem(QGraphicsItem* parent=0);

  // configuration
  QPainterPath mCoreShape;

  // my update
  void Update(void);

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Selected = UserType+104};
  virtual int type(void) const { return Selected;};

protected:

  // derived date
  QRectF mOuterRect;
  QPainterPath mOuterShape;


};


/*
************************************************
************************************************

number item

************************************************
************************************************
*/

class FfDigitItem : public QGraphicsItem {

public:

  // construct 
  FfDigitItem(QGraphicsItem* parent=0);

  // configuration
  void SetDigit(int num);
  qreal mSize;

  // my update
  void Update(void);


  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mCoreShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Digit = UserType+105};
  virtual int type(void) const { return Digit;};

protected:

  // derived date
  int mDigit;
  QRectF mOuterRect;
  QPainterPath mCoreShape;

};




/*
************************************************
************************************************

belt 

************************************************
************************************************
*/

class FfBeltItem : public QGraphicsItem {

public:

  // construct 
  FfBeltItem(QGraphicsItem* parent=0);

  // my update
  void SetLength(int len=1);
  void SetPosition(qreal pos);
  void SetTransform(const QTransform& btrans);

  // update
  void Update(void);

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Belt = UserType+106};
  virtual int type(void) const { return Belt;};

protected:

  // configuration 
  int mLen;
  qreal mPos;
  QRectF       mOuterRect;
  QRectF       mCoreRect;
  QPainterPath mOuterShape;

  // base transform ie rotate
  QTransform mBaseTrans;

  // pens
  QPen mBlackPen;
  QPen mLightGreyPen;
  QBrush mGreyBrush;
};

/*
************************************************
************************************************

push belt 

************************************************
************************************************
*/

class FfPushBeltItem : public QGraphicsItem {

public:

  // construct 
  FfPushBeltItem(QGraphicsItem* parent=0);

  // my update
  void SetPosition(qreal pos);
  void SetTransform(const QTransform& btrans);

  // update
  void Update(void);

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { PushBelt = UserType+107};
  virtual int type(void) const { return PushBelt;};

protected:

  // configuration 
  qreal mPos;
  QRectF       mOuterRect;
  QRectF       mCoreRect;
  QPainterPath mOuterShape;
  QRectF       mSeparator;

  // base transform ie rotate
  QTransform mBaseTrans;

  // statics
  QPen mBlackPen;
  QPen mLightGreyPen;
  QBrush mGreyBrush;
  QBrush mLightGreyBrush;
};

/*
************************************************
************************************************

err indicator item

************************************************
************************************************
*/

class FfErrIndicatorItem : public QGraphicsItem {

public:

  // construct 
  FfErrIndicatorItem(QGraphicsItem* parent=0);

  // configuration
  QPainterPath mCoreShape;

  // presets
  void SetWorkpieceError(void);

  // my update
  void Update(void);

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { ErrIndicator = UserType+108};
  virtual int type(void) const { return ErrIndicator;};

protected:

  // derived date
  QRectF mOuterRect;
  QPainterPath mOuterShape;


};


/*
************************************************
************************************************

pusher

************************************************
************************************************
*/

class FfPusherItem : public QGraphicsItem {

public:

  // construct 
  FfPusherItem(QGraphicsItem* parent=0);

  // my update
  void SetPosition(qreal pos);
  void SetOrigin(int block);
  qreal Position(void) { return mPos;};

  // update
  void Update(void);

  // paths etc maintained by distributor
  QPainterPath mBackgroundPath;
  QLineF mRail;
  QPointF mNormRail;
  bool mActive;
  bool mAutoPush;
  FfButtonItem* mButton;

  // host sensor items
  FfSensorItem* mSensorA;
  FfSensorItem* mSensorB;
  FfSensorItem* mSensorWp;

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Pusher = UserType+109};
  virtual int type(void) const { return Pusher;};

protected:

  // configuration 
  qreal        mPos;
  QPointF      mOrigin;
  QRectF       mOuterRect;
  QRectF       mCoreRect;
  QPainterPath mOuterShape;

  // paths
  QPainterPath mPusherPath;
  QPainterPath mForegroundPath;

};

/*
************************************************
************************************************

conveyor

************************************************
************************************************
*/

class FfConveyorItem : public QGraphicsItem {

public:

  // construct 
  FfConveyorItem(QGraphicsItem* parent=0);

  // track transformation
  void SetTransform(const QTransform& trans);

  // my update
  void SetOrigin(qreal pos);
  qreal Origin(void) { return mOrigin.x();};
  void SetBeltPos(qreal pos);
  qreal BeltPos(void) { return mBeltPos;};

  // update
  void Update(void);

  // state data maintained by rail transport
  QLineF mRail;
  QPointF mNormRail;
  QPointF mOrthRail;
  QPointF mVelocity;
  bool mAutoStop;
  FfButtonItem* mButtonL;
  FfButtonItem* mButtonR;
  FfButtonItem* mButtonU;
  FfButtonItem* mButtonD;
  FfButtonItem* mButtonStop;

  // host sensor items
  FfSensorItem* mSensorWp;

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Conveyor = UserType+110};
  virtual int type(void) const { return Conveyor;};

protected:

  // configuration 
  qreal        mBeltPos;
  QPointF      mOrigin;
  QRectF       mOuterRect;
  QPainterPath mOuterShape;
  QTransform   mTrans;

  // items
  FfRailItem* mLeftRail;
  FfRailItem* mRightRail;
  FfBeltItem* mBelt;

};

/*
************************************************
************************************************

simple processor

************************************************
************************************************
*/

class FfProcessorItem : public QGraphicsItem {

public:

  // construct 
  FfProcessorItem(QGraphicsItem* parent=0);

  // my update
  void SetPosition(qreal pos);
  void SetProgress(qreal prog);
  qreal Position(void) { return mPos;};
  qreal Progress(void) { return mProg;};

  // update
  void Update(void);

 
  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Processor = UserType+111};
  virtual int type(void) const { return Processor;};

protected:

  // configuration 
  qreal        mPos;
  qreal        mProg;
  QRectF       mOuterRect;
  QRectF       mCoreRect;
  QPainterPath mOuterShape;

  // paths
  QPainterPath mOuterFrame;
  QPainterPath mInnerFrame;

};


/*
************************************************
************************************************

univ tool (the revolver with coordinates (0,0) at center)

************************************************
************************************************
*/

class FfUnivtoolItem : public QGraphicsItem {

public:

  // construct 
  FfUnivtoolItem(QGraphicsItem* parent=0);

  // my update
  void SetToolsel(qreal tsel);  // tool-select: rotate items coordinates 0-359 (?) === 0=A, 120=B 240=C
  void SetProgress(qreal prog); // operation progress: 0-100 === start--finish

  qreal Toolsel(void) { return mTsel;};
  qreal Progress(void) { return mProg;};

  // update
  void Update(void);

 
  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Univtool = UserType+112}; 
  virtual int type(void) const { return Univtool;};

protected:

  // configuration 
  qreal        mTsel;
  qreal        mPos;  
  qreal        mProg;
  QRectF       mOuterRect;
  QRectF       mCoreRect;
  QPainterPath mOuterShape;
  QTransform   mTransTsel;


  // paths
  QPainterPath mOuterFrame;
  QPainterPath mInnerFrame;
  QPainterPath mRevolverPath;
  QPainterPath mToolsPath;

};


/*
************************************************
************************************************

univ frame 

************************************************
************************************************
*/

class FfUnivframeItem : public QGraphicsItem {

public:

  // construct 
  FfUnivframeItem(QGraphicsItem* parent=0);

  // update
  void Update(void);

  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Univproc = UserType+113}; 
  virtual int type(void) const { return Univproc;};

protected:

  // configuration 
  QRectF       mOuterRect;
  QRectF       mCoreRect;
  QPainterPath mOuterShape;


  // paths
  QPainterPath mOuterFrame;
  QPainterPath mInnerFrame;

  // child items
  FfUnivtoolItem* mUnivtool;
  FfSensorItem* mSensor;
};


/*
************************************************
************************************************

univ processor (hosts tool revolver and frame)

************************************************
************************************************
*/

class FfUnivprocItem : public QGraphicsItem {

public:

  // construct 
  FfUnivprocItem(QGraphicsItem* parent=0);

  // my update
  void SetPosition(qreal pos);  // tool-position: 0-100 (?) === home-operate
  void SetToolsel(qreal tsel);  // tool-select: 0-359 (?) === 0=A, 120=B 240=C
  void SetProgress(qreal prog); // operation progress: 0-100 === start--finish

  qreal Position(void) { return mPos;};
  qreal Toolsel(void)  { return mUnivtool->Toolsel();};
  qreal Progress(void) { return mUnivtool->Progress();};

  // pass on tool select sensor
  bool Sensor() { return mSensor->mValue;};

  // update
  void Update(void);
 
  // re-implement graphics item
  virtual QRectF boundingRect() const { return mOuterRect; };
  virtual QPainterPath shape() const { return mOuterShape; };
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);
  enum { Univproc = UserType+114}; 
  virtual int type(void) const { return Univproc;};

protected:

  // configuration 
  qreal        mTsel;
  qreal        mPos;  
  qreal        mProg;
  QRectF       mOuterRect;
  QRectF       mCoreRect;
  QPainterPath mOuterShape;


  // child items
  FfUnivframeItem* mUnivframe;
  FfUnivtoolItem* mUnivtool;
  FfSensorItem* mSensor;
};

#endif
