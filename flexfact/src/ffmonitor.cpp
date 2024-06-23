/* ffmonitor.cpp  - fault monitor */

/*
FlexFact --- a configurable factory simulator
Copyright (C) 2014 Thomas Moor

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

#include "ffmonitor.h"



/*
************************************************
************************************************
  
Implementation: FfMonitorButton

************************************************
************************************************
*/


// constructor 
FfMonitorButton::FfMonitorButton(QWidget* parent) : QPushButton(parent) {
  setCheckable(false);
  setFlat(false);
  setFocusPolicy(Qt::NoFocus); 
}

// destructor 
FfMonitorButton::~FfMonitorButton(void) {
  FF_DQM("FfMonitorButton::~FfMonitorButton()");
}

// change style
void FfMonitorButton::Light(bool v) {
  setDefault(v);
}


/*
************************************************
************************************************
  
Implementation: FfMonitorPanel

************************************************
************************************************
*/


// constructor 
FfMonitorPanel::FfMonitorPanel(QWidget* parent) : QWidget(parent) {
  // have a layout
  mGbox = new QGridLayout(this);
  mGbox->setMargin(0);
  mGbox->setSpacing(5);
  mGbox->setHorizontalSpacing(5);
  // my style sheet
  setStyleSheet(
   " QPushButton {"
   " border: 2px solid #8f8f91;"
   " border-radius: 0px;"
   " background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f0f0f0, stop: 1 #f0f0f0);"
   " min-width: 50px;"
#ifdef Q_OS_MAC
   " margin: 3px;"
#else
   " margin: 0px;"
#endif
   " }"
   " QPushButton:default {"
   " border: 2px solid #8f8f91;"
   " background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #d07983, stop: 1 #d07983);"
   " }"
   " QPushButton:pressed {"
   " border: 2px solid #8f8f91;"
   " background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #808080, stop: 1 #808080);"
   " }"
   );
}


// configure (must clear-compile)
void FfMonitorPanel::Clear(void) {
  // disconnect button signals
  for(int y=0; y<mButtons.size(); y++) 
    for(int x=0; x<mButtons.at(y).size(); x++)
      disconnect(mButtons.at(y).at(x),0,0,0);
  // disconnect from button slots (hack: we exclusively have a signal "NotifyLight")
  foreach(FfFault* faultcfg, mFaults) 
    disconnect(faultcfg,SIGNAL(NotifyLight(bool)),0,0);
  // unregister
  mFaults.clear();
}

// configure (must clear-insert-compile)
void FfMonitorPanel::Insert(FfFault* faultcfg) {
  FF_DQM("FfMonitorPanel::Insert()");
  // append
  mFaults.append(faultcfg);
}

// compare for sorted pointer lists
bool FaultPtrLessThan(const FfFault* a, const FfFault* b) {
  if(!a) return false;
  if(!b) return true;
  return a->Name() < b->Name();
};

// configure (this is a mess ... we keep butttons and re-connect to avoid flicker)
void FfMonitorPanel::Compile(void) {
  FF_DQM("FfMonitorPanel::Compile()");
  // sort
  qStableSort(mFaults.begin(), mFaults.end(),FaultPtrLessThan);
  // instantiate buttons on demand
  FF_DQM("FfMonitorPanel::Compile(): instantiate");
  int isup = ( ((mFaults.size()+3) /4) * 4 );
  if(isup<4) isup=4;
  int i;
  for(i=0; i<isup; i++) {
    // have coordinates
    int x=i % 4;
    int y=i / 4;
    // test for button
    if(y < mButtons.size()) 
    if(x < mButtons.at(y).size()) 
       continue;
    // construct button
    FfMonitorButton* ob = new FfMonitorButton(); 
    ob->setProperty("idx",i);
    if(!(y<mButtons.size())) mButtons.append(QList< FfMonitorButton* >()); 
    mButtons[y].append(ob);
    mGbox->addWidget(ob,y,x,1,1);
  }
  // connect buttons
  FF_DQM("FfMonitorPanel::Compile(): connect");
  for(i=0; i<isup; i++) {
    // have coordinates
    int x=i % 4;
    int y=i / 4;
    // configure button
    FfMonitorButton* ob = mButtons.at(y).at(x);
    if(i<mFaults.size()) {
      ob->setText(mFaults.at(i)->Name());
      ob->setEnabled(true);
      ob->Light(mFaults.at(i)->Value());
      connect(mFaults.at(i),SIGNAL(NotifyLight(bool)),ob,SLOT(Light(bool)));
      connect(ob,SIGNAL(pressed(void)),mFaults.at(i),SLOT(UserToggleValue(void)));
    } else {
      ob->setText("--");
      ob->setEnabled(false);
      ob->Light(false);
    }
  }
  // remove other buttons
  FF_DQM("FfMonitorPanel::Compile(): destruct");
  while(true) {
    // have coordinates
    int x=isup % 4;
    int y=isup / 4;
    // test for button
    if(y >= mButtons.size()) break; 
    if(x >= mButtons.at(y).size()) break;
    // destruct button
    FfMonitorButton* ob = mButtons.at(y).at(x);
    mGbox->removeWidget(ob);
    delete ob;
    mButtons[y].removeAt(x);
    if(mButtons.at(y).size()==0) mButtons.removeAt(y);
  }
  // refresh lauout
  mGbox->invalidate();
}


// receive button
/*
void FfMonitorPanel::SenseButton(void) {
  FfMonitorButton* ob= qobject_cast<FfMonitorButton*>( sender());
  if(!ob) return;
  QVariant idx = ob->property("idx");
  if(!idx.isValid()) return;
  int i=idx.toInt();
  bool v=ob->isDown();
  FF_DQM("FfMonitorPanel::SenseButton(): " << i << " to " << v);
  emit Switch(i,v);
}


// set indicator
void FfMonitorPanel::Light(int i, bool v) {
  FF_DQM("FfMonitorPanel::Light(): " << i << " to " << v);
  if(i<0 || i>= mButtons.size()) return;
  mButtons[i]->Light(v);
}
*/
