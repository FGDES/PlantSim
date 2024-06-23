/* ffstackfeeder.cpp  - stackfeeder component */

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

#include "ffstackfeeder.h"
#include "ffscene.h"

/*
************************************************
************************************************
  
Implementation: FfStackfeeder

************************************************
************************************************
*/

// internal defs for signals
#define SIG_FEEDMOTOR  0
#define SIG_FEEDSENSOR 1
#define SIG_WPSENSOR   2

// indernal defs for sensord
#define SNS_FEEDSENSOR 0
#define SNS_WPSENSOR 1


// default constructor
FfStackfeeder::FfStackfeeder(QObject* parent) : FfComponent(parent) {
  FF_DQM("FfStackfeeder::FfStackfeeder()");
  // my items
  mAllRail = new FfRailItem(this);
  mStackDigit = new FfDigitItem(0);
  mSimOverlay=mStackDigit;
  mSimOverlay->setZValue(5);
  if(!mRunning) mSimOverlay->setVisible(false);
  mBelt = new FfPushBeltItem(this);
  // my controls
  mButtonFeed = new FfButtonItem(mSimControls);
  mButtonInsert = new FfButtonItem(mSimControls);
  mSimControls->SetVisible(false);
  // default name
  SetName("SF1");
  FF_DQM("FfStackfeeder::FfStackfeeder(): done");
}

// reset state
void FfStackfeeder::DoReset(void) {
  // parameters
  mFeedVelocity=100*FfStyle::mPitch; // pt per sec
  mStrictAdvance=true;   // down on collision
  // dynamic state
  mFeedPos=-50;            // ranges -50 ... 100
  mIsFeeding=false;      // feed in process
  mIsAutoFeeding=false;  // automatic feed in process
  mStack=0;              // no of workpieces
  // transport
  mVelocity = QPointF(0,0);
  // item consequences
  mStackDigit->SetDigit(mStack);
  mStackDigit->setVisible(mStack>0);
  mBelt->SetPosition(mFeedPos);
  mBelt->Update();
}

// update all 
void FfStackfeeder::DoUpdateAll() {
  // my signals
  FfSignal* sigcfg;
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_FEEDMOTOR);
  sigcfg->SetLastName("FDM");
  sigcfg->SetActuator();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "fdon");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "fdoff");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_FEEDSENSOR);
  sigcfg->SetLastName("FDS");
  sigcfg->SetSensor();
  sigcfg->SetInitialValue(true);
  sigcfg->SetPositiveEdgeEvents(QStringList() << "fdhome");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_WPSENSOR);
  sigcfg->SetLastName("WPS");
  sigcfg->SetSensor();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "wpar");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "wplv");
  mSignals.append(sigcfg);
  // outer shape: pos only
  QRectF brect(-100,-100,200,200);
  mOuterRect=brect.adjusted(-2,-2,+2,+2);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  mCoreShape=QPainterPath();
  mCoreShape.addRect(brect);
  // transport
  mRail=QLineF(0,0,100,0);
  mNormRail=FfStyle::NormalizeF(mRail.p2()-mRail.p1());
  mBlock=QPainterPath();
  mBlock.moveTo(-100,-100);
  mBlock.lineTo(100,-100);
  mBlock.lineTo(100,-50);
  mBlock.lineTo(-50,-50);
  mBlock.lineTo(-50,50);
  mBlock.lineTo(100,50);
  mBlock.lineTo(100,100);
  mBlock.lineTo(-100,100);
  mBlock.lineTo(-100,-100);
  mBlock.closeSubpath();
  // sensors
  mSensors.append(new FfSensorItem(this));
  mSensors.append(new FfSensorItem(this));
  FfSensorItem* sns;
  sns = mSensors.at(SNS_FEEDSENSOR);
  sns->mIndicatorPath=QPainterPath();
  sns->mIndicatorPath.addRect(-60,55,10,10);
  sns->mValue=false;
  sns->mWpSensor=false;
  sns->mRelAddress=SIG_FEEDSENSOR;
  sns->Update();
  sns = mSensors.at(SNS_WPSENSOR);
  sns->mOffPath=QPainterPath();
  sns->mOffPath.addRect(-30,-30,60,60);
  sns->mOnPath=QPainterPath();
  sns->mOnPath.addRect(-10,-10,20,20);
  sns->mIndicatorPath=QPainterPath();
  sns->mIndicatorPath.addRect(-10,55,20,10);
  sns->mValue=false;
  sns->mWpSensor=true;
  sns->mRelAddress=SIG_WPSENSOR;
  sns->Update();
  // my items
  mAllRail->SetLeftHorseShoe();
  mAllRail->setTransform(mTransOrient);
  mStackDigit->mSize=80;
  mBelt->SetTransform(mTransOrient);
  // my controls
  mButtonFeed->SetRightSkip(QPointF(60,0));
  mButtonInsert->SetPlus(QPointF(0,0));
  // fix orientation: base
  FfComponent::DoUpdateAll();
}

// paint myself
void FfStackfeeder::paint(
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
void FfStackfeeder::DoAdvanceTime(qreal dt) {
  // cont dynamics: desrired transport velocity for next time step
  mVelocity=QPointF(0,0);
  if(Signal(SIG_FEEDMOTOR)) { 
    mVelocity=mNormRail * mFeedVelocity;
  }
  // test whether we can move the feed
  if(Signal(SIG_FEEDMOTOR)) { 
    FfScene* sc=Scene();
    if(!sc) return;   
    if(mFeedPos <=90) {
      QPointF nfpos(mFeedPos + dt * mFeedVelocity + 3, 0);
      QList<FfWorkpiece*> wplist= sc->Workpieces(mAllRail->mapToScene(nfpos));
      if(wplist.size()>0) {
        //FF_DQM("FfStackFeeder::DoAdvanceTime(): refuse to move");
        //foreach(FfWorkpiece* wp,wplist) wp->SetError(true); (quietly accept the block)
        mRefuseAdvance=true;
      } 
    }
  }
  // cont dynamics: move my belt
  if(!mRefuseAdvance) 
  if(Signal(SIG_FEEDMOTOR)) { 
    mFeedPos+= dt * mFeedVelocity;
    if(mFeedPos>100) { 
      mFeedPos=-50; 
      if(mIsAutoFeeding) SetSignal(SIG_FEEDMOTOR, false);
      mIsAutoFeeding=false; 
    };
    mBelt->SetPosition(mFeedPos);
  }
  // feed sensor
  SetSignal(SIG_FEEDSENSOR, mFeedPos<-30);
  mSensors.at(SNS_FEEDSENSOR)->SetValue(mFeedPos<-30);
  // get wp from stack
  if(!mIsFeeding && mStack>0 && mFeedPos<-30) {
    mIsFeeding=true;
    mStack--;
    AddWorkpiece(QPointF(0,0),mOrient);
  }
  // stop feed
  if(mIsFeeding && mFeedPos>90) {
    mIsFeeding=false;
  }
  // track stack
  int num = mStack + ( mIsFeeding ? 1 : 0);
  if(num!=0) {
    mStackDigit->SetDigit(num);
    mStackDigit->setVisible(true);
  } else {
    mStackDigit->setVisible(false);
  }    
}


// user commands
void FfStackfeeder::DoUserCommand(QString cmd) {
  FF_DQM("FfStackfeeder::DoUserCommand()" << cmd);
  if(cmd=="RightSkip" && !mIsAutoFeeding) {
    mIsAutoFeeding=true;
    if(mStack<=0 && !mIsFeeding) mStack=1;
    SetSignal(SIG_FEEDMOTOR, true);
  }
  FF_DQM("FfStackfeeder::DoUserCommand()" << cmd);
  if(cmd=="Plus") {
    mStack++;
    if(mStack>9) mStack=9;
    if(mStack>8 && mIsFeeding) mStack=8;
    mStackDigit->SetDigit(mStack);
  }
}
