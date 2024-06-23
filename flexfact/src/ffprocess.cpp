/* process.cpp  - processing machine */

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

#include "ffprocess.h"
#include "ffworkpiece.h"
#include "ffscene.h"

/*
************************************************
************************************************
  
Implementation: FfProcess

************************************************
************************************************
*/

// internal defs for signals
#define SIG_BMOTORA     0
#define SIG_BMOTORB     1
#define SIG_WPSENSORA   2
#define SIG_PMOTORB     3
#define SIG_PMOTORA     4
#define SIG_PSENSORB    5
#define SIG_PSENSORA    6
#define SIG_MREADY      7
#define SIG_MOPERATE    8

#define SNS_WPSENSORA  0
#define SNS_PSENSORA  1
#define SNS_PSENSORB  2

// internal defs for faults
#define FAULT_PROC  (mFaults.at(0)->Value())

// default constructor
FfProcess::FfProcess(QObject* parent) : FfComponent(parent) {
  FF_DQM("FfProcess::FfProcess()");
  // my configuration
  mProcess="A";
  // my items
  mBelt = new FfBeltItem(this);
  mUpperRail = new FfRailItem(this);
  mLowerRail = new FfRailItem(this);
  mProcessor = new FfProcessorItem(0);
  // my controls
  mButtonToA = new FfButtonItem(mSimControls);
  mButtonToB = new FfButtonItem(mSimControls);
  mButtonStop = new FfButtonItem(mSimControls);
  mButtonProc = new FfButtonItem(mSimControls);
  // set to overlay
  mSimOverlay=mProcessor;
  mSimOverlay->setZValue(5);
  if(!mRunning) mSimOverlay->setVisible(false);
  // faults
  FfFault* faultcfg;
  faultcfg= new FfFault(this);
  faultcfg->SetEditorName("Process Breakdown");
  faultcfg->SetShortName("FP");
  mFaults.append(faultcfg);
  // default name
  SetName("PM1");
  FF_DQM("FfProcess::FfProcess(): done");
}



// set my parameters
void FfProcess::SetProcess(const QString& proc) {
  // ignore on trivial
  if(mProcess==proc) return;
  // set
  mProcess=proc;
}


// file io
void FfProcess::AppendToElement(QDomDocument& doc, QDomElement& cele) {
  // construct additional elements
  QDomElement pele = doc.createElement("Parameters");
  pele.setAttribute("proc",mProcess);
  // append
  cele.appendChild(pele);
}

// file io
void FfProcess::ExtractFromElement(QDomElement& elem) {
  // interpret parameters
  QDomElement pele=elem.firstChildElement("Parameters");
  QString pstr = pele.attribute("proc");
  if(pstr=="") pstr="A";
  // set parameter
  SetProcess(pstr);
}


// reset state
void FfProcess::DoReset(void) {
  // parameters
  mBeltVelocity=100*FfStyle::mPitch; // pt per sec
  mPositionVelocity=100*FfStyle::mPitch;; // pt per sec
  mProcessVelocity=50*FfStyle::mPitch; // % per sec
  // dynamic state
  mBeltPos=0;        // ranges 0 ... 20
  mAutoProcess=0;
  pProcWorkpiece=0;
  // transport
  mVelocity = QPointF(0,0);
  // item consequences
  mBelt->SetPosition(mBeltPos);
  mProcessor->SetProgress(0);
  mProcessor->SetPosition(0);
}

// update all 
void FfProcess::DoUpdateAll() {
  // my signals
  FfSignal* sigcfg;
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_BMOTORA);
  sigcfg->SetLastName("BM-");
  sigcfg->SetActuator();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "bm-");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "bm+" << "boff");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_BMOTORB);
  sigcfg->SetLastName("BM+");
  sigcfg->SetActuator();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "bm+");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "bm-" << "boff");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_WPSENSORA);
  sigcfg->SetLastName("WPS");
  sigcfg->SetSensor();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "wpar");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "wplv");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_PMOTORB);
  sigcfg->SetLastName("PM+");
  sigcfg->SetActuator();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "pm+");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "pm-" << "poff");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_PMOTORA);
  sigcfg->SetLastName("PM-");
  sigcfg->SetActuator();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "pm-");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "pm+" << "poff");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_PSENSORB);
  sigcfg->SetLastName("PS+");
  sigcfg->SetSensor();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "ps+");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_PSENSORA);
  sigcfg->SetLastName("PS-");
  sigcfg->SetSensor();
  sigcfg->SetNegativeEdgeEvents(QStringList() << "ps-");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_MREADY);
  sigcfg->SetLastName("MRD");
  sigcfg->SetSensor();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "mrqu");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "mack");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_MOPERATE);
  sigcfg->SetLastName("MOP");
  sigcfg->SetActuator();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "mon");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "moff");
  mSignals.append(sigcfg);
  // my sensors: (re-)instantiate
  mSensors.append(new FfSensorItem(this));
  mSensors.append(new FfSensorItem(this));
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
  sns  = mSensors.at(SNS_PSENSORA);
  sns->mIndicatorPath.addRect(-75,-65,10,10);
  sns->mWpSensor=false;
  sns->mValue=false;
  sns->mRelAddress=SIG_PSENSORA;
  sns->Update();
  sns  = mSensors.at(SNS_PSENSORB);
  sns->mIndicatorPath.addRect(-75,+55,10,10);
  sns->mWpSensor=false;
  sns->mValue=false;
  sns->mRelAddress=SIG_PSENSORB;
  sns->Update();

  // outer shape: pos only
  QRectF brect(-100,-300,200,400);
  mOuterRect=brect.adjusted(-2,-2,+2,+2);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  mCoreShape=QPainterPath();
  mCoreShape.addRect(brect);
  // transport: testing
  mRail=QLineF(-100,0,100,0);
  mNormRail=FfStyle::NormalizeF(mRail.p2()-mRail.p1());
  mBlock=QPainterPath();
  mBlock.addRect(-100,-300,200,250);
  mBlock.addRect(-100,50,200,50);
  // my items
  mBelt->SetLength();
  mBelt->SetTransform(mTransOrient);
  mUpperRail->SetUpper();
  mUpperRail->setTransform(mTransOrient);
  mLowerRail->SetLower();
  mLowerRail->setTransform(mTransOrient);
  mBasePlate=QPainterPath();
  mBasePlate.addRect(-60,-80-100,120,100);
  mBasePlate=mTransOrient.map(mBasePlate);
  mBaseCirc=QPainterPath();
  mBaseCirc.addEllipse(-28,-130-28,56,56);
  mBaseCirc=mTransOrient.map(mBaseCirc);
  // my controls
  mButtonToA->SetLeft(QPointF(-55,0));
  mButtonStop->SetStop(QPointF(0,0));
  mButtonToB->SetRight(QPointF(+55,0));
  mButtonProc->SetDownSkip(QPointF(0,-260));
  mSimControls->SetVisible(false);
  // fix orientation: base
  FfComponent::DoUpdateAll();
  // base does not fix overlay
  mSimOverlay->resetTransform();
  mSimOverlay->setTransform(mTransOrient);
}


// blocks occupied: integer shape
QList<QPoint> FfProcess::TestArea(const QPoint& pos,Orientation o) {
  QList<QPoint> res;
  res.append(pos);
  QPoint pstep;
  switch(o) {
  case(Ostd): pstep=QPoint(0,-1); break;
  case(O90): pstep=QPoint(1,0); break;
  case(O180): pstep=QPoint(0,1); break;
  case(O270): pstep=QPoint(-1,0); break;
  } 
  res.append(pos+pstep);
  return res;
}

// have a new editr
QWidget* FfProcess::NewEditor(void) {
  return new FfProcessEditor(this);  
}

// paint myself
void FfProcess::paint(
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
  painter->setPen(sc->EffectColor(VioGrey));
  painter->setBrush(sc->EffectColor(VioLightGrey));
  painter->drawPath(mBasePlate);
  painter->setPen(sc->EffectColor(VioDarkRed));
  painter->setBrush(sc->EffectColor(VioRed));
  painter->drawPath(mBaseCirc);
  // debug: call base
  FfComponent::paint(painter,option,widget);
}

// update dynamics
void FfProcess::DoAdvanceTime(qreal dt) {
  // belt dynamics
  bool bma=Signal(SIG_BMOTORA);
  bool bmb=Signal(SIG_BMOTORB);
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

  // autoprocess
  if(mAutoProcess==1) {
    SetSignal(SIG_PMOTORA,false);
    SetSignal(SIG_PMOTORB,true);
    mAutoProcess=2;
  }
  if(mAutoProcess==2 && Signal(SIG_PSENSORB)) {
    SetSignal(SIG_PMOTORA,false);
    SetSignal(SIG_PMOTORB,false);
    mAutoProcess=3;
  }
  if(mAutoProcess==3 && Signal(SIG_MREADY)) {
    SetSignal(SIG_MOPERATE,true);
    mAutoProcess=4;
  }
  if(mAutoProcess==4 && !Signal(SIG_MREADY)) {
    SetSignal(SIG_MOPERATE,false);
    mAutoProcess=5;
  }
  if(mAutoProcess==5) {
    SetSignal(SIG_PMOTORA,true);
    SetSignal(SIG_PMOTORB,false);
    mAutoProcess=6;
  }
  if(mAutoProcess==6 && !Signal(SIG_PSENSORA)) {
    SetSignal(SIG_PMOTORA,false);
    SetSignal(SIG_PMOTORB,false);
    FF_DQM("FfProcessor::DoAdvanceTime(): autoprocess off with siga " << Signal(SIG_PSENSORA));
    mAutoProcess=0;
  }

  // machine positioning
  bool pma=Signal(SIG_PMOTORA);
  bool pmb=Signal(SIG_PMOTORB);
  qreal ppos = mProcessor->Position();
  // test for blocks
  /*
  if(pma) {
    FfScene* sc=Scene();
    if(!sc) return;   
    QPointF nfpos(j*400, ppos -50 + dt * mPusherVelocity);
    QList<FfWorkpiece*> wplist= sc->Workpieces(mUpperRail->mapToScene(nfpos));
    if(wplist.size()>0) {
      foreach(FfWorkpiece* wp,wplist) wp->SetError(true);
      mRefuseAdvance=true;
    }
  }
  */
  // refuse
  if(mRefuseAdvance) {
    pma=false; pmb=false;
  }
  // do position
  if(pmb) {
    ppos +=  dt * mPositionVelocity;
    if(ppos>130) ppos=130; // error
    mProcessor->SetPosition(ppos);
  }
  if(pma) {
    ppos -= dt * mPositionVelocity;
    if(ppos<0) ppos=0; // error
    mProcessor->SetPosition(ppos);
  }
  if(pma)
  if(pmb){
     //error
  }
  // track signals
  SetSignal(SIG_PSENSORA,(ppos>1));
  SetSignal(SIG_PSENSORB,(ppos>129));
  mSensors[SNS_PSENSORA]->SetValue(Signal(SIG_PSENSORA));
  mSensors[SNS_PSENSORB]->SetValue(Signal(SIG_PSENSORB));


  // figure wp enter/exit process
  FfWorkpiece* wp = mSensors.at(SNS_WPSENSORA)->mWpReference;
  qreal pprog = mProcessor->Progress();
  if(wp!=0 && pProcWorkpiece==0 && pprog == 0) {
   FF_DQM("FfProcess(): wp enter");
   pProcWorkpiece = wp;
  }
  if(pProcWorkpiece != 0 && pprog >= 99) {
    FF_DQM("FfProcess(): wp done");
    pProcWorkpiece->Process(QString("%1_%2").arg(mName).arg(mProcess));
    pProcWorkpiece = 0;
  }
  if(pProcWorkpiece != 0 && wp==0) {
    FF_DQM("FfProcess(): wp left");
    pProcWorkpiece=0;
  }

  // machine operation
  if(Signal(SIG_PSENSORB) && !Signal(SIG_MREADY) && pprog<=1) {
    SetSignal(SIG_MREADY,true);
  }
  if(Signal(SIG_MOPERATE) && Signal(SIG_MREADY) && pprog <100) {
    qreal pvel = (FAULT_PROC ? 0.0 : mProcessVelocity);
    pprog += dt * pvel;
    if(pprog>100) pprog=100;
    mProcessor->SetProgress(pprog);
  }
  if(pprog >=99) {
    SetSignal(SIG_MREADY,false);
  }
  if(!Signal(SIG_PSENSORB) && pprog>0) {
    pprog=0;
    mProcessor->SetProgress(pprog);
    SetSignal(SIG_MREADY,false);
  }

}

// report workpiece
void FfProcess::DoWorkpiece(FfWorkpiece* wp) {
  (void) wp;
}

// execute user command
void FfProcess::DoUserCommand(QString cmd) {
 FF_DQM("FfProcess::DoUserCommand()" << cmd);
 // stop
 if(cmd=="Stop") {
   SetSignal(SIG_BMOTORA,false);
   SetSignal(SIG_BMOTORB,false);
   return;
 }
 if(cmd=="Right") {
   SetSignal(SIG_BMOTORA,false); SetSignal(SIG_BMOTORB,true); 
   return;
 }  
 if(cmd=="Left") {
   SetSignal(SIG_BMOTORB,false); SetSignal(SIG_BMOTORA,true); 
   return;
 }  
 // process
 if(cmd=="DownSkip") {
   SetSignal(SIG_BMOTORA,false);
   SetSignal(SIG_BMOTORB,false);
   mAutoProcess=1;
   SetSignal(SIG_PMOTORB,true); 
   SetSignal(SIG_PMOTORA,false); 
   return;
 }  
}




/*
*********************************************************
*********************************************************
*********************************************************

FfProcessEditor

*********************************************************
*********************************************************
*********************************************************
*/


// constructor
FfProcessEditor::FfProcessEditor(FfComponent* comp) :
  FfComponentEditor(comp,false)
{
  // my parameters: length
  QLabel* nlabel = new QLabel("Process");
  nlabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed));
  mTypeEdit = new QLineEdit();
  connect(mTypeEdit,SIGNAL(editingFinished(void)),this,SLOT(UpdateFromEditor(void)));
  QRegExpValidator* lvalid = new QRegExpValidator(
    QRegExp("[A-Z]"),0);
  mTypeEdit->setValidator(lvalid);
  mTypeEdit->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed));
  // fill in my values
  DoUpdateFromComponent(comp);
  // add to layout
  mGbox->addWidget(nlabel,1,0);
  mGbox->addWidget(mTypeEdit,1,1);
  // have my faults here
  InsertFaultEditor(comp);
}

// updater
void FfProcessEditor::DoUpdateFromComponent(FfComponent* comp) {
  // run my cast
  FfProcess* conv = dynamic_cast<FfProcess*>(comp);
  if(!conv) return;
  // set my editor
  mTypeEdit->setText(QString("%1").arg(conv->Process()));
}

// updater
void FfProcessEditor::DoUpdateFromEditor(FfComponent* comp) {
  // run my cast
  FfProcess* conv = dynamic_cast<FfProcess*>(comp);
  if(!conv) return;
  FF_DQ("FfProcessEditor::DoUpdateFromEditor()");
  // set my parameter
  QString proc= mTypeEdit->text();
  if(proc!=conv->Process()) {
    conv->SetProcess(proc);
    conv->Update();
  }
}
