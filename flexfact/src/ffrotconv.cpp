/* rotconv.CC  - rotary table incl conveyor belt component */

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


#include "ffrotconv.h"
#include "ffworkpiece.h"

/*
************************************************
************************************************
  
Implementation: FfRotaryTable

************************************************
************************************************
*/

// internal defs for signals
#define SIG_MOTORA       0
#define SIG_MOTORB       1
#define SIG_WPSENSOR     2
#define SIG_TABLECW      3
#define SIG_TABLECCW     4
#define SIG_TBSENSORCW   5
#define SIG_TBSENSORCCW  6

#define SNS_WPSENSOR     0
#define SNS_TBSENSORCW   1
#define SNS_TBSENSORCCW  2


// default constructor
FfRotaryTable::FfRotaryTable(QObject* parent) : FfComponent(parent) {
  FF_DQM("FfRotaryTable::FfRotaryTable()");
  // my items
  mBelt = new FfBeltItem(this);
  mUpperRail = new FfRailItem(this);
  mLowerRail = new FfRailItem(this);
  // my controls
  mButtonToA = new FfButtonItem(mSimControls);
  mButtonToB = new FfButtonItem(mSimControls);
  mButtonRotate = new FfButtonItem(mSimControls);
  mButtonStop = new FfButtonItem(mSimControls);
  // default name
  SetName("RB1");
  FF_DQM("FfRotaryTable::FfRotaryTable(): done");
}

// update all 
void FfRotaryTable::DoUpdateAll() {
  FF_DQM("FfRotaryTable::DoUpdateAll()");
  // my signals
  FfSignal* sigcfg;
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_MOTORA);
  sigcfg->SetLastName("BM-");
  sigcfg->SetActuator();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "bm-");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "bm+" << "boff");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_MOTORB);
  sigcfg->SetLastName("BM+");
  sigcfg->SetActuator();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "bm+");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "bm-" << "boff");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_WPSENSOR);
  sigcfg->SetLastName("WPS");
  sigcfg->SetSensor();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "wpar");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "wplv");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_TABLECW);
  sigcfg->SetLastName("RCW");
  sigcfg->SetActuator();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "rcw");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "rccw" << "roff");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_TABLECCW);
  sigcfg->SetLastName("RCCW");
  sigcfg->SetActuator();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "rccw");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "rcw" << "roff");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_WPSENSOR);
  sigcfg->SetLastName("SCW");
  sigcfg->SetSensor();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "scw");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_WPSENSOR);
  sigcfg->SetLastName("SCCW");
  sigcfg->SetSensor();
  sigcfg->SetInitialValue(true);
  sigcfg->SetPositiveEdgeEvents(QStringList() << "sccw");
  mSignals.append(sigcfg);
  // outer shape: pos only
  QRectF brect(-100,-100,200,200);
  mOuterRect=brect.adjusted(-2,-2,+2,+2);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  mCoreShape=QPainterPath();
  mCoreShape.addRect(brect);
  // resez my dynamic transform
  mTransDynamic=mTransOrient;
  // transport: testing
  mRail=QLineF(-100,0,100,0);
  mNormRail=FfStyle::NormalizeF(mRail.p2()-mRail.p1());
  mBlock=QPainterPath();
  mBlock.addRect(-100,-100,200,50);
  mBlock.addRect(-100,50,200,50);
  // my sensors: instantiate 
  mSensors.append(new FfSensorItem(this));
  mSensors.append(new FfSensorItem(this));
  mSensors.append(new FfSensorItem(this));
  // my sensors: config
  FfSensorItem* sns = mSensors.at(SNS_WPSENSOR);
  sns->mOffPath=QPainterPath();
  sns->mOffPath.addRect(-30,-30,60,60);
  sns->mOnPath=QPainterPath();
  sns->mOnPath.addRect(-10,-10,20,20);
  sns->mIndicatorPath=QPainterPath();
  sns->mIndicatorPath.addRect(-10,55,20,10);
  sns->mWpSensor=true;
  sns->mValue=false;
  sns->mRelAddress=SIG_WPSENSOR;
  sns->Update();
  sns = mSensors.at(SNS_TBSENSORCW);
  sns->mIndicatorPath=QPainterPath();
  sns->mIndicatorPath.moveTo(-80,70);
  sns->mIndicatorPath.lineTo(-90,70);
  sns->mIndicatorPath.lineTo(-90,60);
  sns->mIndicatorPath.lineTo(-80,60);
  sns->mIndicatorPath.closeSubpath();
  sns->setZValue(-60);
  sns->mWpSensor=false;
  sns->mValue=false;
  sns->mRelAddress=SIG_TBSENSORCW;
  sns->Update();
  sns = mSensors.at(SNS_TBSENSORCCW);
  sns->mIndicatorPath=QPainterPath();
  sns->mIndicatorPath.moveTo(60,-90);
  sns->mIndicatorPath.lineTo(70,-90);
  sns->mIndicatorPath.lineTo(70,-80);
  sns->mIndicatorPath.lineTo(60,-80);
  sns->mIndicatorPath.closeSubpath();
  sns->setZValue(-60);
  sns->mWpSensor=false;
  sns->mValue=false;
  sns->mRelAddress=SIG_TBSENSORCCW;
  sns->Update();
  // my items
  mBelt->SetLength(1);
  mUpperRail->SetUpperX();
  mLowerRail->SetLowerX();
  mBelt->SetTransform(mTransOrient);
  mUpperRail->setTransform(mTransOrient);
  mLowerRail->setTransform(mTransOrient);
  // my controls
  mButtonToA->SetLeft(QPointF(-55,0));
  mButtonToB->SetRight(QPointF(55,0));
  mButtonRotate->SetLargeCircle(QPointF(0,0));
  mButtonStop->SetStop(QPointF(0,0));
  mSimControls->SetVisible(false);
  // fix order for Qt 4.4.2
  mButtonToA->setZValue(mButtonToA->zValue()-1);
  mButtonToB->setZValue(mButtonToB->zValue()-1);
  mButtonStop->setZValue(mButtonStop->zValue()+1);
  // fix orientation: base
  FF_DQM("FfRotaryTable::DoUpdateAll(): call base");
  FfComponent::DoUpdateAll();
}

// reset state
void FfRotaryTable::DoReset(void) {
  // parameters
  mBeltVelocity=100*FfStyle::mPitch;  // pt per sec
  mTableVelocity=90*FfStyle::mPitch; // deg per sec
  // dynamic state
  mBeltPos=0;        // ranges 0 ... 20
  mAngle = 0;        // ranges -10 ... 100
  mTransDynamic=mTransOrient;
  // transport
  mVelocity = QPointF(0,0);
  mRTVelocity = 0;
  mAutoRotate=0;
  // item consequences
  mBelt->SetPosition(mBeltPos);
  mBelt->SetTransform(mTransDynamic);
  mUpperRail->setTransform(mTransDynamic);
  mLowerRail->setTransform(mTransDynamic);
  mSensors.at(SNS_WPSENSOR)->setTransform(mTransDynamic);
  prepareGeometryChange();
  mSimControls->setTransform(mTransDynamic);
  mOuterRect=mTransDynamic.mapRect(mNominalOuterRect);
  mOuterShape=mTransDynamic.map(mNominalOuterShape);
  mCoreShape=mTransDynamic.map(mNominalCoreShape);
  mRail=mTransDynamic.map(mNominalRail);
  mNormRail=mTransDynamic.map(mNominalNormRail);
  mBlock=mTransDynamic.map(mNominalBlock);
  update();
}


// paint myself
void FfRotaryTable::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // debug: call base
  FfComponent::paint(painter,option,widget);
}

// update dynamics
void FfRotaryTable::DoAdvanceTime(qreal dt) {
  // cont dynamics: belt
  bool bma=Signal(SIG_MOTORA);
  bool bmb=Signal(SIG_MOTORB);
  if(bma) {
    mBeltPos-= dt * mBeltVelocity;
    while(mBeltPos<0) mBeltPos+=20;
    mBelt->SetPosition(mBeltPos);
    mVelocity= mNormRail * (-1) * mBeltVelocity;
  }
  if(bmb) {
    mBeltPos+= dt * mBeltVelocity;
    while(mBeltPos>20) mBeltPos-=20;
    mBelt->SetPosition(mBeltPos);
    mVelocity=mNormRail * mBeltVelocity;
  }
  if(!bma) 
  if(!bmb) {
    mVelocity=QPointF(0,0);
  }
  if(bma) 
  if(bmb) {
    // error
  }
  // cont dynamics: table
  bool tma=Signal(SIG_TABLECW);
  bool tmb=Signal(SIG_TABLECCW);
  if(tma)
  if(tmb) {
    // error
  }
  // cont dynamics: table
  mRTVelocity =0;
  if(tma) 
    mRTVelocity= mTableVelocity;
  if(tmb) 
    mRTVelocity= - mTableVelocity;
  if(mAutoRotate >0 && mAngle >= 90) 
    mAutoRotate=0;
  if(mAutoRotate <0 && mAngle <=0) 
    mAutoRotate=0;
  if(mAutoRotate !=0) 
    mRTVelocity= mAutoRotate;
  if(mRefuseAdvance) 
    mRTVelocity=0;
  // figure target angle
  qreal nangle = mAngle;
  nangle += dt*mRTVelocity;
  // constraints
  if(nangle>100) nangle=100;
  if(nangle<-10) nangle=-10;
  // cheap lock
  if(nangle > 82.6 && nangle<97.4) SnapOnOrientation(dt,nangle);
  if(nangle > -7.4 && nangle<7.4) SnapOnOrientation(dt,nangle);
  // do rotate
  if(nangle!=mAngle){
    mAngle=nangle;
    //FF_DQM("FfRotaryTable::FfRotaryTable(" << Name() << "): Rotate to " << mAngle);
    // transform
    mTransDynamic=mTransOrient;
    mTransDynamic.rotate(mAngle);
    // my items
    mBelt->SetTransform(mTransDynamic);
    mUpperRail->setTransform(mTransDynamic);
    mLowerRail->setTransform(mTransDynamic);
    mSensors.at(SNS_WPSENSOR)->setTransform(mTransDynamic);
    // base items
    prepareGeometryChange();
    mSimControls->setTransform(mTransDynamic);
    mOuterRect=mTransDynamic.mapRect(mNominalOuterRect);
    mOuterShape=mTransDynamic.map(mNominalOuterShape);
    mCoreShape=mTransDynamic.map(mNominalCoreShape);
    mRail=mTransDynamic.map(mNominalRail);
    mNormRail=mTransDynamic.map(mNominalNormRail);
    mBlock=mTransDynamic.map(mNominalBlock);
    update();
  }
  // key switches
  SetSignal(SIG_TBSENSORCCW, mAngle<=0);
  SetSignal(SIG_TBSENSORCW, mAngle>=90);
  mSensors.at(SNS_TBSENSORCW)->SetValue(mAngle<=0);
  mSensors.at(SNS_TBSENSORCCW)->SetValue(mAngle>=90);
}



// propagate workpiece
void FfRotaryTable::AdvanceWorkpiece(
    const qreal& dt,
    const QPointF& pos, const qreal& orient, 
    QPointF& npos, qreal& norient, bool& strict, bool& err)
{
  // no errors
  err=false;
  // ok center?
  QPointF lpos= mapFromScene(pos);
  if(mBlock.contains(lpos)) {
    npos=pos;
    norient=orient;
    strict=true;
    return;
  }
  // std case: no rotation
  if(mRTVelocity==0) {
    norient=mAngle;
    npos=pos + dt*mVelocity;
    SnapOnRail(dt,npos,norient);
    norient=mAngle; // overwrite snaponrail (not implenmented)
    strict=false;
    return;
  }
  // rotate
  lpos = FfStyle::LotF(mNormRail,lpos);
  norient = qRound(mAngle + dt* mRTVelocity);
  lpos+=dt*mVelocity;
  QTransform atrans;
  atrans.rotate(norient-orient);
  lpos=atrans.map(lpos);
  npos=mapToScene(lpos);
  strict=true;
}


// report workpiece
void FfRotaryTable::DoWorkpiece(FfWorkpiece* wp) {
  (void) wp;
}

// execute user command
void FfRotaryTable::DoUserCommand(QString cmd) {
 FF_DQM("FfRotaryTable::DoUserCommand()" << cmd);
 // stop
 if(cmd=="Stop") {
   SetSignal(SIG_MOTORA,false);
   SetSignal(SIG_MOTORB,false);
   return;
 }
 // left
 if(cmd=="Right") {
   SetSignal(SIG_MOTORA,false); SetSignal(SIG_MOTORB,true); 
   return;
 }  
 // right
 if(cmd=="Left") {
   SetSignal(SIG_MOTORB,false); SetSignal(SIG_MOTORA,true); 
   return;
 }  
 // rotate
 if(cmd=="LargeCircle") {
   if(mAngle > 45) mAutoRotate= -mTableVelocity;
   if(mAngle < 45) mAutoRotate= +mTableVelocity;
 }
}
