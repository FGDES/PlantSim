/* railtrans.cpp  - rail transport system */

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

#include "ffrailtrans.h"
#include "ffworkpiece.h"
#include "ffscene.h"

/*
************************************************
************************************************
  
Implementation: FfRailTransport

************************************************
************************************************
*/

// internal defs for signals
#define SIG_CMOTORB(j) ((j)*5)
#define SIG_CMOTORA(j) ((j)*5+1)
#define SIG_BMOTORB(j) ((j)*5+2)
#define SIG_BMOTORA(j) ((j)*5+3)
#define SIG_SENSORWP(j) ((j)*5+4)
#define SIG_SENSORP(i) (5*mConveyors.size()+ (i))

#define SNS_SENSORP(i) (i)


// default constructor
FfRailTransport::FfRailTransport(QObject* parent) : FfComponent(parent) {
  FF_DQM("FfRailTransport::FfRailTransport()");
  // my configuration
  mLength=4;
  // default name
  SetName("RS1");
  FF_DQM("FfRailTransport::FfRailTransport(): done");
}

// set my parameters
void FfRailTransport::SetLength(int nlen) {
  // ignore on trivial
  if(mLength==nlen) return;
  // test in scene
  if(FfScene* sc=Scene()) {
      (void) sc; // todo?
  }
  // set
  mLength=nlen;
  if(mLength<2) mLength=2;
}


// file io
void FfRailTransport::AppendToElement(QDomDocument& doc, QDomElement& cele) {
  // construct additional elements
  QDomElement pele = doc.createElement("Parameters");
  pele.setAttribute("len",mLength);
  // append
  cele.appendChild(pele);
}

// file io
void FfRailTransport::ExtractFromElement(QDomElement& elem) {
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
void FfRailTransport::DoReset(void) {
  // parameters
  mBeltVelocity=100*FfStyle::mPitch; // pt per sec
  mCartVelocity=100*FfStyle::mPitch; // pt per sec
  // conveyors
  for(int j=0; j<mConveyors.size(); j++) {
    FfConveyorItem* conv= mConveyors[j];
    conv->SetBeltPos(0);
    conv->mAutoStop=false;
    conv->SetOrigin(j/(1.0) * 200.0*(mLength-1));
    conv->mVelocity = QPointF(0,0);
  }
  // setup block (see also DoAdvanceTime)
  mBlock=QPainterPath();
  mBlock.addRect(-100,-100,100+mConveyors.front()->Origin()-50,200);
  for(int j=0; j<mConveyors.size()-1; j++) {
    qreal a=mConveyors[j]->Origin();
    qreal b=mConveyors[j+1]->Origin();
    mBlock.addRect(a+50,-100,b-a-100,200);
  }
  mBlock.addRect(mConveyors.back()->Origin()+50,-100,mLength*200-100 - mConveyors.back()->Origin()-50,200);
  mBlock=mTransOrient.map(mBlock);
  // fix my sensors
  for(int i=0; i<mLength; i++) {
    mSensors[SNS_SENSORP(i)]->SetValue(false);
  }
  for(int j=0; j<mConveyors.size(); j++) {
    qreal a=mConveyors[j]->Origin();
    int i = a/200.0 + 0.5;
    if(i<0) i=0;
    if(i>=mLength) i=mLength-1;
    qreal b = i*200;
    if(qAbs(a-b)<5) 
      mSensors[SNS_SENSORP(i)]->SetValue(true);
  }
  for(int i=0; i<mLength; i++) {
    mSignals[SIG_SENSORP(i)]->SetInitialValue(mSensors[SNS_SENSORP(i)]->mValue);
    mSignals[SIG_SENSORP(i)]->Reset();
  }

}

// update all 
void FfRailTransport::DoUpdateAll() {
  // delete all conveyors
  while(mConveyors.size()>0) {
    FfConveyorItem* conv= mConveyors.front(); 
    delete conv->mButtonL;
    delete conv->mButtonR;
    delete conv->mButtonU;
    delete conv->mButtonD;
    delete conv->mButtonStop;
    delete conv;
    mConveyors.pop_front();
  }
  // instantiate conveyors
  for(int j=0; j<2; j++) {
    FfConveyorItem* conv= new FfConveyorItem(this);
    conv->mButtonL = new FfButtonItem(mSimControls);
    conv->mButtonR = new FfButtonItem(mSimControls);
    conv->mButtonU = new FfButtonItem(mSimControls);
    conv->mButtonD = new FfButtonItem(mSimControls);
    conv->mButtonStop = new FfButtonItem(mSimControls);
    conv->mButtonL->SetLeftSkip();
    conv->mButtonL->SetText(QString("L%1").arg(j+1));
    conv->mButtonL->setVisible(false);
    conv->mButtonR->SetRightSkip();
    conv->mButtonR->SetText(QString("R%1").arg(j+1));
    conv->mButtonR->setVisible(false);
    conv->mButtonU->SetUp();
    conv->mButtonU->SetText(QString("U%1").arg(j+1));
    conv->mButtonU->setVisible(false);
    conv->mButtonD->SetDown();
    conv->mButtonD->SetText(QString("D%1").arg(j+1));
    conv->mButtonD->setVisible(false);
    conv->mButtonStop->SetStop();
    conv->mButtonStop->SetText(QString("S%1").arg(j+1));
    conv->mButtonStop->setVisible(false);
    conv->SetTransform(mTransOrient);
    conv->SetOrigin(j/(1.0) * 200.0*(mLength-1));
    mConveyors.append(conv);
  }
  // my signals
  FfSignal* sigcfg;
  for(int j=0; j<mConveyors.size(); j++) {
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_CMOTORB(j));
    sigcfg->SetLastName(QString("C%1M+").arg(j+1));
    sigcfg->SetActuator();
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("c%1m+").arg(j+1));
    sigcfg->SetNegativeEdgeEvents(QStringList() << QString("c%1m-").arg(j+1) << QString("c%1off").arg(j+1));
    mSignals.append(sigcfg);
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_CMOTORA(j));
    sigcfg->SetLastName(QString("C%1M-").arg(j+1));
    sigcfg->SetActuator();
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("c%1m-").arg(j+1));
    sigcfg->SetNegativeEdgeEvents(QStringList() << QString("c%1m+").arg(j+1) << QString("c%1off").arg(j+1));
    mSignals.append(sigcfg);
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_BMOTORB(j));
    sigcfg->SetLastName(QString("B%1M+").arg(j+1));
    sigcfg->SetActuator();
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("b%1m+").arg(j+1));
    sigcfg->SetNegativeEdgeEvents(QStringList() << QString("b%1m-").arg(j+1) << QString("b%1off").arg(j+1));
    mSignals.append(sigcfg);
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_BMOTORA(j));
    sigcfg->SetLastName(QString("B%1M-").arg(j+1));
    sigcfg->SetActuator();
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("b%1m-").arg(j+1));
    sigcfg->SetNegativeEdgeEvents(QStringList() << QString("b%1m+").arg(j+1) << QString("b%1off").arg(j+1));
    mSignals.append(sigcfg);
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_SENSORWP(j));
    sigcfg->SetLastName(QString("B%1WP").arg(j+1));
    sigcfg->SetSensor();
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("b%1wpar").arg(j+1));
    sigcfg->SetNegativeEdgeEvents(QStringList() << QString("b%1wplv").arg(j+1));
    mSignals.append(sigcfg);
  }
  for(int i=0; i<mLength; i++) {
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_SENSORP(i));
    sigcfg->SetLastName(QString("P%1").arg(i+1));
    sigcfg->SetSensor();
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("p%1car").arg(i+1));
    sigcfg->SetNegativeEdgeEvents(QStringList() << QString("p%1clv").arg(i+1));
    mSignals.append(sigcfg);
  }
  // position sensors
  for(int i=0; i<mLength; i++) {
    FfSensorItem* sns= new FfSensorItem(this);
    sns->mIndicatorPath.addRect(70+i*200,-90,10,10);
    sns->mWpSensor=false;
    sns->mRelAddress=-1;
    sns->setZValue(-30);
    sns->Update();
    mSensors.append(sns);
  }
  // conveyor sensors 
  for(int j=0; j<mConveyors.size(); j++) {
    FfConveyorItem* conv=mConveyors[j];
    FfSensorItem* sns;
    sns=conv->mSensorWp;
    sns->mRelAddress=SIG_SENSORWP(j);
    mSensors.append(sns);
  }
  // outer shape: pos only
  QRectF brect(-100,-100,200*mLength,200);
  mOuterRect=brect.adjusted(-2,-2,+2,+2);
  mOuterShape=QPainterPath();
  mOuterShape.addRect(mOuterRect);
  mCoreShape=QPainterPath();
  mCoreShape.addRect(brect);
  // my items
  mRails=QPainterPath();
  mRails.addRect(-90,-60,200*mLength-20,20);
  mRails.addRect(-90, 40,200*mLength-20,20);
  mRails=mTransOrient.map(mRails);
  for(int j=0; j<mConveyors.size(); j++) {
    mConveyors[j]->mRail = mTransOrient.map(mConveyors[j]->mRail); 
    mConveyors[j]->mNormRail = mTransOrient.map(mConveyors[j]->mNormRail); 
    mConveyors[j]->mOrthRail = mTransOrient.map(mConveyors[j]->mOrthRail); 
  }
  // my controls
  mSimControls->SetVisible(false);
  // fix orientation: base
  FfComponent::DoUpdateAll();
}

// blocks occupied: integer shape shape
QList<QPoint> FfRailTransport::TestArea(const QPoint& pos,Orientation o,int len) {
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
QList<QPoint> FfRailTransport::TestArea(const QPoint& pos,Orientation o) {
  return TestArea(pos,o,mLength);
}

// have a new editr
QWidget* FfRailTransport::NewEditor(void) {
  return new FfRailTransportEditor(this);  
}

// paint myself
void FfRailTransport::paint(
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
  painter->setBrush(sc->EffectColor(VioRed));
  painter->setPen(sc->EffectColor(VioDarkRed));
  painter->drawPath(mRails);
  // debug: call base
  FfComponent::paint(painter,option,widget);
}

// update dynamics
void FfRailTransport::DoAdvanceTime(qreal dt) {
  // belts
  for(int j=0; j<mConveyors.size(); j++) {
    FfConveyorItem* conv=mConveyors[j];
    bool bma=Signal(SIG_BMOTORA(j));
    bool bmb=Signal(SIG_BMOTORB(j));
    // cont dynamics
    conv->mVelocity=QPointF(0,0);
    if(bma) {
      qreal bpos = conv->BeltPos() - dt * mBeltVelocity;
      while(bpos<0) bpos+=20;
      conv->SetBeltPos(bpos);
      conv->mVelocity=  conv->mNormRail * ((-1) * dt * mBeltVelocity);
    }
    if(bmb) {
      qreal bpos = conv->BeltPos() + dt * mBeltVelocity;
      while(bpos>20) bpos-=20;
      conv->SetBeltPos(bpos);
      conv->mVelocity=conv->mNormRail * (dt* mBeltVelocity);
    }
    if(!bma) 
    if(!bmb) {
    }
    if(bma) 
    if(bmb) {
    }
  }
  // carts
  bool move=false;
  if(!mRefuseAdvance) {
    for(int j=0; j<mConveyors.size(); j++) {
      FfConveyorItem* conv=mConveyors[j];
      bool cma=Signal(SIG_CMOTORA(j));
      bool cmb=Signal(SIG_CMOTORB(j));
      // cont dynamics
      if(cma) {
        qreal cpos = conv->Origin() - dt * mCartVelocity;
        qreal cmin = 0;
        if(j-1>=0) 
          cmin=mConveyors[j-1]->Origin()+161;
        if(cpos<cmin) cpos=cmin;
        conv->SetOrigin(cpos);
        conv->mVelocity+=  conv->mOrthRail * ((-1) * dt * mCartVelocity);
        move=true;
      }
      if(cmb) {
        qreal cpos = conv->Origin() + dt * mCartVelocity;
        qreal cmax = 200*(mLength-1);
        if(j+1<mConveyors.size())
          cmax=mConveyors[j+1]->Origin()-161;
        if(cpos>cmax) cpos=cmax;
        conv->SetOrigin(cpos);
        conv->mVelocity+=  conv->mOrthRail * (dt * mCartVelocity);
        move=true;
      }
      if(!cma) 
      if(!cmb) {
      }
      if(cma) 
      if(cmb) {
      // error
      }
      // track sensor
      if(cma || cmb) {
        qreal cpos = conv->Origin();
        int ci = cpos/200.0 + 0.5;
        if(ci<0) ci=0;
        if(ci>=mLength) ci=mLength-1;
        qreal spos = ci*200;
        if(cma) spos-=5;
        if(cmb) spos+=5; 
        bool oval=mSensors[SNS_SENSORP(ci)]->mValue;
        bool nval=qAbs(spos-cpos)<=5;
        mSensors[SNS_SENSORP(ci)]->SetValue(nval);
        SetSignal(SIG_SENSORP(ci),nval);
        if(nval && !oval && conv->mAutoStop){
          SetSignal(SIG_CMOTORA(j),false);
          SetSignal(SIG_CMOTORB(j),false);
          conv->mAutoStop=false;
        }            
      }
    }
  }
  // fix block
  if(move) {
    mBlock=QPainterPath();
    mBlock.addRect(-100,-100,100+mConveyors.front()->Origin()-50,200);
    for(int j=0; j<mConveyors.size()-1; j++) {
      qreal a=mConveyors[j]->Origin();
      qreal b=mConveyors[j+1]->Origin();
      mBlock.addRect(a+50,-100,b-a-100,200);
    }
    mBlock.addRect(mConveyors.back()->Origin()+50,-100,mLength*200-100 - mConveyors.back()->Origin()-50,200);
    mBlock=mTransOrient.map(mBlock);
  }

}

// report workpiece
void FfRailTransport::DoWorkpiece(FfWorkpiece* wp) {
  (void) wp;
}

// propagate workpiece
void FfRailTransport::AdvanceWorkpiece(
    const qreal& dt,
    const QPointF& pos, const qreal& orient, 
    QPointF& npos, qreal& norient, bool& strict, bool& err)
{
  (void) dt;
  // default result: stuck
  npos = pos; 
  norient=orient;
  strict=true;
  err=false;
  // are we with a belt?
  int atj=-1;
  qreal xpos=mSimControls->mapFromScene(pos).x();
  qreal ypos=mSimControls->mapFromScene(pos).y();
  for(int j=0; j<mConveyors.size(); j++) {
    FfConveyorItem* conv=mConveyors[j];
    if(xpos-45 < conv->Origin()-50) continue;
    if(xpos+45 > conv->Origin()+50) continue;
    atj=j; 
    break;
  }
  // belt controls wp
  if(atj>=0) {
    FfConveyorItem* conv=mConveyors[atj];
    //FF_DQM("Distributor::Railtransport(): at conveyor " << atj);
    if( (ypos-45 < -100) || (ypos+45 >100)) { 
      //if(mSignalConfiguration[SIG_CMOTORA(atj)].mValue) err=true;
      //if(mSignalConfiguration[SIG_CMOTORB(atj)].mValue) err=true;
    }
    if(err) return;       
    // fix on rail
    strict=false;
    if(Signal(SIG_CMOTORA(atj))) {strict=true;   xpos=conv->Origin();}
    if(Signal(SIG_CMOTORB(atj))) {strict=true;   xpos=conv->Origin();}
    npos= mSimControls->mapToScene(QPointF(xpos,ypos)) + conv->mVelocity;
    err=false;
    return;
  }
  // no belt responsible
  err=true;
}


// execute user command
void FfRailTransport::DoUserCommand(QString cmd) {
 FF_DQM("FfRailTransport::DoUserCommand()" << cmd);
 QString c1=cmd.left(1);
 int j=cmd.mid(1).toShort()-1;
 if(j<0 || j>= mConveyors.size()) return;
 // stop
 if(c1=="S") {
   SetSignal(SIG_BMOTORA(j),false);
   SetSignal(SIG_BMOTORB(j),false);
   SetSignal(SIG_CMOTORA(j),false);
   SetSignal(SIG_CMOTORB(j),false);
   return;
 }
 // belt
 if(c1=="U") {
   SetSignal(SIG_BMOTORA(j),true);
   SetSignal(SIG_BMOTORB(j),false);
   return;
 }  
 if(c1=="D") {
   SetSignal(SIG_BMOTORB(j),true);
   SetSignal(SIG_BMOTORA(j),false);
   return;
 }  
 // cart
 if(c1=="L") {
   SetSignal(SIG_CMOTORA(j),true);
   SetSignal(SIG_CMOTORB(j),false);
   SetSignal(SIG_BMOTORA(j),false);
   SetSignal(SIG_BMOTORB(j),false);
   mConveyors[j]->mAutoStop=true;
   return;
 }  
 if(c1=="R") {
   SetSignal(SIG_CMOTORB(j),true);
   SetSignal(SIG_CMOTORA(j),false);
   SetSignal(SIG_BMOTORA(j),false);
   SetSignal(SIG_BMOTORB(j),false);
   mConveyors[j]->mAutoStop=true;
   return;
 }  
}




/*
*********************************************************
*********************************************************
*********************************************************

FfRailTransportEditor

*********************************************************
*********************************************************
*********************************************************
*/


// constructor
FfRailTransportEditor::FfRailTransportEditor(FfComponent* comp) :
  FfComponentEditor(comp)
{
  // my parameters: length
  QLabel* nlabel = new QLabel("Length");
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
void FfRailTransportEditor::DoUpdateFromComponent(FfComponent* comp) {
  // run my cast
  FfRailTransport* conv = dynamic_cast<FfRailTransport*>(comp);
  if(!conv) return;
  // set my editor
  mLengthEdit->setText(QString("%1").arg(conv->Length()));
}

// updater
void FfRailTransportEditor::DoUpdateFromEditor(FfComponent* comp) {
  // run my cast
  FfRailTransport* conv = dynamic_cast<FfRailTransport*>(comp);
  if(!conv) return;
  FF_DQ("FfRailTransportEditor::DoUpdateFromEditor()");
  // set my parameter
  int nlen= mLengthEdit->text().toShort();
  if(nlen>4 && nlen <15) 
  if(nlen!=conv->Length()) {
    QList<QPoint> narea=conv->TestArea(conv->Position(),conv->Orient(),nlen);
    if(mScene->ReconfigureComponent(comp,narea)) {
    conv->SetLength(nlen);
    conv->Update();
    mScene->AdjustScene();
    }
  }
}
