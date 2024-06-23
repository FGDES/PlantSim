/* elplant.cpp  - simulate elevator plant with libfaudes interface */

#include "eleplant.h"

/*
************************************************
************************************************
  
Implementation: CabinItem

************************************************
************************************************
*/

// default constructor
CabinItem::CabinItem(void) : QGraphicsItem(0) {
  FF_DQM("CabinItem::CabinItem()");
  mOuterRect=QRectF(0,-50,100,50);
  setZValue(-20);
  SetPosition(0.5);
}

// configure
void CabinItem::SetPosition(qreal posy) {
  mYPos=posy;
  setPos(pos().x(),500*(1.0- mYPos));
  update();
}


// paint myself
void CabinItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // shape
  if(mYPos>0.01 && mYPos < 0.99) {
     painter->setPen(FfStyle::Color(VioBlack));
     painter->setBrush(FfStyle::Color(VioGrey));
  } else {
     painter->setPen(FfStyle::Color(VioDarkRed));
     painter->setBrush(FfStyle::Color(VioRed));
  }
  painter->drawRect(mOuterRect);
}




/*
************************************************
************************************************
  
Implementation: FloorItem

************************************************
************************************************
*/

// default constructor
FloorItem::FloorItem(void) : QGraphicsItem(0) {
  FF_DQM("FloorItem::FloorItem()");
  mOuterRect=QRectF(-100,-50,300,50);
  SetLevel(0.5);
  setZValue(-50);
}

// configure
void FloorItem::SetLevel(qreal posy) {
  mYPos=posy;
  setPos(pos().x(),500*(1.0- mYPos));
}


// paint myself
void FloorItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // debug shape
  painter->setPen(Qt::NoPen);
  painter->setBrush(Qt::black);
  painter->drawPath(shape());
}




/*
************************************************
************************************************
  
Implementation: DoorItem

************************************************
************************************************
*/

// default constructor
DoorItem::DoorItem(void) : QGraphicsItem(0) {
  FF_DQM("DoorItem::DoorItem()");
  SetLevel(0.5);
  setZValue(-10);
}

// configure
void DoorItem::SetLevel(qreal posy) {
  mYPos=posy;
  mOuterRect=QRectF(-10,-55,120,60);
  setPos(pos().x(),500*(1.0-mYPos));
  SetPosition(0.0);
}

// configure
void DoorItem::SetPosition(qreal posx) {
  mXPos = posx;
  update();
}

// paint myself
void DoorItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // figure color
  if(mXPos>0.01 && mXPos < 0.99) {
     painter->setPen(FfStyle::Color(VioBlack));
     painter->setBrush(FfStyle::Color(VioGrey));
  } else {
     painter->setPen(FfStyle::Color(VioDarkRed));
     painter->setBrush(FfStyle::Color(VioRed));
  }
  // door shape 1
  QRectF d1=QRectF(-10,-55,10+50*mXPos/0.9,60);
  painter->drawRect(d1);
  // door shape 2
  QRectF d2=QRectF(100-50*mXPos/0.9,-55,10+50*mXPos/0.9,60);
  painter->drawRect(d2);
}




/*
************************************************
************************************************
  
Implementation: ButtonItem

************************************************
************************************************
*/

// default constructor
ButtonItem::ButtonItem(int xpos) : QGraphicsItem(0) {
  FF_DQM("ButtonItem::ButtonItem()");
  setPos(xpos,0);
  setZValue(-15);
  SetLevel(0.5);
}

// configure
void ButtonItem::SetLevel(qreal posy) {
  mYPos=posy;
  mPressed=false;
  mIlluminated=false;
  mOuterRect=QRectF(-10,-35,20,20);
  setPos(pos().x(),500*(1.0- mYPos));
}

// activate
void ButtonItem::Illuminate(bool on) {
  if(mIlluminated==on) return;
  mIlluminated=on;
  update();
}  

// get mouse events
void ButtonItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  (void) event;
  mPressed=true;
}
void ButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  (void) event;
  mPressed=false;
}

// paint myself
void ButtonItem::paint(
  QPainter *painter, 
  const QStyleOptionGraphicsItem *option,
  QWidget *widget) {
  (void) painter;
  (void) option;
  (void) widget;
  // set pen
  if(mIlluminated) {
    painter->setPen(FfStyle::Color(VioLightGrey));
    painter->setBrush(FfStyle::Color(VioGreen));
  } else {
    painter->setPen(FfStyle::Color(VioGreen));
    painter->setBrush(FfStyle::Color(VioLightGrey));
  }
  // draw
  QRectF b=mOuterRect;
  painter->drawEllipse(b);
}




/*
************************************************
************************************************
  
Implementation: ElePlant

************************************************
************************************************
*/


// construct
ElePlant::ElePlant() : 
  QMainWindow(0)
{

  FF_DQ("ElePlant(): A");

  // set qt window attributes
  setAttribute(Qt::WA_DeleteOnClose);
  setContentsMargins(0,0,0,0);
  setWindowTitle("Elevator Simulator");

  // prepare unser interface
  CreateActions();
  CreateMenus();
  
  // have my base layout
  QWidget* cwid= new QWidget();
  setCentralWidget(cwid);
  QHBoxLayout* mHbox = new QHBoxLayout(cwid);
  mHbox->setContentsMargins(0,0,0,0);
  mHbox->setSpacing(0);
  QVBoxLayout* mVbox = new QVBoxLayout(0);
  mVbox->setContentsMargins(0,0,0,0);
  mVbox->setSpacing(0);
  mHbox->addLayout(mVbox);


  FF_DQ("ElePlant(): B");

  // actuator gui items: groupbox etc
  QGroupBox* actgbox = new QGroupBox("Actuators");
  QGridLayout* actgrid = new QGridLayout(actgbox);
  actgrid->setContentsMargins(5,5,5,5);
  actgrid->setSpacing(5);
  actgrid->setHorizontalSpacing(5);
  mVbox->addWidget(actgbox);
  QLabel* actlabel;

  // actuator gui items: cabin motor
  mCCombo=new QComboBox();
  mCCombo->addItem("Off");
  mCCombo->addItem("Up");
  mCCombo->addItem("Down");
  connect(mCCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  actlabel = new QLabel("Cabin Motor");
  actgrid->addWidget(actlabel,0,0);
  actgrid->addWidget(mCCombo,0,1);

  // actuator gui items: upper door motor
  mUDCombo=new QComboBox();
  mUDCombo->addItem("Off");
  mUDCombo->addItem("Open");
  mUDCombo->addItem("Close");
  connect(mUDCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  actlabel = new QLabel("Upper Door Motor");
  actgrid->addWidget(actlabel,1,0);
  actgrid->addWidget(mUDCombo,1,1);

  // actuator gui items: lower door motor
  mLDCombo=new QComboBox();
  mLDCombo->addItem("Off");
  mLDCombo->addItem("Open");
  mLDCombo->addItem("Close");
  connect(mLDCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  actlabel = new QLabel("Lower Door Motor");
  actgrid->addWidget(actlabel,2,0);
  actgrid->addWidget(mLDCombo,2,1);
 
  // actuator gui items: upper light
  mULCombo=new QComboBox();
  mULCombo->addItem("Off");
  mULCombo->addItem("On");
  connect(mULCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  actlabel = new QLabel("Upper Light");
  actgrid->addWidget(actlabel,3,0);
  actgrid->addWidget(mULCombo,3,1);
 
  // actuator gui items: upper light
  mLLCombo=new QComboBox();
  mLLCombo->addItem("Off");
  mLLCombo->addItem("On");
  connect(mLLCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  actlabel = new QLabel("Lower Light");
  actgrid->addWidget(actlabel,4,0);
  actgrid->addWidget(mLLCombo,4,1);

  // sensor gui items: groupbox etc
  QGroupBox* snsgbox = new QGroupBox("Sensors");
  QGridLayout* snsgrid = new QGridLayout(snsgbox);
  snsgrid->setContentsMargins(5,5,5,5);
  snsgrid->setSpacing(5);
  snsgrid->setHorizontalSpacing(5);
  mVbox->addWidget(snsgbox);
  QLabel* snslabel;

  // sensor gui items: cabin upper key switch
  mCUCombo=new QComboBox();
  mCUCombo->addItem("Released");
  mCUCombo->addItem("Activated");
  mCUCombo->setEnabled(false);
  connect(mCUCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  snslabel = new QLabel("Cabin Upper Floor");
  snsgrid->addWidget(snslabel,0,0);
  snsgrid->addWidget(mCUCombo,0,1);

  // sensor gui items: cabin lower key switch
  mCLCombo=new QComboBox();
  mCLCombo->addItem("Released");
  mCLCombo->addItem("Activated");
  mCLCombo->setEnabled(false);
  connect(mCLCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  snslabel = new QLabel("Cabin Lower Floor");
  snsgrid->addWidget(snslabel,1,0);
  snsgrid->addWidget(mCLCombo,1,1);

  // sensor gui items: upper door open switch
  mUDOCombo=new QComboBox();
  mUDOCombo->addItem("Released");
  mUDOCombo->addItem("Activated");
  mUDOCombo->setEnabled(false);
  connect(mUDOCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  snslabel = new QLabel("Upper Door Open");
  snsgrid->addWidget(snslabel,2,0);
  snsgrid->addWidget(mUDOCombo,2,1);

  // sensor gui items: upper door closed switch
  mUDCCombo=new QComboBox();
  mUDCCombo->addItem("Released");
  mUDCCombo->addItem("Activated");
  mUDCCombo->setEnabled(false);
  connect(mUDCCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  snslabel = new QLabel("Upper Door Closed");
  snsgrid->addWidget(snslabel,3,0);
  snsgrid->addWidget(mUDCCombo,3,1);

  // sensor gui items: lower door open switch
  mLDOCombo=new QComboBox();
  mLDOCombo->addItem("Released");
  mLDOCombo->addItem("Activated");
  mLDOCombo->setEnabled(false);
  connect(mLDOCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  snslabel = new QLabel("Lower Door Open");
  snsgrid->addWidget(snslabel,4,0);
  snsgrid->addWidget(mLDOCombo,4,1);

  // sensor gui items: lower door closed switch
  mLDCCombo=new QComboBox();
  mLDCCombo->addItem("Released");
  mLDCCombo->addItem("Activated");
  mLDCCombo->setEnabled(false);
  connect(mLDCCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  snslabel = new QLabel("Lower Door Closed");
  snsgrid->addWidget(snslabel,5,0);
  snsgrid->addWidget(mLDCCombo,5,1);


  // event logger
  QGroupBox* loggbox = new QGroupBox("Event Log");
  QHBoxLayout* loghbox = new QHBoxLayout(loggbox);
  loghbox->setContentsMargins(5,5,5,5);
  loghbox->setSpacing(5);
  mVbox->addWidget(loggbox);
  mLogger = new QPlainTextEdit();
  mLogger->setReadOnly(true);
  mLogger->setCenterOnScroll(true);
  mLogger->setMaximumBlockCount(1000); // style this
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  mLogger->setFont(font);
  loghbox->addWidget(mLogger);

  // strectch
  mVbox->addStretch(1);

  // controls gui items: groupbox etc
  QGroupBox* ctrgbox = new QGroupBox("Controls");
  QGridLayout* ctrgrid = new QGridLayout(ctrgbox);
  ctrgrid->setContentsMargins(5,5,5,5);
  ctrgrid->setSpacing(5);
  ctrgrid->setHorizontalSpacing(5);
  mVbox->addWidget(ctrgbox);
  QLabel* ctrlabel;

  // control gui items: door control
  mDoorControlCombo=new QComboBox();
  mDoorControlCombo->addItem("Built-In");
  mDoorControlCombo->addItem("External");
  mDoorControlCombo->setCurrentIndex(0);
  connect(mDoorControlCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  ctrlabel = new QLabel("Door Control");
  ctrgrid->addWidget(ctrlabel,0,0);
  ctrgrid->addWidget(mDoorControlCombo,0,1);

  // control gui items: upper light barrier
  mULBCombo=new QComboBox();
  mULBCombo->addItem("Free");
  mULBCombo->addItem("Blocked");
  connect(mULBCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  ctrlabel = new QLabel("Upper Light Barrier");
  ctrgrid->addWidget(ctrlabel,1,0);
  ctrgrid->addWidget(mULBCombo,1,1);

  // control gui items: lower light barrier
  mLLBCombo=new QComboBox();
  mLLBCombo->addItem("Free");
  mLLBCombo->addItem("Blocked");
  connect(mLLBCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  ctrlabel = new QLabel("Lower Light Barrier");
  ctrgrid->addWidget(ctrlabel,2,0);
  ctrgrid->addWidget(mLLBCombo,2,1);

  // general gui items: reset
  mInitButton=new QPushButton("Reset State");
  connect(mInitButton,SIGNAL(pressed()),mInitAct,SLOT(trigger()));
  ctrgrid->addWidget(mInitButton,3,0,1,2);

  FF_DQ("ElePlant(): C");

  // visual state: show a scene
  mGScene = new QGraphicsScene(this);
  mGView = new QGraphicsView();
  mGView->setRenderHints(QPainter::Antialiasing);
  mGView->setScene(mGScene);
  mGView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mGView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mGView->setMinimumWidth(300);
  mGView->setMinimumHeight(500+150);
  mHbox->addWidget(mGView);

  // visual state: show my universe
  QRectF unirect = QRectF(-100,-2*50,300,500+150);
  QGraphicsRectItem* uniitem = new QGraphicsRectItem(unirect);
  uniitem->setBrush(FfStyle::Color(VioLightGrey));
  uniitem->setZValue(-100);
  mGView->setSceneRect(unirect);
  mGScene->addItem(uniitem);
  FloorItem* loweritem = new FloorItem();
  loweritem->SetLevel(0.1);
  mGScene->addItem(loweritem);
  FloorItem* upperitem = new FloorItem();
  upperitem->SetLevel(0.9);
  mGScene->addItem(upperitem);

  // visual state: have a cabin
  mCabinItem = new CabinItem();
  mGScene->addItem(mCabinItem);

  // visual state: have doors
  mUDoorItem = new DoorItem();
  mUDoorItem->SetLevel(0.9);
  mGScene->addItem(mUDoorItem);
  mLDoorItem = new DoorItem();
  mLDoorItem->SetLevel(0.1);
  mGScene->addItem(mLDoorItem);

  // visual state: have buttons
  mCButtonItem = new ButtonItem(50);
  mCButtonItem->SetLevel(0.5);
  mGScene->addItem(mCButtonItem);
  mUButtonItem = new ButtonItem();
  mUButtonItem->SetLevel(0.9);
  mGScene->addItem(mUButtonItem);
  mLButtonItem = new ButtonItem();
  mLButtonItem->SetLevel(0.1);
  mGScene->addItem(mLButtonItem);


  FF_DQ("ElePlant(): D");

  // initialise device

  FF_WARN("Initialize Device");
  mFaudesDev= new faudes::nDevice();
  mFaudesDev->Read(FfStyle::StrFromQStr(
    QCoreApplication::applicationDirPath() + "/eleplant.dev"));
  mFaudesNetName = QHostInfo::localHostName().toLower();
  if(mFaudesNetName != "") 
    mFaudesNetName[0]= mFaudesNetName[0].toUpper();
  mFaudesNetName = "ElevatorLoop@"+mFaudesNetName;

  mFaudesDev->NetworkName(FfStyle::StrFromQStr(mFaudesNetName));
  mFaudesDev->Compile();
  mFaudesDev->Start();
  FF_WARN("Initialized Device: " << mFaudesNetName);

  // initialize simulator
  Initialize();

  // run simulator
  mParaTimeStep=10; // ms
  mParaCabinStep=0.001;
  mParaDoorStep=0.01;
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(TimeStep()));
  timer->start(mParaTimeStep);
}


// initialise: have actions
void ElePlant::CreateActions() {

  // about
  mAboutAct = new QAction(tr("About vioDiag"), this);
  mAboutAct->setStatusTip(tr("Show About-Box"));
  connect(mAboutAct, SIGNAL(triggered()), this, SLOT(About()));

  // init
  mInitAct = new QAction(tr("Init."), this);
  mInitAct->setStatusTip(tr("Reset to Initial State"));
  connect(mInitAct, SIGNAL(triggered()), this, SLOT(Initialize()));

}

// initialise: menues
void ElePlant::CreateMenus(void) {
}


// gui slots: about
void ElePlant::About() {
  
  // use qt about box
  QMessageBox aboutbox;
  aboutbox.setText(QString(
    "<p><b>ElePlant - simulate elevator plant with libFAUDES device interface</b></p>")); 
  aboutbox.setInformativeText(QString(
    "<p><b>Version:</b> %1 </p> " 
    "<p><b>Credits:</b> %2</p> " 
    "<p><b>(c) 2010 Thomas Moor</b></p> "
    ).arg(
      FfStyle::VersionText(),
      FfStyle::ContributorsText()));
    aboutbox.setWindowTitle("About ElePLant"),
    aboutbox.setIcon(QMessageBox::Information);
    aboutbox.addButton(QMessageBox::Ok);
    aboutbox.exec();
}

// gui slots: reset
void ElePlant::Initialize() {
  FF_DQM("ElePlant::Initialize()");
  // mode
  mDoorControl=DExternal;
  if(mDoorControlCombo->currentText()=="Built-In") 
    mDoorControl=DIdle;
  // state
  mUDPosition=0.9;
  mLDPosition=0.1;
  mCPosition=0.1;
  // actuators
  mUDActuator = MOff;
  mLDActuator = MOff;
  mCActuator = MOff;
  mULActuator = LOff;
  mLLActuator = LOff;
  // sensors
  mCUSensor = SRel;
  mCLSensor = SAct;
  mUDOSensor = SRel;
  mUDCSensor = SAct;
  mLDOSensor = SAct;
  mLDCSensor = SRel;
  mCBSensor = SRel;
  mUBSensor = SRel;
  mLBSensor = SRel;
  // light barrier
  mLLBCombo->setCurrentIndex(0);
  mLLBSensor = SRel;
  mULBCombo->setCurrentIndex(0);
  mULBSensor = SRel;
  // reset device
  mFaudesDev->Reset();
  // reset log
  mLogger->clear();
  Logging("INITIALISE");
  Logging(mFaudesNetName);
  // visual
  Update();
}

// update visual
void ElePlant::Update(void) {
  //FF_DQM("ElePlant::Update()");
  // cabin motor
  if(mCActuator==MOff) mCCombo->setCurrentIndex(0); 
  if(mCActuator==MInc) mCCombo->setCurrentIndex(1); 
  if(mCActuator==MDec) mCCombo->setCurrentIndex(2); 
  // cabin sensors
  if(mCUSensor==SRel) mCUCombo->setCurrentIndex(0); 
  if(mCUSensor==SAct) mCUCombo->setCurrentIndex(1); 
  if(mCLSensor==SRel) mCLCombo->setCurrentIndex(0); 
  if(mCLSensor==SAct) mCLCombo->setCurrentIndex(1); 
  // cabin representation
  mCabinItem->SetPosition(mCPosition);
  mCButtonItem->SetLevel(mCPosition);
  // upper door motor
  if(mUDActuator==MOff) mUDCombo->setCurrentIndex(0); 
  if(mUDActuator==MDec) mUDCombo->setCurrentIndex(1); 
  if(mUDActuator==MInc) mUDCombo->setCurrentIndex(2); 
  // upper door sensors
  if(mUDOSensor==SRel) mUDOCombo->setCurrentIndex(0); 
  if(mUDOSensor==SAct) mUDOCombo->setCurrentIndex(1); 
  if(mUDCSensor==SRel) mUDCCombo->setCurrentIndex(0); 
  if(mUDCSensor==SAct) mUDCCombo->setCurrentIndex(1); 
  // upper door representation
  mUDoorItem->SetPosition(mUDPosition);
  // lower door motor
  if(mLDActuator==MOff) mLDCombo->setCurrentIndex(0); 
  if(mLDActuator==MDec) mLDCombo->setCurrentIndex(1); 
  if(mLDActuator==MInc) mLDCombo->setCurrentIndex(2); 
  // upper door sensors
  if(mLDOSensor==SRel) mLDOCombo->setCurrentIndex(0); 
  if(mLDOSensor==SAct) mLDOCombo->setCurrentIndex(1); 
  if(mLDCSensor==SRel) mLDCCombo->setCurrentIndex(0); 
  if(mLDCSensor==SAct) mLDCCombo->setCurrentIndex(1); 
  // upper door representation
  mLDoorItem->SetPosition(mLDPosition);
  // light representation
  mUButtonItem->Illuminate(mULActuator==LOn);
  mLButtonItem->Illuminate(mLLActuator==LOn);
  mCButtonItem->Illuminate(false);
  if(mULActuator==LOff) mULCombo->setCurrentIndex(0); 
  if(mULActuator==LOn) mULCombo->setCurrentIndex(1); 
  if(mLLActuator==LOff) mLLCombo->setCurrentIndex(0); 
  if(mLLActuator==LOn) mLLCombo->setCurrentIndex(1); 
}

// update state
void ElePlant::Trigger(void) {
  FF_DQM("ElePlant::Trigger()");
  // cabin motor
  if(mCCombo->currentText()=="Off") mCActuator=MOff;
  if(mCCombo->currentText()=="Up") mCActuator=MInc;
  if(mCCombo->currentText()=="Down") mCActuator=MDec;
  // upper door motor
  if(mUDCombo->currentText()=="Off") mUDActuator=MOff;
  if(mUDCombo->currentText()=="Open") mUDActuator=MDec;
  if(mUDCombo->currentText()=="Close") mUDActuator=MInc;
  // lower door motor
  if(mLDCombo->currentText()=="Off") mLDActuator=MOff;
  if(mLDCombo->currentText()=="Open") mLDActuator=MDec;
  if(mLDCombo->currentText()=="Close") mLDActuator=MInc;
  // lights
  if(mLLCombo->currentText()=="Off") mLLActuator=LOff;
  if(mLLCombo->currentText()=="On")  mLLActuator=LOn;
  if(mULCombo->currentText()=="Off") mULActuator=LOff;
  if(mULCombo->currentText()=="On")  mULActuator=LOn;
  // door control
  if(mDoorControl==DExternal)
  if(mDoorControlCombo->currentText()=="Built-In") 
    Initialize();
  if(mDoorControl!=DExternal)
  if(mDoorControlCombo->currentText()=="External") 
    Initialize();
}


// run simulator
void ElePlant::TimeStep(void) {
  // FF_DQM("ElePlant::TimeStep()");
  // update actuator settings from external events
  QString actev;
  while((actev=ActuatorEvent())!="") {
    // cabin motors
    if(actev=="c_stp") mCActuator=MOff;
    if(actev=="c_up") mCActuator=MInc;
    if(actev=="c_down") mCActuator=MDec;
    // door motors
    if(actev=="ld_stp") mLDActuator=MOff;
    if(actev=="ld_open") mLDActuator=MDec;
    if(actev=="ld_close") mLDActuator=MInc;
    if(actev=="ud_stp") mUDActuator=MOff;
    if(actev=="ud_open") mUDActuator=MDec;
    if(actev=="ud_close") mUDActuator=MInc;
    // buttons
    if(actev=="o_lwlon") mLLActuator=LOn;
    if(actev=="o_lwloff") mLLActuator=LOff;
    if(actev=="o_uplon") mULActuator=LOn;
    if(actev=="o_uploff") mULActuator=LOff;
  }
  // automatic door control: open upper door
  if(mDoorControl!=DExternal) {
    if(mDoorControl==DIdle)
    if(mCActuator==MOff)
    if(mCUSensor==SAct)
    if(mUDCSensor==SAct) {
      Logging("door control: open");
      mDoorControl=UDOpen;
      mUDActuator=MDec;
    }
    if(mDoorControl==UDOpen)
    if(mUDOSensor==SAct) {
      Logging("door control: done");
      mDoorControl=DIdle;
      mUDActuator=MOff;
    }
  }
  // automatic door control: close upper door
  if(mDoorControl!=DExternal) {
    if(mDoorControl==DIdle)
    if(mCActuator!=MOff)
    if(mCUSensor==SAct)
    if(mUDOSensor==SAct) {
      Logging("door control: close");
      mDoorControl=UDClose;
      mUDActuator=MInc;
    }
    if(mDoorControl==UDClose)
    if(mUDCSensor==SAct) {
      Logging("door control: done");
      mDoorControl=DIdle;
      mUDActuator=MOff;
    }
  }
  // automatic door control: open lower door
  if(mDoorControl!=DExternal) {
    if(mDoorControl==DIdle)
    if(mCActuator==MOff)
    if(mCLSensor==SAct)
    if(mLDCSensor==SAct) {
      Logging("door control: open");
      mDoorControl=LDOpen;
      mLDActuator=MDec;
    }
    if(mDoorControl==LDOpen)
    if(mLDOSensor==SAct) {
      Logging("door control: done");
      mDoorControl=DIdle;
      mLDActuator=MOff;
    }
  }
  // automatic door control: close lower door
  if(mDoorControl!=DExternal) {
    if(mDoorControl==DIdle)
    if(mCActuator!=MOff)
    if(mCLSensor==SAct)
    if(mLDOSensor==SAct) {
      Logging("door control: close");
      mDoorControl=LDClose;
      mLDActuator=MInc;
    }
    if(mDoorControl==LDClose)
    if(mLDCSensor==SAct) {
      Logging("door control: done");
      mDoorControl=DIdle;
      mLDActuator=MOff;
    }
  }
  // simulate cabin motor
  if(mDoorControl == DExternal || mDoorControl == DIdle)
  if(mCActuator==MInc) mCPosition+= 1.0 * mParaCabinStep;
  if(mDoorControl == DExternal || mDoorControl == DIdle)
  if(mCActuator==MDec) mCPosition-= 1.0 * mParaCabinStep;
  if(mCPosition<0.0) mCPosition=0.0;
  if(mCPosition>1.0) mCPosition=1.0;
  // simulate upper door motor
  if(mUDActuator==MInc) mUDPosition+= 1.0 * mParaDoorStep;
  if(mUDActuator==MDec) mUDPosition-= 1.0 * mParaDoorStep;
  if(mUDPosition<0.0) mUDPosition=0.0;
  if(mUDPosition>1.0) mUDPosition=1.0;
  // simulate lower door motor
  if(mLDActuator==MInc) mLDPosition+= 1.0 * mParaDoorStep;
  if(mLDActuator==MDec) mLDPosition-= 1.0 * mParaDoorStep;
  if(mLDPosition<0.0) mLDPosition=0.0;
  if(mLDPosition>1.0) mLDPosition=1.0;
  // sense cabin events
  if(0.05 <= mCPosition && mCPosition <=0.1) {
    if(mCLSensor==SRel) 
    if(mDoorControl == DExternal || mDoorControl == DIdle)
       SensorEvent("c_arlw");
    mCLSensor=SAct;
  } else {
    if(mCLSensor==SAct) SensorEvent("c_lvlw");
    mCLSensor=SRel;
  }
  if(0.9 <= mCPosition && mCPosition <=0.95) {
    if(mCUSensor==SRel) 
    if(mDoorControl == DExternal || mDoorControl == DIdle)
      SensorEvent("c_arup");
    mCUSensor=SAct;
  } else {
    if(mCUSensor==SAct) SensorEvent("c_lvup");
    mCUSensor=SRel;
  }
  // sense upper door events
  if(0.05 <= mUDPosition && mUDPosition <=0.1) {
    if(mUDOSensor==SRel) SensorEvent("ud_arop");
    mUDOSensor=SAct;
  } else {
    if(mUDOSensor==SAct) SensorEvent("ud_lvop");
    mUDOSensor=SRel;
  }
  if(0.90 <= mUDPosition && mUDPosition <=0.95) {
    if(mUDCSensor==SRel) SensorEvent("ud_arcl");
    mUDCSensor=SAct;
  } else {
    if(mUDCSensor==SAct) SensorEvent("ud_lvcl");
    mUDCSensor=SRel;
  }
  // sense lower door events
  if(0.05 <= mLDPosition && mLDPosition <=0.1) {
    if(mLDOSensor==SRel) SensorEvent("ld_arop");
    mLDOSensor=SAct;
  } else {
    if(mLDOSensor==SAct) SensorEvent("ld_lvop");
    mLDOSensor=SRel;
  }
  if(0.90 <= mLDPosition && mLDPosition <=0.95) {
    if(mLDCSensor==SRel) SensorEvent("ld_arcl");
    mLDCSensor=SAct;
  } else {
    if(mLDCSensor==SAct) SensorEvent("ld_lvcl");
    mLDCSensor=SRel;
  }
  // sense buttons
  if(mCButtonItem->Pressed()) {
    if(mCBSensor==SRel) SensorEvent("o_cbb");
    mCBSensor=SAct;
  } else {
    if(mCBSensor==SAct) SensorEvent("");
    mCBSensor=SRel;
  }
  if(mLButtonItem->Pressed()) {
    if(mLBSensor==SRel) SensorEvent("o_lwb");
    mLBSensor=SAct;
  } else {
    if(mLBSensor==SAct) SensorEvent("");
    mLBSensor=SRel;
  }
  if(mUButtonItem->Pressed()) {
    if(mUBSensor==SRel) SensorEvent("o_upb");
    mUBSensor=SAct;
  } else {
    if(mUBSensor==SAct) SensorEvent("");
    mUBSensor=SRel;
  }
  // sense light barrier
  if(mLLBCombo->currentText()=="Blocked") {
    if(mLLBSensor==SRel) SensorEvent("ld_lbbl");
    mLLBSensor=SAct;
  } else {
    if(mLLBSensor==SAct) SensorEvent("ld_lbrl");
    mLLBSensor=SRel;
  }
  // sense light barrier
  if(mULBCombo->currentText()=="Blocked") {
    if(mULBSensor==SRel) SensorEvent("ud_lbbl");
    mULBSensor=SAct;
  } else {
    if(mULBSensor==SAct) SensorEvent("ud_lbrl");
    mULBSensor=SRel;
  }
  // monitor net device status
  static faudes::vDevice::DeviceState recent=faudes::vDevice::Down;
  faudes::vDevice::DeviceState current=faudes::vDevice::Down;
  current=mFaudesDev->Status();
  if(recent!=current) {
    Logging("Device: "+FfStyle::QStrFromStr(mFaudesDev->StatusString()));
  }
  recent=current;
  // update visual
  Update();
  // report
  //FF_DQM("ElePlant::TimeStep(): Cabin at " << mCPosition);
}

// doit: close the window (reimplemet qt window)
void ElePlant::closeEvent ( QCloseEvent * event ){
  // save window geometry
  QSettings settings("Faudes", "ElePlant");
  settings.setValue("geometry", saveGeometry());
  // call base
  QMainWindow::closeEvent(event);
}


// generate libfaudes event
void ElePlant::SensorEvent(QString event) {
  FF_DQM("ElePlant::SensorEvent(): " << FfStyle::StrFromQStr(event));
  faudes::Idx ev = mFaudesDev->Outputs().Index(FfStyle::StrFromQStr(event));
  if(!mFaudesDev->Outputs().Exists(ev)) return;
  Logging(event);
  mFaudesDev->WriteOutput(ev);
}

// sense libfaudes event
QString ElePlant::ActuatorEvent(void) {
  if(!mFaudesDev->InputReady()) return "";
  faudes::Idx ev = mFaudesDev->ReadInput();
  QString res= FfStyle::QStrFromStr(mFaudesDev->Inputs().SymbolicName(ev));
  FF_DQM("ElePlant::ActuatorEvent(): " << FfStyle::StrFromQStr(res));
  Logging(res);
  return res;
}


// logging
void ElePlant::Logging(QString message) {
  mLogger->textCursor().movePosition(QTextCursor::End);
  mLogger->textCursor().insertText(message);
  mLogger->textCursor().insertText("\n");
  // mLogger->appendPlainText(message);
  if(!mLogger->verticalScrollBar()->isSliderDown())
     mLogger->ensureCursorVisible();
}

/*
************************************************
************************************************
  
Implementation: main()

************************************************
************************************************
*/


int main(int argc, char *argv[]) {

  // say hello
  FF_DQ("ElePlant:main()");

  // osx 11: fix fonts (nee to be before QApplication)
  /*
#ifdef Q_OS_MACOS
  QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
  QFont::insertSubstitution(".Helvetica Neue DeskInterface", "Helvetica Neue");
  QFont::insertSubstitution(".SF NS Text", "Helvetica Neue");
#endif
  */

  // let Qt see commandline
  QApplication app(argc, argv);
 

#ifdef Q_OS_WIN32  
  // redirect libfaudes error to file
  QTemporaryFile violog(QDir::tempPath()+ QDir::separator()+ "eleplant_log_XXXXXX");
  violog.setAutoRemove(false);
  violog.open(); 
  QString viologname = violog.fileName(); 
  violog.close();
  faudes::ConsoleOut::G()->ToFile(FfStyle::StrFromQStr(viologname));
  FF_WARN("eleplant logfile");
#endif

#ifdef Q_OS_WIN32  
#ifdef FAUDES_IODEVICE_SIMPLENET
  // if we have a net device on windows, initialise winsocks
  FF_WARN("Main(): init winsock");
  WSADATA wsaData; 
  if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0) {
    FF_WARN("cannot open netdevice (network error)");  
    return 1;
  }
#endif
#endif


  // add my searchpath for plugins (obsolete?)
  QApplication::addLibraryPath(QCoreApplication::applicationDirPath() + QDir::separator()+ "plugins"); 

  // my id
  QApplication::setApplicationName("ElePlant");
  QApplication::setOrganizationName("Faudes");
  QApplication::setOrganizationDomain("faudes.org");

  // initialize style
  FfStyle::Initialise();

  // load file and go
  ElePlant *win = new ElePlant();
  QSettings settings("Faudes", "ElePlant");
  win->restoreGeometry(settings.value("geometry").toByteArray());
  win->show();
  FF_DQ("ElePlant:main(): do loop");
  return app.exec();
}
