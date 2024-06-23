/* fffault.cpp  - classes to model and to represent a fault */

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

#include "fffault.h"

#ifdef FF_LIBFAUDES
#include "libfaudes.h"
#endif

// resolve forwards
#include "ffcomponent.h"


/*
************************************************
************************************************
  
Implementation: FfFault

************************************************
************************************************
*/

// default constructor
FfFault::FfFault(FfComponent* parent) : QObject(parent) { 
  // track parent
  mComp = parent;
  // configure
  mEditorName="UNDEFINED FAULT";
  mActive=false;
  mTauFail=0;
  mTauRepair=0;
  mStochastic=false;
  // dynamic data
  mValue=false;
}


// set fault 
void FfFault::SetValue(bool val) {
  if(mValue==val) return;
  mValue=val;
  if(pFailureSignal) pFailureSignal->SetValue(mValue);    
  // reset timer 
  if(mValue) mTimer = mTauRepair;
  if(!mValue) mTimer = mTauFail;
  // notify
  emit NotifyValue(mValue);
  emit NotifyLight(mValue);
}

// user set fault (mute by maintenance)
void FfFault::UserSetValue(bool val) {
  if(mValue==val) return;
  if(val && pMaintenanceSignal) 
  if(pMaintenanceSignal->Value()) {
    QCheckBox* cbox = qobject_cast<QCheckBox*>(sender());
    if(cbox) cbox->setChecked(false); 
    return;    
  }
  SetValue(val);
}

// user toggle fault (mute by maintenance)
void FfFault::UserToggleValue(void) {
  bool val = ! mValue;
  if(val && pMaintenanceSignal) 
  if(pMaintenanceSignal->Value()) 
    return;    
  SetValue(val);
}


// edit parameters
void FfFault::SetActive(bool act) {
  // bail out in trivial
  if(mActive==act) return;
  // set
  mActive = act;
  mComp->Update(); // reconfigure signals, emit parameter changed signal
};

// edit parameters
void FfFault::SetStochastic(bool stoc) {
  // bail out in trivial
  if(mStochastic==stoc) return;
  // set
  mStochastic=stoc;
  mComp->Update(); // reconfigure signals, emit parameter changed signal
};

// edit parameters
void FfFault::SetTauFail(qreal tau) {
  // bail out in trivial
  if(mTauFail==tau) return;
  // set
  mTauFail=tau;
  mComp->Update(); // reconfigure signals, emit parameter changed signal
}

// edit parameters
void FfFault::SetTauRepair(qreal tau) {
  // bail out in trivial
  if(mTauRepair==tau) return;
  // set
  mTauRepair=tau;
  mComp->Update(); // reconfigure signals, emit parameter changed signal
}

// file io
void FfFault::AppendToElement(QDomDocument& doc, QDomElement& elem) {
  // construct additional element and append
  QDomElement cele = doc.createElement("FaultConfig");
  cele.setAttribute("id",mEditorName);
  cele.setAttribute("enabled", mActive ? "yes" : "no");
  elem.appendChild(cele);
  // construct childs for parameters
  QDomElement pele = doc.createElement("Stochastic");
  pele.setAttribute("enabled", mStochastic ? "yes" : "no");
  pele.setAttribute("t_f",mTauFail);
  pele.setAttribute("t_r",mTauRepair);
  cele.appendChild(pele);

}

// file io
void FfFault::ExtractFromElement(QDomElement& elem) {
  // interpret parameters
  QDomElement cele=elem.firstChildElement("FaultConfig");
  while(cele.isElement()) {
    QString lstr = cele.attribute("id");
    if(lstr!=mEditorName) { cele=cele.nextSiblingElement("FaultConfig"); continue;}
    mActive = (cele.attribute("enabled") == "yes");
    QDomElement pele=cele.firstChildElement("Stochastic");
    mStochastic = (pele.attribute("enabled") == "yes");
    mTauFail=pele.attribute("t_f").toDouble();
    mTauRepair=pele.attribute("t_r").toDouble();
    break;
  }
}


// instantiate signals (owned by component) 
FfSignal* FfFault::NewFailureSignal(void) {
  pFailureSignal = new FfSignal(mComp); 
  pFailureSignal->SetRelAddress(mComp->ImageSize());
  pFailureSignal->SetLastName(mShortName.toUpper()+"_D");
  pFailureSignal->SetSensor();
  pFailureSignal->SetPositiveEdgeEvents(QStringList() << mShortName.toLower() + "_f");
  pFailureSignal->SetNegativeEdgeEvents(QStringList() << mShortName.toLower() + "_r");
  return pFailureSignal;
}

// instantiate signals (owned by component) 
FfSignal* FfFault::NewMaintenanceSignal(void) {
  pMaintenanceSignal = new FfSignal(mComp); 
  pMaintenanceSignal->SetRelAddress(mComp->ImageSize());
  pMaintenanceSignal->SetLastName(mShortName.toUpper()+"_M");
  pMaintenanceSignal->SetActuator();
  pMaintenanceSignal->SetPositiveEdgeEvents(QStringList() << mShortName.toLower()+ "_m");
  pMaintenanceSignal->SetNegativeEdgeEvents(QStringList() << mShortName.toLower() + "_n");
  return pMaintenanceSignal;
}


// dynamics
void FfFault::Reset() {
  mValue=0;
  mTimer=mTauFail;
  emit NotifyValue(mValue);
  emit NotifyLight(mValue);
}

// dynamics
void FfFault::AdvanceTime(qreal dt) {
  // progress timer
  mTimer-=dt;
  bool nval=mValue;
  // absense of fault
  if(!mValue) {
    // periodic occurence
    if((mTimer<=0) && mTauFail>0) nval=1;
    // ignore if ext maintenance is active
    if(pMaintenanceSignal) if(pMaintenanceSignal->Value()) nval=0;
  }
  // presence of fault
  if(mValue) {
    // stoch repair
    if((mTimer<=0) && mTauRepair>0) nval=0;
    // ext maintenance 
    if(pMaintenanceSignal) if(pMaintenanceSignal->Value()) nval=0;
  }
  // update
  if(mValue!=nval) {
    SetValue(nval);
  } 
  // bound timer
  if(mTimer<0) mTimer=0;
  // reset timer while in maintenance
  if(pMaintenanceSignal) 
  if(pMaintenanceSignal->Value()) 
  if(!mValue) 
    mTimer=mTauFail;
}

// construct checkbox
QCheckBox* FfFault::NewCheckBox(void) {
    QCheckBox* cbox = new QCheckBox(mShortName);
    cbox->setChecked(mValue);
    cbox->setProperty("ShortName",QVariant(mShortName));
    connect(cbox,SIGNAL(clicked(bool)),this,SLOT(UserSetValue(bool)));
    connect(this,SIGNAL(NotifyValue(bool)),cbox,SLOT(setChecked(bool)));
    return cbox;
}

