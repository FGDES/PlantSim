/* ffelemens.cpp  - light weight aux items */

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

#include "ffelements.h"
#include "ffcomponent.h"
#include "ffworkpiece.h"
#include "ffscene.h"

/*
Primitive paint() debugging, to 
inspect performance issues 
*/

// no debugging 
#define FF_DQP(message) {};
// report painting to std out
//#define FF_DQP(message) FAUDES_WRITE_CONSOLE("FAUDES_QPAINT: " << message)
// dont paint
//#define FF_DQP(message) { return; };





/*
************************************************
************************************************
  
Implementation: FfSensor

************************************************
************************************************
*/

// default constructor
FfSensorItem::FfSensorItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfSensorItem::FfSensorItem()");
  setZValue(-10);
  setPos(0,0);
  setAcceptedMouseButtons(Qt::NoButton);
  mWpReference=0;
  mValue=false;
  mMute=false;
  mWpSensor=0;
}


// paint myself
void FfSensorItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  FF_DQP("FfSensorItem::paint(): " << mValue);
  (void) option;
  (void) widget;
  // prepare effect colors
  FfScene* sc = qobject_cast<FfScene*>(scene());
  if(!sc) return;
  // paint
  if(mValue) {
    painter->setBrush(Qt::NoBrush);
    painter->setPen(sc->EffectColor(VioGreen));
    painter->fillPath(mIndicatorPath,sc->EffectColor(VioGreen));
    painter->setPen(sc->EffectColor(VioBlack));
    painter->drawPath(mIndicatorPath);
  }
  if(!mValue) {
    painter->setPen(sc->EffectColor(VioBlack));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(mIndicatorPath);
  }
  // debug
  /*
  painter->setPen(sc->EffectColor(VioBlack));
  painter->setBrush(Qt::NoBrush);
  painter->drawPath(mOnPath);
  painter->drawPath(mOffPath);
  */
}

// updater
void FfSensorItem::Update(void) {
  FF_DQ("FfSensorItem::FfSensorItem(): update");
  prepareGeometryChange();
  mOuterRect=mIndicatorPath.boundingRect();
  mOuterRect.adjust(-2,-2,2,2);
  update();
}

// updater
void FfSensorItem::SetValue(bool val) {
  if(val==mValue) return;
  mValue=val;
  update();
}
 
/*
************************************************
************************************************
  
Implementation: FfRail

************************************************
************************************************
*/

// default constructor
FfRailItem::FfRailItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfRailItem::FfRailItem()");
  setZValue(-50);
  setPos(0,0);
  setAcceptedMouseButtons(Qt::NoButton);
}


// paint myself
void FfRailItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  FF_DQP("FfRailItem::paint()");
  (void) painter;
  (void) option;
  (void) widget;
  // prepare effect colors
  FfScene* sc = qobject_cast<FfScene*>(scene());
  if(!sc) return;
  // paint
  painter->setBrush(sc->EffectColor(VioRed));
  painter->setPen(sc->EffectColor(VioDarkRed));
  painter->drawPath(mRailPath);
}

// updater
void FfRailItem::SetUpper(int len) {
  prepareGeometryChange();
  mOuterRect=QRectF(-100,-80,len*200,30);
  mRailPath=QPainterPath();
  mRailPath.moveTo(-90,-50);
  mRailPath.lineTo(-90,-60);
  mRailPath.lineTo(-70,-80);
  mRailPath.lineTo(70+(len-1)*200,-80);
  mRailPath.lineTo(90+(len-1)*200,-60);
  mRailPath.lineTo(90+(len-1)*200,-50);
  mRailPath.lineTo(-90,-50);
  mRailPath.closeSubpath();
  mOuterRect.adjust(-2,-2,2,2);
  update();
}

// updater
void FfRailItem::SetUpperX(int len) {
  prepareGeometryChange();
  mOuterRect=QRectF(-100,-100,len*200,50);
  mRailPath=QPainterPath();
  mRailPath.moveTo(-90,-50);
  mRailPath.lineTo(-90,-60);
  mRailPath.lineTo(-80,-60);
  mRailPath.lineTo(-80,-70);
  mRailPath.lineTo(-70,-80);
  mRailPath.lineTo(70+(len-1)*200,-80);
  mRailPath.lineTo(90+(len-1)*200,-60);
  mRailPath.lineTo(90+(len-1)*200,-50);
  mRailPath.lineTo(-90,-50);
  mRailPath.closeSubpath();
  mOuterRect.adjust(-2,-2,2,2);
  update();
}


// updater
void FfRailItem::SetUpperP(int len) {
  int pcnt=len/2 +1;
  int glen=1;
  prepareGeometryChange();
  mOuterRect=QRectF(-100,-80,len*200,30);
  mRailPath=QPainterPath();
  //left
  mRailPath.moveTo(-90,-50);
  mRailPath.lineTo(-90,-60);
  mRailPath.lineTo(-70,-80);
  mRailPath.lineTo(-30,-80);
  mRailPath.lineTo(-50,-60);
  mRailPath.lineTo(-50,-50);
  mRailPath.closeSubpath();
  // middle
  for(int i=0; i<pcnt-1; i++) {
    mRailPath.moveTo(50+i*400,-50);
    mRailPath.lineTo(50+i*400,-60);
    mRailPath.lineTo(30+i*400,-80);
    mRailPath.lineTo(170+200*glen+i*400,-80);
    mRailPath.lineTo(150+200*glen+i*400,-60);
    mRailPath.lineTo(150+200*glen+i*400,-50);
    mRailPath.closeSubpath();
  }
  //left
  mRailPath.moveTo(50+(len-1)*200,-50);
  mRailPath.lineTo(50+(len-1)*200,-60);
  mRailPath.lineTo(30+(len-1)*200,-80);
  mRailPath.lineTo(70+(len-1)*200,-80);
  mRailPath.lineTo(90+(len-1)*200,-60);
  mRailPath.lineTo(90+(len-1)*200,-50);
  mRailPath.closeSubpath();
  mOuterRect.adjust(-2,-2,2,2);
  update();
}

// updater
void FfRailItem::SetUpperC(int len) {
  prepareGeometryChange();
  mOuterRect=QRectF(-100,-80,len*200,30);
  mRailPath=QPainterPath();
  mRailPath.moveTo(-90,-50);
  mRailPath.lineTo(-90,-70);
  mRailPath.lineTo(-80,-70);
  mRailPath.lineTo(-80,-80);
  mRailPath.lineTo(70+(len-1)*200,-80);
  mRailPath.lineTo(90+(len-1)*200,-60);
  mRailPath.lineTo(90+(len-1)*200,-50);
  mRailPath.lineTo(-90,-50);
  mRailPath.closeSubpath();
  mOuterRect.adjust(-2,-2,2,2);
  update();
}

// updater
void FfRailItem::SetLower(int len) {
  prepareGeometryChange();
  mOuterRect=QRectF(-100,50,len*200,30);
  mRailPath=QPainterPath();
  mRailPath.moveTo(-90,50);
  mRailPath.lineTo(90+(len-1)*200,50);
  mRailPath.lineTo(90+(len-1)*200,60);
  mRailPath.lineTo(70+(len-1)*200,80);
  mRailPath.lineTo(-70,80);
  mRailPath.lineTo(-90,60);
  mRailPath.lineTo(-90,50);
  mRailPath.closeSubpath();
  mOuterRect.adjust(-2,-2,2,2);
  update();
}

// updater
void FfRailItem::SetLowerP(int len) {
  int pcnt=len/2 +1;
  int glen=1;
  prepareGeometryChange();
  mOuterRect=QRectF(-100,50,len*200,30);
  mRailPath=QPainterPath();
  //left
  mRailPath.moveTo(-90,50);
  mRailPath.lineTo(-90,60);
  mRailPath.lineTo(-70,80);
  mRailPath.lineTo(-50,80);
  mRailPath.lineTo(-60,60);
  mRailPath.lineTo(-60,50);
  mRailPath.closeSubpath();
  // middle
  for(int i=0; i<pcnt-1; i++) {
    mRailPath.moveTo(60+i*400,50);
    mRailPath.lineTo(60+i*400,60);
    mRailPath.lineTo(50+i*400,80);
    mRailPath.lineTo(150+200*glen+i*400,80);
    mRailPath.lineTo(140+200*glen+i*400,60);
    mRailPath.lineTo(140+200*glen+i*400,50);
    mRailPath.closeSubpath();
  }
  //left
  mRailPath.moveTo(90+(len-1)*200,50);
  mRailPath.lineTo(90+(len-1)*200,60);
  mRailPath.lineTo(70+(len-1)*200,80);
  mRailPath.lineTo(50+(len-1)*200,80);
  mRailPath.lineTo(60+(len-1)*200,60);
  mRailPath.lineTo(60+(len-1)*200,50);
  mRailPath.closeSubpath();

  mOuterRect.adjust(-2,-2,2,2);
  update();
}


// updater
void FfRailItem::SetLowerX(int len) {
  prepareGeometryChange();
  prepareGeometryChange();
  mOuterRect=QRectF(-100,50,len*200,30);
  mRailPath=QPainterPath();
  mRailPath.moveTo(-90,50);
  mRailPath.lineTo(-90,60);
  mRailPath.lineTo(-80,60);
  mRailPath.lineTo(-80,70);
  mRailPath.lineTo(-70,80);
  mRailPath.lineTo(70+(len-1)*200,80);
  mRailPath.lineTo(90+(len-1)*200,60);
  mRailPath.lineTo(90+(len-1)*200,50);
  mRailPath.lineTo(-90,50);
  mRailPath.closeSubpath();
  mOuterRect.adjust(-2,-2,2,2);
  update();
}

// updater
void FfRailItem::SetLeftHorseShoe(int len) {
  prepareGeometryChange();
  mOuterRect=QRectF(-100,-80,len*200,160);
  mRailPath=QPainterPath();
  mRailPath.moveTo(90+(len-1)*200,-50);
  mRailPath.lineTo(90+(len-1)*200,-60);
  mRailPath.lineTo(70+(len-1)*200,-80);
  mRailPath.lineTo(-70,-80);
  mRailPath.lineTo(-90,-60);
  mRailPath.lineTo(-90, 60);
  mRailPath.lineTo(-70, 80);
  mRailPath.lineTo(70+(len-1)*200, 80);
  mRailPath.lineTo(90+(len-1)*200, 60);
  mRailPath.lineTo(90+(len-1)*200, 50);
  mRailPath.lineTo(-60, 50);
  mRailPath.lineTo(-60, -50);
  mRailPath.lineTo(90+(len-1)*200,-50);
  mRailPath.closeSubpath();
  mOuterRect.adjust(-2,-2,2,2);
  update();
}

// updater
void FfRailItem::SetRightHorseShoe(int len) {
  prepareGeometryChange();
  mOuterRect=QRectF(-100-(len-1)*200,-80,len*200,160);
  mRailPath=QPainterPath();
  mRailPath.moveTo(-90-(len-1)*200,-50);
  mRailPath.lineTo(-90-(len-1)*200,-60);
  mRailPath.lineTo(-70-(len-1)*200,-80);
  mRailPath.lineTo(80,-80);
  mRailPath.lineTo(100,-60);
  mRailPath.lineTo(100, 60);
  mRailPath.lineTo(80, 80);
  mRailPath.lineTo(-70-(len-1)*200, 80);
  mRailPath.lineTo(-90-(len-1)*200, 60);
  mRailPath.lineTo(-90-(len-1)*200, 50);
  mRailPath.lineTo(70, 50);
  mRailPath.lineTo(70, -50);
  mRailPath.lineTo(-90-(len-1)*200,-50);
  mRailPath.closeSubpath();
  mOuterRect.adjust(-2,-2,2,2);
  update();
}

// updater
void FfRailItem::SetRightOpen(int len) {
  prepareGeometryChange();
  mOuterRect=QRectF(-100-(len-1)*200,-80,len*200,160);
  mRailPath=QPainterPath();
  mRailPath.moveTo(-90-(len-1)*200,-50);
  mRailPath.lineTo(-90-(len-1)*200,-60);
  mRailPath.lineTo(-70-(len-1)*200,-80);
  mRailPath.lineTo(70,-80);
  mRailPath.lineTo(90,-60);
  mRailPath.lineTo(90,-50);
  mRailPath.closeSubpath();
  mRailPath.moveTo(-90-(len-1)*200,50);
  mRailPath.lineTo(-90-(len-1)*200,60);
  mRailPath.lineTo(-70-(len-1)*200,80);
  mRailPath.lineTo(70,80);
  mRailPath.lineTo(90,60);
  mRailPath.lineTo(90,50);
  mRailPath.closeSubpath();
  mOuterRect.adjust(-2,-2,2,2);
  update();
}


/*
************************************************
************************************************
  
Implementation: FfSimControlsItem

************************************************
************************************************
*/

// default constructor
FfSimControlsItem::FfSimControlsItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfSimControlsItem::FfSimControlsItem()");
  setZValue(20);
  setPos(0,0);
  setAcceptsHoverEvents(true);
  mVisible=true;
}


// paint myself
void FfSimControlsItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  FF_DQP("FfSimControlItem::paint()");
  (void) painter;
  (void) option;
  (void) widget;
  // bail out
  if(!mVisible) return;
  // paint
  QColor pale=FfStyle::Color(VioGreen);
  pale.setAlphaF(0.50);
  painter->setPen(FfStyle::Color(VioDarkGreen));
  painter->setBrush(pale);
  painter->drawPath(mCoreShape);
}

// updater
void FfSimControlsItem::Update(void) {
  FF_DQ("FfSimControlsItem::FfSimControlsItem(): update in scene " << scene());
  prepareGeometryChange();
  mOuterRect=mCoreShape.boundingRect();
  mOuterRect.adjust(-2,-2,2,2); // must have larget shape than item
  mOuterShape= QPainterPath();
  mOuterShape.addRect(mOuterRect);
  update();
}

// catch mouse
void FfSimControlsItem::hoverEnterEvent(QGraphicsSceneHoverEvent * ev) {
  //FF_DQ("FfSimControlsItem::HoverEnter()");
  SetVisible(true);
  QGraphicsItem::hoverEnterEvent(ev);
}
void FfSimControlsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * ev) {
  //FF_DQ("FfSimControlsItem::HoverLeave()");
  SetVisible(false);
  QGraphicsItem::hoverLeaveEvent(ev);
}
void FfSimControlsItem::hoverMoveEvent(QGraphicsSceneHoverEvent * ev) {
  QGraphicsItem::hoverMoveEvent(ev);
}

// my visible 
// (to hack issues with mac osx mouse hover, we
// formaly remain visible)
void FfSimControlsItem::SetVisible(bool vis) {
  if(mVisible==vis) return;
  mVisible=vis;
  foreach(QGraphicsItem* item, childItems())
    item->setVisible(vis);
  update();
}
 
/*
************************************************
************************************************
  
Implementation: FfSelectedItem

************************************************
************************************************
*/

// default constructor
FfSelectedItem::FfSelectedItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfSelectedItem::FfSelectedItem()");
  setZValue(20);
  setPos(0,0);
}


// paint myself
void FfSelectedItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  FF_DQP("FfSimControlItem::paint()");
  // figure my component parent
  //FfComponent* comp = qgraphicsitem_cast<FfComponent*>(parentItem()); // not functional
  FfComponent* comp = dynamic_cast<FfComponent*>(parentItem());
  if(!comp) return;
  if(!comp->isSelected()) return;
  // paint
  FF_DQP("FfSelectedItem::paint(): " << comp->Name());
  if(comp->GoodPosition()) {
    QColor pale=FfStyle::Color(VioGreen);
    pale.setAlphaF(0.25);
    painter->fillPath(mCoreShape,pale);
    painter->setPen(FfStyle::Color(VioDarkGreen));
    painter->drawPath(mCoreShape);
  } else {
    QColor pale=FfStyle::Color(VioRed);
    pale.setAlphaF(0.25);
    painter->fillPath(mCoreShape,pale);
    painter->setPen(FfStyle::Color(VioDarkRed));
    painter->drawPath(mCoreShape);
  }

}

// updater
void FfSelectedItem::Update(void) {
  FF_DQ("FfSelectedItem::FfSelectedItem(): update in scene " << scene());
  prepareGeometryChange();
  mOuterRect=mCoreShape.boundingRect();
  mOuterRect.adjust(-15,-15,15,15);
  mOuterShape= QPainterPath();
  mOuterShape.addRect(mOuterRect);
  update();
}

 
/*
************************************************
************************************************
  
Implementation: FfButtonItem

************************************************
************************************************
*/

// default constructor
FfButtonItem::FfButtonItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfButtonItem::FfButtonItem()");
  setZValue(22);
  setPos(0,0);
  setAcceptsHoverEvents(true);
  mUnderMouse=false;
  SetColorGreen();
}


// color style
void FfButtonItem::SetColorGreen(void) {
  mNormColor= FfStyle::Color(VioGreen);
  mLightColor= FfStyle::Color(VioLightGreen);
  mDarkColor= FfStyle::Color(VioDarkGreen);
  update();
}

// color style
void FfButtonItem::SetColorRed(void) {
  mNormColor= FfStyle::Color(VioLightRed);
  mLightColor= FfStyle::Color(VioRed);
  mDarkColor= FfStyle::Color(VioDarkRed);
  update();
}

// catch mouse
// NOTE for some reasons the isUnderMouse() isn't functional (Mac OSx)
void FfButtonItem::hoverEnterEvent(QGraphicsSceneHoverEvent * ev) {
  //FF_DQ("FfButtonItem::HoverEnter()");
  mUnderMouse=true;
  QGraphicsItem::hoverEnterEvent(ev);
}
void FfButtonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * ev) {
  //FF_DQ("FfButtonItem::HoverLeave()");  
  mUnderMouse=false;
  QGraphicsItem::hoverLeaveEvent(ev);
}
void FfButtonItem::hoverMoveEvent(QGraphicsSceneHoverEvent * ev) {
  QGraphicsItem::hoverMoveEvent(ev);
}


// paint myself
void FfButtonItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  FF_DQP("FfSimButtonItem::paint()" << mText);
  // paint             
  if(!mUnderMouse) {
    painter->setBrush(mLightColor);
    painter->setPen(mDarkColor);
    painter->drawPath(mCoreShape);
  } else {
    painter->setBrush(mNormColor);
    painter->setPen(mLightColor);
    painter->drawPath(mCoreShape);
  }
}

// updater
void FfButtonItem::Update(void) {
  FF_DQ("FfButtonItem::FfButtonItem(): update");
  prepareGeometryChange();
  mOuterRect=mCoreShape.boundingRect();
  mOuterRect.adjust(-2,-2,2,2);
  mOuterShape= QPainterPath();
  mOuterShape.addRect(mOuterRect);
  update();
}

// prestes
void FfButtonItem::SetLeft(const QPointF& pos) {
  setPos(pos);
  mText="Left";
  mCoreShape = QPainterPath();
  mCoreShape.moveTo(+25,-25);
  mCoreShape.lineTo(-25, 0);
  mCoreShape.lineTo(+25,+25);
  mCoreShape.closeSubpath();
  mOuterShape=mCoreShape;
  Update();
}
 
// prestes
void FfButtonItem::SetRight(const QPointF& pos) {
  setPos(pos);
  mText="Right";
  mCoreShape = QPainterPath();
  mCoreShape.moveTo(-25,-25);
  mCoreShape.lineTo(25, 0);
  mCoreShape.lineTo(-25,25);
  mCoreShape.closeSubpath();
  mOuterShape=mCoreShape;
  Update();
}

// prestes
void FfButtonItem::SetUp(const QPointF& pos) {
  setPos(pos);
  mText="Up";
  mCoreShape = QPainterPath();
  mCoreShape.moveTo(+25,-25);
  mCoreShape.lineTo(-25, 0);
  mCoreShape.lineTo(+25,+25);
  mCoreShape.closeSubpath();
  QTransform trans;
  trans.rotate(90);
  mCoreShape=trans.map(mCoreShape);
  mOuterShape=mCoreShape;
  Update();
}
 
// prestes
void FfButtonItem::SetDown(const QPointF& pos) {
  setPos(pos);
  mText="Down";
  mCoreShape = QPainterPath();
  mCoreShape.moveTo(+25,-25);
  mCoreShape.lineTo(-25, 0);
  mCoreShape.lineTo(+25,+25);
  mCoreShape.closeSubpath();
  QTransform trans;
  trans.rotate(-90);
  mCoreShape=trans.map(mCoreShape);
  mOuterShape=mCoreShape;
  Update();
}
 
// prestes
void FfButtonItem::SetRightSkip(const QPointF& pos) {
  setPos(pos);
  mText="RightSkip";
  mCoreShape = QPainterPath();
  mCoreShape.moveTo(-25,-25);
  mCoreShape.lineTo(12,  -5);
  mCoreShape.lineTo(12, -25);
  mCoreShape.lineTo(22, -25);
  mCoreShape.lineTo(22,  25);
  mCoreShape.lineTo(12,  25);
  mCoreShape.lineTo(12,   5);
  mCoreShape.lineTo(-25,25);
  mCoreShape.closeSubpath();
  mOuterShape=mCoreShape;
  Update();
}

// prestes
void FfButtonItem::SetDownSkip(const QPointF& pos) {
  setPos(pos);
  mText="DownSkip";
  mCoreShape = QPainterPath();
  mCoreShape.moveTo(-25,-25);
  mCoreShape.lineTo(12,  -5);
  mCoreShape.lineTo(12, -25);
  mCoreShape.lineTo(22, -25);
  mCoreShape.lineTo(22,  25);
  mCoreShape.lineTo(12,  25);
  mCoreShape.lineTo(12,   5);
  mCoreShape.lineTo(-25,25);
  mCoreShape.closeSubpath();
  QTransform trans;
  trans.rotate(90);
  mCoreShape=trans.map(mCoreShape);
  mOuterShape=mCoreShape;
  Update();
}

// prestes
void FfButtonItem::SetLeftSkip(const QPointF& pos) {
  setPos(pos);
  mText="DownSkip";
  mCoreShape = QPainterPath();
  mCoreShape.moveTo(-25,-25);
  mCoreShape.lineTo(12,  -5);
  mCoreShape.lineTo(12, -25);
  mCoreShape.lineTo(22, -25);
  mCoreShape.lineTo(22,  25);
  mCoreShape.lineTo(12,  25);
  mCoreShape.lineTo(12,   5);
  mCoreShape.lineTo(-25,25);
  mCoreShape.closeSubpath();
  QTransform trans;
  trans.rotate(180);
  mCoreShape=trans.map(mCoreShape);
  mOuterShape=mCoreShape;
  Update();
}


// prestes
void FfButtonItem::SetMinus(const QPointF& pos) {
  setPos(pos);
  mText="Minus";
  mCoreShape = QPainterPath();
  mCoreShape.addRect(-25,-7,50,14);
  mOuterShape= QPainterPath();
  mOuterShape.addRect(-25,-25,50,50);
  Update();
}

// prestes
void FfButtonItem::SetPlus(const QPointF& pos) {
  setPos(pos);
  mText="Plus";
  mCoreShape = QPainterPath();
  mCoreShape.moveTo(-25,-7);
  mCoreShape.lineTo(-7, -7);
  mCoreShape.lineTo(-7, -25);
  mCoreShape.lineTo(7,  -25);
  mCoreShape.lineTo(7, -7);
  mCoreShape.lineTo(25, -7);
  mCoreShape.lineTo(25,  7);
  mCoreShape.lineTo(7,  7);
  mCoreShape.lineTo(7,  25);
  mCoreShape.lineTo(-7,  25);
  mCoreShape.lineTo(-7,  7);
  mCoreShape.lineTo(-25,  7);
  mCoreShape.closeSubpath();
  mOuterShape= QPainterPath();
  mOuterShape.addRect(mCoreShape.boundingRect());
  Update();
}


// prestes
void FfButtonItem::SetStop(const QPointF& pos) {
  setPos(pos);
  mText="Stop";
  mCoreShape = QPainterPath();
  mCoreShape.addRect(-20,-20,40,40);
  mOuterShape=mCoreShape;
  Update();
}
 
// prestes
void FfButtonItem::SetLargeCircle(const QPointF& pos) {
  setPos(pos);
  mText="LargeCircle";
  mCoreShape = QPainterPath();
  mCoreShape.addEllipse(-40,-40,80,80);
  mOuterShape=mCoreShape;
  Update();
}
 
// prestes
void FfButtonItem::SetCircle(const QPointF& pos) {
  setPos(pos);
  mText="Circle";
  mCoreShape = QPainterPath();
  mCoreShape.addEllipse(-25,-25,50,50);
  mOuterShape=mCoreShape;
  Update();
}
 

/*
************************************************
************************************************
  
Implementation: FfDigitItem

************************************************
************************************************
*/

// default constructor
FfDigitItem::FfDigitItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfDigitItem::FfDigitItem()");
  setZValue(5);
  mDigit=-1;
  setPos(0,0);
  setAcceptedMouseButtons(Qt::NoButton);
}


// paint myself
void FfDigitItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // prepare effect colors
  FfScene* sc = qobject_cast<FfScene*>(scene());
  if(!sc) return;
  // paint
  FF_DQP("FfDigitItem::paint()");
  QColor pale=sc->EffectColor(VioDarkRed);
  pale.setAlphaF(0.750);
  painter->fillPath(mCoreShape,pale);
  painter->setPen(sc->EffectColor(VioBlack));
  painter->drawPath(mCoreShape);
}

// set digit
void FfDigitItem::SetDigit(int num) {
  if(mDigit==num) return;
  mDigit=num;
  Update();
}

// updater
void FfDigitItem::Update(void) {
  FF_DQ("FfDigitItem::FfDigitItem(): update");
  prepareGeometryChange();
  // stage 1: draw the text
  mCoreShape=QPainterPath();
  mCoreShape.addText(QPointF(0,0),FfStyle::DefaultHugeFont(),QString("#%1").arg(mDigit));
  mOuterRect=mCoreShape.boundingRect();
  // stage 2: scale the text
  qreal dst = mSize;
  qreal src = mOuterRect.width();
  if(src>1) {
    QTransform sc;
    sc.scale(dst/src,dst/src);
    mCoreShape=sc.map(mCoreShape);
    mOuterRect=mCoreShape.boundingRect();
  }
  // stage 3: figure center an fix
  QTransform tr;
  tr.translate(-mOuterRect.center().x(),-mOuterRect.center().y());
  mCoreShape=tr.map(mCoreShape);
  mOuterRect=mCoreShape.boundingRect();
  // fix outer rect
  mOuterRect.adjust(-2,-2,2,2);
  update();
}

/*
************************************************
************************************************
  
Implementation: FfBeltItem

************************************************
************************************************
*/

// default constructor
FfBeltItem::FfBeltItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfBeltItem::FfBeltItem()");
  // configure
  setZValue(-50);
  SetLength(1);
  setAcceptedMouseButtons(Qt::NoButton);
}

// paint myself
void FfBeltItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // prepare effect colors
  FfScene* sc = qobject_cast<FfScene*>(scene());
  if(!sc) return;
  FF_DQP("FfBeltItem::paint()");
  // set up pens and brushes
  mGreyBrush = QBrush(sc->EffectColor(VioGrey));
  mBlackPen  = QPen(sc->EffectColor(VioBlack));
  mLightGreyPen  = QPen(sc->EffectColor(VioLightGrey));
  // shape
  painter->setPen(mBlackPen);
  painter->setBrush(mGreyBrush);
  painter->drawRect(mCoreRect);
  // texture
  qreal max=200*(mLen-1) + 90;
  for(qreal x=-110+mPos; x<max; x+=20) {
    if(x+1>=-90 && x+1< max) {
      painter->setPen(mLightGreyPen);
      painter->drawLine(QPointF(x+1,-50),QPointF(x+1,50));
    }
    if(x+2>=-90 && x+2< max) {
      painter->setPen(mBlackPen);
      painter->drawLine(QPointF(x+2,-50),QPointF(x+2,50));
    }
  }
}

// set len
void FfBeltItem::SetLength(int len) {
  mLen=len;
  mPos=0;
  mBaseTrans.reset();
  Update();
}


// set base transform
void FfBeltItem::SetTransform(const QTransform& btrans) {
  mBaseTrans=btrans;
  setTransform(btrans);
  SetPosition(mPos);
}


// shift
void FfBeltItem::SetPosition(qreal pos) {
  mPos=pos;
  update();
}

// updater
void FfBeltItem::Update(void) {
  FF_DQ("FfBeltItem::Update");
  prepareGeometryChange();
  // outer rect
  mCoreRect=QRect(-90,-50,200*mLen-20,100);
  mOuterRect=mCoreRect.adjusted(-2,-2,2,2);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  update();
}
 
 
/*
************************************************
************************************************
  
Implementation: FfPushBeltItem

************************************************
************************************************
*/

// default constructor
FfPushBeltItem::FfPushBeltItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfPushBeltItem::FfPushBeltItem()");
  // configure
  setZValue(-50);
  SetPosition(-50);
  setAcceptedMouseButtons(Qt::NoButton);
  Update();
}

// paint myself
void FfPushBeltItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // prepare effect colors
  FfScene* sc = qobject_cast<FfScene*>(scene());
  if(!sc) return;
  FF_DQP("FfPushBeltItem::paint()");
  // set up pens and brushes
  mGreyBrush = QBrush(sc->EffectColor(VioGrey));
  mBlackPen  = QPen(sc->EffectColor(VioBlack));
  mLightGreyPen  = QPen(sc->EffectColor(VioLightGrey));
  // shape
  painter->setPen(mBlackPen);
  painter->setBrush(mGreyBrush);
  painter->drawRect(mCoreRect);
  // texture
  qreal relpos = mPos +50;
  relpos = relpos - ((int) (relpos / 20.0))*20; 
  for(qreal x=-80+relpos; x<=90; x+=20) {
    if(x+1>=-60 && x+1< 90) {
      painter->setPen(mLightGreyPen);
      painter->drawLine(QPointF(x+1,-50),QPointF(x+1,50));
    }
    if(x+2>=-60 && x+2< 90) {
      painter->setPen(mBlackPen);
      painter->drawLine(QPointF(x+2,-50),QPointF(x+2,50));
    }
  }
  // separator
  painter->setPen(mBlackPen);
  painter->setBrush(sc->EffectColor(VioLightGrey));
  painter->drawRect(mSeparator);
  // bar
  if(mPos <=86) {
    painter->setPen(mBlackPen);
    painter->setBrush(sc->EffectColor(VioRed));
    painter->drawRect(mPos-10,-50,14,100);
  } else {
    qreal aw = 100-mPos;
    qreal bw = 14-aw;
    painter->setPen(mBlackPen);
    painter->setBrush(sc->EffectColor(VioRed));
    painter->drawRect(mPos-10,-50,aw,100);
    painter->drawRect(-60,-50,bw,100);
  }
}


// set base transform
void FfPushBeltItem::SetTransform(const QTransform& btrans) {
  mBaseTrans=btrans;
  setTransform(btrans);
  Update();
}


// shift
void FfPushBeltItem::SetPosition(qreal pos) {
  mPos=pos;
  update();
}

// updater
void FfPushBeltItem::Update(void) {
  FF_DQ("FfPushBeltItem::Update");
  prepareGeometryChange();
  // outer rect
  mCoreRect=QRect(-60,-50,150,100);
  mOuterRect=mCoreRect.adjusted(0,2,0,-2);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  // separateor
  mSeparator=QRect(-60,-25,150,50);
  update();
}
 
 

/*
************************************************
************************************************
  
Implementation: FfErrIndicatorItem

************************************************
************************************************
*/

// default constructor
FfErrIndicatorItem::FfErrIndicatorItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfErrIndicatorItem::FfErrIndicatorItem()");
  setZValue(30);
  setPos(0,0);
  setAcceptsHoverEvents(true);
}


// paint myself
void FfErrIndicatorItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  FF_DQP("FfSimControlItem::paint()");
  (void) painter;
  (void) option;
  (void) widget;
  // paint
  QColor pale=FfStyle::Color(VioRed);
  pale.setAlphaF(0.50);
  painter->fillPath(mCoreShape,pale);
  painter->setPen(FfStyle::Color(VioDarkRed));
  painter->drawPath(mCoreShape);
}

// updater
void FfErrIndicatorItem::Update(void) {
  FF_DQ("FfErrIndicatorItem::FfErrIndicatorItem(): update in scene " << scene());
  prepareGeometryChange();
  mOuterRect=mCoreShape.boundingRect();
  mOuterRect.adjust(-2,-2,2,2); // must have larget shape than item
  mOuterShape= QPainterPath();
  mOuterShape.addRect(mOuterRect);
  update();
}

// preset: wp circle
void FfErrIndicatorItem::SetWorkpieceError(void) {
  mCoreShape = QPainterPath();
  mCoreShape.addEllipse(-120,-120,240,240);
  Update();
}


/*
************************************************
************************************************
  
Implementation: FfPusher

************************************************
************************************************
*/

// default constructor
FfPusherItem::FfPusherItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfPusherItem::FfPusherItem()");
  setZValue(-10);
  setPos(0,0);
  setAcceptedMouseButtons(Qt::NoButton);
  mPos=0;
  mSensorA = new FfSensorItem(this);
  mSensorB = new FfSensorItem(this);
  mSensorWp = new FfSensorItem(this);  
  Update();
}


// paint myself
void FfPusherItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  FF_DQP("FfPusherItem::paint()");
  (void) option;
  (void) widget;
  // prepare effect colors
  FfScene* sc = qobject_cast<FfScene*>(scene());
  if(!sc) return;
  // paint
  painter->setBrush(sc->EffectColor(VioLightGrey));
  painter->setPen(sc->EffectColor(VioGrey));
  painter->drawRect(mOrigin.x()-20,mOrigin.y()-30-170+mPos,10,170);
  painter->drawRect(mOrigin.x()+10,mOrigin.y()-30-170+mPos,10,170);
  painter->setBrush(sc->EffectColor(VioRed));
  painter->setPen(sc->EffectColor(VioDarkRed));
  painter->drawRect(mOrigin.x()-30,mOrigin.y()-50-170+mPos,60,20);
  painter->save();
  painter->translate(mOrigin.x(),mOrigin.y()+mPos);
  painter->setBrush(sc->EffectColor(VioRed));
  painter->setPen(sc->EffectColor(VioDarkRed));
  painter->drawPath(mPusherPath);
  painter->restore();
  painter->setBrush(sc->EffectColor(VioGrey));
  painter->setPen(sc->EffectColor(VioBlack));
  painter->drawPath(mForegroundPath);

}


// location
void FfPusherItem::SetOrigin(int pos) {
  mOrigin=QPointF(200*pos,-50);
  Update();
}

// shift
void FfPusherItem::SetPosition(qreal pos) {
  mPos=pos;
  mSensorB->SetValue(mPos<1);
  mSensorA->SetValue(mPos>139);
  mSensorWp->setPos(mOrigin+QPointF(0,mPos-10));
  update();
}

// updater
void FfPusherItem::Update(void) {
  FF_DQ("FfPusherItem::Update");
  prepareGeometryChange();
  // origin translate
  QTransform trorg;
  trorg.translate(mOrigin.x(),mOrigin.y());
  // pusher shape
  mPusherPath=QPainterPath();
  mPusherPath.moveTo(-50,0);
  mPusherPath.lineTo(-50,-10);
  mPusherPath.lineTo(-30,-30);
  mPusherPath.lineTo(+30,-30);
  mPusherPath.lineTo(+50,-10);
  mPusherPath.lineTo(+50,0);
  mPusherPath.closeSubpath();
  // background shape
  mBackgroundPath=QPainterPath();
  mBackgroundPath.moveTo(-50,0);
  mBackgroundPath.lineTo(-50,-10);
  mBackgroundPath.lineTo(-30,-30);
  mBackgroundPath.lineTo(+30,-30);
  mBackgroundPath.lineTo(+50,-10);
  mBackgroundPath.lineTo(+50,0);
  mBackgroundPath.closeSubpath();
  mBackgroundPath.moveTo(-60,100);
  mBackgroundPath.lineTo(-60,110);
  mBackgroundPath.lineTo(-50,130);
  mBackgroundPath.lineTo(-45,140);
  mBackgroundPath.lineTo(+45,140);
  mBackgroundPath.lineTo(+50,130);
  mBackgroundPath.lineTo(+60,110);
  mBackgroundPath.lineTo(+60,100);
  mBackgroundPath.closeSubpath();
  mBackgroundPath=trorg.map(mBackgroundPath);
  // background shape
  mForegroundPath=QPainterPath();
  mForegroundPath.moveTo(-30,-30);
  mForegroundPath.lineTo(-30,-60);
  mForegroundPath.lineTo( 30,-60);
  mForegroundPath.lineTo( 30,-30);
  mForegroundPath.closeSubpath();
  mForegroundPath=trorg.map(mForegroundPath);
  // transport
  mRail=QLineF(0,0,0,180);
  mRail= trorg.map(mRail);
  mNormRail=FfStyle::NormalizeF(mRail.p2()-mRail.p1());
  // outer rect
  mCoreRect=QRect(mOrigin.x()-50,mOrigin.y()-220,100,380);
  mOuterRect=mCoreRect.adjusted(-2,-2,2,2);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  // my sensors (no childitems)
  mSensorA->mIndicatorPath=QPainterPath();
  mSensorA->mIndicatorPath.addRect(-30,-60,10,10);
  mSensorA->mIndicatorPath=trorg.map(mSensorA->mIndicatorPath);
  mSensorA->mWpSensor=false;
  mSensorA->mRelAddress=-1;
  mSensorA->Update();
  mSensorB->mIndicatorPath=QPainterPath();
  mSensorB->mIndicatorPath.addRect(-30,-40,10,10);
  mSensorB->mIndicatorPath=trorg.map(mSensorB->mIndicatorPath);
  mSensorB->mWpSensor=false;
  mSensorB->mRelAddress=-1;
  mSensorB->Update();
  mSensorWp->mIndicatorPath=QPainterPath();
  mSensorWp->mIndicatorPath.addRect(-10,-5,20,10);
  mSensorWp->mOffPath=QPainterPath();
  mSensorWp->mOffPath.addRect(-30,30,60,60);
  mSensorWp->mOnPath=QPainterPath();
  mSensorWp->mOnPath.addRect(-10,50,20,20);
  mSensorWp->mWpSensor=true;
  mSensorWp->mValue=false;
  mSensorWp->mRelAddress=-1;
  mSensorWp->Update();
  update();
}




/*
************************************************
************************************************
  
Implementation: FfConveyor

************************************************
************************************************
*/

// default constructor
FfConveyorItem::FfConveyorItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfConveyorItem::FfConveyorItem()");
  setZValue(-20);
  setPos(0,0);
  setAcceptedMouseButtons(Qt::NoButton);
  mBeltPos=0;
  mOrigin=QPointF(0,0);
  // fix orientation
  QTransform tcw;
  tcw.rotate(90);
  // my items
  mSensorWp  = new FfSensorItem(this);
  mSensorWp->mOffPath.addRect(-30,-30,60,60);
  mSensorWp->mOnPath.addRect(-10,-10,20,20);
  mSensorWp->mIndicatorPath.addRect(-10,55,20,10);
  mSensorWp->mWpSensor=true;
  mSensorWp->mValue=false;
  mSensorWp->mRelAddress=-1;
  mSensorWp->setTransform(tcw);
  mSensorWp->Update();
  mLeftRail  = new FfRailItem(this); 
  mLeftRail->SetUpperC();
  mLeftRail->setTransform(tcw);
  mRightRail = new FfRailItem(this); 
  mRightRail->setTransform(tcw);
  mRightRail->SetLower();
  mBelt      = new FfBeltItem(this); 
  mBelt->setTransform(tcw);
  // bounding box
  mOuterRect=QRectF(-90,-90,180,180);
  mOuterShape.addRect(mOuterRect);
  mOuterRect.adjust(-2,-2,2,2);
  // transport
  mRail=QLineF(0,-100,0,100);
  mNormRail=FfStyle::NormalizeF(mRail.p2()-mRail.p1());
  mOrthRail= (-1) * FfStyle::NormalF(mNormRail);
  // invalidate buttons
  mButtonL=0;
  mButtonR=0;
  mButtonU=0;
  mButtonD=0;
  mButtonStop=0;
  // dynamics
  Update();
}


// paint myself
void FfConveyorItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  FF_DQP("FfConveyorItem::paint()");
  (void) option;
  (void) widget;
  (void) painter;
}


// orientation
void FfConveyorItem::SetTransform(const QTransform& trans) {
  mTrans=trans;
  setPos(mTrans.map(mOrigin));
  setTransform(trans);
}

// location
void FfConveyorItem::SetOrigin(qreal pos) {
  mOrigin=QPointF(pos,-0);
  setPos(mTrans.map(mOrigin));
  if(mButtonStop) 
    mButtonStop->setPos(mOrigin);
  if(mButtonL) 
    mButtonL->setPos(mOrigin+QPointF(-55,0));
  if(mButtonR) 
    mButtonR->setPos(mOrigin+QPointF(55,0));
  if(mButtonU) 
    mButtonU->setPos(mOrigin+QPointF(0,-55));
  if(mButtonD) 
    mButtonD->setPos(mOrigin+QPointF(0,55));
}

// shift
void FfConveyorItem::SetBeltPos(qreal pos) {
  mBeltPos=pos;
  mBelt->SetPosition(mBeltPos);
}

// updater
void FfConveyorItem::Update(void) {
  FF_DQ("FfConveyorItem::Update");
  update();
}



/*
************************************************
************************************************
  
Implementation: FfProcessor

************************************************
************************************************
*/

// default constructor
FfProcessorItem::FfProcessorItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfProcessorItem::FfProcessorItem()");
  setZValue(-10);
  setPos(0,0);
  setAcceptedMouseButtons(Qt::NoButton);
  mPos=0;
  mProg=0;
  Update();
}


// paint myself
void FfProcessorItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  FF_DQP("FfProcessorItem::paint()");
  (void) option;
  (void) widget;
  // prepare effect colors
  FfScene* sc = qobject_cast<FfScene*>(scene());
  if(!sc) return;
  // paint
  painter->save();
  painter->translate(0,mPos);
  // frame
  QColor pale=sc->EffectColor(VioDarkGrey);
  pale.setAlphaF(0.70);
  painter->setBrush(pale);
  painter->setPen(Qt::NoPen);
  painter->drawPath(mInnerFrame);
  painter->setPen(sc->EffectColor(VioDarkRed));
  painter->setBrush(sc->EffectColor(VioRed));
  painter->drawPath(mOuterFrame);
  // pie
  painter->setPen(sc->EffectColor(VioDarkGrey));
  painter->setBrush(sc->EffectColor(VioGrey));
  painter->drawEllipse(0-30,-130-30,60,60);
  if(mProg>=1){
    painter->setPen(sc->EffectColor(VioDarkGrey));
    painter->setBrush(sc->EffectColor(VioLightGrey));
    painter->drawPie(0-30,-130-30,60,60,90*16,-mProg/100.0*16*360);
  } 
  if(mProg>=99){
    painter->setPen(sc->EffectColor(VioDarkGrey));
    painter->setBrush(sc->EffectColor(VioLightGrey));
    painter->drawEllipse(0-30,-130-30,60,60);
  }
  painter->restore();
}


// shift
void FfProcessorItem::SetPosition(qreal pos) {
  mPos=pos;
  update();
}

// shift
void FfProcessorItem::SetProgress(qreal prog) {
  mProg=prog;
  update();
}


// updater
void FfProcessorItem::Update(void) {
  FF_DQ("FfProcessorItem::Update");
  prepareGeometryChange();
  // processor shape
  mOuterFrame=QPainterPath();
  mOuterFrame.moveTo(-65,-65);
  mOuterFrame.lineTo(65,-65);
  mOuterFrame.lineTo(65,-195);
  mOuterFrame.lineTo(-65,-195);
  mOuterFrame.closeSubpath();
  mOuterFrame.moveTo(-50,-80);
  mOuterFrame.lineTo(50,-80);
  mOuterFrame.lineTo(50,-180);
  mOuterFrame.lineTo(-50,-180);
  mOuterFrame.closeSubpath();
  mInnerFrame=QPainterPath();
  mInnerFrame.moveTo(-50,-80);
  mInnerFrame.lineTo(50,-80);
  mInnerFrame.lineTo(50,-180);
  mInnerFrame.lineTo(-50,-180);
  mInnerFrame.closeSubpath();
  // outer rect
  mCoreRect=QRect(-60,-190,120,260);
  mOuterRect=mCoreRect.adjusted(-6,-6,6,6);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  update();
}



/*
************************************************
************************************************
  
Implementation: FfUnivtool

************************************************
************************************************
*/

// default constructor
FfUnivtoolItem::FfUnivtoolItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfUnivtoolItem::FfUnivtoolItem()");
  setZValue(-20);
  setPos(0,0);
  setAcceptedMouseButtons(Qt::NoButton);
  mTsel=0;
  mProg=0;
  Update();
}


// paint myself
void FfUnivtoolItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  FF_DQP("FfUnivtoolItem::paint()");
  (void) option;
  (void) widget;
  // prepare effect colors
  FfScene* sc = qobject_cast<FfScene*>(scene());
  if(!sc) return;
  QColor pale=sc->EffectColor(VioDarkGrey);
  pale.setAlphaF(0.70);
  // outer disc
  //painter->setPen(sc->EffectColor(VioDarkGrey));
  //painter->setBrush(pale);
  //painter->drawEllipse(-90,-90,180,180);
  // revolver
  painter->setPen(sc->EffectColor(VioGrey));
  painter->setBrush(sc->EffectColor(VioLightGrey));
  painter->drawPath(mRevolverPath);
  // tools
  painter->setPen(sc->EffectColor(VioDarkGrey));
  painter->setBrush(sc->EffectColor(VioDarkGrey));
  painter->drawPath(mToolsPath);
}


// select tool
void FfUnivtoolItem::SetToolsel(qreal tsel) {
  mTsel=tsel;
  mTransTsel.reset();
  mTransTsel.rotate(tsel);
  setTransform(mTransTsel);
  update();
}

// operate
void FfUnivtoolItem::SetProgress(qreal prog) {
  mProg=prog;
  update();
}


// updater
void FfUnivtoolItem::Update(void) {
  FF_DQ("FfUnivtoolItem::Update");
  prepareGeometryChange();
  // outer rect
  mCoreRect=QRect(-90,-90,180,180);
  mOuterRect=mCoreRect.adjusted(-6,-6,6,6);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  // revolver ....
  mRevolverPath = QPainterPath();
  mRevolverPath.setFillRule(Qt::WindingFill);
  // revolver: inner circ
  mRevolverPath.addEllipse(-45,-45,90,90);
  QPainterPath smallcirc;
  smallcirc.addEllipse(-35,15,70,70);
  QTransform rot120;
  // revolver: tool A
  mRevolverPath=mRevolverPath.united(rot120.map(smallcirc));
  rot120.rotate(120);
  // revolver: tool B
  mRevolverPath=mRevolverPath.united(rot120.map(smallcirc));
  rot120.rotate(120);
  // revolver: tool C
  mRevolverPath=mRevolverPath.united(rot120.map(smallcirc));
  // tool ...
  mToolsPath = QPainterPath();
  mToolsPath.setFillRule(Qt::WindingFill);
  // tools: tool A
  rot120.reset();
  QPainterPath toolA;
  toolA.addRect(-15,35,30,30);
  mToolsPath=mToolsPath.united(rot120.map(toolA));
  // tools: tool B
  QPainterPath toolB;
  toolB.moveTo(-18,35);
  toolB.lineTo(+18,35);
  toolB.lineTo(0,+74);
  toolB.closeSubpath();
  rot120.rotate(120);
  mToolsPath=mToolsPath.united(rot120.map(toolB));
  // tools: tool C
  QPainterPath toolC;
  toolC.addEllipse(-18,32,36,36);
  rot120.rotate(120);
  mToolsPath=mToolsPath.united(rot120.map(toolC));
  // update
  update();
}




/*
************************************************
************************************************
  
Implementation: FfUnivframe

************************************************
************************************************
*/

// default constructor
FfUnivframeItem::FfUnivframeItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfUnivframeItem::FfUnivframeItem()");
  setZValue(-15);
  setPos(0,0);
  setAcceptedMouseButtons(Qt::NoButton);
  Update();
}


// paint myself
void FfUnivframeItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  FF_DQP("FfUnivframeItem::paint()");
  (void) option;
  (void) widget;
  // prepare effect colors
  FfScene* sc = qobject_cast<FfScene*>(scene());
  if(!sc) return;
  // frame
  QColor pale=sc->EffectColor(VioDarkGrey);
  pale.setAlphaF(0.70);
  painter->setBrush(pale);
  painter->setPen(Qt::NoPen);
  painter->drawPath(mInnerFrame);
  painter->setPen(sc->EffectColor(VioDarkRed));
  painter->setBrush(sc->EffectColor(VioRed));
  painter->drawPath(mOuterFrame);
}


// updater
void FfUnivframeItem::Update(void) {
  FF_DQ("FfUnivframeItem::Update");
  prepareGeometryChange();
  // frameessor shape
  mOuterFrame=QPainterPath();
  mOuterFrame.moveTo(-65,-65);
  mOuterFrame.lineTo(65,-65);
  mOuterFrame.lineTo(65,-265);
  mOuterFrame.lineTo(-65,-265);
  mOuterFrame.closeSubpath();
  mOuterFrame.moveTo(-50,-80);
  mOuterFrame.lineTo(50,-80);
  mOuterFrame.lineTo(50,-250);
  mOuterFrame.lineTo(-50,-250);
  mOuterFrame.closeSubpath();
  mInnerFrame=QPainterPath();
  mInnerFrame.moveTo(-50,-80);
  mInnerFrame.lineTo(50,-80);
  mInnerFrame.lineTo(50,-250);
  mInnerFrame.lineTo(-50,-250);
  mInnerFrame.closeSubpath();
  // outer rect
  mCoreRect=QRect(-60,-190,120,270);
  mOuterRect=mCoreRect.adjusted(-6,-6,6,6);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  // done
  update();
}


/*
************************************************
************************************************
  
Implementation: FfUnivproc (hosts a FfUnivtool, a FfUnivframe and a FfSensor)

************************************************
************************************************
*/

// default constructor
FfUnivprocItem::FfUnivprocItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  FF_DQ("FfUnivprocItem::FfUnivprocItem()");
  setZValue(-10);
  setPos(0,0);
  setAcceptedMouseButtons(Qt::NoButton);
  mPos=0;
  mUnivtool = new FfUnivtoolItem(this);
  mUnivframe = new FfUnivframeItem(this);
  mSensor = new FfSensorItem(this);
  mSensor->mWpSensor=false;
  mSensor->mValue=false;
  mSensor->mRelAddress=-1;
  Update();
}


// paint myself
void FfUnivprocItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  FF_DQP("FfUnivprocItem::paint()");
  (void) option;
  (void) widget;
  (void) painter;

  /*

  // prepare effect colors
  FfScene* sc = qobject_cast<FfScene*>(scene());
  if(!sc) return;
  // paint
  painter->save();
  painter->translate(0,mPos);

  painter->setPen(sc->EffectColor(VioDarkGrey));
  painter->setBrush(sc->EffectColor(VioGrey));
  painter->drawEllipse(0-30,-130-30,60,60);

  //if(mProg>=1){
    painter->setPen(sc->EffectColor(VioDarkGrey));
    painter->setBrush(sc->EffectColor(VioLightGrey));
    painter->drawPie(0-30,-130-30,60,60,45*16,-mProg/100.0*16*360);
    //} 
  if(mProg>=99){
    painter->setPen(sc->EffectColor(VioDarkGrey));
    painter->setBrush(sc->EffectColor(VioLightGrey));
    painter->drawEllipse(0-30,-130-30,60,60);
  }
  painter->restore();

  */
}


// shift
void FfUnivprocItem::SetPosition(qreal pos) {
  mPos=pos;
  mUnivtool->setPos(mUnivtool->x(),pos-185); // qt 4.2 has no setY
  mUnivframe->setPos(mUnivframe->x(),pos);
  mSensor->setPos(mSensor->x(),pos);
  update();
}


// shift
void FfUnivprocItem::SetToolsel(qreal tsel) {
  mUnivtool->SetToolsel(tsel);
  tsel = mUnivtool->Toolsel();
  bool sns=false;
  if(   -3 < tsel && tsel <    +3) sns = true; 
  if(120-3 < tsel && tsel < 120+3) sns = true; 
  if(240-3 < tsel && tsel < 240+3) sns = true; 
  if(360-3 < tsel && tsel < 360+3) sns = true; 
  mSensor->SetValue(sns);
}

// shift
void FfUnivprocItem::SetProgress(qreal prog) {
  mUnivtool->SetProgress(prog);
}


// updater
void FfUnivprocItem::Update(void) {
  FF_DQ("FfUnivprocItem::Update");
  prepareGeometryChange();
  // outer rect
  mCoreRect=QRect(-60,-190,120,270);
  mOuterRect=mCoreRect.adjusted(-6,-6,6,6);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  // my sensor
  mSensor->mIndicatorPath.addRect(52.5,-257.5,10,10);
  mSensor->Update();
  update();
}



