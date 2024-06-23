/* distributor.cpp  - wp distribution unit */

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

#include "ffdistributor.h"
#include "ffworkpiece.h"
#include "ffscene.h"

/*
************************************************
************************************************
  
Implementation: FfDistributor

************************************************
************************************************
*/

// internal defs for signals
#define SIG_BMOTORA     0
#define SIG_BMOTORB     1
#define SIG_PMOTORA(j)  (5*(j)+2)
#define SIG_PMOTORB(j)  (5*(j)+3)
#define SIG_PSENSORA(j)  (5*(j)+4)
#define SIG_PSENSORB(j)  (5*(j)+5)
#define SIG_PSENSORWP(j)  (5*(j)+6)


// default constructor
FfDistributor::FfDistributor(QObject* parent) : FfComponent(parent) {
  FF_DQM("FfDistributor::FfDistributor()");
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
  // default name
  SetName("DS1");
  FF_DQM("FfDistributor::FfDistributor(): done");
}



// set my parameters
void FfDistributor::SetLength(int nlen) {
  // ignore on trivial
  if(mLength==nlen) return;
  // test in scene
  if(FfScene* sc=Scene()) {
    // see editor / todo
      (void) sc;
  }
  // set
  mLength=nlen;
}


// file io
void FfDistributor::AppendToElement(QDomDocument& doc, QDomElement& cele) {
  // construct additional elements
  QDomElement pele = doc.createElement("Parameters");
  pele.setAttribute("len",mLength);
  // append
  cele.appendChild(pele);
}

// file io
void FfDistributor::ExtractFromElement(QDomElement& elem) {
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
void FfDistributor::DoReset(void) {
  // parameters
  mBeltVelocity=100*FfStyle::mPitch; // pt per sec
  mPusherVelocity=75*FfStyle::mPitch; // pt per sec (be slower than std belt)
  // dynamic state
  mBeltPos=0;        // ranges 0 ... 20
  mBelt->SetPosition(mBeltPos);
  // transport
  mVelocity = QPointF(0,0);
  // pusher
  foreach(FfPusherItem* pusher, mPushers) {
    pusher->SetPosition(0);
    pusher->mActive=false;
    pusher->mAutoPush=false;
  }
}

// update all 
void FfDistributor::DoUpdateAll() {
  // delete all pusher items
  while(mPushers.size()>0) {
    FfPusherItem* pusher= mPushers.front(); 
    delete pusher->mButton;
    delete pusher;
    mPushers.pop_front();
  }
  // instatiate pushers
  for(int i=0; i<mLength; i+=2) {
    FfPusherItem* pusher= new FfPusherItem(this);
    pusher->SetOrigin(i);
    pusher->mButton = new FfButtonItem(mSimControls);
    mPushers.append(pusher);
  }
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

  for(int j=0; j<mPushers.size(); j++) {
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_PMOTORA(j));
    sigcfg->SetLastName(QString("P%1M+").arg(j+1));
    sigcfg->SetActuator();
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("p%1m+").arg(j+1));
    sigcfg->SetNegativeEdgeEvents(QStringList() << QString("p%1m-").arg(j+1) << QString("p%1off").arg(j+1));
    mSignals.append(sigcfg);
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_PMOTORB(j));
    sigcfg->SetLastName(QString("P%1M-").arg(j+1));
    sigcfg->SetActuator();
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("p%1m-").arg(j+1));
    sigcfg->SetNegativeEdgeEvents(QStringList() << QString("p%1m+").arg(j+1) << QString("p%1off").arg(j+1));
    mSignals.append(sigcfg);
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_PSENSORA(j));
    sigcfg->SetLastName(QString("P%1S+").arg(j+1));
    sigcfg->SetSensor();
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("p%1s+").arg(j+1));
    mSignals.append(sigcfg);
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_PSENSORB(j));
    sigcfg->SetLastName(QString("P%1S-").arg(j+1));
    sigcfg->SetSensor();
    sigcfg->SetInitialValue(true);
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("p%1s-").arg(j+1));
    mSignals.append(sigcfg);
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_PSENSORWP(j));
    sigcfg->SetLastName(QString("P%1WP").arg(j+1));
    sigcfg->SetSensor();
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("p%1wpar").arg(j+1));
    sigcfg->SetNegativeEdgeEvents(QStringList() << QString("p%1wplv").arg(j+1));
    mSignals.append(sigcfg);
  }
  // pusher sensors 
  for(int j=0; j<mPushers.size(); j++) {
    FfPusherItem* pusher=mPushers[j];
    FfSensorItem* sns;
    sns=pusher->mSensorA;
    sns->mRelAddress=SIG_PSENSORA(j);
    mSensors.append(sns);
    sns=pusher->mSensorB;
    sns->mRelAddress=SIG_PSENSORB(j);
    mSensors.append(sns);
    sns=pusher->mSensorWp;
    sns->mRelAddress=SIG_PSENSORWP(j);
    mSensors.append(sns);
  }
  // outer shape
  mCoreShape=QPainterPath();
  mCoreShape.moveTo(-100,100);
  for(int i=0; i<mLength; i++) {
    if(i%2==0) {
      mCoreShape.lineTo(-100+i*200,-300);
      mCoreShape.lineTo( 100+i*200,-300);
    } else {
      mCoreShape.lineTo(-100+i*200,-100);
      mCoreShape.lineTo( 100+i*200,-100);
    }
  }
  mCoreShape.lineTo(100+(mLength-1)*200,+100);
  mCoreShape.closeSubpath();
  QRectF brect = mCoreShape.boundingRect();
  mOuterRect=brect.adjusted(-2,-2,+2,+2);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  // transport: testing
  mRail=QLineF(-100,0,-100+200*mLength,0);
  mNormRail=FfStyle::NormalizeF(mRail.p2()-mRail.p1());
  mBlock=QPainterPath();
  mBlock.addRect(-100,-100,200*mLength,50);
  mBlock.addPath(mLowerRail->shape());
  // my items
  mBelt->SetLength(mLength);
  mUpperRail->SetUpperP(mLength);
  mLowerRail->SetLowerP(mLength);
  mBelt->SetTransform(mTransOrient);
  mUpperRail->setTransform(mTransOrient);
  mLowerRail->setTransform(mTransOrient);
  for(int j=0; j<mPushers.size(); j++) {
    mPushers[j]->setTransform(mTransOrient);
    mPushers[j]->mBackgroundPath = mTransOrient.map(mPushers[j]->mBackgroundPath); 
    mPushers[j]->mRail = mTransOrient.map(mPushers[j]->mRail); 
    mPushers[j]->mNormRail = mTransOrient.map(mPushers[j]->mNormRail); 
    mPushers[j]->mButton->SetDownSkip(QPointF(j*400,0));
    mPushers[j]->mButton->SetText(QString("%1").arg(j+1));
    mPushers[j]->mButton->setVisible(false);
  }
  // my controls
  mButtonToA->SetLeft(QPointF(200-55,0));
  mButtonStop->SetStop(QPointF(200,0));
  mButtonToB->SetRight(QPointF(200+55,0));
  mSimControls->SetVisible(false);
  // fix orientation: base
  FfComponent::DoUpdateAll();
}

// blocks occupied: integer shape shape
QList<QPoint> FfDistributor::TestArea(const QPoint& pos, Orientation o, int len) {
  QList<QPoint> res;
  res.append(pos);
  QPoint istep;
  QPoint pstep;
  switch(o) {
  case(Ostd): istep=QPoint(1,0); pstep=QPoint(0,-1);  break;
  case(O90):  istep=QPoint(0,1); pstep=QPoint(1,0);   break;
  case(O180): istep=QPoint(-1,0); pstep=QPoint(0,1);  break;
  case(O270): istep=QPoint(0,-1); pstep=QPoint(-1,0); break;
  } 
  for(int i=1; i<len; i++) 
    res.append(pos+i*istep);
  for(int i=0; i<len; i+=2) 
    res.append(pos+i*istep+pstep);
  return res;
}

// blocks occupied: integer shape
QList<QPoint> FfDistributor::TestArea(const QPoint& pos,Orientation o) {
  return TestArea(pos,o,mLength);
}

// have a new editr
QWidget* FfDistributor::NewEditor(void) {
  return new FfDistributorEditor(this);  
}

// paint myself
void FfDistributor::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // prepare effect colors
  FfScene* sc = qobject_cast<FfScene*>(scene());
  if(!sc) return;
  // draw my background
  painter->setBrush(sc->EffectColor(VioLightGrey));
  painter->setPen(sc->EffectColor(VioGrey));
  foreach(FfPusherItem* pusher,mPushers) {
    painter->drawPath(pusher->mBackgroundPath);
  }
  // debug: call base
  FfComponent::paint(painter,option,widget);
}

// update dynamics
void FfDistributor::DoAdvanceTime(qreal dt) {
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
  // pushers
  for(int j=0; j<mPushers.size(); j++) {
    FfPusherItem* pusher=mPushers[j];
    bool pma=Signal(SIG_PMOTORA(j));
    bool pmb=Signal(SIG_PMOTORB(j));
    qreal ppos = pusher->Position();
    // autopush
    if(pusher->mAutoPush) {
      if((!pma) && (!pmb)) {
        SetSignal(SIG_BMOTORA,false);
        SetSignal(SIG_BMOTORB,false);
        SetSignal(SIG_PMOTORB(j),false);
        SetSignal(SIG_PMOTORA(j),true);
      }
      if((pma) && (ppos==140)) {
        SetSignal(SIG_PMOTORA(j),false);
        SetSignal(SIG_PMOTORB(j),true);
      }
      if((pmb) && (ppos==0)) {
        SetSignal(SIG_PMOTORA(j),false);
        SetSignal(SIG_PMOTORB(j),false);
        pusher->mAutoPush=false;
      }
      pma=Signal(SIG_PMOTORA(j));
      pmb=Signal(SIG_PMOTORB(j));
    }
    // test for block
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
    // refuse
    if(mRefuseAdvance) {
      pma=false; pmb=false;
    }
    // move pusher
    if(pma) {
      ppos +=  dt * mPusherVelocity;
      if(ppos>140) ppos=140; // error
      pusher->SetPosition(ppos);
      //pusher->mVelocity=mNormPusherRail * mPusherVelocity;
    }
    if(pmb) {
      ppos -= dt * mPusherVelocity;
      if(ppos<0) ppos=0; // error
      pusher->SetPosition(ppos);
      //pusher->mVelocity=QPointF(0,0);
    }
    if(!pma)
    if(!pmb){
      //pusher->mVelocity=QPointF(0,0);
    }
    if(pma)
    if(pmb){
      //error
    }
    // update state
    pusher->mActive=true;  
    if(pusher->Position()<=3)
    if(!pma) 
      pusher->mActive=false;  
    // copy signals from sensors
    SetSignal(SIG_PSENSORA(j), pusher->mSensorA->mValue);
    SetSignal(SIG_PSENSORB(j), pusher->mSensorB->mValue);
  }
}

// propagate workpiece
void FfDistributor::AdvanceWorkpiece(
    const qreal& dt,
    const QPointF& pos, const qreal& orient, 
    QPointF& npos, qreal& norient, bool& strict, bool& err)
{
  // default result: stuck
  npos = pos; 
  norient=orient;
  strict=true;
  err=false;
  // are we with an active pusher?
  int atj=-1;
  qreal xpos=mUpperRail->mapFromScene(pos).x();
  qreal ypos=mUpperRail->mapFromScene(pos).y();
  for(int j=0; j<mPushers.size(); j++) {
    if(!mPushers[j]->mActive) continue;
    if(xpos-45 > j*400 +50) continue;
    if(xpos+45 < j*400 -50) continue;
    atj=j; 
    break;
  }
  // pusher controls wp
  if(atj>=0) {
    //FF_DQM("Distributor::AdvanceWorkpiece(): at pusher " << atj);
    if(Signal(SIG_BMOTORA)) err=true;
    if(Signal(SIG_BMOTORB)) err=true;
    if(xpos+45 > atj*400 +60) err=true;
    if(xpos-45 < atj*400 -60) err=true;
    if(ypos-45 <mPushers[atj]->Position()-50) err=true;
    if(err) return;       
    //FF_DQM("Distributor::AdvanceWorkpiece(): at pusher move");
    qreal nppos = mPushers[atj]->Position()-50 + 1 + mPusherVelocity * dt;
    if(nppos > ypos-45) {
      //npos = pos + mPushers[atj]->mNormRail * (nppos-ypos+45);
      ypos = nppos + 45;
      if(nppos> -40 && nppos<0) 
        xpos = (nppos+40)/40*atj*400 +(-nppos)/40 * xpos; 
      if(nppos>0) 
        xpos = atj*400;
      npos= mUpperRail->mapToScene(QPointF(xpos,ypos));
      return;
    }
  }
  // belt controls wp
  npos = pos + Velocity() * dt;
  norient=orient;
  strict=false;
}


// report workpiece
void FfDistributor::DoWorkpiece(FfWorkpiece* wp) {
  (void) wp;
}

// execute user command
void FfDistributor::DoUserCommand(QString cmd) {
 FF_DQM("FfDistributor::DoUserCommand():" << cmd);
 // belt
 if(cmd=="Stop") {
   SetSignal(SIG_BMOTORA,false);
   SetSignal(SIG_BMOTORB,false);
   return;
 }
 if(cmd=="Right") {
   SetSignal(SIG_BMOTORA,false); 
   SetSignal(SIG_BMOTORB,true); 
   return;
 }  
 if(cmd=="Left") {
   SetSignal(SIG_BMOTORB,false); 
   SetSignal(SIG_BMOTORA,true); 
   return;
 }  
 // pusher
 int j=cmd.toShort()-1;
 if(j>=0 && j<mPushers.size())
   mPushers[j]->mAutoPush=true; 

}




/*
*********************************************************
*********************************************************
*********************************************************

FfDistributorEditor

*********************************************************
*********************************************************
*********************************************************
*/


// constructor
FfDistributorEditor::FfDistributorEditor(FfComponent* comp) :
  FfComponentEditor(comp)
{
  // my parameters: length
  QLabel* nlabel = new QLabel("Pushers");
  mLengthEdit = new QLineEdit();
  connect(mLengthEdit,SIGNAL(editingFinished(void)),this,SLOT(UpdateFromEditor(void)));
  QRegExpValidator* lvalid = new QRegExpValidator(
    QRegExp("[1-9][0-9]*"),0);
  mLengthEdit->setValidator(lvalid);
  // fill in my values
  DoUpdateFromComponent(comp);
  // add to layout
  mGbox->addWidget(nlabel,1,0);
  mGbox->addWidget(mLengthEdit,1,1);

}

// updater
void FfDistributorEditor::DoUpdateFromComponent(FfComponent* comp) {
  // run my cast
  FfDistributor* conv = dynamic_cast<FfDistributor*>(comp);
  if(!conv) return;
  // set my editor
  mLengthEdit->setText(QString("%1").arg(conv->Length()/2+1));
}

// updater
void FfDistributorEditor::DoUpdateFromEditor(FfComponent* comp) {
  // run my cast
  FfDistributor* conv = dynamic_cast<FfDistributor*>(comp);
  if(!conv) return;
  FF_DQ("FfDistributorEditor::DoUpdateFromEditor()");
  // set my parameter
  int nlen= mLengthEdit->text().toShort();
  nlen=1+2*(nlen-1);
  if(nlen>0 && nlen <10) 
  if(nlen!=conv->Length()) {
    QList<QPoint> narea=conv->TestArea(conv->Position(),conv->Orient(),nlen);
    if(mScene->ReconfigureComponent(comp,narea)) {
    conv->SetLength(nlen);
    conv->Update();
    mScene->AdjustScene();
    }
  }
}
