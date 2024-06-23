/* ffoperator.h  - operator controls */

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


#ifndef FAUDES_FFOPERATOR_H
#define FAUDES_FFOPERATOR_H

#include <QtGui>
#include "ffcomponent.h"
#include "ffelements.h"


/*
************************************************
************************************************
  
FfOperatorButton class

Pushbutton for operator conrol, i.e.
click/indicate mapped to a pair of FfSignals

************************************************
************************************************
*/



class FfOperatorButton : public QPushButton  {

Q_OBJECT

public:

  // construct/destruct
  FfOperatorButton(QWidget* parent=0);
  virtual ~FfOperatorButton(void);

public slots:
  // on/off
  void Light(bool v);

protected:

  // my palettes
  QPalette mStdPalette;
  QPalette mAltPalette;

};

/*
************************************************
************************************************
  
FfOperatorPanel class

Panel of pushbuttons

************************************************
************************************************
*/



class FfOperatorPanel : public QWidget  {

Q_OBJECT

public:

  // constructor 
  FfOperatorPanel(QWidget* parent=0);

  // configure
  void SetSize(int sz);
  int Size(void) { return mSize;};

  // signal/slot interface
signals:
  void Switch(int i, bool v);
public slots:
  void Light(int i, bool v);


protected:

  // main layout
  QGridLayout* mGbox;

  // configuration
  int mSize;

  // my buttons
  QList<FfOperatorButton*> mButtons;

protected slots:

  // read button
  void SenseButton(void);

};


#endif

