/* multiproc.cpp  - universal machine */

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

#include "ffmultimach.h"
#include "ffworkpiece.h"
#include "ffscene.h"

/*
************************************************
************************************************
  
Implementation: FfMultimach

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
#define SIG_TMOTORCW     7
#define SIG_TMOTORCCW    8
#define SIG_TSENSOR      9
#define SIG_MREADY      10
#define SIG_MOPERATE    11

// internal sensor defs
#define SNS_WPSENSORA  0
#define SNS_PSENSORA  1
#define SNS_PSENSORB  2

// internal defs for faults
#define FAULT_PA  (mFaults.at(0)->Value())
#define FAULT_PB  (mFaults.at(1)->Value())
#define FAULT_PC  (mFaults.at(2)->Value())


// default constructor
FfMultimach::FfMultimach(QObject* parent) : FfComponent(parent) {
  FF_DQM("FfMultimach::FfMultimach()");
  // my configuration
  mProcessA="A";  
  mProcessB="B";  
  mProcessC="C";  
  mProcess=mProcessA;
  // my items
  mBelt      = new FfBeltItem(this);
  mUpperRail = new FfRailItem(this);
  mLowerRail = new FfRailItem(this);
  mProcessor = new FfUnivprocItem(0); // no parrent, will become SimOverlay
  // my controls
  mButtonToA = new FfButtonItem(mSimControls);
  mButtonToB = new FfButtonItem(mSimControls);
  mButtonStop = new FfButtonItem(mSimControls);
  mButtonTool = new FfButtonItem(mSimControls);
  mButtonProc = new FfButtonItem(mSimControls);
  // set to overlay
  mSimOverlay=mProcessor;
  mSimOverlay->setZValue(5);
  mSimOverlay->setVisible(mRunning);
  // faults
  FfFault* faultcfg;
  faultcfg= new FfFault(this);
  faultcfg->SetEditorName("Process A Breakdown");
  faultcfg->SetShortName("FA");
  mFaults.append(faultcfg);
  faultcfg= new FfFault(this);
  faultcfg->SetEditorName("Process B Breakdown");
  faultcfg->SetShortName("FB");
  mFaults.append(faultcfg);
  faultcfg= new FfFault(this);
  faultcfg->SetEditorName("Process C Breakdown");
  faultcfg->SetShortName("FC");
  mFaults.append(faultcfg);
  // default name
  SetName("UM1");
  FF_DQM("FfMultimach::FfMultimach(): done");
}



// set my parameters (should imply reset)  
void FfMultimach::SetProcessA(const QString& proc) {
  // ignore on trivial
  if(mProcessA==proc) return;
  // set
  mProcessA=proc;
  mProcess=mProcessA;
}

// set my parameters  
void FfMultimach::SetProcessB(const QString& proc) {
  // ignore on trivial
  if(mProcessB==proc) return;
  // set
  mProcessB=proc;
}

// set my parameters  
void FfMultimach::SetProcessC(const QString& proc) {
  // ignore on trivial
  if(mProcessC==proc) return;
  // set
  mProcessC=proc;
}


// file io
void FfMultimach::AppendToElement(QDomDocument& doc, QDomElement& cele) {
  // construct additional elements
  QDomElement pele = doc.createElement("Parameters");
  pele.setAttribute("proc_a",mProcessA);
  pele.setAttribute("proc_b",mProcessB);
  pele.setAttribute("proc_c",mProcessC);
  // append
  cele.appendChild(pele);
}

// file io
void FfMultimach::ExtractFromElement(QDomElement& elem) {
  // interpret parameters
  QDomElement pele=elem.firstChildElement("Parameters");
  QString apstr = pele.attribute("proc_a");
  if(apstr=="") apstr="A";
  QString bpstr = pele.attribute("proc_b");
  if(bpstr=="") bpstr="B";
  QString cpstr = pele.attribute("proc_c");
  if(cpstr=="") cpstr="C";
  // set parameter
  SetProcessA(apstr);
  SetProcessB(bpstr);
  SetProcessC(cpstr);
}


// reset state
void FfMultimach::DoReset(void) {
  // parameters
  mBeltVelocity=100*FfStyle::mPitch; // pt per sec
  mPositionVelocity=100*FfStyle::mPitch;; // pt per sec
  mToolselVelocity= (360.0/6.0) *FfStyle::mPitch; // deg per sec
  mProcessVelocity=50*FfStyle::mPitch; // % per sec
  // dynamic state
  mBeltPos=0;        // ranges 0 ... 20
  mAutoProcess=0;
  mAutoTool=0;
  pProcWorkpiece=0;
  // transport
  mVelocity = QPointF(0,0);
  // item consequences
  mBelt->SetPosition(mBeltPos);
  mProcessor->SetProgress(0);
  mProcessor->SetPosition(0);
  mProcessor->SetToolsel(0);
  mProcess=mProcessA;
}

// update all 
void FfMultimach::DoUpdateAll() {
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
  sigcfg->SetRelAddress(SIG_TMOTORCW);
  sigcfg->SetLastName("TCW");
  sigcfg->SetActuator();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "tcw");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "tccw" << "toff");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_TMOTORCCW);
  sigcfg->SetLastName("TCCW");
  sigcfg->SetActuator();
  sigcfg->SetPositiveEdgeEvents(QStringList() << "tccw");
  sigcfg->SetNegativeEdgeEvents(QStringList() << "tcw" << "toff");
  mSignals.append(sigcfg);
  sigcfg= new FfSignal(this);
  sigcfg->SetRelAddress(SIG_TSENSOR);
  sigcfg->SetLastName("TS");
  sigcfg->SetSensor();
  sigcfg->SetInitialValue(true);
  sigcfg->SetPositiveEdgeEvents(QStringList() << "trd");
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
  mBasePlate.addRect(-60,-80-150,120,150);
  mBasePlate=mTransOrient.map(mBasePlate);
  mBaseCirc=QPainterPath();
  mBaseCirc.addEllipse(-85,-85-185,170,170);
  mBaseCirc=mTransOrient.map(mBaseCirc);
  mInnerCirc=QPainterPath();
  mInnerCirc.addEllipse(-45,-45-185,90,90);
  mInnerCirc=mTransOrient.map(mInnerCirc);
  // my controls
  mButtonToA->SetLeft(QPointF(-55,0));
  mButtonStop->SetStop(QPointF(0,0));
  mButtonToB->SetRight(QPointF(+55,0));
  mButtonTool->SetCircle(QPointF(0,-185));
  mButtonProc->SetDownSkip(QPointF(0,-260));
  mSimControls->SetVisible(false);
  // fix orientation: base
  FfComponent::DoUpdateAll();
  // base does not fix overlay
  mSimOverlay->resetTransform();
  mSimOverlay->setTransform(mTransOrient);
}


// blocks occupied: integer shape
QList<QPoint> FfMultimach::TestArea(const QPoint& pos,Orientation o) {
  QList<QPoint> res;
  res.append(pos);
  QPoint pstep;
  switch(o) {
  case(Ostd): pstep=QPoint(0,-1); break;
  case(O90):  pstep=QPoint(1,0); break;
  case(O180): pstep=QPoint(0,1); break;
  case(O270): pstep=QPoint(-1,0); break;
  } 
  res.append(pos+pstep);
  return res;
}

// have a new editr
QWidget* FfMultimach::NewEditor(void) {
  return new FfMultimachEditor(this);  
}

// paint myself
void FfMultimach::paint(
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
  painter->setPen(sc->EffectColor(VioGrey));
  painter->setBrush(sc->EffectColor(VioLightGrey));
  painter->drawPath(mInnerCirc);
  // debug: call base
  FfComponent::paint(painter,option,widget);
}

// update dynamics
void FfMultimach::DoAdvanceTime(qreal dt) {

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

  // autotool
  if(mAutoTool==1) {
    SetSignal(SIG_TMOTORCW, true);
    SetSignal(SIG_TMOTORCCW,false);
    mAutoTool=2;
  }
  if(mAutoTool==2 && !Signal(SIG_TSENSOR)) {
    SetSignal(SIG_TMOTORCW, true);
    SetSignal(SIG_TMOTORCCW,false);
    mAutoTool=3;
  }
  if(mAutoTool==3 && Signal(SIG_TSENSOR)) {
    SetSignal(SIG_TMOTORCW, false);
    SetSignal(SIG_TMOTORCCW,false);
    mAutoTool=0;
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



  // tool select 
  bool tcw=  Signal(SIG_TMOTORCW);
  bool tccw= Signal(SIG_TMOTORCCW);
  qreal tsel = mProcessor->Toolsel();
  // test for blocks
  // .... set mRefuseAdvance tio true ob bloc/
  // refuse
  if(mRefuseAdvance) {
    tcw=false; tccw=false;
  }
  // do position
  if(tcw) {
    tsel +=  dt * mToolselVelocity;
    if(tsel>360) tsel-=360; 
    mProcessor->SetToolsel(tsel);
  }
  if(tccw) {
    tsel -= dt * mToolselVelocity;
    if(tsel<0) tsel+=360; 
    mProcessor->SetToolsel(tsel);
  }
  if(tcw && tccw){
     //error
  }
  if(tcw || tccw) { // needs more tolerance than sensor 
    mProcess="*";
    if(   -3 < tsel && tsel <    +3) mProcess=mProcessA;
    if(120-3 < tsel && tsel < 120+3) mProcess=mProcessB; 
    if(240-3 < tsel && tsel < 240+3) mProcess=mProcessC;
    if(360-3 < tsel && tsel < 360+3) mProcess=mProcessA;
  }
  // track signals
  SetSignal(SIG_TSENSOR,(mProcessor->Sensor()));


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
    qreal pvel=mProcessVelocity;
    if(FAULT_PA) if(mProcess==mProcessA) pvel=0;
    if(FAULT_PB) if(mProcess==mProcessB) pvel=0;
    if(FAULT_PC) if(mProcess==mProcessC) pvel=0;
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
void FfMultimach::DoWorkpiece(FfWorkpiece* wp) {
  (void) wp;
}

// execute user command
void FfMultimach::DoUserCommand(QString cmd) {
 FF_DQM("FfMultimach::DoUserCommand()" << cmd);
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
   return;
 }  
 // change tool
 if(cmd=="Circle") {
   if(mAutoProcess!=0) return;
   SetSignal(SIG_BMOTORA,false);
   SetSignal(SIG_BMOTORB,false);
   SetSignal(SIG_PMOTORB,false); 
   SetSignal(SIG_PMOTORA,false); 
   mAutoTool=1;
   return;
 }  
}




/*
*********************************************************
*********************************************************
*********************************************************

FfMultimachEditor

*********************************************************
*********************************************************
*********************************************************
*/


// constructor
FfMultimachEditor::FfMultimachEditor(FfComponent* comp) :
  FfComponentEditor(comp,false)
{
  // my parameters: proc types
  QLabel* nlabel = new QLabel("Proc.");
  mProcAEdit = new QLineEdit();
  mProcBEdit = new QLineEdit();
  mProcCEdit = new QLineEdit();
  connect(mProcAEdit,SIGNAL(editingFinished(void)),this,SLOT(UpdateFromEditor(void)));
  connect(mProcBEdit,SIGNAL(editingFinished(void)),this,SLOT(UpdateFromEditor(void)));
  connect(mProcCEdit,SIGNAL(editingFinished(void)),this,SLOT(UpdateFromEditor(void)));
  QRegExpValidator* lvalid = new QRegExpValidator(
    QRegExp("[A-Z]"),0);
  mProcAEdit->setValidator(lvalid);
  mProcBEdit->setValidator(lvalid);
  mProcCEdit->setValidator(lvalid);
  // fill in my values
  DoUpdateFromComponent(comp);
  // add to layout
  QWidget* wline = new QWidget();
  wline->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed));
  QHBoxLayout* sline = new QHBoxLayout(wline);
  sline->setSpacing(2);
  sline->setMargin(0);
  mProcAEdit->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed));
  mProcAEdit->setFixedWidth(12*5);
  mProcBEdit->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed));
  mProcBEdit->setFixedWidth(12*5);
  mProcCEdit->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed));
  mProcCEdit->setFixedWidth(12*5);
  sline->addWidget(nlabel,Qt::AlignLeft);
  sline->addWidget(new QLabel("[A/B/C] = ["));
  sline->addWidget(mProcAEdit);
  sline->addWidget(new QLabel("/"));
  sline->addWidget(mProcBEdit);
  sline->addWidget(new QLabel("/"));
  sline->addWidget(mProcCEdit);
  sline->addWidget(new QLabel("]"));
  sline->addWidget(new QLabel(" "),1,Qt::AlignRight);
  //sline->addStretch(10);
  //sline->addWidget(new QLabel(" "),1,Qt::AlignRight);
  mGbox->addWidget(nlabel,1,0);
  mGbox->addWidget(wline,1,1,Qt::AlignLeft);
  // have my faults here
  InsertFaultEditor(comp);
}

// updater
void FfMultimachEditor::DoUpdateFromComponent(FfComponent* comp) {
  // run my cast
  FfMultimach* conv = dynamic_cast<FfMultimach*>(comp);
  if(!conv) return;
  // set my editor
  mProcAEdit->setText(QString("%1").arg(conv->ProcessA()));
  mProcBEdit->setText(QString("%1").arg(conv->ProcessB()));
  mProcCEdit->setText(QString("%1").arg(conv->ProcessC()));
}

// updater
void FfMultimachEditor::DoUpdateFromEditor(FfComponent* comp) {
  // run my cast
  FfMultimach* conv = dynamic_cast<FfMultimach*>(comp);
  if(!conv) return;
  FF_DQ("FfMultimachEditor::DoUpdateFromEditor()");
  // set my parameter
  conv->SetProcessA(mProcAEdit->text());
  conv->SetProcessB(mProcBEdit->text());
  conv->SetProcessC(mProcCEdit->text());
}
