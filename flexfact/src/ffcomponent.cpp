/* component.cpp  - base calss for productionline components */

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

#include "ffcomponent.h"
#include "ffscene.h"
#include "ffworkpiece.h"

#include "ffstackfeeder.h"
#include "ffconveyor.h"
#include "ffsink.h"
#include "ffrotconv.h"
#include "ffdistributor.h"
#include "ffrailtrans.h"
#include "ffprocess.h"
#include "ffmultimach.h"

/*
************************************************
************************************************
  
Implementation: FfComponent

************************************************
************************************************
*/


// default constructor
FfComponent::FfComponent(QObject* parent) : QObject(parent), QGraphicsItem(0) {
  FF_DQ("FfComponent::FfComponent()");
  setZValue(-5);
  mName="CMP";
  mStrictAdvance=false;
  //setAcceptHoverEvents(true);
  mSimControls = new FfSimControlsItem();
  mSimOverlay=0;
  mSelected = new FfSelectedItem(this);
  // start in non running mode
  mRunning=false;
  mSimControls->setVisible(false);  
  mSelected->setVisible(true);  
  setFlag(ItemIsMovable,true);
  setFlag(ItemIsSelectable,true);
  // dummy pos
  mPos=QPoint(0,0);
  mOrient=Ostd;
  FF_DQ("FfComponent::FfComponent(): done");
}

//destructor
FfComponent::~FfComponent(void) {
  FF_DQ("FfComponent::~FfComponent()");
  if(mSimOverlay) {
    FF_DQ("FfComponent::~FfComponent(): delete oberlay with scene  " << mSimOverlay->scene());  
    if(mSimOverlay->scene()) mSimOverlay->scene()->removeItem(mSimOverlay);
    delete mSimOverlay;
    mSimOverlay=0;
  }
  if(mSimControls) {
    FF_DQ("FfComponent::~FfComponent(): remove controls from scene  " << mSimControls->scene());  
    if(mSimControls->scene()) mSimControls->scene()->removeItem(mSimControls);
    FF_DQ("FfComponent::~FfComponent(): delete controls  with scene  " << mSimControls->scene());  
    delete mSimControls;
    mSimControls=0;
  }
}

// primitive rtti/factory mechanism
QString FfComponent::Type(const FfComponent* comp) {
  QString stype="";
  if(!comp) return stype;
  if(comp->type()==FfStackfeeder::StackFeeder) stype="StackFeeder";
  if(comp->type()==FfConveyor::ConveyorBelt) stype="ConveyorBelt";
  if(comp->type()==FfSink::ExitSlide) stype="ExitSlide";
  if(comp->type()==FfRotaryTable::RotaryTable) stype="RotaryTable";
  if(comp->type()==FfDistributor::Distributor) stype="Distributor";
  if(comp->type()==FfRailTransport::RailTransport) stype="RailTransport";
  if(comp->type()==FfProcess::Machine) stype="Machine";
  if(comp->type()==FfMultimach::Multimach) stype="UniversalMachine";
  return stype;
}

// primitive rtti/factory mechanism
QStringList FfComponent::AvailableTypes(void) {
  return QStringList() 
    << "StackFeeder" << "ConveyorBelt" 
    << "ExitSlide" << "RotaryTable" << "Distributor" << "RailTransport" << "Machine" << "UniversalMachine";
}

// primitive rtti/factory mechanism
FfComponent* FfComponent::New(const QString& type) {
  if(type=="StackFeeder") return new FfStackfeeder();
  if(type=="ConveyorBelt") return new FfConveyor();
  if(type=="ExitSlide") return new FfSink();
  if(type=="RotaryTable") return new FfRotaryTable();
  if(type=="Distributor") return new FfDistributor();
  if(type=="RailTransport") return new FfRailTransport();
  if(type=="Machine") return new FfProcess();
  if(type=="UniversalMachine") return new FfMultimach();
  return 0;
}


// figure scene
FfScene* FfComponent::Scene(void) {
  return qobject_cast<FfScene*>(scene());
}

// configure
void FfComponent::SetOrient(Orientation o) {
  mOrient=o;
}

// configure
void FfComponent::SetPosition(const QPoint& pos) {
  mPos=pos;
}

// blocks occupied: pos only
QList<QPoint> FfComponent::TestArea(const QPoint& pos,Orientation o) {
  (void) o;
  QList<QPoint> res;
  res.append(pos);
  return res;
}

// good position (refering to pos as opposed to mPos, eg during drag)
bool FfComponent::GoodPosition(void) {
  // comp is not good when out of grid
  QPointF fpos=scenePos();
  QPointF dpos=fpos-FfStyle::Grid200(fpos);
  if(FfStyle::NormF(dpos)>66) return false;
  // comp is not good when on busy field
  if(FfScene* sc=Scene()) { 
    QPoint  dipos=FfStyle::Grid200I(fpos) - mPos;
    for(int i=0; i< mArea.size(); i++) {
      QPoint oipos=mArea.at(i)+dipos;
      FfComponent* other = sc->Component(oipos);
      if(!other) continue;
      if(other->isSelected()) continue; // drag: it's me, or my mates moving
      return false; 
    }
  }
  // ok then 
  return true;
}

// do configure
void FfComponent::Update(void) {
  // geometry we know
  setPos(mPos*200);
  mSimControls->setPos(mPos*200);
  if(mSimOverlay) mSimOverlay->setPos(mPos*200);
  mTransOrient.reset();
  switch(mOrient) {
  case(Ostd): mTransOrient.reset(); break;
  case(O90) : mTransOrient.rotate(90); break;
  case(O180): mTransOrient.rotate(180); break;
  case(O270): mTransOrient.rotate(270); break;
  } 
  mArea = TestArea(mPos,mOrient);
  // clear all signals
  foreach(FfSignal* sig, mSignals) 
    delete sig;
  mSignals.clear();
  // clear all sensors
  foreach(FfSensorItem* sns, mSensors) 
    delete sns;
  mSensors.clear();
  // derived classes take care
  DoUpdateAll();          // derived class update (does optionally call base)
  // set up my error signals
  int idx=0;
  FfFault* faultcfg;
  while((faultcfg=FaultCfg(idx++))!=0) { 
    if(!faultcfg->Active()) continue;
    FfSignal* failsig=faultcfg->NewFailureSignal();
    mSignals.append(failsig);
    FfSignal* maintsig=faultcfg->NewMaintenanceSignal();
    mSignals.append(maintsig);
  }
  // fix signal and fault full names
  foreach(FfSignal* signalcfg, mSignals)
    signalcfg->SetFirstName(mName);
  foreach(FfFault* faultcfg, mFaults)
    faultcfg->SetFirstName(mName);
  // dynamic reset
  FfComponent::DoReset(); // my reset (signal initial values)
  DoReset();              // derived class reset (does not call base)
  // report to editors and scene
  emit NotifyParameterChanged();
}

// recognize scene change
QVariant FfComponent::itemChange(GraphicsItemChange change, const QVariant &value) {
  // ignore others
  if(change!=QGraphicsItem::ItemSceneHasChanged) return value;
  FF_DQ("FfComponent::itemChange(): new scene is " << scene());
  // place companion items in same scene
  if(mSimControls && scene()) 
  if(mSimControls->scene() != scene())
    scene()->addItem(mSimControls);
  if(mSimOverlay && scene()) 
  if(mSimOverlay->scene() != scene())
    scene()->addItem(mSimOverlay);
  // done, dont interfer
  return value;
}


// get config
const QList<QPoint>& FfComponent::Area(void) { 
  return mArea;
}

// set Name
void FfComponent::SetName(const QString& name) {
  // bail out on trivial
  if(mName==name) return;
  // set and report
  mName=name;
  // fix signals and faults
  foreach(FfSignal* signalcfg,mSignals)
    signalcfg->SetFirstName(mName);
  foreach(FfFault* faultcfg, mFaults)
    faultcfg->SetFirstName(mName);
  // report
  emit NotifyParameterChanged();
}

// get Name
const QString& FfComponent::Name(void) {
  return mName;
}



// file io
QDomElement FfComponent::ToElement(QDomDocument& doc) {
  // actual elements (return void on components that do not participate
  // in our rtti . eg, operator)
  QDomElement cele = doc.createElement(Type(this));
  cele.setAttribute("id",mName);
  QDomElement pele = doc.createElement("Position");
  pele.setAttribute("x",mPos.x());
  pele.setAttribute("y",mPos.y());
  switch(mOrient) {
  case(Ostd):   pele.setAttribute("phi","std");  break;
  case(O90):    pele.setAttribute("phi","+90");  break;
  case(O180):   pele.setAttribute("phi","+180"); break;
  case(O270):   pele.setAttribute("phi","+270"); break;
  } 
  cele.appendChild(pele);
  // iterate faults
  int idx=0;
  FfFault* faultcfg;
  while((faultcfg=FaultCfg(idx++))!=0) 
    faultcfg->AppendToElement(doc, cele);
  // allow derived class to append their conf data
  AppendToElement(doc, cele);
  // donw
  return cele;
}

// file io
FfComponent* FfComponent::FromElement(QDomElement& elem) {
  // instantiate component
  QString stype=elem.tagName();
  FfComponent* comp=New(stype);
  if(!comp) {
    FF_DQM("FileIO: cannot instantiate component type " << stype); 
    return 0;
  }
  // figure id
  QString name=elem.attribute("id");
  if(name!="") comp->SetName(name);
  // figure position/orientation
  QDomElement pele=elem.firstChildElement("Position");
  QString xstr = pele.attribute("x");
  QString ystr = pele.attribute("y");
  bool okx;
  bool oky;
  int x= xstr.toInt(&okx); 
  int y= ystr.toInt(&oky); 
  if((!okx) || (!oky)) {
    FF_DQM("FileIO: could not figure position of component " << name);
    delete comp;
    return 0;
  }
  QString phistr = pele.attribute("phi");
  Orientation o=Ostd;
  if(phistr=="+90") o=O90;
  if(phistr=="+180") o=O180;
  if(phistr=="+270") o=O270;
  // iterate faults
  int idx=0;
  FfFault* faultcfg;
  while((faultcfg=comp->FaultCfg(idx++))!=0) 
    faultcfg->ExtractFromElement(elem);
  // call derived class here 
  comp->ExtractFromElement(elem);
  // configure component
  comp->SetOrient(o);
  comp->SetPosition(QPoint(x,y));
  comp->Update();
  // ok
  return comp;
}



// reset to initial state
void FfComponent::Reset(void) {
  FfComponent::DoReset(); // my reset (signal initial values)
  DoReset();              // derived class reset (does not call base)
}

// enable simulation
void FfComponent::Run(bool run) {
  if(mRunning==run) return;
  mRunning=run;
  if(!mRunning){
    mSimControls->setVisible(false);  
    mSelected->setVisible(true);  
    if(mSimOverlay) mSimOverlay->setVisible(false);
    setFlag(ItemIsMovable,true);
    setFlag(ItemIsSelectable,true);
  } else {
    mSimControls->setVisible(true);  
    mSelected->setVisible(false);  
    if(mSimOverlay) mSimOverlay->setVisible(true);
    setFlag(ItemIsMovable,false);
    setFlag(ItemIsSelectable,false);
  }
  update();
};


// transport geometry
const QPointF& FfComponent::Velocity(void) {
  return mVelocity;
}
const QLineF& FfComponent::Rail(void) {
  return mRail;
}
const QPainterPath&  FfComponent::Block(void) {
  return mBlock;
}
void FfComponent::Workpiece(FfWorkpiece* wp) {
  // deal with sensors (sense new hit)
  for(int i=0; i<mSensors.size(); i++) {
    FfSensorItem* sns=mSensors[i];
    if(!sns->mWpSensor) continue;
    if(sns->mValue) continue;
    if(sns->mMute) continue;
    QPointF wpos= sns->mapFromScene(wp->pos());
    if(sns->mOnPath.contains(wpos)) {
      sns->SetValue(true);
      SetSignal(sns->mRelAddress,true);
      sns->mWpReference=wp;
    }
  }
  // call virtual
  DoWorkpiece(wp);
};


// signal interface: size
int FfComponent::ImageSize(void) {
  return mSignals.size();
}

// signal interface: set
void FfComponent::SetSignal(int reladdress, bool val) {
  if(reladdress<0) return;
  if(reladdress>=mSignals.size()) return;
  FfSignal* sigcfg=mSignals[reladdress];
  sigcfg->SetValue(val);
}

// signal interface: get
bool FfComponent::Signal(int reladdress) {
  if(reladdress<0) return false;
  if(reladdress>=mSignals.size()) return false;
  return mSignals[reladdress]->Value();
}


// signal interface: get symbolic name
QString FfComponent::SignalName(int reladdress) {
  if(reladdress<0) return "";
  if(reladdress>=mSignals.size()) return "";
  return mSignals[reladdress]->Name();
}

// signal interface: get symbolic name
QString FfComponent::SignalShortName(int reladdress) {
  if(reladdress<0) return "";
  if(reladdress>=mSignals.size()) return "";
  return mSignals[reladdress]->ShortName();
}


// signal interface: type
bool FfComponent::IsActuator(int reladdress) {
  if(reladdress<0) return false;
  if(reladdress>=mSignals.size()) return false;
  return mSignals[reladdress]->IsActuator();
}

// signal interface: type
bool FfComponent::IsSensor(int reladdress) {
  if(reladdress<0) return false;
  if(reladdress>=mSignals.size()) return false;
  return mSignals[reladdress]->IsSensor();
}

// signal interface: get config
FfSignal* FfComponent::SignalCfg(int reladdr) {
  return mSignals.at(reladdr);
}

// fault interface: get config (safe)
FfFault* FfComponent::FaultCfg(int idx) {
  if((idx <0) || (idx >= mFaults.size())) return 0;
  return mFaults.at(idx);
}



// add workpiece to scene
void FfComponent::AddWorkpiece(const QPointF& pos, Orientation o) {
  FF_DQM("FfComponent::AddWorkPiece(): at (" << 
   mPos.x()+pos.x()/200 << ","<< mPos.y() + +pos.y()/200 << ")");
  FfScene* sc=Scene();
  if(!sc) return;
  sc->AddWorkpiece(mPos* 200 + pos,o);
}

// paint myself
void FfComponent::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // debug: shape
  /*
  painter->setPen(FfStyle::Color(VioBlack));
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(mOuterRect);
  */
  // debug: shape
  /*
  painter->setPen(FfStyle::Color(VioBlack));
  painter->setBrush(Qt::NoBrush);
  painter->drawPath(mOuterShape);
  */
  // debug: rail
  /*
  painter->setPen(FfStyle::Color(VioDarkGreen));
  painter->setBrush(Qt::NoBrush);
  painter->drawLine(mRail);
  */
  // debug: block
  /*
  painter->setPen(FfStyle::Color(VioDarkRed));
  painter->setBrush(Qt::NoBrush);
  painter->drawPath(mBlock);
  */
}


// advance time
void FfComponent::AdvanceTime(qreal dt) {
  // call devired
  DoAdvanceTime(dt);
  // test for sensor release
  foreach(FfSensorItem* sns, mSensors) {
    if(!sns->mWpSensor) continue;
    if(!sns->mValue)    continue;
    if(sns->mMute) {
      sns->SetValue(false);
      SetSignal(sns->mRelAddress,false);
    }
    FfWorkpiece* wp=sns->mWpReference;
    if(wp) {
      QPointF wpos= sns->mapFromScene(wp->pos());
      if(!sns->mOnPath.contains(wpos)) wp=0;
    }
    if(!wp) {
      sns->SetValue(false);
      SetSignal(sns->mRelAddress,false);
      sns->mWpReference=0;
    }
  }
  // fault dynamics
  foreach(FfFault* faultcfg,mFaults) 
    if(faultcfg->Active())
    faultcfg->AdvanceTime(dt);
  // unlock
  mRefuseAdvance=false;
}

// update position config 
void FfComponent::DoUpdateAll() {
  // have a copy of the nominal geometry (set up by derived class)
  mNominalOuterRect=mOuterRect;
  mNominalOuterShape=mOuterShape;
  mNominalCoreShape=mCoreShape;
  mNominalRail=mRail;
  mNominalNormRail=mNormRail;
  mNominalBlock=mBlock;
  // fix controls from CoreShape
  mSimControls->mCoreShape = mCoreShape;
  mSimControls->resetTransform();
  mSimControls->setTransform(mTransOrient);
  mSimControls->Update();
  // fix selected from CoreShape
  mSelected->mCoreShape = mCoreShape;
  mSelected->resetTransform();
  mSelected->setTransform(mTransOrient);
  mSelected->Update();
  // fix orientation
  mOuterRect=mTransOrient.mapRect(mOuterRect);
  mOuterShape=mTransOrient.map(mOuterShape);
  mCoreShape=mTransOrient.map(mCoreShape);
  mRail=mTransOrient.map(mRail);
  mNormRail=mTransOrient.map(mNormRail);
  mBlock=mTransOrient.map(mBlock);
  for(int i=0; i<mSensors.size(); i++) {
    FfSensorItem* sns=mSensors[i];
    if(sns->parentItem()==this) 
      sns->setTransform(mTransOrient);
  } 

}


// update dynamics
void FfComponent::DoAdvanceTime(qreal dt) {
  (void) dt;
}

// report workpiece
void FfComponent::DoWorkpiece(FfWorkpiece* wp) {
  (void) wp;
}

// reset state
void FfComponent::DoReset(void) {
  // my state
  mRefuseAdvance=false;
  // quietly reset sig values
  for(int i=0; i<mSignals.size(); i++)
    mSignals[i]->Reset();
  // initialize my parameters
  mSnapRate = 1.0; // tau = 1/mSnapRate
  // initialize fault dynamics
  foreach(FfFault* faultcfg,mFaults) 
   faultcfg->Reset();
}


// get new context menu
QMenu* FfComponent::NewSimulationContextMenu(void) {
  // build signal value widget
  QWidget* wsig= new QWidget();
  QVBoxLayout* vbox = new QVBoxLayout(wsig);
  vbox->setMargin(2);
  vbox->setSpacing(0);
  vbox->addWidget(new QLabel(QString("Signals") /*+Name()*/));
  for(int i=0; i<ImageSize(); i++) {
    QCheckBox* cbox = SignalCfg(i)->NewCheckBox();
    vbox->addWidget(cbox);
  }
  // build fault control widget
  QWidget* fsig= new QWidget();
  QVBoxLayout* vfbox = new QVBoxLayout(fsig);
  vfbox->setMargin(2);
  vfbox->setSpacing(0);
  vfbox->addWidget(new QLabel(QString("Fault Ctrl") /*+Name()*/));
  bool fempty=true;
  int idx=0;
  FfFault* faultcfg=0;
  while((faultcfg=FaultCfg(idx++))!=0) {
    if(!faultcfg->Active()) continue;
    QCheckBox* cbox = faultcfg->NewCheckBox();
    vfbox->addWidget(cbox);
    fempty=false;
  }
  if(fempty) {delete fsig; fsig=0;}
  // setup menu
  QMenu* menu= new QMenu(mName);
  QWidgetAction* wact = new QWidgetAction(0);
  wact->setDefaultWidget(wsig); 
  menu->addAction(wact);
  if(fsig) {
    QWidgetAction* fact = new QWidgetAction(0);
    fact->setDefaultWidget(fsig); 
    menu->addAction(fact);
  }
  return menu;
}


// have anew editr
QWidget* FfComponent::NewEditor(void) {
  return new FfComponentEditor(this);  
}

 
// process context menu events
void FfComponent::UpdateSignal(void) {
  FF_DQ("FfComponent::UpdateSignal()");
  // bail out on wrong sender
  QCheckBox* cbox = qobject_cast<QCheckBox*>(sender());
  if(!cbox) return;
  // figure signal
  QVariant val = cbox->property("RelAddress");
  if(!val.isValid()) return;
  int reladdress = val.toInt();
  // test for valid address
  if(reladdress<0) return;
  if(reladdress>=mSignals.size()) return;
  if(!mSignals[reladdress]->IsActuator()) return; 
  // set the value
  SetSignal(reladdress,cbox->isChecked());
}


// catch contextmenu
void FfComponent::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
  // only while simulating (editing context is done by the scene)
  if(!mRunning) return;
  // have a menu (trust qt to delete it later)
  QMenu* menu = NewSimulationContextMenu();
  // run it
  menu->popup(event->screenPos());
  // accept the event
  event->accept();
}

// catch mouse: figure user command
void FfComponent::mousePressEvent(QGraphicsSceneMouseEvent * ev) {
  if(!mRunning) return;
  // pass on any modified clicks
  if(ev->modifiers()!=Qt::NoModifier) {
    QGraphicsItem::mousePressEvent(ev);
    return;
  }
  // pass on any other then the left button
  if(ev->buttons()!=Qt::LeftButton) {
    QGraphicsItem::mousePressEvent(ev);
    return;
  }
  // figure my simcontrol button
  FF_DQ("FfComponent::mousePressEvent()");
  QList<QGraphicsItem *> items = mSimControls->childItems();
  QString text="NoButton";
  foreach(QGraphicsItem* item, items) {
    FfButtonItem* button=qgraphicsitem_cast<FfButtonItem*>(item);
    if(!button) continue; 
    if(!button->UnderMouse()) continue; 
    text = button->Text();
    break;
  }
  if(text!="NoButton") {
    FF_DQ("FfComponent::mousePressEvent(): " << text);
    DoUserCommand(text);
    ev->accept();
  }
  // pass on
  QGraphicsItem::mousePressEvent(ev);
}

// catch mouse
/*
void FfComponent::hoverEnterEvent(QGraphicsSceneHoverEvent * ev) {
  FF_DQ("FfComponent::HoverEnter()");
  QGraphicsItem::hoverEnterEvent(ev);
}
void FfComponent::hoverLeaveEvent(QGraphicsSceneHoverEvent * ev) {
  FF_DQ("FfComponent::HoverLeave()");
  QGraphicsItem::hoverLeaveEvent(ev);
}
void FfComponent::hoverMoveEvent(QGraphicsSceneHoverEvent * ev) {
  QGraphicsItem::hoverMoveEvent(ev);
}
*/

// execute user command
void FfComponent::DoUserCommand(QString cmd) {
 (void) cmd;
 FF_DQM("FfComponent::DoUserCommand()" << cmd);
}

// snap on rail transport helper
void FfComponent::SnapOnRail(qreal dt, QPointF& pos, qreal& orient) {
  QPointF rpos=mapFromScene(pos)-mRail.p1();
  QPointF orthrail = FfStyle::NormalF(mNormRail);
  qreal nrpos= FfStyle::ScalarF(rpos,mNormRail);
  qreal orpos=FfStyle::ScalarF(rpos,orthrail);
  orpos*=(1-dt*mSnapRate);
  pos= mapToScene(nrpos*mNormRail+orpos*orthrail + mRail.p1()); 
  orient=0;
}

// snap on point position helper (tens)
void FfComponent::SnapOnPosition(qreal dt, qreal& pos) {
  qreal target=pos;
  target = 10* qRound(target/10.0);
  qreal npos = pos + dt*mSnapRate  * (target -pos);
  npos = qRound(npos*100) / 100.00;
  if((npos == pos) && (npos != target)) npos=target; 
  pos=npos;
}

// snap on orientation helper (15ns)
void FfComponent::SnapOnOrientation(qreal dt, qreal& orient) {
  qreal target=orient;
  target = 15* qRound(target/15.0);
  qreal norient = orient + dt*mSnapRate  * (target -orient);
  norient = qRound(norient*90) / 90.00;
  if((norient == orient) && (norient != target)) norient=target; 
  orient=norient;
}

// propagate workpiece
void FfComponent::AdvanceWorkpiece(
    const qreal& dt,
    const QPointF& pos, const qreal& orient, 
    QPointF& npos, qreal& norient, bool& strict, bool& err)
{
  err=false;
  npos=pos;
  norient=orient;
  SnapOnRail(dt,npos,norient);
  npos = npos + Velocity() * dt;
  strict=mStrictAdvance;
}



/*
*********************************************************
*********************************************************
*********************************************************

FfComponentEditor

*********************************************************
*********************************************************
*********************************************************
*/


// constructor
FfComponentEditor::FfComponentEditor(FfComponent* comp, bool insfault) :
  QWidget(0)
{
  // connect with component
  connect(comp,SIGNAL(NotifyParameterChanged(void)),this,SLOT(UpdateFromComponent(void)));
  mName = comp->Name();
  mScene = comp->Scene();
  // top level layout
  this->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed));
  mGbox = new QGridLayout(this);  
  mGbox->setSpacing(2);
  mGbox->setMargin(2);
  // edit name
  QLabel* nlabel = new QLabel("Name");
  nlabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed));
  mNameEdit = new QLineEdit(comp->Name());
  connect(mNameEdit,SIGNAL(editingFinished(void)),this,SLOT(SetName(void)));
  QRegExpValidator* nvalid = new QRegExpValidator(
    QRegExp("[A-Za-z][A-Za-z0-9_\\^\\(\\)\\[\\]{}+-*]+"),0);
  mNameEdit->setValidator(nvalid);
  mNameEdit->setMinimumWidth(12*10);
  mNameEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
  // add to layout
  mGbox->addWidget(nlabel,0,0);
  mGbox->addWidget(mNameEdit,0,1);
  // optionally have fault editor (constructor flag)
  if(insfault) InsertFaultEditor(comp);
}


// insert editors for faults (part of constructor, typically invoked from derived class)
void FfComponentEditor::InsertFaultEditor(FfComponent* comp) {
  int rows=mGbox->rowCount();
  int idx=0;
  FfFault* faultcfg;
  while((faultcfg=comp->FaultCfg(idx))!=0) {
    // some extra space
    if(idx==0) mGbox->addItem(new QSpacerItem(10,5),rows++,0,1,2);
    // headline 
    QLabel* nlabel = new QLabel(
       QString("%1 [%2]").arg(faultcfg->EditorName()).arg(faultcfg->ShortName()));
    mGbox->addWidget(nlabel,rows,0,1,2);
    rows++;
    // activate check
    QCheckBox* fcheck = new QCheckBox("enabled",this);
    fcheck->setChecked(faultcfg->Active());
    fcheck->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    connect(fcheck,SIGNAL(clicked(bool)),faultcfg,SLOT(SetActive(bool))); // incl. Update/Signal 
    // tau fail
    QLineEdit* etauf = new QLineEdit();
    connect(etauf,SIGNAL(editingFinished(void)),this,SLOT(UpdateFromEditor(void)));
    QRegExpValidator* lfvalid = new QRegExpValidator(QRegExp("[0-9]*"),0);
    etauf->setValidator(lfvalid);
    etauf->setText(QString("%1").arg(faultcfg->TauFail()));
    etauf->setProperty("fpar","tauf");
    etauf->setProperty("fidx",idx);
    etauf->setFixedWidth(12*5);
    // tau repair
    QLineEdit* etaur = new QLineEdit();
    connect(etaur,SIGNAL(editingFinished(void)),this,SLOT(UpdateFromEditor(void)));
    QRegExpValidator* lrvalid = new QRegExpValidator(QRegExp("[0-9]*"),0);
    etaur->setValidator(lrvalid);
    etaur->setText(QString("%1").arg(faultcfg->TauRepair()));
    etaur->setProperty("fpar","taur");
    etaur->setProperty("fidx",idx);
    etaur->setFixedWidth(12*5);
    // activate line
    QHBoxLayout* aline = new QHBoxLayout();
    aline->setSpacing(2);
    aline->setMargin(0);
    aline->addWidget(fcheck,Qt::AlignLeft);
    aline->addWidget(new QLabel(" [t_f/t_r] = ["),0);
    aline->addWidget(etauf,0);
    aline->addWidget(new QLabel("/"),0);
    aline->addWidget(etaur,0);
    aline->addWidget(new QLabel("]"),0);
    //sline->addStretch(2);
    aline->addWidget(new QLabel(" "),1,Qt::AlignRight);
    mGbox->addLayout(aline,rows,1,1,Qt::AlignLeft);
    rows++;
    // stochastic check
    /*
    QCheckBox* scheck = new QCheckBox("stochasic",this);
    scheck->setChecked(faultcfg->Stochastic());
    scheck->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    connect(scheck,SIGNAL(clicked(bool)),faultcfg,SLOT(SetStochastic(bool))); // incl. Update/Signal 
    mGbox->addWidget(scheck,rows,1);
    rows++;
    */
    // loop all faults
    idx++;
  }
}


// figure my component
FfComponent* FfComponentEditor::Component(void) {
  if(!mScene) return 0;
  return mScene->Component(mName);
}


// updater
void FfComponentEditor::SetName(QString newname)  {
  if(newname=="") {
    newname=mNameEdit->text();
  }
  FF_DQ("FfComponentEditor::SetName(): for " << mName << " in " << mScene << " to " << newname);
  FfComponent* comp=Component();
  if(!comp) return;
  mScene->RenameComponent(comp,newname);
  UpdateFromComponent(comp);
}


// updater
void FfComponentEditor::UpdateFromComponent(FfComponent* comp) {
  // figure component
  if(!comp) comp=Component();
  if(!comp) return;
  // do the update
  mName=comp->Name();
  mNameEdit->setText(comp->Name());
  // invoke hook for derived classes
  DoUpdateFromComponent(comp);
}

// updater
void FfComponentEditor::UpdateFromEditor(FfComponent* comp) {
  // figure component
  if(!comp) comp=Component();
  if(!comp) return;
  // figure sender (to do the fault editors)
  QLineEdit* ledit = qobject_cast<QLineEdit*>(sender());
  if(ledit) {
    QVariant vidx = ledit->property("fidx");
    QVariant vpar = ledit->property("fpar");
    if(vidx.isValid() && vpar.isValid()) {
      qreal tau = ledit->text().toDouble();
      int idx = vidx.toInt();
      FfFault* fcfg = comp->FaultCfg(idx);
      if(fcfg) {
        if(vpar.toString()=="tauf") fcfg->SetTauFail(tau);
        if(vpar.toString()=="taur") fcfg->SetTauRepair(tau);
      }
    }   
  }
  // invoke hook for derived classes
  DoUpdateFromEditor(comp);
}

// default hooks 
void FfComponentEditor::DoUpdateFromComponent(FfComponent* comp) {(void) comp;}
void FfComponentEditor::DoUpdateFromEditor(FfComponent* comp) {(void) comp;}
