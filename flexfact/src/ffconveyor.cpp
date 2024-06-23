/* conveyor.cpp  - conveyor belt components */

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

#include "ffconveyor.h"
#include "ffworkpiece.h"
#include "ffscene.h"

/*
************************************************
************************************************
  
Implementation: FfConveyor

************************************************
************************************************
*/

// internal defs for signals
#define SIG_MOTORA     0
#define SIG_MOTORB     1
#define SIG_WPSENSORA  2
#define SIG_WPSENSORB  3

// internal defs for sensors
#define SNS_WPSENSORA  0
#define SNS_WPSENSORB  1

// internal defs for faults
#define FAULT_BM  (mFaults.at(0)->Value())
#define FAULT_WPS (mFaults.at(1)->Value())



// default constructor
FfConveyor::FfConveyor(QObject* parent) : FfComponent(parent) {
  FF_DQM("FfConveyor::FfConveyor()");
  // my configuration
  mLength=1;
  // my items
  mBelt = new FfBeltItem(this);
  mUpperRail = new FfRailItem(this);
  mLowerRail = new FfRailItem(this);
  // my controls
  mButtonToA = new FfButtonItem(mSimControls);
  mButtonToB = new FfButtonItem(mSimControls);
  mButtonStop = new FfButtonItem(mSimControls);
  // faults
  FfFault* faultcfg;
  faultcfg= new FfFault(this);
  faultcfg->SetEditorName("Motor Breakdown");
  faultcfg->SetShortName("FM");
  mFaults.append(faultcfg);
  faultcfg= new FfFault(this);
  faultcfg->SetEditorName("Blind Sensor");
  faultcfg->SetShortName("FS");
  mFaults.append(faultcfg);
  // default name
  SetName("CB1");
  FF_DQM("FfConveyor::FfConveyor(): done");
}



// set my parameters
void FfConveyor::SetLength(int nlen) {
  // ignore on trivial
  if(mLength==nlen) return;
  // test in scene
  if(FfScene* sc=Scene()) {
      (void) sc; // todo ?
  }
  // set
  mLength=nlen;
}


// file io
void FfConveyor::AppendToElement(QDomDocument& doc, QDomElement& cele) {
  // construct additional elements
  QDomElement pele = doc.createElement("Parameters");
  pele.setAttribute("len",mLength);
  // append
  cele.appendChild(pele);
}

// file io
void FfConveyor::ExtractFromElement(QDomElement& elem) {
  // interpret parameters
  QDomElement pele=elem.firstChildElement("Parameters");
  QString lstr = pele.attribute("len");
  bool okl;
  int len= lstr.toInt(&okl); 
  if(!okl) {
    FF_DQM("FileIO: could not figure length of conveyor belt " << mName);
    len=1;
  }
  // set parameter
  SetLength(len);
}


// reset state
void FfConveyor::DoReset(void) {
  // parameters
  mBeltVelocity=100*FfStyle::mPitch; // pt per sec
  // dynamic state
  mBeltPos=0;        // ranges 0 ... 20
  // transport
  mVelocity = QPointF(0,0);
  // item consequences
  mBelt->SetPosition(mBeltPos);
}

// update all 
void FfConveyor::DoUpdateAll() {
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
  if(mLength==1) {
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_WPSENSORA);
    sigcfg->SetLastName("WPS");
    sigcfg->SetSensor();
    sigcfg->SetPositiveEdgeEvents(QStringList() << "wpar");
    sigcfg->SetNegativeEdgeEvents(QStringList() << "wplv");
    mSignals.append(sigcfg);
  } else {
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_WPSENSORA);
    sigcfg->SetLastName("WPS-");
    sigcfg->SetSensor();
    sigcfg->SetPositiveEdgeEvents(QStringList() << "wpar-");
    sigcfg->SetNegativeEdgeEvents(QStringList() << "wplv-");
    mSignals.append(sigcfg);
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_WPSENSORB);
    sigcfg->SetLastName("WPS+");
    sigcfg->SetSensor();
    sigcfg->SetPositiveEdgeEvents(QStringList() << "wpar+");
    sigcfg->SetNegativeEdgeEvents(QStringList() << "wplv+");
    mSignals.append(sigcfg);
  }
  // my sensor: instatiate
  mSensors.append(new FfSensorItem(this));
  if(mLength>1) 
    mSensors.append(new FfSensorItem(this));
  // my sensors: configure
  FfSensorItem* sns;
  sns  = mSensors.at(SNS_WPSENSORA);
  sns->mOffPath.addRect(-30,-30,60,60);
  sns->mOnPath.addRect(-10,-10,20,20);
  sns->mIndicatorPath.addRect(-10,55,20,10);
  sns->mWpSensor=true;
  sns->mValue=false;
  sns->mRelAddress=SIG_WPSENSORA;
  sns->Update();
  if(mLength>1) {
    sns = mSensors.at(SNS_WPSENSORB);
    sns->mOffPath.addRect(-30+200*(mLength-1),-30,60,60);
    sns->mOnPath.addRect(-10+200*(mLength-1),-10,20,20);
    sns->mIndicatorPath.addRect(-10+200*(mLength-1),55,20,10);
    sns->mWpSensor=true;
    sns->mValue=false;
    sns->mRelAddress=SIG_WPSENSORB;
    sns->Update();
  }
  // outer shape: pos only
  QRectF brect(-100,-100,200*mLength,200);
  mOuterRect=brect.adjusted(-2,-2,+2,+2);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  mCoreShape=QPainterPath();
  mCoreShape.addRect(brect);
  // transport: testing
  mRail=QLineF(-100,0,-100+200*mLength,0);
  mNormRail=FfStyle::NormalizeF(mRail.p2()-mRail.p1());
  mBlock=QPainterPath();
  mBlock.addRect(-100,-100,200*mLength,50);
  mBlock.addRect(-100,50,200*mLength,50);
  // my items
  mBelt->SetLength(mLength);
  mUpperRail->SetUpper(mLength);
  mLowerRail->SetLower(mLength);
  mBelt->SetTransform(mTransOrient);
  mUpperRail->setTransform(mTransOrient);
  mLowerRail->setTransform(mTransOrient);
  // my controls
  mButtonToA->SetLeft(QPointF(100*(mLength-1)-55,0));
  mButtonStop->SetStop(QPointF(100*(mLength-1),0));
  mButtonToB->SetRight(QPointF(100*(mLength-1)+55,0));
  mSimControls->SetVisible(false);
  // fix orientation: base
  FfComponent::DoUpdateAll();
}

// blocks occupied: integer shape shape
QList<QPoint> FfConveyor::TestArea(const QPoint& pos,Orientation o,int len) {
  QList<QPoint> res;
  res.append(pos);
  QPoint istep;
  switch(o) {
  case(Ostd): istep=QPoint(1,0); break;
  case(O90): istep=QPoint(0,1); break;
  case(O180): istep=QPoint(-1,0); break;
  case(O270): istep=QPoint(0,-1); break;
  } 
  for(int i=1; i<len; i++) 
    res.append(pos+i*istep);
  return res;
}

// blocks occupied: integer shape
QList<QPoint> FfConveyor::TestArea(const QPoint& pos,Orientation o) {
  return TestArea(pos,o,mLength);
}

// have a new editr
QWidget* FfConveyor::NewEditor(void) {
  return new FfConveyorEditor(this);  
}

// paint myself
void FfConveyor::paint(
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
void FfConveyor::DoAdvanceTime(qreal dt) {
  // mute sensor of faults
  bool wpm = FAULT_WPS;
  mSensors.at(SNS_WPSENSORA)->mMute = wpm;
  if(mLength>1)
    mSensors.at(SNS_WPSENSORB)->mMute = wpm;
  // belt dynamics
  bool bma=Signal(SIG_MOTORA);
  bool bmb=Signal(SIG_MOTORB);
  qreal bmv=(FAULT_BM ? 0.0 : 1.0);
  if(bma) {
    mBeltPos-= dt * mBeltVelocity * bmv;
    while(mBeltPos<0) mBeltPos+=20;
    mBelt->SetPosition(mBeltPos);
    mVelocity= mNormRail * (-1) * mBeltVelocity * bmv;
  }
  if(bmb){
    mBeltPos+= dt * mBeltVelocity * bmv;
    while(mBeltPos>20) mBeltPos-=20;
    mBelt->SetPosition(mBeltPos);
    mVelocity=mNormRail * mBeltVelocity * bmv;
  }
  if(!bma)
  if(!bmb) {
    mVelocity=QPointF(0,0);
  }
  if(bma && bmb) {
    // error
  }

}

// report workpiece
void FfConveyor::DoWorkpiece(FfWorkpiece* wp) {
  (void) wp;
}

// execute user command
void FfConveyor::DoUserCommand(QString cmd) {
 FF_DQM("FfConveyor::DoUserCommand()" << cmd);
 // stop
 if(cmd=="Stop") {
   SetSignal(SIG_MOTORA,false);
   SetSignal(SIG_MOTORB,false);
   return;
 }
 if(cmd=="Right") {
   SetSignal(SIG_MOTORA,false); SetSignal(SIG_MOTORB,true); 
   return;
 }  
 if(cmd=="Left") {
   SetSignal(SIG_MOTORB,false); SetSignal(SIG_MOTORA,true); 
   return;
 }  
}




/*
*********************************************************
*********************************************************
*********************************************************

FfConveyorEditor

*********************************************************
*********************************************************
*********************************************************
*/


// constructor
FfConveyorEditor::FfConveyorEditor(FfComponent* comp) :
  FfComponentEditor(comp,false)
{
  // my parameters: length
  QLabel* nlabel = new QLabel("Length");
  nlabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed));
  mLengthEdit = new QLineEdit();
  connect(mLengthEdit,SIGNAL(editingFinished(void)),this,SLOT(UpdateFromEditor(void)));
  QRegExpValidator* lvalid = new QRegExpValidator(
    QRegExp("[1-9][0-9]*"),0);
  mLengthEdit->setValidator(lvalid);
  mLengthEdit->setMinimumWidth(12*10);
  mLengthEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
  // fill in my values
  DoUpdateFromComponent(comp);
  // add to layout
  mGbox->addWidget(nlabel,1,0);
  mGbox->addWidget(mLengthEdit,1,1);
  // have my faults here
  InsertFaultEditor(comp);
}

// updater
void FfConveyorEditor::DoUpdateFromComponent(FfComponent* comp) {
  // run my cast
  FfConveyor* conv = dynamic_cast<FfConveyor*>(comp);
  if(!conv) return;
  // set my editor
  mLengthEdit->setText(QString("%1").arg(conv->Length()));
}

// updater
void FfConveyorEditor::DoUpdateFromEditor(FfComponent* comp) {
  // run my cast
  FfConveyor* conv = dynamic_cast<FfConveyor*>(comp);
  if(!conv) return;
  FF_DQ("FfConveyorEditor::DoUpdateFromEditor()");
  // set my parameter
  int nlen= mLengthEdit->text().toShort();
  if(nlen>0 && nlen <10) 
  if(nlen!=conv->Length()) {
    QList<QPoint> narea=conv->TestArea(conv->Position(),conv->Orient(),nlen);
    if(mScene->ReconfigureComponent(comp,narea)) {
    conv->SetLength(nlen);
    conv->Update(); // this issues the parameter changed signal
    mScene->AdjustScene();
    }
  }
}
