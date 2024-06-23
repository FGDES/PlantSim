/* fffault.h  - classes to model and a represent a fault */

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

#ifndef FAUDES_FAULT_H
#define FAUDES_FAULT_H

#include <QtGui>
#include <QtXml>
#include <QtNetwork>
#include "ffstyle.h"
#include "ffsignal.h"

// forward 
class FfComponent;

/*
************************************************
************************************************
  
FfFault

This class is used to maintain a single fault.
Instances MUST be owned by a component. Faults
must be activated to take effect. When activated, 
it refers to a pair of signals (defect/repair). A fault 
can be configured e.g. to occur spontaneously. 

************************************************
************************************************
*/

class FfFault : public QObject {

Q_OBJECT

public:
   
    //constructor for sensible defaults
    FfFault(FfComponent* parent);

public slots:
    
    // write dynamic data 
    void SetValue(bool val);
    void UserSetValue(bool val);
    void UserToggleValue(void);

    // set config
    void SetActive(bool act);
    void SetTauFail(qreal tau);
    void SetTauRepair(qreal tau);
    void SetStochastic(bool stoc);

public:

    // read dynamic data
    bool Value(void) const { return mValue; };

    // set fault config data
    void SetEditorName(const QString& nname) {mEditorName=nname;};
    void SetShortName(const QString& nname) {mShortName=nname; mName=nname;};
    void SetFirstName(const QString& nname) {mName=nname+"_"+mShortName;};

    // get fault config data
    const QString&  EditorName(void) const { return mEditorName; };
    const QString&  ShortName(void) const { return mShortName; };
    const QString&  Name(void) const { return mName; };
    bool Active(void) const { return mActive;};
    bool Stochastic(void) const { return mStochastic;};
    qreal TauFail(void) const { return mTauFail;};
    qreal TauRepair(void) const { return mTauRepair;};

    // file io
    void AppendToElement(QDomDocument& doc, QDomElement& cele);
    void ExtractFromElement(QDomElement& elem);
   
    // instantiate signals (owned by component) 
    FfSignal* NewFailureSignal(void);
    FfSignal* NewMaintenanceSignal(void);

    // dynamics
    void Reset();
    void AdvanceTime(qreal dt);

    // control widget
    QCheckBox* NewCheckBox(void);

signals:

    void NotifyValue(bool val);
    void NotifyLight(bool val);

private:
  
    // fixed ref to component the fault belongs to
    FfComponent* mComp;

    // fault configuration data
    QString mEditorName;   
    QString mShortName;   
    QString mName;   
    bool  mActive;
    qreal mTauFail;
    qreal mTauRepair;
    bool mStochastic;


    // dynamic data
    bool    mValue;   
    qreal mTimer;
    QPointer<FfSignal> pFailureSignal;
    QPointer<FfSignal> pMaintenanceSignal;
     
};






#endif
