/* eleplant.h  - elevator plant simulator */


#ifndef FAUDES_ELEPLANT_H
#define FAUDES_ELEPLANT_H

#include <QtGui>
#include <QtWidgets>
#include <QtWidgets/QApplication>
#include <QtNetwork>
#include "libfaudes.h"
#include "ffstyle.h"




/*
************************************************
************************************************
  
Cabin graphics item

************************************************
************************************************
*/


class CabinItem : public QGraphicsItem {

public:

  //constructor 
  CabinItem(void);

  // set my position
  void SetPosition(qreal pos);

  // bounding rect and shape
  virtual QRectF boundingRect() const { return mOuterRect; };

  // paint myself
  void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);

  // rtti
  enum { Type = UserType+1};
  virtual int type(void) const { return Type;};

 protected:

  // shape 
  QRectF mOuterRect;

  // configuration
  qreal mYPos;


private:


};

/*
************************************************
************************************************
  
Floor graphics item

************************************************
************************************************
*/


class FloorItem : public QGraphicsItem {

public:

  //constructor 
  FloorItem(void);

  // set my position
  void SetLevel(qreal pos);

  // bounding rect and shape
  virtual QRectF boundingRect() const { return mOuterRect; };

  // paint myself
  void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);

  // rtti
  enum { Type = UserType+2};
  virtual int type(void) const { return Type;};

 protected:

  // shape 
  QRectF mOuterRect;

  // configuration
  qreal mYPos;


private:


};

/*
************************************************
************************************************
  
Door graphics item

************************************************
************************************************
*/


class DoorItem : public QGraphicsItem {

public:

  //constructor 
  DoorItem(void);

  // set my position
  void SetLevel(qreal pos);
  void SetPosition(qreal pos);

  // bounding rect and shape
  virtual QRectF boundingRect() const { return mOuterRect; };

  // paint myself
  void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);

  // rtti
  enum { Type = UserType+3};
  virtual int type(void) const { return Type;};

 protected:

  // shape 
  QRectF mOuterRect;

  // configuration
  qreal mYPos;
  qreal mXPos;


private:


};


/*
************************************************
************************************************
  
Button graphics item

************************************************
************************************************
*/


class ButtonItem : public QGraphicsItem {

public:

  //constructor 
  ButtonItem(int xpos=150);

  // set my position
  void SetLevel(qreal pos);

  // ask/set my state
  bool Pressed(void) { return mPressed; };
  void Illuminate(bool on);

  // bounding rect and shape
  virtual QRectF boundingRect() const { return mOuterRect; };

  // paint myself
  void paint(QPainter*, const QStyleOptionGraphicsItem* ,QWidget*);

  // rtti
  enum { Type = UserType+4};
  virtual int type(void) const { return Type;};

 protected:

  // get mouse events
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

  // shape 
  QRectF mOuterRect;

  // configuration
  qreal mYPos;
  bool mPressed;
  bool mIlluminated;


private:


};

/*
************************************************
************************************************
  
Elevator main window

************************************************
************************************************
*/

class ElePlant : public QMainWindow {

 Q_OBJECT

public:

  // constructor
  ElePlant(void);


public slots:

  // gui slots
  void About(void);
  void Initialize(void);

  // update visual from state
  void Update();

  // update state from visual
  void Trigger();

  // run simulator
  void TimeStep();

  // logger
  void Logging(QString message);

protected:

  // reimplement close event handler
  void closeEvent(QCloseEvent * event);

  // generate/sense libfaudes events
  QString ActuatorEvent(void);
  void SensorEvent(QString event);

private slots:

private:

  // initialise
  void CreateActions(void);
  void CreateMenus(void);


  // my actions
  QAction *mAboutAct;
  QAction *mInitAct;

  // my layout
  QHBoxLayout* mHbox;
  QHBoxLayout* mVbox;

  // my layout items
  QPushButton* mInitButton;
  QComboBox* mCCombo;
  QComboBox* mUDCombo;
  QComboBox* mLDCombo;
  QComboBox* mULCombo;
  QComboBox* mLLCombo;
  QComboBox* mCUCombo;
  QComboBox* mCLCombo;
  QComboBox* mUDOCombo;
  QComboBox* mUDCCombo;
  QComboBox* mLDOCombo;
  QComboBox* mLDCCombo;
  QComboBox* mLLBCombo;
  QComboBox* mULBCombo;
  QComboBox* mDoorControlCombo;
  QPlainTextEdit* mLogger;  

  // simulator mode/discrete state
  typedef enum {DExternal,DIdle,UDOpen,UDClose,LDOpen,LDClose} DMode;
  DMode mDoorControl; 

  // simulator continuous state
  qreal mUDPosition;
  qreal mLDPosition;
  qreal mCPosition;

  // simulator actuators
  typedef enum {MInc,MDec,MOff} Motor;
  Motor mUDActuator;
  Motor mLDActuator;
  Motor mCActuator;
  typedef enum {LOn,LOff} Light;
  Light mULActuator;
  Light mLLActuator;

  // simulator sensors
  typedef enum {SAct, SRel} Sensor;
  Sensor mCUSensor;
  Sensor mCLSensor;
  Sensor mUDOSensor;
  Sensor mUDCSensor;
  Sensor mLDOSensor;
  Sensor mLDCSensor;
  Sensor mCBSensor;
  Sensor mUBSensor;
  Sensor mLBSensor;
  Sensor mULBSensor;
  Sensor mLLBSensor;

  // simulator parameters
  int mParaTimeStep;
  qreal mParaCabinStep;
  qreal mParaDoorStep;

  // simulator visual
  QGraphicsScene* mGScene;
  QGraphicsView* mGView;
  CabinItem* mCabinItem;
  DoorItem* mUDoorItem;
  DoorItem* mLDoorItem;
  ButtonItem* mUButtonItem;
  ButtonItem* mLButtonItem;
  ButtonItem* mCButtonItem;
   
  // faudes devive
  faudes::nDevice* mFaudesDev;
  QString mFaudesNetName;

};

#endif
