/* ffscene  - graphics scene to host productionline components */

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


#include "ffscene.h"
#include <typeinfo>
#include <QtSvg>


/*
************************************************
************************************************
  
Scene of Components

************************************************
************************************************
*/

// internal defs for signals
#define SIG_LIGHT(i)        (2*(i))
#define SIG_SWITCH(i)       (2*(i)+1)


// constructor 
FfScene::FfScene(QObject* parent) : QGraphicsScene(parent) {
  // a friendly background
  setBackgroundBrush(FfStyle::Color(VioAllmostWhite));
  //setBackgroundBrush(FfStyle::Color(VioRed));
  mRunning=false;
  // have process image as item model
  mImage=new FfProcessImage(this);
  connect(mImage,SIGNAL(NotifySensorEvent(QString)),this,SIGNAL(NotifySensorEvent(QString)));
  connect(mImage,SIGNAL(NotifySignal(int,bool)),this,SIGNAL(NotifySignal(int,bool)));
  // default name/config
  mName="FlexFact";
  mOperatorComponentSize=0;
  // no monitor
  pMonitor=0;
  // clear to reset other vars
  Clear();
}

// have a name
void FfScene::SetName(QString name) {
  mName=name;
  mImage->SetName(mName);
}

// clear all
void FfScene::Clear() {
  FF_DQM("FfScene::Clear()");
  // stop simulation
  Run(false);
  // remove workpieces
  foreach(FfWorkpiece* wp, mWorkpieces) 
    RemoveWorkpiece(wp);
  // remove components 
  foreach(FfComponent* comp, mComponents) {
    removeItem(comp);
    delete comp;
  }
  mComponents.clear();
  mComponentPosMap.clear();
  mComponentNameMap.clear();
  AdjustScene();
  FF_DQM("FfScene::Clear(): ok, now recompile");
  ClearImage();
  ClearMonitor();
  CompileMonitor();
  OperatorPanelSize(mOperatorComponentSize);
}

// configure operator component
void FfScene::OperatorPanelSize(int sz) {
  mOperatorComponentSize=sz;
  FF_DQM("FfScene::OperatorPanelSize()");
  // clear all signals
  foreach(FfSignal* sig, mOperatorSignals) 
    delete sig;
  mOperatorSignals.clear();
  // construct all my signals
  FfSignal* sigcfg;
  for(int j=0; j<sz; j++) {
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_LIGHT(j));
    sigcfg->SetLastName(QString("L%1").arg(j+1));
    sigcfg->SetActuator();
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("l%1on").arg(j+1));
    sigcfg->SetNegativeEdgeEvents(QStringList() << QString("l%1off").arg(j+1));
    sigcfg->SetFirstName("OP");
    connect(sigcfg,SIGNAL(NotifyValue(bool)),this,SLOT(EmitOperatorLight(void)));
    mOperatorSignals.append(sigcfg);
    sigcfg= new FfSignal(this);
    sigcfg->SetRelAddress(SIG_SWITCH(j));
    sigcfg->SetLastName(QString("S%1").arg(j+1));
    sigcfg->SetSensor();
    sigcfg->SetPositiveEdgeEvents(QStringList() << QString("s%1act").arg(j+1));
    sigcfg->SetNegativeEdgeEvents(QStringList() << QString("s%1rel").arg(j+1));
    sigcfg->SetFirstName("OP");
    mOperatorSignals.append(sigcfg);
  }
  // invalidate image
  ClearImage();
}

// dispatch operator light
void FfScene::EmitOperatorLight(void) {
  // figure rel address
  FfSignal* sig = qobject_cast<FfSignal*>(sender());
  if(!sig) return;
  int addr = sig->RelAddress();
  // emit
  FF_DQM("FfScene::EmitOperatorLight()");
  emit OperatorLight(addr/2,sig->Value());
}  


// register monitor
void FfScene::SetMonitor(FfMonitorPanel* monitor) {
  pMonitor=monitor;
  CompileMonitor();
}

// register monitor
FfMonitorPanel* FfScene::Monitor(void) {
  return pMonitor;
}

// edit monitor
void FfScene::ClearMonitor(void) {
  if(!pMonitor) return;
  pMonitor->Clear();
  pMonitor->Compile();
}

// edit monitor
void FfScene::CompileMonitor(void) {
  if(!pMonitor) return;
  // stage 0: clear
  pMonitor->Clear();
  // stage 1: insert faults
  for(int i=0; i<mComponents.size(); i++) {
    FfComponent* comp  = mComponents.at(i);
    int j=0;
    FfFault* faultcfg=0;
    while((faultcfg=comp->FaultCfg(j++))!=0) {
      if(!faultcfg->Active()) continue;
      pMonitor->Insert(faultcfg);
    }
  }
  // stage 2: compile
  pMonitor->Compile();
}


// ensure the scene to be large enough
void FfScene::AdjustScene(void) {
  // items
  QRectF arect= itemsBoundingRect();
  // cover all viewports ( we have only one anyway)
  foreach(QGraphicsView* gview, views()) {
    QPoint vbr(gview->viewport()->size().width(),gview->viewport()->size().height());
    QPoint vtl(0,0);
    QPointF sbr = gview->mapToScene(vbr);
    QPointF stl = gview->mapToScene(vtl);
    QRectF srect(stl,sbr);
    arect |= srect;    
  }
  // threshold
  QRectF drect(sceneRect().topLeft() - arect.topLeft(), sceneRect().bottomRight() - arect.bottomRight());
  drect=drect.normalized();
  if(drect.width() < 200) 
  if(drect.height() < 200)
    return;
  // cover largest viewport
  setSceneRect(arect);
  // redraw backdound (should be automatic anyway?)
  invalidate(QRectF(),QGraphicsScene::BackgroundLayer);
}

// access components
bool FfScene::AddComponent(FfComponent* comp) {
  FF_DQM("FfScene::AddComponent():" << comp->Name() << " #" << comp->Area().size());
  // test overlap
  foreach(QPoint pp, comp->Area())
    if(mComponentPosMap.contains(pp)) return false;
  // make name unique ...
  QString nname=comp->Name();
  // 0. reserved names
  if(nname=="OP") nname="OP1";
  if(mComponentNameMap.contains(nname)) {
    // 1. eliminate trailing digits
    int apos=nname.size();
    while(apos>0) if(nname.at(apos-1).isDigit()) apos--; else break;
    nname=nname.left(apos);
    // 2. append digits to fit
    QString uname;
    int i=1;
    do {
      uname=nname + QString().setNum(i++);
    } while(mComponentNameMap.contains(uname));      
    nname=uname;
  }
  comp->SetName(nname);
  // append
  mComponents.append(comp);
  addItem(comp);
  // record area
  foreach(QPoint pp, comp->Area())
    mComponentPosMap[pp]=comp;
  // record name
  mComponentNameMap[nname]=comp;
  // connect
  connect(comp,SIGNAL(NotifyParameterChanged()),this,SLOT(ClearImage()));
  connect(comp,SIGNAL(NotifyParameterChanged()),this,SLOT(CompileMonitor()));
  FF_DQM("FfScene::AddComponent(): done");
  AdjustScene();
  ClearImage();
  ClearMonitor();
  return true;
}

// update component area (false on error)
bool FfScene::ReconfigureComponent(FfComponent* comp, QList<QPoint> & narea) {
  // test overlap
  foreach(QPoint pp, narea) {
    if(!mComponentPosMap.contains(pp)) continue;
    if(mComponentPosMap[pp]==comp) continue;
    return false;
  }
  // clear old area
  QList<QPoint> area= comp->Area();
  foreach(const QPoint& p,area) 
    mComponentPosMap.remove(p);
  // record new area
  foreach(QPoint pp, narea)
    mComponentPosMap[pp]=comp;
  FF_DQM("FfScene::ReconfigureComponent(): done");
  AdjustScene();
  return true;
}


// access components
bool FfScene::MoveComponent(FfComponent* comp, QPoint ipos) {
  // test overlap
  foreach(QPoint pp, comp->TestArea(ipos,comp->Orient())) {
    if(!mComponentPosMap.contains(pp)) continue;
    if(mComponentPosMap[pp]==comp) continue;
    return false;
  }
  // clear old area
  QList<QPoint> area= comp->Area();
  foreach(const QPoint& p,area) 
    mComponentPosMap.remove(p);
  // set component
  comp->SetPosition(ipos);
  comp->Update();
  // record new area
  foreach(QPoint pp, comp->Area())
    mComponentPosMap[pp]=comp;
  FF_DQM("FfScene::Reposition(): done");
  AdjustScene();
  return true;
}

// access components (absolut orientation)
bool FfScene::OrientComponent(FfComponent* comp, FfComponent::Orientation o) {
  // overlap?
  foreach(QPoint pp, comp->TestArea(comp->Position(),o)) {
    if(!mComponentPosMap.contains(pp)) continue;
    if(mComponentPosMap[pp]==comp) continue;
    return false;
  }
  // clear old area
  QList<QPoint> area= comp->Area();
  foreach(const QPoint& p,area) 
    mComponentPosMap.remove(p);
  // set component
  comp->SetOrient(o);
  comp->Update();
  // record new area
  foreach(QPoint pp, comp->Area())
    mComponentPosMap[pp]=comp;
  FF_DQM("FfScene::Reposition(): done");
  AdjustScene();
  return true;
}

// access components
bool FfScene::RotateComponent(FfComponent* comp, const QPoint& iorigin, bool cw) {
  // figure new ipos
  QPoint inpos;
  QPoint ipos=comp->Position();
  QPoint idpos= ipos - iorigin;
  if(cw) {
    inpos.ry()= idpos.x();
    inpos.rx()=-idpos.y();
    inpos+=iorigin;
  } else {
    inpos.rx()=idpos.y();
    inpos.ry()=-idpos.x();
    inpos+= iorigin;
  }
  // figure new orientation
  FfComponent::Orientation no=FfComponent::Ostd;
  if(cw) switch(comp->Orient()) {
    case(FfComponent::Ostd):  no=FfComponent::O90; break;
    case(FfComponent::O90) :  no=FfComponent::O180; break;
    case(FfComponent::O180):  no=FfComponent::O270; break;
    case(FfComponent::O270):  no=FfComponent::Ostd; break;
  } else switch(comp->Orient()) {
    case(FfComponent::Ostd):  no=FfComponent::O270; break;
    case(FfComponent::O90) :  no=FfComponent::Ostd; break;
    case(FfComponent::O180):  no=FfComponent::O90; break;
    case(FfComponent::O270):  no=FfComponent::O180; break;
  }
  // test overlap
  foreach(QPoint pp, comp->TestArea(inpos,no)) {
    if(!mComponentPosMap.contains(pp)) continue;
    if(mComponentPosMap[pp]==comp) continue;
    return false;
  }
  // clear old area
  QList<QPoint> area= comp->Area();
  foreach(const QPoint& p,area) 
    mComponentPosMap.remove(p);
  // set component
  comp->SetOrient(no);
  comp->SetPosition(inpos);
  comp->Update();
  // record new area
  foreach(QPoint pp, comp->Area())
    mComponentPosMap[pp]=comp;
  FF_DQM("FfScene::Reposition(): done");
  AdjustScene();
  return true;
}


// remove component
void FfScene::RemoveComponent(FfComponent* comp) {
  if(mRunning) return;
  FF_DQM("FfScene::RemoveComponent():  " << (void*) comp << " from (" <<
     comp->x() << "," << comp->y() << ")");
  // take from maps
  QList<QPoint> area= comp->Area();
  foreach(const QPoint& p,area) 
    mComponentPosMap.remove(p);
  mComponentNameMap.remove(comp->Name());
  // take item 
  removeItem(comp);
  FF_DQ("FfComponent::RemoveComponent(): scene is now   " << comp->scene());  
  // finally, delete
  mComponents.removeAll(comp);
  delete comp;
  // must recompile (component id changed)
  ClearImage();
  ClearMonitor();
  AdjustScene();
};

// rename component
bool FfScene::RenameComponent(QString oldname, QString newname) {
  FfComponent* comp = Component(oldname);
  if(!comp) return false;
  return RenameComponent(comp,newname);
}

// rename component
bool FfScene::RenameComponent(FfComponent* comp, QString nname) {
  // refuse if not in scene
  if(!mComponents.contains(comp)) return false;
  // bail out on no change
  if(nname==comp->Name()) return true;
  // make unique
  QString uname=nname;
  // 0. reserved names
  if(nname=="OP") nname="OP1";
  if(mComponentNameMap.contains(nname)) {
    // 1. eliminate trailing digits
    int apos=nname.size();
    while(apos>0) if(nname.at(apos-1).isDigit()) apos--; else break;
    nname=nname.left(apos);
    // 2. append digits to fit
    int i=1;
    do {
      uname=nname + QString().setNum(i++);
    } while(mComponentNameMap.contains(uname));      
  }
  // doit
  mComponentNameMap.remove(comp->Name());
  comp->SetName(uname);
  mComponentNameMap[uname]=comp;
  return true;
}

// access components
FfComponent* FfScene::Component(QPoint pos) {
  if(!mComponentPosMap.contains(pos)) return 0;
  return mComponentPosMap[pos];
}

// access components
FfComponent* FfScene::Component(const QString& name) {
  if(!mComponentNameMap.contains(name)) return 0;
  return mComponentNameMap[name];
}


// invalidate global signal and event structures
void FfScene::ClearImage(void) {
  mImage->Clear();
  emit NotifyClearImage();
}

// assemble event lists etc
void FfScene::CompileImage(void) {
  FF_DQM("FfScene::CompileImage() 1");
  // stage 1: merge all componeny signals
  mImage->Clear();
  for(int i=0; i<mComponents.size(); i++) {
    FfComponent* comp  = mComponents.at(i);
    FF_DQM("FfScene::CompileImage(): " << comp->Name());
    for(int j=0; j<comp->ImageSize(); j++) 
     mImage->Insert(comp->SignalCfg(j));
  }
  // stage 2: merge operator signals
  for(int j=0; j< mOperatorSignals.size(); j++) 
    mImage->Insert(mOperatorSignals.at(j));
  // stage 3: do compile
  mImage->Compile();
  FF_DQM("FfScene::CompileImage() done");
}


// take workpieces
void FfScene::RemoveWorkpiece(FfWorkpiece* wp) {
  FF_DQM("FfScene::RemoveWorkPiece():  from (" << 
     wp->x()<< ","<< wp->y() << ")");
  // add item
  mWorkpieces.removeAll(wp);
  removeItem(wp);
  wp->deleteLater();
};

// access workpieces
void FfScene::AddWorkpiece(const QPointF& pos, FfComponent::Orientation o) {
  FF_DQM("FfScene::AddWorkPiece(): at (" << 
   pos.x()<< ","<< pos.y() << ")");
  // instantiate wp
  FfWorkpiece* wp = new FfWorkpiece();
  // set position
  qreal oo;
  switch(o) {
  case(FfComponent::Ostd):  oo=0; break;
  case(FfComponent::O90) :  oo=90; break;
  case(FfComponent::O180):  oo=180; break;
  case(FfComponent::O270):  oo=270; break;
  }
  wp->SetPosition(pos,oo);
  wp->Run(true);
  // add item
  mWorkpieces.append(wp);
  addItem(wp);
};


// access workpieces
QList<FfWorkpiece*> FfScene::Workpieces(const QPainterPath& path) {
  QList<FfWorkpiece*> res;
  foreach(FfWorkpiece* wp, mWorkpieces) 
    if(wp->collidesWithPath(wp->mapFromScene(path))) res.append(wp);
  return res;
}

// access workpieces
QList<FfWorkpiece*> FfScene::Workpieces(const QPointF& pos) {
  QList<FfWorkpiece*> res;
  foreach(FfWorkpiece* wp, mWorkpieces) 
    if(wp->contains(wp->mapFromScene(pos))) res.append(wp);
  return res;
}

// file io
QDomElement FfScene::ToElement(QDomDocument& doc) {
  QDomElement scele = doc.createElement("PlantComponents");
  for(int i=0; i< mComponents.size(); i++) {
    FfComponent* comp=mComponents[i];
    QDomElement cele=comp->ToElement(doc);
    scele.appendChild(cele);
  }
  return scele;
}

// file io
void FfScene::FromElement(QDomElement& elem) {
  // clear all
  Clear();
  // find components
  QDomElement pele = elem.firstChildElement("PlantComponents");
  QDomElement cele = pele.firstChildElement();
  while(!cele.isNull()) {
    // instantiate component
    FfComponent* comp= FfComponent::FromElement(cele);
    // insert component
    if(comp) AddComponent(comp);
    // proceed
    cele=cele.nextSiblingElement();
  }
  // invalidate image
  ClearImage();
  CompileMonitor();
}


// user: delete
void FfScene::UserDelSelection(void) {
  foreach(QGraphicsItem* item,selectedItems()) {
    FfComponent* scomp = dynamic_cast<FfComponent*>(item);  // q-cast broken?
    if(scomp) RemoveComponent(scomp);
  }
  // invalidate image
  ClearImage();
  CompileMonitor();
}


// user: rotate (current implementation: only good for single selection)
void FfScene::UserRotateSelection(const QPoint& iorigin, bool cw) {
  foreach(QGraphicsItem* item,selectedItems()) {
    FfComponent* scomp = dynamic_cast<FfComponent*>(item);  // q-cast broken?
    if(!scomp) continue;
    FF_DQ("FfScene::UserRotateSelection(): comp at   " << scomp->Position().x() << "-" << scomp->Position().y());
    FF_DQ("FfScene::UserRotateSelection(): origin at " << iorigin.x() << "-" << iorigin.y());
    RotateComponent(scomp,iorigin,cw);

    FF_DQ("FfScene::UserRotateSelection(): new comp at   " << scomp->Position().x() << "-" << scomp->Position().y());
  }
}


// svg write
int FfScene::ExportSvg(const QString &svgfile) {
  // todo: errors?
  FF_DQ("FfScene::SvgWrite(file)");
  QSvgGenerator svggen;
  svggen.setFileName(svgfile);
  svggen.setSize(QSize((int) width()+1, (int) height()+1));
  QPainter painter(&svggen);
  render(&painter,QRectF(),itemsBoundingRect());
  painter.end();
  return 0;
}



// reset state
void FfScene::Reset(void) {
  // remove workpieces
  foreach(FfWorkpiece* wp, mWorkpieces) 
    RemoveWorkpiece(wp);
  FfWorkpiece::ResetId();
  // reset components
  foreach(FfComponent* comp, mComponents) 
    comp->Reset();
  // reset operator signals
  for(int j=0; j< mOperatorSignals.size(); j++) 
    mOperatorSignals.at(j)->SetValue(false);
  // need to compile for newly created component signals
  CompileImage();
}

// start/stop
void FfScene::Run(bool run) {
  // bail out on trivial
  if(mRunning==run) return;
  mRunning=run;
  // deselect
  clearSelection();
  // make sure, image is ok
  CompileImage();
  // pass on
  foreach(FfComponent* comp, mComponents) {
    FF_DQM("FfScene::Run(): comp " << comp->Name());
    comp->Run(mRunning);
  }
  foreach(FfWorkpiece* wp, mWorkpieces) {
    FF_DQM("FfScene::Run(): wp " << wp->Name());
    wp->Run(mRunning);
  }
  // adjust myself
  if(!mRunning) {
    setBackgroundBrush(FfStyle::Color(VioAllmostWhite));
    //setBackgroundBrush(FfStyle::Color(VioRed));
  }else {
    setBackgroundBrush(QColor(240,240,160));
  }
  // redraw backdound
  invalidate(QRectF(),QGraphicsScene::BackgroundLayer);
  // refresh
  update();
}


// advance time
void FfScene::AdvanceTime(qreal dt) {
  if(!mRunning) return;
  for(int i=0; i< mWorkpieces.size(); i++)
    mWorkpieces.at(i)->AdvanceTimeA(dt);
  for(int i=0; i< mWorkpieces.size(); i++)
    mWorkpieces.at(i)->AdvanceTimeB(dt);
  for(int i=0; i< mWorkpieces.size(); i++)
    mWorkpieces.at(i)->AdvanceTimeC(dt);
  for(int i=0; i< mComponents.size(); i++)
    mComponents.at(i)->AdvanceTime(dt);
}


// signal interface: size
int FfScene::ImageSize(void) {
  return mImage->Size();
}

// signal interface: set
void FfScene::SetSignal(int address, bool val) {
  mImage->SetValue(address,val);
}

// signal interface: get
bool FfScene::Signal(int address) {
  return mImage->Value(address);
}

// signal interface: get symbolic name
QString FfScene::SignalName(int address) {
  return mImage->Name(address);
}

// signal interface: type
bool FfScene::IsActuator(int address) {
  return mImage->IsActuator(address);
}

// signal interface: type
bool FfScene::IsSensor(int address) {
  return mImage->IsSensor(address);
}


// execute actuator
bool FfScene::ExecuteActuatorEvent(const QString& actev) {
  return mImage->ExecuteActuatorEvent(actev);
} 

// report all known actuator events
QStringList FfScene::ActuatorEvents(void) {
  return mImage->ActuatorEvents();
}

// report all known sensor events
QStringList FfScene::SensorEvents(void) {
  return mImage->SensorEvents();
}

// execute op switch
void FfScene::OperatorSwitch(int i, bool v) {
  int addr = SIG_SWITCH(i);
  if(addr < 0 || addr >= mOperatorSignals.size()) return;
  mOperatorSignals[addr]->SetValue(v);
}


// poor men's graphics effect
const QColor& FfScene::EffectColor(int vcol) {
  // built in data/initializer
  static QList<QColor> mEffectColorDefs;
  static bool ninit=true;
  if(ninit) {
    ninit=false;
    for(int i=0; i<FfStyle::Colors(); i++)
      mEffectColorDefs.append(FfStyle::ColorGrey(FfStyle::Color(i)));
  }
  // no effect
  if(mRunning) return FfStyle::Color(vcol);
  // grey effect
  return mEffectColorDefs.at(vcol);
}



// handle my events: mouse press
void FfScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {

  // anything else: pass on to scene
  /*
  QGraphicsScene::mousePressEvent(event);
  return;
  */

  // where are we
  QList<QGraphicsItem*> mitems = items(event->scenePos());
 
  // reset move operation
  mMoveSelection=false;
  mMovingSelection=false;

  /********************************************
  * mode: simulation: items take care
  *********************************************/ 
  if(mRunning) {
    
    // scene does dispatch to my items
    QGraphicsScene::mousePressEvent(event);
    return;

  }

  /********************************************
  * mode: editing: we organize select
  *********************************************/ 

  if(!mRunning) {

  // figure component
  FfComponent* comp=0;
  foreach(QGraphicsItem* item,mitems) 
    if((comp = dynamic_cast<FfComponent*>(item))!=0) break;
  QGraphicsItem* item=comp;


  // find selectable item
  /*
  while(item) {
    FF_DQM("FfScene::mousePress(): type  " << typeid(*item).name());
    if(item == item->parentItem()) break; 
    if(item->flags()& QGraphicsItem::ItemIsSelectable) break;
    item=item->parentItem();
  }
  */

  // report
  FF_DQM("FfScene::mousePress(): item " << item );

  // hit item that is selected: upcomming item move
  if(item) 
  if(item->isSelected()) 
  if(event->button() == Qt::LeftButton)
  if(!(event->modifiers() & Qt::ControlModifier)) {
    mMoveSelection=true;
    QGraphicsScene::mousePressEvent(event);
    return;
  }
  // hit item that is not selected: select myself 
  // (.. and pass on to let the detect upcomming move)
  if(item) 
  if(!item->isSelected()) 
  if(event->button() == Qt::LeftButton) 
  if(!(event->modifiers() & Qt::ControlModifier)) {
    FF_DQM("FfScene::mousePress(): select");
    clearSelection();
    item->setSelected(true);
    mMoveSelection=true;
    event->accept();
    QGraphicsScene::mousePressEvent(event);
    return;
  }
  // hit item that is selected with ctrl: deselect
  if(item) 
  if(item->isSelected()) 
  if(event->button() == Qt::LeftButton)
  if(event->modifiers() & Qt::ControlModifier) {
    FF_DQM("FfScene::mousePress(): ctrl deselect");
    item->setSelected(false);
    event->accept();
    return;
  }
  // hit item that is not selected with ctrl: select
  if(item) 
  if(!item->isSelected()) 
  if(event->button() == Qt::LeftButton)
  if(event->modifiers() & Qt::ControlModifier) {
    FF_DQM("FfScene::mousePress(): ctrl select");
    item->setSelected(true);
    event->accept();
    return;
  }
  // no item: select nothing and start drag
  if(!item) 
  if(event->button() == Qt::LeftButton) 
  if(!(event->modifiers() & Qt::ControlModifier)) {
    clearSelection();
    event->ignore();
    //event->accept(); // note: drag cannot have accept here
    return;
  }
  // no item with ctrl mod: ignore
  if(!item) 
  if(event->button() == Qt::LeftButton)
  if(event->modifiers() & Qt::ControlModifier) {
    event->accept();
    return;
  }
  // right button: done by context menu, no accept here
  if(event->button() == Qt::RightButton) {  
    event->ignore();
    return;
  }

  // anything else: pass on to scene
  QGraphicsScene::mousePressEvent(event);
  return;

  } // end modes

}


// handle my events: mouse release
void FfScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  FF_DQ("FfScene::mouseReleaseEvent(...)"); 
  // move selection: set on grid
  if(mMovingSelection) {
    FF_DQ("Ff::mouseReleaseEvent(...): done moving"); 
    mMoveSelection=false;
    mMovingSelection=false;
    // test: can we move?
    // (this one is really tricky if one mate after the move
    // ends up on the other mates field; the trick is done in
    // GoodPosition, tailored for that purpose)
    bool allgood=true;
    foreach(QGraphicsItem* item, selectedItems()) {
      //FfComponent* comp = qgraphicsitem_cast<FfComponent*>(item);  // not functional
      FfComponent* comp = dynamic_cast<FfComponent*>(item);
      if(!comp) continue;
      if(comp->GoodPosition()) continue;
      FF_DQ("Ff::mouseReleaseEvent(...): refuse " << comp->Name());
      allgood=false; 
      break;
    }
    // move all
    if(allgood) {
      FF_DQ("FfScene::mouseReleaseEvent(...): free original area");
      foreach(QGraphicsItem* item, selectedItems()) {
        FfComponent* comp = dynamic_cast<FfComponent*>(item);
        if(!comp) continue;
        QList<QPoint> area= comp->Area();
        foreach(const QPoint& p,area) 
          mComponentPosMap.remove(p);
      }
      FF_DQ("FfScene::mouseReleaseEvent(...): do move");
      foreach(QGraphicsItem* item, selectedItems()) {
        FfComponent* comp = dynamic_cast<FfComponent*>(item);
        if(!comp) continue;
        QPoint  ipos=FfStyle::Grid200I(comp->pos());
        comp->SetPosition(ipos);
        comp->Update();
      }
      FF_DQ("FfScene::mouseReleaseEvent(...): record area");
      foreach(QGraphicsItem* item, selectedItems()) {
        FfComponent* comp = dynamic_cast<FfComponent*>(item);
        if(!comp) continue;
        QList<QPoint> area= comp->Area();
        foreach(const QPoint& pp,area) 
          mComponentPosMap[pp]=comp;
      }
      FF_DQ("FfScene::mouseReleaseEvent(...): move done");
      AdjustScene();
    }
    // revert all
    if(!allgood) {
      FF_DQ("FfScene::mouseReleaseEvent(...): revert all");
      foreach(QGraphicsItem* item, selectedItems()) {
        FfComponent* comp = dynamic_cast<FfComponent*>(item);
        if(!comp) continue;
        comp->SetPosition(comp->Position());
        comp->Update();
      }
    }
    // fix zvalue (put back to our std -5)
    foreach(QGraphicsItem* item, selectedItems()) 
      item->setZValue(-5);
    // adjust
    AdjustScene();
  }
  // anything else: trust in scene to dispatch
  FF_DQ("FfScene::mouseResleaseEvent(...): pass to scene"); 
  QGraphicsScene::mouseReleaseEvent(event);
}


// handle my events: mouse move
void FfScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  // sense start of move
  if(mMoveSelection && !mMovingSelection) {
    FF_DQ("FfScene::mouseMoveEvent(...): start moving selection"); 
    mMovingSelection=true;
    // fix zvalue (get in front of std -5)
    foreach(QGraphicsItem* item, selectedItems()) 
      item->setZValue(0);
  }
  // track move
  if(mMovingSelection) {
    /*
    FF_DQ("FfScene::mouseMoveEvent(...): started x " << event->scenePos().x() 
	  << " y " << event->scenePos().y());
    FF_DQ("FfScene::mouseMoveEvent(...): now at  x " << event->scenePos().x() 
	  << " y " << event->scenePos().y());
    */
  }
  // anything else: trust in scene to dispatch to items
  QGraphicsScene::mouseMoveEvent(event);
}


// handle my events: key
void FfScene::keyPressEvent(QKeyEvent *keyEvent) {
  FF_DQ("FfScene::keyPressEvent(...)"); 
  // delete selection
  if(keyEvent->key()==Qt::Key_Backspace || keyEvent->key()==Qt::Key_Delete) {
    UserDelSelection();
    keyEvent->accept();
  }
  // testing features
  if(keyEvent->key()==Qt::Key_A) {
    FF_DQ("FfScene::keyPressEvent: Testing Features a"); 
    //MarkUndoPoint();
    FF_DQ("FfScene::keyPressEvent: Testing Features done"); 
  }  
  // rotate 
  if(keyEvent->key()==Qt::Key_R) 
  if(keyEvent->modifiers()== (Qt::ShiftModifier | Qt::ControlModifier)) {
    // invent origin
    QPoint iorigin(0,0);
    foreach(QGraphicsItem* item,selectedItems()) {
      FfComponent* scomp = dynamic_cast<FfComponent*>(item);  // q-cast broken?
      if(!scomp) continue;
      iorigin=scomp->Position();
    }
    // doit
    UserRotateSelection(iorigin,true);
    keyEvent->accept();
  }
  // pass on
  QGraphicsScene::keyPressEvent(keyEvent);
}

// handle my events: contex menu
void FfScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {

  // when simulation is on, items have their local menus
  if(mRunning) {
    QGraphicsScene::contextMenuEvent(event);
    return;
  }
  
  // scene context
  QGraphicsItem* item = itemAt(event->scenePos());
  if(!item) {
    SceneContextMenu(event->screenPos(),event->scenePos());
  }

  // find a component
  FfComponent* comp =0;
  while(item) {
    FF_DQM("FfScene::mousePress(): type  " << typeid(*item).name());
    if(item == item->parentItem()) break; 
    comp = dynamic_cast<FfComponent*>(item);  // q-cast broken?
    if(comp) break;
    item = item->parentItem();
  }
  if(!comp) return;

  // component context
  ComponentContextMenu(event->screenPos(),event->scenePos(),comp);
}

// edit component data via context menu
void FfScene::ComponentContextMenu(const QPoint& screenpos, const QPointF& scenepos, FfComponent* comp) {
  FF_DQ("FfScene::ComponentContextMenu(): #" << selectedItems().size() << " with comp " << comp->Name());
  // fix selection
  if(!comp->isSelected()) {
    clearSelection();
    comp->setSelected(true);
  }
  bool single = selectedItems().size()==1;
  // have std menu
  QMenu* menu = new QMenu("Component");
  // add the components parameter editor
  QWidget* paredit = comp->NewEditor();
  QWidgetAction* paraction = new QWidgetAction(0);
  paraction->setDefaultWidget(paredit);
  menu->addAction(paraction); 
  menu->addSeparator();
  // add my buttons: rotate
  QAction* rotcw= menu->addAction("Rotate CW");
  rotcw->setEnabled(single);
  rotcw->setShortcut(tr("Ctrl+Shift+R"));
  QAction* rotccw= menu->addAction("Rotate CCW");
  rotccw->setEnabled(single);
  // add my buttons: delete 
  menu->addSeparator();
  QAction* deleteaction;
  deleteaction=menu->addAction("Delete");
  // run menu
  QAction *selaction = menu->exec(screenpos);
  // rotate
  if(selaction==rotcw){
    QPoint ipos=FfStyle::Grid200I(scenepos);
    UserRotateSelection(ipos,true);
  } 
  if(selaction==rotccw){
    QPoint ipos=FfStyle::Grid200I(scenepos);
    UserRotateSelection(ipos,false);
  }
  // delete
  if(selaction==deleteaction){
    UserDelSelection();
  }
  // done
  delete menu;
  FF_DQ("FfScene::ComponentContextMenu: done ");
}


// scene context aka insert
void FfScene::SceneContextMenu(const QPoint& screenpos, const QPointF& scenepos) {
  FF_DQ("FfScene::SceneContextMenu()");
  // fix selection
  clearSelection();
  // have std menu
  QMenu* menu = new QMenu("Insert Component");
  QAction* label= menu->addAction("Insert ...");
  label->setEnabled(false);
  menu->addSeparator();
  // add components we can insert
  QStringList types=FfComponent::AvailableTypes();
  types.sort();
  for(int i=0; i< types.size(); i++) {
    menu->addAction(types.at(i));
  }
  // run menu
  QAction *selaction = menu->exec(screenpos);
  // figure type
  if(selaction) {
    QString ctype=selaction->text();
    FfComponent* comp = FfComponent::New(ctype);
    if(comp) {
      FF_DQ("FfScene::stateContextMenu: insert " << ctype);
      QPoint ipos=FfStyle::Grid200I(scenepos);
      comp->SetPosition(ipos);
      comp->Update();
      AddComponent(comp);
      ClearImage();
      CompileMonitor();
    }
  }
  // done
  delete menu;
  FF_DQ("FfScene::stateContextMenu: done ");
}


// reimplement background
void FfScene::drawBackground(QPainter * painter, const QRectF & rect) {
  // give base a try for my colors
  QGraphicsScene::drawBackground(painter,rect);
  if(mRunning) return;
  // be nice to pen
  painter->save();
  // debug
  /*
  static bool flip = false;
  if(flip) painter->setBrush(FfStyle::Color(VioYellow));
  else painter->setBrush(FfStyle::Color(VioRed));
  flip=!flip;
  painter->setPen(Qt::NoPen);  
  painter->drawRect(rect);
  */
  // add by grid
  qreal gw=2*200;
  QPointF xgw=  QPointF(gw,0);
  QPointF ygw=  QPointF(0,gw);
  // chess skin
  painter->setBrush(FfStyle::Color(VioLighterGrey));
  painter->setPen(Qt::NoPen);
  QPointF it1, it2;
  it1.ry()=rect.top()-400;
  while(it1.y()<= rect.bottom()+100) {
    it1.rx()=rect.left()-400;
    while(it1.x()<= rect.right()+100) {
      it2= FfStyle::Grid400(it1);
      painter->drawRect(it2.x()+100,it2.y()+100,200,200);
      painter->drawRect(it2.x()+300,it2.y()+300,200,200);
      it1+=xgw;
    }
    it1+=ygw;
  }
  // done
  painter->restore();
};

