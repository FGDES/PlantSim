/* ffoperator.cpp  - operator controls */

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

#include "ffoperator.h"



/*
************************************************
************************************************
  
Implementation: FfOperatorButton

************************************************
************************************************
*/


// constructor 
FfOperatorButton::FfOperatorButton(QWidget* parent) : QPushButton(parent) {
  setCheckable(false);
  setFlat(false);
  setFocusPolicy(Qt::NoFocus); 
}

// destructor 
FfOperatorButton::~FfOperatorButton(void) {
  FF_DQM("FfOperatorButton::~FfOperatorButton()");
}

// change palette
void FfOperatorButton::Light(bool v) {
  setDefault(v);
}

/*
************************************************
************************************************
  
Implementation: FfOperatorPanel

************************************************
************************************************
*/


// constructor 
FfOperatorPanel::FfOperatorPanel(QWidget* parent) : QWidget(parent) {
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
   //" background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);"
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
   " background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffffc0, stop: 1 #ffffc0);"
   " }"
   " QPushButton:pressed {"
   " border: 2px solid #8f8f91;"
   " background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #808080, stop: 1 #808080);"
   " }");

}


// configure
void FfOperatorPanel::SetSize(int sz) {
  FF_DQM("FfOperatorPanel::SetSize()");
  // clear all gio items
  QLayoutItem *child;
  while((child = mGbox->takeAt(0)) != 0) { 
    if(child->widget()) delete child->widget();
  }
  mGbox->invalidate();
  mButtons.clear();
  // introduce buttons
  for(int i=0; i<sz; i++) {
    FfOperatorButton* ob = new FfOperatorButton(); 
    ob->setProperty("idx",i);
    ob->setText(QString("OP_x%1").arg(i+1));
    connect(ob,SIGNAL(pressed(void)),this,SLOT(SenseButton(void)));
    connect(ob,SIGNAL(released(void)),this,SLOT(SenseButton(void)));
    int x=i % 4;
    int y=i / 4;
    mGbox->addWidget(ob,y,x,1,1);
    mButtons.append(ob);
  }
  // record
  mSize = sz;
}

// receive button
void FfOperatorPanel::SenseButton(void) {
  FfOperatorButton* ob= qobject_cast<FfOperatorButton*>( sender());
  if(!ob) return;
  QVariant idx = ob->property("idx");
  if(!idx.isValid()) return;
  int i=idx.toInt();
  bool v=ob->isDown();
  FF_DQM("FfOperatorPanel::SenseButton(): " << i << " to " << v);
  emit Switch(i,v);
}


// set indicator
void FfOperatorPanel::Light(int i, bool v) {
  FF_DQM("FfOperatorPanel::Light(): " << i << " to " << v);
  if(i<0 || i>= mButtons.size()) return;
  mButtons[i]->Light(v);
}
