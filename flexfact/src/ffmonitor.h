/* ffmonitor.h  - fault monitor */

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


#ifndef FAUDES_FFMONITOR_H
#define FAUDES_FFMONITOR_H

#include <QtGui>
#include "ffcomponent.h"
#include "ffelements.h"


/*
************************************************
************************************************
  
FfMonitorButton class

Pushbutton for fault monitor, i.e.
click/indicate mapped to an FfFault

************************************************
************************************************
*/



class FfMonitorButton : public QPushButton  {

Q_OBJECT

public:

  // construct/destruct
  FfMonitorButton(QWidget* parent=0);
  virtual ~FfMonitorButton(void);

public slots:
  // on/off
  void Light(bool v);

signals:

  // clicked
  void Down(bool v);
  
protected:

  // my palettes
  QPalette mStdPalette;
  QPalette mAltPalette;

};

/*
************************************************
************************************************
  
FfMonitorPanel class

Panel of pushbuttons

************************************************
************************************************
*/



class FfMonitorPanel : public QWidget  {

Q_OBJECT

public:

  // constructor 
  FfMonitorPanel(QWidget* parent=0);

  // configure
  void Clear(void);
  void Insert(FfFault* faultcfg);
  void Compile(void);

  // signal/slot interface
  //signals:
  //void Down(int i, bool v);
  //public slots:
  //void Light(int i, bool v);


protected:

  // main layout
  QGridLayout* mGbox;

  // configuration
  QList< QPointer< FfFault > > mFaults;

  // my buttons
  QList< QList< FfMonitorButton* > > mButtons;

};


#endif
// KfdFBwn40  169.254.1.1
