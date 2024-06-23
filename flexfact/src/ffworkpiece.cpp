/* workpiece.cpp  - workpiece item  */

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

#include "ffworkpiece.h"
#include "ffcomponent.h"
#include "ffscene.h"

/*
************************************************
************************************************
  
Implementation: FfWorkpiece

************************************************
************************************************
*/

// static id
int FfWorkpiece::mLastId=0;

// default constructor
FfWorkpiece::FfWorkpiece(QObject* parent) : QObject(parent), QGraphicsItem(0) {
  FF_DQM("FfWorkpiece::FfWorkpiece()");
  // my configuration
  setZValue(0);
  mName=QString("WP_%1").arg(++mLastId,3,10,QLatin1Char('0'));
  // my indicators
  mErrIndicator= new FfErrIndicatorItem();
  mErrIndicator->SetWorkpieceError();
  mErrButton = new FfButtonItem(mErrIndicator);
  mErrButton->SetCircle();
  mErrButton->SetColorRed();
  FF_DQM("FfWorkpiece::FfWorkpiece(): done");
  // start with not running
  mRunning=false;
  setVisible(false);
  mErrIndicator->setVisible(false);
}


// destructor
FfWorkpiece::~FfWorkpiece(void) {
  FF_DQM("FfWorkpiece::~FfWorkpiece()");
  // remove my companions
  if(mErrIndicator->scene()) mErrIndicator->scene()->removeItem(mErrIndicator);
  delete mErrIndicator;
}


// recognize scene change
QVariant FfWorkpiece::itemChange(GraphicsItemChange change, const QVariant &value) {
  // ignore others
  if(change!=QGraphicsItem::ItemSceneHasChanged) return value;
  FF_DQ("FfWorkpiece::itemChange(): new scene is " << scene());
  // place companion items in same scene
  if(scene()) 
  if(mErrIndicator->scene() != scene())
    scene()->addItem(mErrIndicator);
  // done, dont interfer
  return value;
}


// figure scene
FfScene* FfWorkpiece::Scene(void) {
  return qobject_cast<FfScene*>(scene());
}

// configure
void FfWorkpiece::SetPosition(const QPointF& npos, const qreal& orient) {
  // record
  mPos=npos;
  mOrient=orient;
  // initialize shape 
  mNominalNorm=QPointF(1,0);
  mNominalOrth=QPointF(0,-1);
  mNominalCoreShape= QPainterPath();
  mNominalCoreShape.moveTo( 45 * mNominalNorm + 45 * mNominalOrth);
  mNominalCoreShape.lineTo( 45 * mNominalNorm - 45 * mNominalOrth);
  mNominalCoreShape.lineTo( - 45 * mNominalNorm - 45 * mNominalOrth);
  mNominalCoreShape.lineTo( - 45 * mNominalNorm + 45 * mNominalOrth);
  mNominalCoreShape.lineTo( + 45 * mNominalNorm + 45 * mNominalOrth);
  mNominalCoreShape.closeSubpath();
  mNominalOuterShape=mNominalCoreShape;
  mNominalOuterRect=mNominalCoreShape.boundingRect().adjusted(-25,-25,25,25);
  // rotate
  QTransform rot;
  rot.rotate(orient);
  mNorm=rot.map(mNominalNorm);
  mOrth=rot.map(mNominalOrth);
  mCoreShape=rot.map(mNominalCoreShape);
  mOuterShape=mCoreShape;
  mOuterRect=mNominalOuterRect;
  // place in scene
  SetPosition(npos);
}

// configure, position onlz
void FfWorkpiece::SetPosition(const QPointF& npos) {
  // record
  mPos=npos;
  // set in scene
  setPos(mPos);
  mErrIndicator->setPos(mPos);
  // dont know my component
  mComponent=0;
}



// set Name
void FfWorkpiece::SetName(const QString& name) {
  mName=name;
}

// get Name
const QString& FfWorkpiece::Name(void) {
  return mName;
}

// reset if
void FfWorkpiece::ResetId(void) {
  mLastId=0;
}


// figure my components
FfComponent* FfWorkpiece::Component(void) { 
  /* we gave up tracking ...
  // known
  if(mComponent) return mComponent;
  */
  // ask  scene
  FfScene* sc=Scene();
  if(!sc) return 0;
  QPoint ipos = FfStyle::Grid200I(pos());
  // ... but we still buffer
  mComponent =sc->Component(ipos);
  return mComponent;
}

// figure my components
const QSet<FfComponent*>&  FfWorkpiece::TransComponents(void) {
  // reset
  mTransComponents.clear();
  // ask  scene
  FfScene* sc=Scene();
  if(!sc) return mTransComponents;
  // figure neighborfields
  QPoint ipos = FfStyle::Grid200I(pos());
  QPoint irpos = FfStyle::Grid200I(pos()+QPointF(50,0));
  QPoint ilpos = FfStyle::Grid200I(pos()+QPointF(-50,0));
  QPoint iupos = FfStyle::Grid200I(pos()+QPointF(0,-50));
  QPoint idpos = FfStyle::Grid200I(pos()+QPointF(0,50));
  // set up list
  FfComponent* cmp;
  if(irpos!=ipos) {
    cmp=sc->Component(irpos);
    if(cmp && (cmp != mComponent)) mTransComponents.insert(cmp);
  }
  if(ilpos!=ipos) {
    cmp=sc->Component(ilpos);
    if(cmp && (cmp != mComponent)) mTransComponents.insert(cmp);
  }
  if(iupos!=ipos) {
    cmp=sc->Component(iupos);
    if(cmp && (cmp != mComponent)) mTransComponents.insert(cmp);
  }
  if(idpos!=ipos) {
    cmp=sc->Component(idpos);
    if(cmp && (cmp != mComponent)) mTransComponents.insert(cmp);
  }
  //done
  return mTransComponents;
}


// paint myself
void FfWorkpiece::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // debug: shape
  /*
  painter->setPen(FfStyle::Color(VioDarkBlue));
  painter->setBrush(Qt::NoBrush);
  painter->drawPath(mCoreShape);
  */
  // shape
  painter->setPen(FfStyle::Color(VioBlack));
  painter->setBrush(FfStyle::Color(VioYellow));
  /*
  if(mState==WJammed) {
    painter->setPen(FfStyle::Color(VioBlack));
    painter->setBrush(FfStyle::Color(VioRed));
  }
  */
  painter->drawPath(mCoreShape);
}


// mode running/editing
void FfWorkpiece::Run(bool run) {
  if(mRunning==run) return;
  mRunning=run;
  setVisible(mRunning);
  mError=false;
  mErrIndicator->setVisible(mError);
}

// track/indicate errors
void FfWorkpiece::SetError(bool err) {
  if(mError==err) return;
  mError=err;
  mErrIndicator->setVisible(mError);
} 

// advance time, testing for collisions
void FfWorkpiece::AdvanceTimeA(qreal dt) {

  // figure components
  bool err=false;
  FfScene* sc=Scene();
  if(!sc) return;   
  FfComponent* comp=Component();
  if(!comp) {
    //FF_DQM("FfWorkpiece::AdvanceTimeA(): wp with no component");
    err=true;
  }

  // figure next position
  //mPos=pos();
  mNextPos=mPos;
  mNextOrient=mOrient;
  mNextStrict=false;
  bool cerr=false;
  if(comp) 
    comp->AdvanceWorkpiece(dt,mPos,mOrient,mNextPos,mNextOrient,mNextStrict,cerr);
  if(cerr) err=true;

  if(mNextOrient==mOrient) {
    mNextShape=mCoreShape;
  } else {
    QTransform rot;
    rot.rotate(mNextOrient);
    mNextShape = rot.map(mNominalCoreShape);
  }     
  QTransform trans;
  trans.translate(mNextPos.x(),mNextPos.y());
  QPainterPath nshape = trans.map(mNextShape);

  // test for collisions: other workpieces
  bool wcoll=false;
  QList<FfWorkpiece*> wplist= sc->Workpieces(nshape);
  if(wplist.size()>1 && mNextStrict) {
    FF_DQM("FfWorkpiece::TestAdvanceTime(): wp/wp collision: break down #" << wplist.size());
    err=true;
  }
  if(wplist.size()>1) {
    wcoll=true;
  }
  // test for collisions: my component blocks
  bool bcoll=false;
  if(comp)
  if(comp->mapToScene(comp->Block()).intersects(nshape)) bcoll=true;
  if(bcoll && mNextStrict) {
    FF_DQM("FfWorkpiece::TestAdvanceTime(): wp/block collision: break down");
    err=true;
  }
  // test for collisions: other components blocks
  const QSet<FfComponent*> tcomps=TransComponents();
  foreach(FfComponent* tcmp,tcomps) {
    if(tcmp==comp) continue;
    bool ocoll=tcmp->mapToScene(tcmp->Block()).intersects(nshape);
    if(ocoll) bcoll=true;
    if(ocoll /* && mNextStrict */) {
      err=true;
      tcmp->RefuseAdvance();
    }
  }

  // record to state
  mBlocked = bcoll || wcoll || err;
  SetError(err);
  if(err) if(comp) comp->RefuseAdvance();
}

// advance time
void FfWorkpiece::AdvanceTimeB(qreal dt) {
  (void) dt;

  // figure components
  FfScene* sc=Scene();
  if(!sc) return;   
  
  // doit
  bool moving=false;
  if(!mBlocked) {
    if(mNextPos != mPos) { 
      SetPosition(mNextPos);  
      moving=true;
    }
    if(mNextOrient!=mOrient) {
      //FF_DQM("FfWorkpiece::TestAdvanceTime(): DO ROTATE");
      QTransform rot;
      rot.rotate(mNextOrient);
      mOrient=mNextOrient;
      mNorm=rot.map(mNominalNorm);
      mOrth=rot.map(mNominalOrth);
      mCoreShape=mNextShape;
      mOuterShape=mCoreShape;
      moving=true;
   }  
  }

  // update
  if(moving) update();
  // tell my friends
  FfComponent* comp=Component();
  if(comp) comp->Workpiece(this);
}

// advance time
void FfWorkpiece::AdvanceTimeC(qreal dt) {
  (void) dt;
}


// record history
void FfWorkpiece::Process(const QString& proc) { 
  FF_DQM("FfWorkpiece::Process(): " << proc);
  mHistory.append(proc);
}


// catch mouse: figure user command
void FfWorkpiece::mousePressEvent(QGraphicsSceneMouseEvent * ev) {
  if(!mRunning) return;
  // prepare
  QString text="NoAction";
  // dispatch: pass on when modifieres are present
  if(ev->modifiers()!=Qt::NoModifier) {
    QGraphicsItem::mousePressEvent(ev);
    return;
  }
  // dispatch: accept and swallow left click on overlay button
  if(ev->buttons()==Qt::LeftButton) {
    QList<QGraphicsItem *> items = mErrIndicator->childItems();
    foreach(QGraphicsItem* item, items) {
      FfButtonItem* button=qgraphicsitem_cast<FfButtonItem*>(item);
      if(!button) continue; 
      if(!button->UnderMouse()) continue; 
      text=button->Text();
      break;
    }
    FF_DQ("FfWorkpiece::mousePressEvent(): " << text);
    if(text=="Circle") {
      FfScene* sc=Scene();
      if(sc) sc->RemoveWorkpiece(this);
      ev->accept();
      return;
    }
  }
  // pass on everything else
  QGraphicsItem::mousePressEvent(ev);
}


// catch contextmenu (used to display processing history)
void FfWorkpiece::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
  // only while simulating (no editing context anyway)
  if(!mRunning) return;
  FF_DQ("FfWorkpiece::contextMenuEvent()");
  // prepare processing history
  QWidget* whis= new QWidget();
  QVBoxLayout* vbox = new QVBoxLayout(whis);
  vbox->setMargin(3);
  vbox->setSpacing(3);
  vbox->addWidget(new QLabel(QString(mName)));
  for(int i=0; i<mHistory.size(); i++)
    vbox->addWidget(new QLabel(mHistory.at(i)));
  // have a menu (trust qt to delete it later)
  QMenu* menu = new QMenu(mName);
  QWidgetAction* wact = new QWidgetAction(0);
  wact->setDefaultWidget(whis); 
  menu->addAction(wact);
  // run it
  menu->popup(event->screenPos());
  // accept the event
  event->accept();
}




 

