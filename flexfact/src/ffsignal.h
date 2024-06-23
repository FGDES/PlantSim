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

#ifndef FAUDES_SIGNAL_H
#define FAUDES_SIGNAL_H

#include <QtGui>
#include <QtNetwork>
#include "ffstyle.h"

// forward 
class FfScene;
class FfWorkpiece;
class FfModbusDevice;
class FfSimplenetDevice;

/*
************************************************
************************************************
  
FfSignal

This class is used to maintain one signal
configuration, both, from the component and
scene perspective.

Basic configuration is done by the
component. Global process image address and
global name are added by the scene on
CompileImage.

************************************************
************************************************
*/

class FfSignal : public QObject {

Q_OBJECT

public:
   
    //constructor for sensible defaults
    FfSignal(QObject* parent=0);

public:

    // dynamic data (a)
    bool Value(void) const { return mValue; };

public slots:

    // dynamic data (b)
    void SetValue(bool val);
    void Reset(void) { mValue=mInitialValue; };

public:

    // set signal config data
    void SetLastName(const QString& nname) {mLastName=nname; mName=mLastName;};
    void SetRelAddress(int reladdr) {mRelAddress=reladdr;}
    void SetInitialValue(bool val) { mInitialValue=val; };
    void SetActuator(void) {mIsActuator=true; mIsSensor=false;};
    void SetSensor(void) {mIsActuator=false; mIsSensor=true;};
    void SetPositiveEdgeEvents(const QStringList& evs) { mPosEdgeEventsLastName= evs; mPosEdgeEvents=evs;};
    void SetNegativeEdgeEvents(const QStringList& evs) { mNegEdgeEventsLastName= evs; mNegEdgeEvents=evs;};
    void SetToggleEvents(const QStringList& evs)       { mToggleEventsLastName= evs;  mToggleEvents=evs;};


    // get signal config data
    bool IsActuator(void) const {return mIsActuator;};
    bool IsSensor(void) const {return mIsSensor;};
    int  RelAddress(void) const {return mRelAddress;};

    // set first name (i.e. component) and access derived config data
    void SetFirstName(const QString& nname);
    const QString&  Name(void) const { return mName; };
    const QString&  ShortName(void) const { return mLastName; };
    const QStringList& PositiveEdgeEvents(void) const { return mPosEdgeEvents;};
    const QStringList& NegativeEdgeEvents(void) const { return mNegEdgeEvents;};
    const QStringList& ToggleEvents(void)   const { return mToggleEvents;};

    // checkbox to control/inspect this signal
    QCheckBox* NewCheckBox(void);

    // more config, pragmatic public
    int     mAbsAddress;   // managed by scene
    int     mComponent;    // managed by scene


signals:

    // notify dynamics
    void NotifySensorEvent(const QString& ev);
    void NotifyValue(bool val);


private:

    // signal configuration data
    QString mLastName;     // managed by component (const)
    int     mRelAddress;   // managed by component (const)
    bool    mIsActuator;   // managed by component (const)
    bool    mIsSensor;     // managed by component (const)
    bool    mInitialValue; // managed by component (const)
    QStringList mPosEdgeEventsLastName; // managed by component (const)
    QStringList mNegEdgeEventsLastName; // managed by component (const)
    QStringList mToggleEventsLastName;  // managed by component (const)

    // component derived configuration
    QString mName;         // managed by component (updated by SetFirstName)
    QStringList mPosEdgeEvents; // managed by component (updated by SetFirstName)
    QStringList mNegEdgeEvents; // managed by component (updated by SetFirstName)
    QStringList mToggleEvents;  // managed by component (updated by SetFirstName)


    // dynamic data
    bool    mValue;        // managed by component (dynamic)
};





/*
************************************************
************************************************
  
FfProcessImage

A process image is a list of signals.
The image does not own the signals, they are
e.g. owned by the component. In the current
implementation, the signal carries some data 
that logicaly belongs to the image. Thus,
a signal may only belong to one image.

A process image can be accessed from the gui
as standard item model. It can be accessed
externally via Modbus/TCP or libFAUDES/Simplenet.

************************************************
************************************************
*/

class FfProcessImage : public QObject {

Q_OBJECT

public:

  // construct
  FfProcessImage(QObject* parent=0);
  ~FfProcessImage();

  // set up 
  void SetName(QString name) { mName=name; };
  void Clear(void);
  void Insert(FfSignal* sig);
  void Compile(void);

  // signal access
  int Size(void);
  bool Value(int i);
  void SetValue(int i, bool val);
  QString Name(int i);
  bool IsActuator(int i);
  bool IsSensor(int i);
  QCheckBox* NewCheckBox(int i);
  FfSignal* Signal(int i);
  void Reset(void);

  // get std item model (owned by image)
  QStandardItemModel* Model(void);

  // event interface (a)
  QStringList ActuatorEvents(void) { return mActuatorEvents.keys(); };
  QStringList SensorEvents(void) { return mSensorEvents.keys(); };

  // devices: simplenet (requires libFAUDES)
  void InitializeSimplenetDevice(void);
  void ExportSimplenetConfiguration(QString filename);

  // devices: simplenet local (requires libFAUDES)
  void InitializeSimplenetlocalDevice(void);
  void ExportSimplenetlocalConfiguration(QString filename);

  // devices: modbus
  void InitializeModbusDevice(void);
  void ExportModbusConfiguration(QString filename);

  // devices: operate
  void StartDevice(void);
  void StopDevice(void);
  void CloseDevice(void);
  void DeviceSynchronize(void);

  // devices: request status
  typedef enum { DNone, DDown, DStartUp, DUp, DShutDown} DState;
  const QString& DeviceStatusString(void);
  DState DeviceStatus(void);

public slots:

  // event interface (b)
  bool ExecuteActuatorEvent(const QString& actev);
  void ReportSensorEvent(const QString& actev);

signals:

  // notify dynamics
  void NotifySensorEvent(const QString& ev);
  void NotifyActuatorEvent(const QString& ev);
  void NotifySignal(int addr, bool val);

protected slots:

  // sender signal notifies value change
  void UpdateFromSignal(void);

protected:

  // image name
  QString mName;
  
  // signals as ffsignals
  QList< QPointer<FfSignal> > mSignals;

  //signals as item model
  QStandardItemModel* mModel;

  // dispatch tables
  typedef struct {
    QList<int> mPosEdgeSignals;
    QList<int> mNegEdgeSignals;
    QList<int> mToggleSignals;
  } EventConfiguration;
  QMap<QString,EventConfiguration> mActuatorEvents;
  QMap<QString,EventConfiguration>  mSensorEvents;

  // external io
  FfSimplenetDevice* mSimplenetDevice;
  FfModbusDevice* mModbusDevice;
  QString mDeviceStatusString;

};



/*
************************************************
************************************************

FfSimplenetDevice

Qt-wrapper for libFAUDES/Simplenet device


************************************************
************************************************
*/

class FfSimplenetDevice : public QObject {

Q_OBJECT


public:

  // construct/destruct
  FfSimplenetDevice(const QString& name, QObject* parent);
  ~FfSimplenetDevice();

  // configure
  void RestrictLocalhost(void);
  void InsSensorEvent(const QString& sev);
  void InsActuatorEvent(const QString& aev);
  void Compile(void);

  // control
  void Start(void);
  void Stop(void);
  FfProcessImage::DState Status();

  // sync events
  QString ReadActuatorEvent();
  void WriteSensorEvent(const QString& sensor);

protected:

  void*  mFaudesSimplenetDevice;

};


/*
************************************************
************************************************
  
FfModbusDevice

Provide image via modbus server


************************************************
************************************************
*/

class FfModbusDevice : public QTcpServer {

Q_OBJECT

  // share my data
  friend class FfModbusThread;

public:

  // construct 
  FfModbusDevice(QObject* parent);

  // faudes style interface
  void Start(void);
  void Stop(void);
  FfProcessImage::DState Status();

  // external data access
  void Synchronize(FfProcessImage* img);

protected:

  // reimplement tcpserver
  void incomingConnection(int socketDescriptor);

  // status
  FfProcessImage::DState mState;

  // count connections
  int mCount;

  // modbus image
  QVector<bool> mCoils;

  // universal mutex
  QMutex mMutex;

};

class FfModbusThread : public QThread {

Q_OBJECT

public:

  // consruct
  FfModbusThread(int socket, FfModbusDevice *device);

  // reimplement thread
  void run();

protected:

  // socket we are listening
  int mSocket;

  // device we are serving
  FfModbusDevice* mDevice;

};




#endif
