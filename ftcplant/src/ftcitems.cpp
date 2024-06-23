/* ftcitems.cpp  - items for productionline simulator */

#include "ftcitems.h"

/*
************************************************
************************************************
  
Implementation: ConveyorItem

************************************************
************************************************
*/

// default constructor
ConveyorItem::ConveyorItem(void) : QGraphicsItem(0) {
  FF_DQM("ConveyorItem::ConveyorItem()");
  setZValue(-20);
  setPos(0,0);
  mOuterRect=QRectF(-505,-100,1010,200);
  mBeltRect=QRectF(-505,-50,1010,100);
  mSensorS1Rect=QRectF(297.5,-75,20,20);
  mSensorS2Rect=QRectF(190,-75,20,20);
  mSensorS3Rect=QRectF(-300,-75,20,20);
  mVelocity=0;
  mPosition=0;
  mMotor=MOff;
  mSensorS1=false;
  mSensorS2=false;
  mSensorS3=false;
  update();
  FF_DQM("ConveyorItem::ConveyorItem(): done");
}


// paint myself
void ConveyorItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) 
{
  (void) painter;
  (void) option;
  (void) widget;
  // shape
  painter->setPen(Qt::NoPen);
  painter->setBrush(FfStyle::Color(VioGrey));
  painter->drawRect(mBeltRect);
  // texture
  for(qreal x= -505 + mPosition; x<505; x+=40) {
    painter->setPen(FfStyle::Color(VioLightGrey));
    painter->drawLine(QPointF(x,-50),QPointF(x,50));
    //painter->setPen(FfStyle::Color(VioBlack));
    //painter->drawLine(QPointF(x,-50),QPointF(x,50));
  }
  // sensor
  painter->setPen(FfStyle::Color(VioBlack));
  painter->setBrush(FfStyle::Color(VioDarkRed));
  if(mSensorS1)
    painter->setBrush(FfStyle::Color(VioGreen));
  painter->drawRect(mSensorS1Rect);
  painter->setBrush(FfStyle::Color(VioDarkRed));
  if(mSensorS2)
    painter->setBrush(FfStyle::Color(VioGreen));
  painter->drawRect(mSensorS2Rect);
  painter->setBrush(FfStyle::Color(VioDarkRed));
  if(mSensorS3)
    painter->setBrush(FfStyle::Color(VioGreen));
  painter->drawRect(mSensorS3Rect);

}


// discrete state
void ConveyorItem::Motor(MState s) {
  mMotor=s;
  if(mMotor==MOff) mVelocity=0;
  if(mMotor==MLeft) mVelocity=-1;
}

// discrete state
ConveyorItem::MState ConveyorItem::Motor(void) {
  return mMotor;
}

// get velocity
qreal ConveyorItem::Velocity(void) {
  return mVelocity;
}

// advance time
void ConveyorItem::TimeStep(void) {
  // track position
  mPosition+=mVelocity;
  if(mPosition>=40) mPosition-=40;
  if(mPosition>=40) mPosition=39;
  if(mPosition<0) mPosition+=40;
  if(mPosition<0) mPosition=0;
  // redraw
  if(//(sense != mSensor) ||
    (mVelocity!=0)) {
    //mSensor=sense;
    update();
  }
}

// sensor read
bool ConveyorItem::SensorS1(void) {
  return mSensorS1;
}
bool ConveyorItem::SensorS2(void) {
  return mSensorS2;
}
bool ConveyorItem::SensorS3(void) {
  return mSensorS3;
}
// sensor write
void ConveyorItem::SensorS1(bool s) {
  mSensorS1=s;
  update();
}
void ConveyorItem::SensorS2(bool s) {
  mSensorS2=s;
  update();
}
void ConveyorItem::SensorS3(bool s) {
  mSensorS3=s;
  update();
}


/*
************************************************
************************************************
  
Implementation: WorkPieceItem

************************************************
************************************************
*/

// default constructor
WorkPieceItem::WorkPieceItem(void) : QGraphicsItem(0) {
  FF_DQM("WorkPieceItem::WorkPieceItem()");
  setZValue(0);
  mProcess1A=0;
  mProcess1B=0;
  mProcess2A=0;
  mProcess2B=0;
  mState=WPlain;
  // construct geometry
  mOuterRect=QRectF(-50,-50,100,100);
  mInnerRect=mOuterRect.adjusted(+15,+15,-15,-15);
  mMainPath.addEllipse(mInnerRect);
  // process 1/2 rect
  mP1Rect = QRectF(-35,-35,70,70);
  mP2Rect = QRectF(-35+15,-35+15,70-2*15,70-2*15);
  // progress A
  for(int i=1; i<=4; i++) {
    QRectF irect =QRectF(-35+4*i,-35+4*i,70-8*i,70-8*i);
    mP1APath.addEllipse(irect);
  }
  for(int i=4; i<=8; i++) {
    QRectF irect =QRectF(-35+4*i,-35+4*i,70-8*i,70-8*i);
    mP2APath.addEllipse(irect);
  }
  // progress B
  for(int j=0;j<20;j++) {
    mP1BPath.moveTo(qCos(3.14*2/360*(j*18))*35/2,qSin(3.14*2/360*(j*18))*35/2);
    mP1BPath.lineTo(qCos(3.14*2/360*(j*18))*34,qSin(3.14*2/360*(j*18))*34);
  }
  for(int j=0;j<20;j++) {
    mP2BPath.moveTo(0,0);
    mP2BPath.lineTo(qCos(3.14*2/360*(j*18))*35/2,qSin(3.14*2/360*(j*18))*35/2);
  }

  // position
  SetPosition(400,0,0);
  FF_DQM("WorkPieceItem::WorkPieceItem(): done");
}

// configure
void WorkPieceItem::SetPosition(qreal x, qreal y, qreal o) {
  //FF_DQM("WorkPieceItem::SetPosition(): x=" << x);
  mX=x;
  mY=y;
  mO=o;
  setPos(mX,mY);
  QTransform rtrans;
  rtrans.rotate(o);
  setTransform(rtrans);
  update();
}


// paint myself
void WorkPieceItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // shape
  painter->setPen(FfStyle::Color(VioBlack));
  painter->setBrush(FfStyle::Color(VioBabyBlue));
  if(mState==WJammed) {
    painter->setPen(FfStyle::Color(VioBlack));
    painter->setBrush(FfStyle::Color(VioRed));
  }
  painter->drawPath(mMainPath);
  // process A/B
  painter->setPen(FfStyle::Color(VioDarkGrey));
  painter->setBrush(Qt::NoBrush);
  if(mProcess1A>0) painter->drawPath(mP1APath);
  if(mProcess2A>0) painter->drawPath(mP2APath);
  if(mProcess1B>0) painter->drawPath(mP1BPath);
  if(mProcess2B>0) painter->drawPath(mP2BPath);
  // process stage
  qreal p1 = qMax(mProcess1A,mProcess1B);
  qreal p2 = qMax(mProcess2A,mProcess2B);
  painter->setPen(Qt::NoPen);
  painter->setBrush(FfStyle::Color(VioBabyBlue));
  if(p1<25) painter->drawPie(mP1Rect,0,16*360*(25-p1)/25);
  if(p2<25) painter->drawPie(mP2Rect,0,16*360*(25-p2)/25);
}


// discrete state
void WorkPieceItem::State(EState s) {
  mState=s;
  update();
}

// discrete state
WorkPieceItem::EState WorkPieceItem::State(void) {
  return mState;
}


// set process
void WorkPieceItem::ApplyA(void) {
  if(mProcess1A<25 && mProcess1B==0) {
    update();
    //FF_DQM("WorkPieceItem::WorkPieceItem(): ApplyA 1st");
     mProcess1A+=1;
    if(mProcess1A>25) mProcess1A=25;
    return;
  }
  if(mProcess1A==25 || mProcess1B==25) 
  if(mProcess2A<25)
  {
    update();
    //FF_DQM("WorkPieceItem::WorkPieceItem(): ApplyA 2nd");
    mProcess2A+=1;
    if(mProcess2A>25) mProcess2A=25;
    return;
  }
  State(WJammed);
}

// set process
void WorkPieceItem::ApplyB(void) {
  if(mProcess1B<25 && mProcess1A==0) {
    update();
    //FF_DQM("WorkPieceItem::WorkPieceItem(): ApplyB 1st");
    mProcess1B+=1;
    if(mProcess1B>25) mProcess1B=25;
    return;
  }
  if(mProcess1B==25 || mProcess1A==25)
  if(mProcess2B<25) {
    update();
    //FF_DQM("WorkPieceItem::WorkPieceItem(): ApplyB 2nd");
    mProcess2B+=1;
    if(mProcess2B>25) mProcess2B=25;
    return;
  }
  State(WJammed);
}

// set process
bool WorkPieceItem::Complete(void) {
  if(mProcess2B==25 || mProcess2A==25) {
    return true;
  }
  if(mProcess2B>0 || mProcess2A>0) {
    return false;
  }
  if(mProcess1B==25 || mProcess1A==25) {
    return true;
  }
  return false;
}

// advance time
void WorkPieceItem::TimeStep(void) {
  /*
  QRectF r=QRectF(200*x+50,150*mY+25,100,100);
  QGraphicsScene* sc=scene();
  if(!sc) return;
  QList<QGraphicsItem*> other = sc->items(r);
  bool err=false;
  foreach(QGraphicsItem* wpi,other) {
    if(wpi==this) continue;
    if(qgraphicsitem_cast<WorkPieceItem*>(wpi)) err=true;
  }
  if(err) {
    State(WJammed);
  }
  if(!err) {
    prepareGeometryChange();
    mX=x;
    mOuterRect=r;
    mInnerRect=mOuterRect.adjusted(+1,+1,-1,-1);
  }
  */
  update();
}

// collision
/*
bool  WorkPieceItem::Overlaps(int x, int y) {
  if(mY!=y) return false;
  if(200*mX+150-5 < 200*x-100) return false;
  if(200*mX+50 +5 > 200*x+200+100) return false;
  return true;
}
*/

// collision
/*
bool  WorkPieceItem::Within(int x, int y) {
  if(mY!=y) return false;
  if(200*mX+100 -5 > 200*x+200) return false;
  if(200*mX+100 +5 < 200*x) return false;
  return true;
}
*/

// collision
/*
bool  WorkPieceItem::Sensor(int x, int y) {
  if(mY!=y) return false;
  if(200*mX-5 < 200*x) 
  if(200*mX+5 > 200*x)
    return true;
  return false;
}
*/


/*
************************************************
************************************************
  
Implementation: DistributorItem

************************************************
************************************************
*/

// default constructor
DistributorItem::DistributorItem(QObject* parent) : QObject(parent), QGraphicsItem(0) {
  FF_DQM("DistributorItem::DistributorItem()");
  setZValue(-10);
  setPos(0,0);
  mMotor=MOff;
  mVelocity=0;
  // construct geometry
  mOuterRect=QRectF(-200,-200,400,400);
  mInnerRect=mOuterRect.adjusted(+10,+10,-10,-10);
  mMainPath = QPainterPath();
  mMainPath.addEllipse(mInnerRect);
  for(qreal i=0;i<360; i+=30) {
    QPainterPath scirc;
    QRectF srect(-35 + 190*qCos(i/360 * 2 * 3.141593),-35 + 190*qSin(i/360 * 2 * 3.141593),70,70);
    scirc.addEllipse(srect);
    mMainPath=mMainPath.subtracted(scirc);
  }
  QPainterPath mcirc;
  mcirc.addEllipse(mOuterRect.adjusted(100,100,-100,-100));
  mMainPath=mMainPath.subtracted(mcirc);
  // set transform
  SetOrientation(0);
  // enable/track hover
  setAcceptsHoverEvents(true);
  mHovered=false;
  FF_DQM("DistributorItem::DistributorItem(): done");
}

// configure
void DistributorItem::SetOrientation(qreal o) {
  //FF_DQM("DistributorItem::SetPosition(): o=" << o);
  mO=qRound(o);
  if(mO>=360) mO-=360;
  if(mO<0) mO-=360;
  QTransform rtrans;
  rtrans.rotate(o);
  setTransform(rtrans);
  bool nr = (( qRound(mO/30) * 30) == mO ); 
  mReady =nr;
  update();
}

// paint myself
void DistributorItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // shape
  painter->setPen(FfStyle::Color(VioBlack));
  painter->setBrush(FfStyle::Color(VioLightGrey));
  if(mHovered) {
    //painter->setBrush(FfStyle::Color(VioLightGreen));
    painter->setPen(FfStyle::Color(VioLightGreen));
  }
  painter->drawPath(mMainPath);
}


// discrete state
void DistributorItem::Motor(MState s) {
  mMotor=s;
  if(mMotor==MOff) mVelocity=0;
  if(mMotor==MRight) mVelocity=1;
}

// discrete state
DistributorItem::MState DistributorItem::Motor(void) {
  return mMotor;
}

// get velocity
qreal DistributorItem::Velocity(void) {
  return mVelocity;
}


// advance time
void DistributorItem::TimeStep(void) {
  if(mVelocity==0) return;
  SetOrientation(mO+mVelocity);
  update();
}

// catch mouse: hover
void DistributorItem::hoverEnterEvent(QGraphicsSceneHoverEvent * ev) {
  //FF_DQ("FfSimControlsItem::HoverEnter()");
  mHovered=true;
  QGraphicsItem::hoverEnterEvent(ev);
}
void DistributorItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * ev) {
  //FF_DQ("FfSimControlsItem::HoverLeave()");
  mHovered=false;
  QGraphicsItem::hoverLeaveEvent(ev);
}
void DistributorItem::hoverMoveEvent(QGraphicsSceneHoverEvent * ev) {
  QGraphicsItem::hoverMoveEvent(ev);
}

// catch mouse: click
void DistributorItem::mousePressEvent(QGraphicsSceneMouseEvent * ev) {
  //if(!mRunning) return;
  // unmodified left click only
  if((ev->modifiers()!=Qt::NoModifier) || (ev->buttons()!=Qt::LeftButton)) {
    QGraphicsItem::mousePressEvent(ev);
    return;
  }
  // raise signal
  emit OperatorEvent("R");
}

/*
************************************************
************************************************
  
Implementation: StopperItem

************************************************
************************************************
*/

// default constructor
StopperItem::StopperItem(QObject* parent) : QObject(parent), QGraphicsItem(0) {
  FF_DQM("StopperItem::StopperItem()");
  setZValue(-10);
  setPos(0,0);
  mMotorL=MOff;
  mMotorR=MOff;
  mVelocityL=0;
  mVelocityR=0;
  // construct geometry
  mOuterRect=QRectF(250,-150,150,300);
  mTriangleU = QPainterPath();
  mTriangleU.moveTo(0,0);
  mTriangleU.lineTo(30,0);
  mTriangleU.lineTo(15,30);
  mTriangleU.closeSubpath();
  mTriangleL = QPainterPath();
  mTriangleL.moveTo(0,0);
  mTriangleL.lineTo(30,0);
  mTriangleL.lineTo(15,-30);
  mTriangleL.closeSubpath();
  // set
  SetPositionL(25); // ie: closed
  SetPositionR(0);  // ie: open
  // enable/track hover
  setAcceptsHoverEvents(true);
  mHovered=false;
  FF_DQM("StopperItem::StopperItem(): done");
}

// configure
void StopperItem::SetPositionL(qreal y) {
  if(y>=25) y=25;
  if(y<=0)  y=0;
  mYL=y;
  mMainPath = QPainterPath();
  QTransform trans;
  mMainPath.addPath( QTransform().translate(260,-75+mYL).map(mTriangleU));
  mMainPath.addPath( QTransform().translate(325,-75+mYR).map(mTriangleU));
  mMainPath.addPath( QTransform().translate(260, 75-mYL).map(mTriangleL));
  mMainPath.addPath( QTransform().translate(325, 75-mYR).map(mTriangleL));
  /* hmmm ... should move to Qt 4.6 
  mMainPath.addPath(mTriangleU.translated(260,-75+mYL));
  mMainPath.addPath(mTriangleU.translated(325,-75+mYR));
  mMainPath.addPath(mTriangleL.translated(260, 75-mYL));
  mMainPath.addPath(mTriangleL.translated(325, 75-mYR));
  */
  update();
}

// configure
void StopperItem::SetPositionR(qreal y) {
  if(y>=25) y=25;
  if(y<=0)  y=0;
  mYR=y;
  SetPositionL(mYL);
}

// paint myself
void StopperItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // shape
  painter->setPen(FfStyle::Color(VioBlack));
  painter->setBrush(FfStyle::Color(VioLightGrey));
  if(mMotorL==MJammed) painter->setBrush(FfStyle::Color(VioRed));
  if(mMotorR==MJammed) painter->setBrush(FfStyle::Color(VioRed));
  if(mHovered) {
    painter->setPen(FfStyle::Color(VioGreen));
  }
  painter->drawPath(mMainPath);
}


// discrete state
void StopperItem::MotorL(MState s) {
  mMotorL=s;
  if(mMotorL==MOff) mVelocityL=0;
  if(mMotorL==MJammed) mVelocityL=0;
  if(mMotorL==MOpen) mVelocityL=-2;
  if(mMotorL==MClose) mVelocityL=2;
}

// discrete state
void StopperItem::MotorR(MState s) {
  mMotorR=s;
  if(mMotorR==MOff)    mVelocityR=0;
  if(mMotorR==MJammed) mVelocityR=0;
  if(mMotorR==MOpen)   mVelocityR=-2;
  if(mMotorR==MClose)  mVelocityR=2;
}

// discrete state
StopperItem::MState StopperItem::MotorL(void) {
  return mMotorL;
}

// discrete state
StopperItem::MState StopperItem::MotorR(void) {
  return mMotorR;
}

// advance time
void StopperItem::TimeStep(void) {
  if(mVelocityL!=0) {
    SetPositionL(mYL+mVelocityL);
    update();
  }
  if(mVelocityR!=0) {
    SetPositionR(mYR+mVelocityR);
    update();
  }
}

// catch mouse: hover
void StopperItem::hoverEnterEvent(QGraphicsSceneHoverEvent * ev) {
  //FF_DQ("FfSimControlsItem::HoverEnter()");
  mHovered=true;
  QGraphicsItem::hoverEnterEvent(ev);
}
void StopperItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * ev) {
  //FF_DQ("FfSimControlsItem::HoverLeave()");
  mHovered=false;
  QGraphicsItem::hoverLeaveEvent(ev);
}
void StopperItem::hoverMoveEvent(QGraphicsSceneHoverEvent * ev) {
  QGraphicsItem::hoverMoveEvent(ev);
}

// catch mouse: click
void StopperItem::mousePressEvent(QGraphicsSceneMouseEvent * ev) {
  //if(!mRunning) return;
  // unmodified left click only
  if((ev->modifiers()!=Qt::NoModifier) || (ev->buttons()!=Qt::LeftButton)) {
    QGraphicsItem::mousePressEvent(ev);
    return;
  }
  // raise signal
  emit OperatorEvent("V");
}

/*
************************************************
************************************************
  
Implementation: ProcessItem

************************************************
************************************************
*/

// default constructor
ProcessItem::ProcessItem(QObject* parent) : QObject(parent), QGraphicsItem(0) {
  FF_DQM("ProcessItem::ProcessItem()");
  setZValue(5);
  setPos(0,190);
  mMachine=MOff;
  mDefectB=false;
  // construct overall geometry
  mOuterRect=QRectF(-50,-50,100,100);
  mInnerRect=mOuterRect.adjusted(5,5,-5,-5);
  // have A and B rectangle
  mARect=QRectF(-45,-45,45,90);
  mBRect=QRectF(0,-45,45,90);
  // fit letters A
  // state 1: draw letter
  mAPath=QPainterPath();
  mAPath.addText(QPointF(0,0),FfStyle::DefaultHugeFont(),QString("A"));
  QRectF rect=mAPath.boundingRect();
  // stage 2: scale the text to width 40
  qreal dst = 40;
  qreal src = rect.width();
  QTransform sc;
  if(src>1) {
    sc.scale(dst/src,dst/src);
    mAPath=sc.map(mAPath);
    rect=mAPath.boundingRect();
  }
  // stage 3: figure center an fix
  QTransform tr;
  tr.translate(-rect.right()-2,+40 -rect.bottom());
  mAPath=tr.map(mAPath);
  // same for B
  mBPath=QPainterPath();
  mBPath.addText(QPointF(0,0),FfStyle::DefaultHugeFont(),QString("B"));
  mBPath=sc.map(mBPath);
  rect=mBPath.boundingRect();
  tr.reset();
  tr.translate(-rect.left()+2,+40 -rect.bottom());
  mBPath=tr.map(mBPath);
  // enable/track hover
  setAcceptsHoverEvents(true);
  mHovered=false;
  mAHovered=false;
  mBHovered=false;
  FF_DQM("ProcessItem::ProcessItem(): done");
}


// paint myself
void ProcessItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // shape
  QColor col=FfStyle::Color(VioDarkGrey);
  if(mMachine==MJammed) {
    col = FfStyle::Color(VioRed);
  }
  col.setAlphaF(0.50);
  painter->setBrush(col);
  painter->setPen(FfStyle::Color(VioBlack));
  if(mHovered) painter->setPen(FfStyle::Color(VioLightGreen));
  painter->drawRect(mInnerRect);
  // A and B
  if(mAHovered) {
    painter->setBrush(Qt::NoBrush);
    painter->setPen(FfStyle::Color(VioLightGreen));
    painter->drawPath(mAPath);
    painter->setPen(FfStyle::Color(VioBlack));
    painter->drawPath(mBPath);
  }
  if(mBHovered) {
    painter->setBrush(Qt::NoBrush);
    painter->setPen(FfStyle::Color(VioLightGreen));
    painter->drawPath(mBPath);
    painter->setPen(FfStyle::Color(VioBlack));
    painter->drawPath(mAPath);
  }
}


// discrete state
void ProcessItem::Machine(MState s) {
  if(mMachine==s) return;
  mMachine=s;
  FF_DQM("ProcessItem::Machine(): " << s << " for wp " << pWorkPiece);
  if((mMachine != MOff) && (!pWorkPiece)) mMachine=MJammed;
  update();
}

// discrete state
ProcessItem::MState ProcessItem::Machine(void) {
  return mMachine;
}

// record wp
void ProcessItem::WorkPiece(WorkPieceItem* wpi) { 
  pWorkPiece=wpi;
  if((mMachine != MOff) && (!pWorkPiece)) Machine(MJammed);  
};


// advance time
void ProcessItem::TimeStep(void) {
  if(pWorkPiece && (mMachine ==MProcA)) 
    pWorkPiece->ApplyA();
  if(pWorkPiece && (mMachine ==MProcB) && (!mDefectB))
    pWorkPiece->ApplyB();
  update();
}

// catch mouse: hover
void ProcessItem::hoverEnterEvent(QGraphicsSceneHoverEvent * ev) {
  //FF_DQ("FfSimControlsItem::HoverEnter()");
  mHovered=true;
  mAHovered=false;
  mBHovered=false;
  QGraphicsItem::hoverEnterEvent(ev);
}
void ProcessItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * ev) {
  //FF_DQ("FfSimControlsItem::HoverLeave()");
  mHovered=false;
  mAHovered=false;
  mBHovered=false;
  QGraphicsItem::hoverLeaveEvent(ev);
}
void ProcessItem::hoverMoveEvent(QGraphicsSceneHoverEvent * ev) {
  QPointF pos=ev->pos();
  if(mARect.contains(pos) && !mAHovered) {
    mAHovered=true;
    mBHovered=false;
    update();
  }
  if(mBRect.contains(pos) && !mBHovered) {
    mBHovered=true;
    mAHovered=false;
    update();
  }
  QGraphicsItem::hoverMoveEvent(ev);
}

// catch mouse: click
void ProcessItem::mousePressEvent(QGraphicsSceneMouseEvent * ev) {
  if((ev->modifiers()!=Qt::NoModifier) || (ev->buttons()!=Qt::LeftButton)) {
    QGraphicsItem::mousePressEvent(ev);
    return;
  }
  // raise signal
  if(mAHovered)
    emit OperatorEvent("A");
  if(mBHovered)
    emit OperatorEvent("B");
}
