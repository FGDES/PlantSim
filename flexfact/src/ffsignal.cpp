/* signal.h  - struct to model a signal configuration */

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

#include "ffsignal.h"

#ifdef FF_LIBFAUDES
#include "libfaudes.h"
#endif

// resolve forwards
#include "ffcomponent.h"

#ifdef Q_OS_WIN32  
#include <windows.h>
#define sleep(sec) Sleep((sec) * 1000)
#define usleep(usec) Sleep((usec) / 1000)
#endif

// more modbus debugging
#define FF_DQNN(m) {}
//#define FF_DQNN(m) FF_DQN(m)


/*
************************************************
************************************************
  
Implementation: FfSignal

************************************************
************************************************
*/

// default constructor
FfSignal::FfSignal(QObject* parent) : QObject(parent) { 
  mRelAddress=-1;      // provoke error
  mIsActuator=false;
  mIsSensor=false;
  mInitialValue=false;
  mValue=false;
  mAbsAddress=-1;      // provoke error
  mComponent=-1;       // provoke error
}

// fix my names
void FfSignal::SetFirstName(const QString& npre) {
  // figure prefix
  QString pre=npre;
  if(pre.size()!=0) pre = pre +QString("_");
  // fix name
  mName = pre + mLastName;
  // fix event names
  pre=pre.toLower();
  mPosEdgeEvents.clear();
  for(int j=0; j<mPosEdgeEventsLastName.size(); j++) 
    mPosEdgeEvents << pre + mPosEdgeEventsLastName[j];
  mNegEdgeEvents.clear();
  for(int j=0; j<mNegEdgeEventsLastName.size(); j++) 
    mNegEdgeEvents << pre + mNegEdgeEventsLastName[j];
  mToggleEvents.clear();
  for(int j=0; j<mToggleEventsLastName.size(); j++) 
    mToggleEvents << pre + mToggleEventsLastName[j];
}


// set value
void FfSignal::SetValue(bool val) { 
  if(mValue==val) return;
  //FF_DQM("FfSignal::SetValue(): " << mName << " to " << val);
  mValue=val;
  emit NotifyValue(val);
  if(mIsSensor) {
    if(val) for(int j=0; j< mPosEdgeEvents.size(); j++)
      emit NotifySensorEvent(mPosEdgeEvents[j]);
    if(!val) for(int j=0; j< mNegEdgeEvents.size(); j++)
      emit NotifySensorEvent(mNegEdgeEvents[j]);
    for(int j=0; j< mToggleEvents.size(); j++)
      emit NotifySensorEvent(mToggleEvents[j]);
  }
}


// construct checkbox
QCheckBox* FfSignal::NewCheckBox(void) {
    QCheckBox* cbox = new QCheckBox(mName);
    cbox->setChecked(mValue);
    cbox->setEnabled(mIsActuator);
    cbox->setProperty("Address",QVariant(mAbsAddress));
    connect(cbox,SIGNAL(clicked(bool)),this,SLOT(SetValue(bool)));
    connect(this,SIGNAL(NotifyValue(bool)),cbox,SLOT(setChecked(bool)));
    return cbox;
}

// compare for sorted pointer lists
bool SignalPtrLessThan(const FfSignal* a, const FfSignal* b) {
  if(!a) return false;
  if(!b) return true;
  return a->Name() < b->Name();
};




/*
************************************************
************************************************
  
Implementation: Process image

************************************************
************************************************
*/

// construct
FfProcessImage::FfProcessImage(QObject* parent) : QObject(parent) {
  FF_DQM("FfProcessImage::FfProcessImage()");
  mSimplenetDevice=0;
  mModbusDevice=0;
  mName="Image";
  mModel = new QStandardItemModel(this);
  Clear();
}

// destruct
FfProcessImage::~FfProcessImage() {
  CloseDevice();
}


// configure: invalidate
void FfProcessImage::Clear(void) {
  foreach(FfSignal* sig, mSignals) {
    if(!sig) continue;
    disconnect(sig,0,this,0);
  }
  mSignals.clear();
  mModel->clear();
}

// configure: insert
void FfProcessImage::Insert(FfSignal* sig) {
  mSignals.append(sig);
}

// configure: compile
void FfProcessImage::Compile(void) {
  FF_DQM("FfProcessImage::CompileImage() 1");
  // sort
  qStableSort(mSignals.begin(), mSignals.end(),SignalPtrLessThan);
  // stage 1: record order an connect
  for(int i=0; i<mSignals.size(); i++) {
    mSignals[i]->mAbsAddress=i;
    connect(mSignals[i],SIGNAL(NotifyValue(bool)),this,SLOT(UpdateFromSignal(void)));
    connect(mSignals[i],SIGNAL(NotifySensorEvent(const QString&)),this,SLOT(ReportSensorEvent(const QString&)));
  }
  // stage 2: set up image item model 
  FF_DQM("FfProcessImage::CompileImage() 2");
  mModel->clear();
  mModel->setColumnCount(1);
  mModel->setHorizontalHeaderLabels(QStringList() << "signal");
  for(int i=0; i<mSignals.size(); i++) {
    QList<QStandardItem*> row;
    FfSignal* sig=mSignals[i];
    //QStandardItem* iidx= new QStandardItem(QString("#%1").arg(comp->SignalCfg(j)->mAbsAddress));
    //row.append(iidx);
    QStandardItem* isig= new QStandardItem(QString("%1 - %2").arg(sig->mAbsAddress).arg(sig->Name()));
    isig->setCheckable(true);
    isig->setEnabled(false);
    if(sig->Value())
      isig->setCheckState(Qt::Checked);
    else 
      isig->setCheckState(Qt::Unchecked);
    row.append(isig);
    mModel->appendRow(row);
  }
  // stage 3: set up dispatch table for actuator events
  FF_DQM("FfProcessImage::CompileImage() 3");
  mActuatorEvents.clear();
  for(int i=0; i<Size(); i++) {
    const FfSignal* sigcfg= mSignals[i];
    if(sigcfg->IsActuator()) {
      for(int j=0; j<sigcfg->PositiveEdgeEvents().size(); j++)
        mActuatorEvents[sigcfg->PositiveEdgeEvents()[j]].mPosEdgeSignals.append(i);
      for(int j=0; j<sigcfg->NegativeEdgeEvents().size(); j++)
        mActuatorEvents[sigcfg->NegativeEdgeEvents()[j]].mNegEdgeSignals.append(i);
      for(int j=0; j<sigcfg->ToggleEvents().size(); j++)
        mActuatorEvents[sigcfg->ToggleEvents()[j]].mToggleSignals.append(i);
    }
  }
  // stage 4: set up dispatch table for sensor events
  FF_DQM("FfProcessImage::CompileImage() 4");
  mSensorEvents.clear();
  for(int i=0; i<Size(); i++) {
    const FfSignal* sigcfg= mSignals[i];
    if(sigcfg->IsSensor()) {
      for(int j=0; j<sigcfg->PositiveEdgeEvents().size(); j++)
        mSensorEvents[sigcfg->PositiveEdgeEvents()[j]].mPosEdgeSignals.append(i);
      for(int j=0; j<sigcfg->NegativeEdgeEvents().size(); j++)
        mSensorEvents[sigcfg->NegativeEdgeEvents()[j]].mNegEdgeSignals.append(i);
      for(int j=0; j<sigcfg->ToggleEvents().size(); j++)
        mSensorEvents[sigcfg->ToggleEvents()[j]].mToggleSignals.append(i);
    }
  }
  FF_DQM("FfProcessImage::CompileImage() done");
}

// get model  
QStandardItemModel* FfProcessImage::Model(void) { return mModel;}

// size
int FfProcessImage::Size(void) {
  return mSignals.size();
}

// set valua
void FfProcessImage::SetValue(int address, bool val) {
  if(address<0) return;
  if(address>=mSignals.size()) return;
  FfSignal* sigcfg=mSignals[address];
  //FF_DQM("FfProcessImage::SetValue(): signal " << sigcfg->Name() << " on comp no " << sigcfg->mComponent);
  sigcfg->SetValue(val);
}

// signal interface: get
bool FfProcessImage::Value(int address) {
  if(address<0) return false;
  if(address>=mSignals.size()) return false;
  const FfSignal* sigcfg = mSignals[address];
  return sigcfg->Value();
}

// signal interface: get symbolic name
QString FfProcessImage::Name(int address) {
  if(address<0) return "";
  if(address>=mSignals.size()) return "";
  return mSignals[address]->Name();
}

// signal interface: type
bool FfProcessImage::IsActuator(int address) {
  if(address<0) return false;
  if(address>=mSignals.size()) return false;
  return mSignals[address]->IsActuator();
}

// signal interface: type
bool FfProcessImage::IsSensor(int address) {
  if(address<0) return false;
  if(address>=mSignals.size()) return false;
  return mSignals[address]->IsSensor();
}

// signal interface: all
FfSignal* FfProcessImage::Signal(int address) {
  if(address<0) return 0;
  if(address>=mSignals.size()) return 0;
  return mSignals[address];
}

// reset 
void FfProcessImage::Reset(void) {
  foreach(FfSignal* sig, mSignals) sig->Reset();
}

// execute actuator
bool FfProcessImage::ExecuteActuatorEvent(const QString& actev) {
  if(!mActuatorEvents.contains(actev)) return false;
  FF_DQM("FfProcessImage::ExecuteActuatorEvnet(): " << actev);
  const EventConfiguration& actcfg=mActuatorEvents[actev];
  for(int i=0; i<actcfg.mPosEdgeSignals.size(); i++) 
    SetValue(actcfg.mPosEdgeSignals[i],1);
  for(int i=0; i<actcfg.mNegEdgeSignals.size(); i++) 
    SetValue(actcfg.mNegEdgeSignals[i],0);
  for(int i=0; i<actcfg.mToggleSignals.size(); i++) 
    SetValue(actcfg.mToggleSignals[i], !Signal(actcfg.mToggleSignals[i]));
  FF_DQM("FfProcess::ExecuteActuatorEvnet(): pass on");
  emit NotifyActuatorEvent(actev);
  return true;
} 

// report sensor event
void FfProcessImage::ReportSensorEvent(const QString& sev) {
  if(!mSensorEvents.contains(sev)) return;
  FF_DQM("FfProcessImage::ReportSensorEvent(): " << sev);
  if(mSimplenetDevice) {
    mSimplenetDevice->WriteSensorEvent(sev);
  }
  FF_DQM("FfProcess::ReportSensorEvnet(): pass on to gui");
  emit NotifySensorEvent(sev);
}


// signal interface: track signal edges
void FfProcessImage::UpdateFromSignal(void) {
  //FF_DQM("FfProcessImage::UpdateSignal(): from sender " << sender());
  FfSignal* sig=qobject_cast<FfSignal*>(sender());
  if(!sig) return;
  int addr= sig->mAbsAddress;
  FF_DQM("FfProcessImage::UpdateSignal(): from signal " << sig->Name() << " #" << addr << " to " << sig->Value());
  QStandardItem* sval=mModel->item(addr,0);
  if(sval) {
    if(sig->Value()) 
      sval->setCheckState(Qt::Checked);
    else 
      sval->setCheckState(Qt::Unchecked);
  }
  FF_DQM("FfScene::UpdateSignal(): passing on ");
  emit NotifySignal(addr,sig->Value());
}

// get a checkbox
QCheckBox* FfProcessImage::NewCheckBox(int address) {
  if(address<0) return 0;
  if(address>=mSignals.size()) return 0;
  return mSignals[address]->NewCheckBox();
}


// export simplenet configuration
void FfProcessImage::ExportSimplenetConfiguration(QString filename) {
  (void) filename;
#ifdef FF_LIBFAUDES
  // figure events
  QStringList actuatorevs = ActuatorEvents();
  QStringList sensorevs =   SensorEvents();
  // set up supervisor device 
  faudes::nDevice* sdev=new faudes::nDevice();
  sdev->Name("Supervisor");
  sdev->ServerAddress("localhost:40001");
  sdev->NetworkName(FfStyle::StrFromQStr(mName));
  sdev->InsNode("Plant");
  sdev->InsNode("Supervisor");
  foreach(const QString& ev, sensorevs)
    sdev->InsInputEvent(FfStyle::StrFromQStr(ev));
  foreach(const QString& ev, actuatorevs)
    sdev->InsOutputEvent(FfStyle::StrFromQStr(ev));
  sdev->XWrite(FfStyle::LfnFromQStr(filename));
  delete sdev;  
#endif
}

// devices: simplenet
void FfProcessImage::InitializeSimplenetDevice(void) {
  // stop other devices
  CloseDevice();
#ifdef FF_LIBFAUDES
  FF_DQN("FfProcessImage::InitializeSimplenetDevice(): set up faudes device");
  // retrieve my events
  QStringList actuatorevs = ActuatorEvents();
  QStringList sensorevs =   SensorEvents();
  // set up plant device
  mSimplenetDevice = new FfSimplenetDevice(mName,this);
  foreach(const QString& sev, sensorevs)
    mSimplenetDevice->InsSensorEvent(sev);
  foreach(const QString& aev, actuatorevs)
    mSimplenetDevice->InsActuatorEvent(aev);
  mSimplenetDevice->Compile();
#endif
}


// export simplenet configuration
void FfProcessImage::ExportSimplenetlocalConfiguration(QString filename) {
  (void) filename;
#ifdef FF_LIBFAUDES
  // figure events
  QStringList actuatorevs = ActuatorEvents();
  QStringList sensorevs =   SensorEvents();
  // set up supervisor device 
  faudes::nDevice* sdev=new faudes::nDevice();
  sdev->Name("Supervisor");
  sdev->ServerAddress("localhost:40001");
  sdev->BroadcastAddress("127.0.0.1:40000");
  sdev->NetworkName(FfStyle::StrFromQStr(mName));
  sdev->InsNode("Plant");
  sdev->InsNodeAddress("Plant","localhost:40000");
  sdev->InsNode("Supervisor");
  sdev->InsNodeAddress("Supervisor","localhost:40001");
  foreach(const QString& ev, sensorevs)
    sdev->InsInputEvent(FfStyle::StrFromQStr(ev));
  foreach(const QString& ev, actuatorevs)
    sdev->InsOutputEvent(FfStyle::StrFromQStr(ev));
  sdev->XWrite(FfStyle::LfnFromQStr(filename));
  delete sdev;  
#endif
}

// devices: simplenet local
void FfProcessImage::InitializeSimplenetlocalDevice(void) {
  // stop other devices
  CloseDevice();
#ifdef FF_LIBFAUDES
  FF_DQN("FfProcessImage::InitializeSimplenetlocalDevice(): set up faudes device");
  // do simplenet config
  InitializeSimplenetDevice();
  if(!mSimplenetDevice) return;
  // make local
  mSimplenetDevice->RestrictLocalhost();
#endif
}


// export modbus configuration
void FfProcessImage::ExportModbusConfiguration(QString filename) {
  (void) filename;
#ifdef FF_LIBFAUDES
  // set up supervisor device
  faudes::mbDevice* mdev=new faudes::mbDevice();
  mdev->Name(FfStyle::StrFromQStr(mName+"Supervisor"));
  mdev->TimeScale(10); // ms/ftu
  mdev->CycleTime(1000); // 1000us 
  mdev->SlaveAddress("localhost:1502");
  mdev->AppendRemoteInputs(1,0,mSignals.size(),0);
  mdev->AppendRemoteOutputs(1,0,mSignals.size(),0);
  // actuators
  QMap<QString,EventConfiguration>::iterator eit;
  for(eit= mActuatorEvents.begin();eit!=mActuatorEvents.end(); eit++){
    std::string evstr=FfStyle::StrFromQStr(eit.key());
    mdev->InsOutputEvent(evstr);
    for(int j=0; j<eit.value().mNegEdgeSignals.size(); j++) {
      faudes::sDevice::Action act;
      act.mBit=eit.value().mNegEdgeSignals[j];
      act.mValue=faudes::AttributeSignalOutput::Clr;
      mdev->AppendAction(evstr,act);
    }
    for(int j=0; j<eit.value().mToggleSignals.size(); j++) {
      faudes::sDevice::Action act;
      act.mBit=eit.value().mToggleSignals[j];
      act.mValue=faudes::AttributeSignalOutput::Inv;
      mdev->AppendAction(evstr,act);
    }
    for(int j=0; j<eit.value().mPosEdgeSignals.size(); j++) {
      faudes::sDevice::Action act;
      act.mBit=eit.value().mPosEdgeSignals[j];
      act.mValue=faudes::AttributeSignalOutput::Set;
      mdev->AppendAction(evstr,act);
    }
  }
  // sensors
  for(eit= mSensorEvents.begin();eit!=mSensorEvents.end(); eit++){
    std::string evstr=FfStyle::StrFromQStr(eit.key());
    mdev->InsInputEvent(evstr);
    for(int j=0; j<eit.value().mNegEdgeSignals.size(); j++) {
      faudes::sDevice::Trigger trig;
      trig.mBit=eit.value().mNegEdgeSignals[j];
      trig.mNeg=true;
      mdev->AppendTrigger(evstr,trig);
    }
    for(int j=0; j<eit.value().mPosEdgeSignals.size(); j++) {
      faudes::sDevice::Trigger trig;
      trig.mBit=eit.value().mPosEdgeSignals[j];
      trig.mPos=true;
      mdev->AppendTrigger(evstr,trig);
    }
  }
  // done
  FF_DQM("FfProcessImage::CompileImage() done");
  mdev->Compile();
  mdev->XWrite(FfStyle::LfnFromQStr(filename));
  delete mdev;  
#endif
}

// devices: modbus
void FfProcessImage::InitializeModbusDevice(void) {
  // stop other devices
  CloseDevice();
  // set up plant device
  FfModbusDevice*  ndev=new FfModbusDevice(this);
  mModbusDevice = ndev;
}



// devices: start
void FfProcessImage::StartDevice(void) {
  if(mSimplenetDevice) {
    mSimplenetDevice->Start();
  }
  if(mModbusDevice) {
    mModbusDevice->Start();
  }
}

// devices: stop
void FfProcessImage::StopDevice(void) {
  if(mSimplenetDevice) {
    mSimplenetDevice->Stop();
  }
  if(mModbusDevice) {
    mModbusDevice->Stop();
  }
}

// devices: stop
void FfProcessImage::CloseDevice(void) {
  // simplenet
  if(mSimplenetDevice) {
    FF_DQN("FfProcessImage::CloseDevice(): stop/cancel simplenet device");
    mSimplenetDevice->Stop();
    while(mSimplenetDevice->Status()!=DDown) {
      sleep(1);
      FF_DQN("FfProcessImage::CloseDevice(): stop/cancel simplenet device: status: " << mModbusDevice->Status());
    }
    delete mSimplenetDevice;
    mSimplenetDevice=0;
  }
  // modbus
  if(mModbusDevice) {
    FF_DQN("FfProcessImage::CloseDevice(): stop/cancel modbus device");
    mModbusDevice->Stop();
    while(mModbusDevice->Status()!=DDown) {
      sleep(1);
      FF_DQN("FfProcessImage::CloseDevice(): stop/cancel modbus device: status: " << mModbusDevice->Status());
    }
    delete mModbusDevice;
    mModbusDevice=0;  
  }
}

// devices: status
const QString& FfProcessImage::DeviceStatusString(void) {
  // default: no device
  mDeviceStatusString="no i/o device";
  // prefix device type
  if(mSimplenetDevice){
    mDeviceStatusString = QString("network %1 is ").arg(mName);
  }
  if(mModbusDevice){
    mDeviceStatusString = QString("modbus is ");
  }
  // append actual status
  DState ds = DeviceStatus();
  switch(ds) {
    case DDown:    mDeviceStatusString+="down"; break;
    case DStartUp: mDeviceStatusString+="starting up"; break;
    case DUp:      mDeviceStatusString+="up"; break;
    case DShutDown:mDeviceStatusString+="shutting down"; break;
    default: break;
  }
  // done
  return mDeviceStatusString;
}


// devices: status
FfProcessImage::DState FfProcessImage::DeviceStatus(void) {
  DState ds=DNone;
  if(mSimplenetDevice){
    ds = mSimplenetDevice->Status();
  }
  if(mModbusDevice){
    ds = mModbusDevice->Status();
  }
  return ds;
}


// devices: sync
void FfProcessImage::DeviceSynchronize(void) {
  // for an event based devive
  if(mSimplenetDevice){
    // read actuator events and excute them
    // (sensor events are sent directly)
    QString evs = mSimplenetDevice->ReadActuatorEvent();
    if(evs!="") {
      FF_DQM("FfProcessImage::DeviceSynchronize(): sensed " << evs);
      ExecuteActuatorEvent(evs);
    }
  }
  // for a signal based device
  if(mModbusDevice){
    mModbusDevice->Synchronize(this);
  }
}

/*
************************************************
************************************************

Implementation: FfSimplenetDevice

note: this looks a bit ugly, but it's a one-to-one
wrapper to libFAUDES//SimplenetDevive

************************************************
************************************************
*/

// construct
FfSimplenetDevice::FfSimplenetDevice(const QString& name, QObject* parent) : QObject(parent)
{
  (void) name;
#ifdef FF_LIBFAUDES
  faudes::nDevice* ndev=new faudes::nDevice();
  ndev->Name("Plant");
  ndev->ServerAddress("localhost:40000");
  ndev->NetworkName(FfStyle::StrFromQStr(name));
  ndev->InsNode("Plant");
  ndev->InsNode("Supervisor");
  mFaudesSimplenetDevice = ndev;
#endif
}

// destruct
FfSimplenetDevice::~FfSimplenetDevice(void) {
#ifdef FF_LIBFAUDES
  delete static_cast<faudes::nDevice*>(mFaudesSimplenetDevice);
#endif
}

// local host option
void FfSimplenetDevice::RestrictLocalhost(void) {
  FF_DQN("FfSimplenetDevice::RestrictLocalhost");
#ifdef FF_LIBFAUDES
  static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->InsNodeAddress("Plant","localhost:40000");
  static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->InsNodeAddress("Supervisor","localhost:40001");
  static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->BroadcastAddress("localhost:40000");
  //  static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->Write();
#endif
}


// configuration interface
void FfSimplenetDevice::InsSensorEvent(const QString& sev) {
  (void) sev;
#ifdef FF_LIBFAUDES
  static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->InsOutputEvent(FfStyle::StrFromQStr(sev));
#endif
}
void FfSimplenetDevice::InsActuatorEvent(const QString& aev) {
  (void) aev;
#ifdef FF_LIBFAUDES
  static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->InsInputEvent(FfStyle::StrFromQStr(aev));
#endif
}
void FfSimplenetDevice::Compile() {
#ifdef FF_LIBFAUDES
  static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->Compile();
#endif
}


// faudes style interface
void FfSimplenetDevice::Start(void) {
#ifdef FF_LIBFAUDES
  try{
  static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->Start();
  } catch(faudes::Exception) {
    FF_WARN("FfSimplenetDevice::Start: fatal network error");
  }
#endif
}
void FfSimplenetDevice::Stop(void) {
#ifdef FF_LIBFAUDES
  static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->Stop();
#endif
}
FfProcessImage::DState FfSimplenetDevice::Status() {
  FfProcessImage::DState res= FfProcessImage::DDown;
#ifdef FF_LIBFAUDES
  faudes::vDevice::DeviceState ds = static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->Status();
  switch(ds) {
    case faudes::vDevice::Down:     res=FfProcessImage::DDown; break;
    case faudes::vDevice::StartUp:  res=FfProcessImage::DStartUp; break;
    case faudes::vDevice::Up:       res=FfProcessImage::DUp; break;
    case faudes::vDevice::ShutDown: res=FfProcessImage::DShutDown; break;
  }
#endif
  return res;
}

// networked events
QString FfSimplenetDevice::ReadActuatorEvent() {
  QString res;
#ifdef FF_LIBFAUDES
  faudes::Idx ev = static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->ReadInput();
  res = FfStyle::QStrFromStr(static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->Inputs().SymbolicName(ev));
#endif
  return res;
};
void FfSimplenetDevice::WriteSensorEvent(const QString& sev){
  (void) sev;
#ifdef FF_LIBFAUDES
  faudes::Idx ev=static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->Outputs().Index(FfStyle::StrFromQStr(sev));
  if(static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->Outputs().Exists(ev))
    static_cast<faudes::nDevice*>(mFaudesSimplenetDevice)->WriteOutput(ev);
#endif
};


/*
************************************************
************************************************
  
Implementation: FfModbusDevice

************************************************
************************************************
*/


// construct 
FfModbusDevice::FfModbusDevice(QObject* parent) : QTcpServer(parent) {
  mState=FfProcessImage::DDown;
  mCount=0;
}


// faudes style interface: status
FfProcessImage::DState FfModbusDevice::Status(void) {
  static QString res;
  mMutex.lock();
  if(mCount>0 && (mState==FfProcessImage::DStartUp))
    mState=FfProcessImage::DUp;
  if(mCount==0 && (mState==FfProcessImage::DShutDown))
    mState=FfProcessImage::DDown;
  if(mCount==0 && (mState==FfProcessImage::DUp))
    mState=FfProcessImage::DStartUp;
  mMutex.unlock();
  return mState;
}

// faudes style interface: start
void FfModbusDevice::Start(void) {
  mMutex.lock();
  if(mState == FfProcessImage::DDown) {
    if(!listen(QHostAddress::Any,1502)) {
      FF_DQN("FfModbusDevice::Start():cannot open port");
      close();
    } else {
      mState=FfProcessImage::DStartUp;
    }
  }
  mMutex.unlock();
}

// faudes style interface: start
void FfModbusDevice::Stop(void) {
  mMutex.lock();
  if(mState != FfProcessImage::DDown) {
    close();
    mState=FfProcessImage::DShutDown;
  }
  mMutex.unlock();
}

// reimplement tcpserver to start my thread
void FfModbusDevice::incomingConnection(int socket) {
  FF_DQN("FfModbusDevice::incomingConnection()");
  FfModbusThread *mth = new FfModbusThread(socket, this);
  connect(mth, SIGNAL(finished()), mth, SLOT(deleteLater()));
  mth->start();
}

// data access
void FfModbusDevice::Synchronize(FfProcessImage* img) {
  mMutex.lock();
  mCoils.resize(img->Size());
  for(int i=0; i<img->Size(); i++)  {
    if(img->IsSensor(i)) mCoils[i]= img->Value(i);
    if(img->IsActuator(i)) img->SetValue(i, mCoils.at(i));
  }
  mMutex.unlock();
}

// construct
FfModbusThread::FfModbusThread(int socket, FfModbusDevice *device) : QThread(device) {
  mDevice=device;
  mSocket=socket;
}

// modbus protocol macros
#define MB_PDU_OFFSET 7
#define MB_AP_LENGTH  4

#define MB_GETINT(p) (( ((int)request.at(p)) << 8) + request.at(p+1))
#define MB_SETINT(p,i) {request[p]= ((i) >> 8); request[p+1]= (i) & 0xff;}


// reimplement thread
void FfModbusThread::run() {
  FF_DQN("FfModbusThread::run(): start master thread on socket " << mSocket);
  // setup qt socket
  QTcpSocket qsocket;
  if(!qsocket.setSocketDescriptor(mSocket)) {
    FF_DQN("FfModbusThread::run(): failed to open socket");
    return;
  }
  // wait for connected
  if(!qsocket.waitForConnected(1000)) {
    FF_DQN("FfModbusThread::run(): failed to accept connection");
    return;
  }
  // report stats
  mDevice->mMutex.lock();
  mDevice->mCount++;
  mDevice->mMutex.unlock();
  // loop until error
  bool cancelreq=false;
  while(!cancelreq) {
    // cancel request
    mDevice->mMutex.lock();
    if(mDevice->mState==FfProcessImage::DShutDown) cancelreq=true;
    mDevice->mMutex.unlock();
    // validity
    if(!qsocket.isValid()) break;
    if(qsocket.state()!=QAbstractSocket::ConnectedState) break;
    // try to read
    FF_DQNN("FfModbusThread::run(): waiting for data");
    if(!qsocket.waitForReadyRead(3000)) continue;
    QByteArray request=qsocket.readAll();
    FF_DQNN("FfModbusThread::run(): read request #" << request.size());
    if(request.size()<MB_PDU_OFFSET+1) {
      FF_DQN("FfModbusThread::run(): invalid header"); 
      break;  
    }
    int mlength = MB_GETINT(MB_AP_LENGTH)+6;
    if(request.size()!=mlength) {
      FF_DQN("FfModbusThread::run(): incomplete request"); 
      break;  
    }
    // switch modbus function
    int fnct=request.at(MB_PDU_OFFSET);
    int errcode = 0x01;
    // read inputs or coils
    if((fnct==0x01) || (fnct==0x02)) {
      FF_DQNN("FfModbusThread::run(): coil-read or input read request");
      int addr =  MB_GETINT(MB_PDU_OFFSET+1);
      int count = MB_GETINT(MB_PDU_OFFSET+3);
      int bcount= ((count-1)/8+1);
      FF_DQNN("FfModbusThread::run(): address range: @" << addr << " #" << count);
      // test validity
      errcode=0x00;
      if(addr+count>mDevice->mCoils.size()) 
        errcode=0x02;
      // perform
      if(errcode==0x00) {    
        // fill in bits
        request.resize(MB_PDU_OFFSET+2+ bcount);
        mDevice->mMutex.lock();
        int dst=MB_PDU_OFFSET+2;
        int data=0x00;
        int shft=0x01;
        while(count) {
          if(mDevice->mCoils.at(addr)) data |= shft;
          addr++; count--; shft = shft <<1;
          if(shft==0x100) { shft=0x01; request[dst++]=data; data=0x00;}
        }
        if(shft!=0x01) { request[dst++]=data;};
        mDevice->mMutex.unlock();
        request[MB_PDU_OFFSET+1]=bcount;
        // set header length
        mlength=bcount+2;
      }
    }
    // read input registers or holding registers
    if((fnct==0x03) || (fnct==0x04)) {
      FF_DQNN("FfModbusThread::run(): register or holding register read request");
      int addr =  MB_GETINT(MB_PDU_OFFSET+1);
      int count = MB_GETINT(MB_PDU_OFFSET+3);
      FF_DQNN("FfModbusThread::run(): address range: @" << addr << " #" << count);
      // test validity
      errcode=0x00;
      if(16*addr+16*count>mDevice->mCoils.size()) 
        errcode=0x02;
      // perform
      if(errcode==0x00) {    
        // set header length
        mlength=2*count+2;
        request[MB_PDU_OFFSET+1]=2*count;
        // fill in bits
        request.resize(MB_PDU_OFFSET+2+ 2*count);
        mDevice->mMutex.lock();
        int src= addr*16;
        int dst=MB_PDU_OFFSET+2;
        for(;count>0; count--) {
          int shft=0x01;
          int lbyte=0x00;
          for(;src<mDevice->mCoils.size() && shft!=0x100; src++, shft = shft << 1)
            if(mDevice->mCoils.at(src)) lbyte |= shft;
          shft=0x01;
          int hbyte=0x00;
          for(;src<mDevice->mCoils.size() && shft!=0x100; src++, shft = shft << 1)
            if(mDevice->mCoils.at(src)) hbyte |= shft;
          request[dst++]=hbyte;
          request[dst++]=lbyte;
	}
        mDevice->mMutex.unlock();
      }
    }
    // write single coil
    if(fnct==0x05) {
      FF_DQNN("FfModbusThread::run(): write single coil request");
      int addr =  MB_GETINT(MB_PDU_OFFSET+1);
      bool val = ( ((unsigned char)request.at(MB_PDU_OFFSET+3))==0xff);
      FF_DQNN("FfModbusThread::run(): write single coil request: " << addr << " to " << val);
      // test
      errcode=0x00;
      if(addr>=mDevice->mCoils.size())
        errcode=0x02;
      // perform
      if(errcode==0x00) {    
        mDevice->mMutex.lock();
        mDevice->mCoils[addr] = val;
        mDevice->mMutex.unlock();
        // setup reply
        mlength=5;
      }
    }
    // write single holding registers
    if(fnct==0x06) {
      FF_DQNN("FfModbusThread::run(): write single holding register request");
      int addr =  MB_GETINT(MB_PDU_OFFSET+1);
      int val = MB_GETINT(MB_PDU_OFFSET+3);
      FF_DQNN("FfModbusThread::run(): set  @" << addr << " to " << val);
      // test validity
      errcode=0x00;
      if(16*addr+16 >mDevice->mCoils.size()) 
        errcode=0x02;
      // perform
      if(errcode==0x00) {    
        // extract  bits
        mDevice->mMutex.lock();
        int dst=16*addr;
        int hbyte= (val >> 8);    // :-)
        int lbyte= (val & 0xff);
        int shft;
        for(shft=0x01; shft!=0x100; shft = shft << 1, dst++)
          mDevice->mCoils[dst] = (( lbyte & shft) != 0);
        for(shft=0x01; shft!=0x100; shft = shft << 1, dst++)
          mDevice->mCoils[dst] = (( hbyte & shft) != 0);
        mDevice->mMutex.unlock();
        // setup reply
        mlength=5;
      }
    }
    // write multiple coils
    if(fnct==0x0f) {
      FF_DQNN("FfModbusThread::run(): write multiple coils request");
      int addr =  MB_GETINT(MB_PDU_OFFSET+1);
      int count = MB_GETINT(MB_PDU_OFFSET+3);
      int bcount= request.at(MB_PDU_OFFSET+5);
      FF_DQNN("FfModbusThread::run(): address range: @" << addr << " #" << count << "(" << bcount << ")");
      // test validity
      errcode=0x00;
      if(addr+count>mDevice->mCoils.size()) 
        errcode=0x02;
      if( (bcount < ((count-1)/8+1)) || (mlength < MB_PDU_OFFSET+6+bcount) )    
        errcode=0x03;
      // perform
      if(errcode==0x00) {    
        // extract  bits
        mDevice->mMutex.lock();
        int src=MB_PDU_OFFSET+6;
        int data=0;
        int shft=0x100;
        while(count) {
          if(shft==0x100) { shft=0x01; data=request.at(src++);};
          mDevice->mCoils[addr] = (( data & shft) != 0);
          addr++; count--; shft = shft <<1;    
        }
        mDevice->mMutex.unlock();
        // setup reply
        mlength=5;
      }
    }
    // write multiple holding registers
    if(fnct==0x10) {
      FF_DQNN("FfModbusThread::run(): write multiple holding registers request");
      int addr =  MB_GETINT(MB_PDU_OFFSET+1);
      int count = MB_GETINT(MB_PDU_OFFSET+3);
      int bcount= request.at(MB_PDU_OFFSET+5);
      FF_DQNN("FfModbusThread::run(): address range: @" << addr << " #" << count << "(" << bcount << ")");
      // test validity
      errcode=0x00;
      if(16*addr+16*count>mDevice->mCoils.size()) 
        errcode=0x02;
      if( bcount != 2* count) 
        errcode=0x03;
      // perform
      if(errcode==0x00) {    
        // extract  bits
        mDevice->mMutex.lock();
        int src=MB_PDU_OFFSET+6;
        int dst=16*addr;
        for(;count>0;count--) {
          int hbyte=request.at(src++);
          int lbyte=request.at(src++);
          int shft;
          for(shft=0x01; shft!=0x100; shft = shft << 1, dst++)
            mDevice->mCoils[dst] = (( lbyte & shft) != 0);
          for(shft=0x01; shft!=0x100; shft = shft << 1, dst++)
            mDevice->mCoils[dst] = (( hbyte & shft) != 0);
	}
        mDevice->mMutex.unlock();
        // setup reply
        request[MB_PDU_OFFSET+1]=bcount;
        mlength=2;
      }
    }
    // send reply
    if(errcode==0x00) {
      FF_DQNN("FfModbusThread::run(): sending reply #" << mlength);
      MB_SETINT(MB_AP_LENGTH, mlength+1);
      request.resize(MB_PDU_OFFSET+mlength);
      qsocket.write(request);
    }
    // send error
    if(errcode!=0x00) {
      FF_DQN("FfModbusThread::run(): sending error reply, code " << errcode);
      request[MB_PDU_OFFSET]  =  fnct | 0x80;
      request[MB_PDU_OFFSET+1] = errcode;
      mlength=2;
      request.resize(mlength+MB_PDU_OFFSET);
      MB_SETINT(MB_AP_LENGTH, mlength+1);
      qsocket.write(request);
    }

  }
  // disconnect
  FF_DQN("FfModbusThread::run(): disconnecting");
  qsocket.disconnectFromHost();
  qsocket.waitForDisconnected();
  // report stats
  mDevice->mMutex.lock();
  mDevice->mCount--;
  mDevice->mMutex.unlock();
  FF_DQN("FfModbusThread::run(): finishing master thread");
}



/*

example data

00 12 00 00 00 06 00 01 00 00 00 67 
00 12 00 00 00 10 00 01 0D 00 00 00 84 00 00 80 40 20 20 00 02 0A 
00 13 00 00 00 06 00 01 00 00 00 67 
00 13 00 00 00 10 00 01 0D 00 00 00 84 00 00 80 40 20 20 00 02 0A 00 14 00 00 00 06 00 01 00 00 00 67 00 14 00 00 00 10 00 01 0D 00 00 00 84 00 00 80 40 20 20 00 02 0A 00 15 00 00 00 06 00 01 00 00 00 67 00 15 00 00 00 10 00 01 0D 00 00 00 84 00 00 80 40 20 20 00 02 0A 00 16 00 00 00 06 00 01 00 00 00 67 00 16 00 00 00 10 00 01 0D 00 00 00 84 00 00 80 40 20 20 00 02 0A 

*/


