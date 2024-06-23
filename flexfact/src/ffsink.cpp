/* sink.cpp  - sink components */

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

#include "ffsink.h"
#include "ffscene.h"


/*
************************************************
************************************************
  
Implementation: FfSink

************************************************
************************************************
*/

// internal defs for signals
#define SIG_WPSENSOR  0
#define SNS_WPSENSOR  0

// default constructor
FfSink::FfSink(QObject* parent) : FfComponent(parent) {
  FF_DQM("FfSink::FfSink()");
  // my items
  mAllRail = new FfRailItem(this);
  // my controls
  mButtonClear = new FfButtonItem(mSimControls);
  mButtonClearAll = new FfButtonItem(mSimControls);
  // set to defaults
  SetName("XS1");
  FF_DQM("FfSink::FfSink(): done");
}

// reset state
void FfSink::DoReset(void) {
  // parameters
  mSlideVelocity=200*FfStyle::mPitch; // pt per sec
  // transport
  mVelocity = mNormRail * mSlideVelocity;
}

// blocks occupied: pos and next
QList<QPoint> FfSink::TestArea(const QPoint& ipos,Orientation o) {
  QList<QPoint> res;
  res.append(ipos);
  switch(o) {
  case(Ostd): res.append(ipos+QPoint(-1,0)); break;
  case(O90) : res.append(ipos+QPoint(0,-1)); break;
  case(O180): res.append(ipos+QPoint(1,0)); break;
  case(O270): res.append(ipos+QPoint(0,1)); break;
  } 
  return res;
}

// update all 
void FfSink::DoUpdateAll() {
  // my signals
  FfSignal* sigcfg;
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_WPSENSOR);
  sigcfg->SetLastName("WPS");
  sigcfg->SetSensor();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "wpar");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "wplv");
  mSignals.append(sigcfg);
  // nice gradient
  switch(mOrient) {
  case(Ostd): mGrad=QLinearGradient(QPointF(0,0.5), QPointF(1,0.5));break;
  case(O90):  mGrad=QLinearGradient(QPointF(0.5,0), QPointF(0.5,1));break;
  case(O180): mGrad=QLinearGradient(QPointF(1,0.5), QPointF(0,0.5));break;
  case(O270): mGrad=QLinearGradient(QPointF(0.5,1), QPointF(0.5,0));break;
  } 
  mGrad.setColorAt(0, FfStyle::Color(VioGrey));
  mGrad.setColorAt(1, FfStyle::Color(VioLightGrey));
  mGrad.setCoordinateMode(QGradient::ObjectBoundingMode);
  // outer shape: pos plus left neighbor
  QRectF brect(-300,-100,400,200);
  mOuterRect=brect.adjusted(-2,-2,+2,+2);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  mCoreShape=QPainterPath();
  mCoreShape.addRect(brect);
  // transport: testing
  mRail=QLineF(-300,0,20,0);
  mNormRail=FfStyle::NormalizeF(mRail.p2()-mRail.p1());
  mBlock=QPainterPath();
  mBlock.moveTo(-300,-100);
  mBlock.lineTo( 100,-100);
  mBlock.lineTo( 100, 100);
  mBlock.lineTo(-300, 100);
  mBlock.lineTo(-300,  50);
  mBlock.lineTo(  85,  50);
  mBlock.lineTo(  85, -50);
  mBlock.lineTo(-300, -50);
  mBlock.lineTo(-300, -100);
  mBlock.closeSubpath();
  // sensors
  mSensors.append(new FfSensorItem(this));
  FfSensorItem* sns;
  sns = mSensors.at(SNS_WPSENSOR);
  sns->mOffPath=QPainterPath();
  sns->mOffPath.addRect(-25-240,-30,60,60);
  sns->mOnPath=QPainterPath();
  sns->mOnPath.addRect(-5-240,-10,20,20);
  sns->mIndicatorPath=QPainterPath();
  sns->mIndicatorPath.addRect(-245,55,20,10);
  sns->mValue=false;
  sns->mWpSensor=true;
  sns->mRelAddress=SIG_WPSENSOR;
  sns->Update();
  // my items
  mAllRail->SetRightOpen(2);
  mAllRail->setTransform(mTransOrient);
  mBase=QPainterPath();
  mBase.addRect(-290,-50,380,100);
  mBase=mTransOrient.map(mBase);
  // my controls
  mButtonClear->SetRight(QPointF(-50,0));
  mButtonClearAll->SetRightSkip(QPointF(40,0));
  mSimControls->SetVisible(false);
  // fix orientation: base
  FfComponent::DoUpdateAll();
}


// paint myself
void FfSink::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // paint
  painter->setPen(Qt::NoPen); //setPen(FfStyle::Color(VioGrey));
  painter->setBrush(mGrad); //FfStyle::Color(VioLightGrey));
  painter->drawPath(mBase);
  // debug: call base (incl sensors)
  FfComponent::paint(painter,option,widget);
}


// update dynamics
void FfSink::DoAdvanceTime(qreal dt) {
  (void) dt;
}


// user commands
void FfSink::DoUserCommand(QString cmd) {
  FF_DQM("FfSink::DoUserCommand(): " << cmd);
  if(cmd=="RightSkip") {
    QPainterPath sink;
    sink.addRect(-250,-20,350,40);
    sink=mTransOrient.map(sink);
    QList<FfWorkpiece*> del=Scene()->Workpieces(mapToScene(sink));
    FF_DQM("FfSink::DoUserCommand(): del #" << del.size());
    foreach(FfWorkpiece* wp, del) Scene()->RemoveWorkpiece(wp);
    SetSignal(SIG_WPSENSOR,false);
    mSensors[SNS_WPSENSOR]->SetValue(false);
  }
  if(cmd=="Right") {
    QPainterPath sink;
    sink.addRect(50,-20,40,40);
    sink=mTransOrient.map(sink);
    QList<FfWorkpiece*> del=Scene()->Workpieces(mapToScene(sink));
    FF_DQM("FfSink::DoUserCommand(): del #" << del.size());
    foreach(FfWorkpiece* wp, del) Scene()->RemoveWorkpiece(wp);
  }
}
