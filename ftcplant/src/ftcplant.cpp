/* ftcplant.cpp  - simulate manufacturing system with libfaudes interface */

#include "ftcplant.h"
#include <QSvgGenerator>

/*
************************************************
************************************************
  
Implementation: FtcPlant

************************************************
************************************************
*/


// construct
FtcPlant::FtcPlant() : 
  QMainWindow(0),
  mGView(0)
{

  FF_DQ("FtcPlant(): A");

  // hide
  hide();

  // set qt window attributes
  setAttribute(Qt::WA_DeleteOnClose);
  setContentsMargins(0,0,0,0);
  setWindowTitle("Bottling Plant Simulation");

  // prepare unser interface
  CreateActions();
  CreateMenus();
  FF_DQ("FtcPlant(): B");

  // have my base layout
  QWidget* cwid= new QWidget();
  setCentralWidget(cwid);
  QHBoxLayout* mHbox = new QHBoxLayout(cwid);
  mHbox->setMargin(0);
  mHbox->setSpacing(0);

  FF_DQ("FtcPlant(): C");

  // visual state: show a scene
  mGScene = new QGraphicsScene(this);
  mGView = new QGraphicsView();
  mGView->setRenderHints(QPainter::Antialiasing);
  mGView->setScene(mGScene);
  mGView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mGView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mGView->setMinimumWidth(500);
  mGView->setMinimumHeight(300);
  mGView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  mHbox->addWidget(mGView);

  // visual state: show my universe
  QRectF unirect = QRectF(-500,-300,1000,600);
  mGView->setSceneRect(unirect);
  mGScene->setBackgroundBrush(FfStyle::Color(VioOldLace));

  // visual state: have conveyor belt and distributor
  mConveyorItem = new ConveyorItem();
  mDistributorItem = new DistributorItem();
  mStopperItem = new StopperItem();
  mProcessItem = new ProcessItem();
  mGScene->addItem(mConveyorItem);
  mGScene->addItem(mDistributorItem);
  mGScene->addItem(mStopperItem);
  mGScene->addItem(mProcessItem);

  // connect visual state signals
  connect(mStopperItem,SIGNAL(OperatorEvent(QString)),this,SLOT(ScheduleEvent(QString)));
  connect(mProcessItem,SIGNAL(OperatorEvent(QString)),this,SLOT(ScheduleEvent(QString)));
  connect(mDistributorItem,SIGNAL(OperatorEvent(QString)),this,SLOT(ScheduleEvent(QString)));



  FF_DQ("FtcPlant(): B1");

  // have my vbox (lives in a widget, so we can set the size policy)
  QWidget* vwid = new QWidget();
  mHbox->addWidget(vwid);
  vwid->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
  QVBoxLayout* mVbox = new QVBoxLayout(vwid);
  mVbox->setMargin(0);
  mVbox->setSpacing(0);

  // controls gui items: groupbox etc (debugging/development only)
  FF_DQ("FtcPlant(): B5");
  QGroupBox* ctrgbox = new QGroupBox("Controls");
  ctrgbox->hide(); // hide  (show for development/debugging)
  QGridLayout* ctrgrid = new QGridLayout(ctrgbox);
  ctrgrid->setMargin(5);
  ctrgrid->setSpacing(5);
  ctrgrid->setHorizontalSpacing(5);
  mVbox->addWidget(ctrgbox);


  // control gui items: stretch
  QVBoxLayout* ctrlsvbox = new QVBoxLayout();
  ctrlsvbox->setMargin(0);
  ctrlsvbox->addStretch(10);
  ctrgrid->addLayout(ctrlsvbox,4,0,1,2);
  
  // control gui items: proc a/b
  mProcAButton=new QPushButton("Process [A]");
  connect(mProcAButton,SIGNAL(pressed()),this,SLOT(StartProcessA()));
  ctrgrid->addWidget(mProcAButton,5,0,1,2);
  mProcBButton=new QPushButton("Process [B]");
  connect(mProcBButton,SIGNAL(pressed()),this,SLOT(StartProcessB()));
  ctrgrid->addWidget(mProcBButton,6,0,1,2);

  // control gui items: step
  mDStep1Button=new QPushButton("Rotate 1/12 [R]");
  connect(mDStep1Button,SIGNAL(pressed()),this,SLOT(DStep1()));
  ctrgrid->addWidget(mDStep1Button,7,0,1,2);
  mDStep3Button=new QPushButton("Rotate 1/4 [RR]");
  connect(mDStep3Button,SIGNAL(pressed()),this,SLOT(DStep3()));
  ctrgrid->addWidget(mDStep3Button,8,0,1,2);

  // control gui items: flip stopper
  mStopperButton=new QPushButton("Operate Stopper [V]");
  connect(mStopperButton,SIGNAL(pressed()),this,SLOT(StartSmartStopper()));
  ctrgrid->addWidget(mStopperButton,9,0,1,2);


  // actuator gui items: groupbox etc (debugging/development only)
  FF_DQ("FtcPlant(): B2");
  QGroupBox* actgbox = new QGroupBox("Actuators");
  actgbox->hide(); // hide
  QGridLayout* actgrid = new QGridLayout(actgbox);
  actgrid->setMargin(5);
  actgrid->setSpacing(5);
  actgrid->setHorizontalSpacing(5);
  mVbox->addWidget(actgbox);
  QLabel* actlabel;

  // actuator gui items: conveyor motor
  mCBCombo=new QComboBox();
  mCBCombo->addItem("Off");
  mCBCombo->addItem("On");
  connect(mCBCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  actlabel = new QLabel("Conveyor Belt");
  actgrid->addWidget(actlabel,0,0);
  actgrid->addWidget(mCBCombo,0,1);

  // actuator gui items: distributor
  mDCombo=new QComboBox();
  mDCombo->addItem("Off");
  mDCombo->addItem("On");
  connect(mDCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  actlabel = new QLabel("Roundabout");
  actgrid->addWidget(actlabel,1,0);
  actgrid->addWidget(mDCombo,1,1);
  FF_DQ("FtcPlant(): B2b");

  // actuator gui items: left stopper
  mSMLCombo=new QComboBox();
  mSMLCombo->addItem("Off");
  mSMLCombo->addItem("Open");
  mSMLCombo->addItem("Close");
  connect(mSMLCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  actlabel = new QLabel("Left Stopper");
  actgrid->addWidget(actlabel,2,0);
  actgrid->addWidget(mSMLCombo,2,1);


  // actuator gui items: right stopper
  mSMRCombo=new QComboBox();
  mSMRCombo->addItem("Off");
  mSMRCombo->addItem("Open");
  mSMRCombo->addItem("Close");
  connect(mSMRCombo,SIGNAL(activated(int)),this,SLOT(Trigger()));
  actlabel = new QLabel("Right Stopper");
  actgrid->addWidget(actlabel,3,0);
  actgrid->addWidget(mSMRCombo,3,1);


  // process logger
  FF_DQ("FtcPlant(): B3a");
  QGroupBox* ploggbox = new QGroupBox("Process Log");
  QHBoxLayout* ploghbox = new QHBoxLayout(ploggbox);
  ploghbox->setMargin(5);
  ploghbox->setSpacing(5);
  mVbox->addWidget(ploggbox);
  mProcessLogger=new QPlainTextEdit();
  mProcessLogger->setReadOnly(true);
  mProcessLogger->setPlainText("--");
  mProcessLogger->setCenterOnScroll(true);
  mProcessLogger->setMaximumBlockCount(1000); // style this
  QFont pfont;
  pfont.setFamily("Courier");
  pfont.setFixedPitch(true);
  mProcessLogger->setFont(pfont);
  mProcLogReq = new QTextCursor(mProcessLogger->document());
  mProcLogEntry = new QTextCursor(mProcessLogger->document());
  mProcLogProc = new QTextCursor(mProcessLogger->document());
  mProcLogExit = new QTextCursor(mProcessLogger->document());
  mProcLogAck = new QTextCursor(mProcessLogger->document());
  ploghbox->addWidget(mProcessLogger);


  // event logger
  FF_DQ("FtcPlant(): B4");
  QGroupBox* loggbox = new QGroupBox("Event Log");
  QHBoxLayout* loghbox = new QHBoxLayout(loggbox);
  loghbox->setMargin(5);
  loghbox->setSpacing(5);
  mVbox->addWidget(loggbox);
  mEventLogger = new QPlainTextEdit();
  mEventLogger->setReadOnly(true);
  mEventLogger->setCenterOnScroll(true);
  mEventLogger->setMaximumBlockCount(1000); // style this
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  mEventLogger->setFont(font);
  loghbox->addWidget(mEventLogger);

  // strectch
  //mVbox->addStretch(1);

  // operator gui items: groupbox etc
  FF_DQ("FtcPlant(): B4b");
  QGroupBox* opgbox = new QGroupBox("Operator");
  QGridLayout* opgrid = new QGridLayout(opgbox);
  opgrid->setMargin(5);
  opgrid->setSpacing(5);
  opgrid->setHorizontalSpacing(5);
  mVbox->addWidget(opgbox);

  // operator gui items: place request
  mRequest1Button=new QPushButton("Request AB [P1]");
  connect(mRequest1Button,SIGNAL(pressed()),this,SLOT(PlaceRequest1()));
  opgrid->addWidget(mRequest1Button,0,0,1,2);
  mRequest2Button=new QPushButton("Request AA [P2]");
  connect(mRequest2Button,SIGNAL(pressed()),this,SLOT(PlaceRequest2()));
  opgrid->addWidget(mRequest2Button,1,0,1,2);
  mRequest3Button=new QPushButton("Request BB [P3]");
  connect(mRequest3Button,SIGNAL(pressed()),this,SLOT(PlaceRequest3()));
  opgrid->addWidget(mRequest3Button,2,0,1,2);

  // operator gui items: break b
  mBreakBButton=new QPushButton("Break Proc. B");
  connect(mBreakBButton,SIGNAL(pressed()),this,SLOT(BreakB()));
  opgrid->addWidget(mBreakBButton,3,0,1,2);

  // operator gui items: insert wp
  mWPInsertButton=new QPushButton("Insert Container");
  connect(mWPInsertButton,SIGNAL(pressed()),this,SLOT(RequestWPInsert()));
  opgrid->addWidget(mWPInsertButton,4,0,1,2);

  // operator gui items: stretch
  //QVBoxLayout* opsvbox = new QVBoxLayout();
  //opsvbox->setMargin(0);
  //opsvbox->addStretch(10);
  //opgrid->addLayout(opsvbox,4,0,1,2);

  // operator gui items: reset
  mInitButton=new QPushButton("Reset State");
  connect(mInitButton,SIGNAL(pressed()),mInitAct,SLOT(trigger()));
  opgrid->addWidget(mInitButton,5,0,1,2);

  // operator gui items: pause
  mPauseButton=new QPushButton("Pause");
  connect(mPauseButton,SIGNAL(pressed()),this,SLOT(Trigger()));
  opgrid->addWidget(mPauseButton,6,0,1,2);


  // initialise device
  FF_WARN("Initialize Device");
  FF_DQ("FtcPlant(): D");
  mFaudesDev=faudes::vDevice::FromFile(FfStyle::StrFromQStr(
    QCoreApplication::applicationDirPath() + "/ftcplant.dev"));
  mFaudesDev->Start();

  // initialize simulator
  Initialize();

  // run simulator
  mPause=false;
  mParaTimeStep=50; // ms
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(TimeStep()));
  timer->start(mParaTimeStep);

}


// initialise: have actions
void FtcPlant::CreateActions() {

  // about (not used)
  mAboutAct = new QAction(tr("About vioDiag"), this);
  mAboutAct->setStatusTip(tr("Show About-Box"));
  connect(mAboutAct, SIGNAL(triggered()), this, SLOT(About()));

  // init
  mInitAct = new QAction(tr("Init."), this);
  mInitAct->setStatusTip(tr("Reset to Initial State"));
  mInitAct->setShortcut(tr("Ctrl+I"));
  connect(mInitAct, SIGNAL(triggered()), this, SLOT(Initialize()));

  // shot
  mShotAct = new QAction(tr("Screen Shot"), this);
  mShotAct->setStatusTip(tr("Screen Schot"));
  mShotAct->setShortcut(tr("Ctrl+P"));
  this->addAction(mShotAct);
  mShotAct->setEnabled(true);
  connect(mShotAct, SIGNAL(triggered()), this, SLOT(ScreenShot()));

}

// initialise: menues
void FtcPlant::CreateMenus(void) {
}


// gui slots: about (not used)
void FtcPlant::About() {
  
  // use qt about box
  QMessageBox aboutbox;
  aboutbox.setText(QString(
    "<p><b>FtcPlant - simulate production line with libFAUDES device interface</b></p>")); 
  aboutbox.setInformativeText(QString(
    "<p><b>Version:</b> %1</p> " 
    "<p><b>Credits:</b> %2</p> " 
    "<p><b>(c) 2012 Thomas Moor</b></p> "
    ).arg(
      FfStyle::VersionText(),
      FfStyle::ContributorsText()));
    aboutbox.setWindowTitle("About FtcPlant"),
    aboutbox.setIcon(QMessageBox::Information);
    aboutbox.addButton(QMessageBox::Ok);
    aboutbox.exec();
}

// gui slots: reset
void FtcPlant::Initialize() {
  FF_DQM("FtcPlant::Initialize()");
  // mode
  /*
  mSLControl=DManual;
  mSRControl=DManual;
  mPRControl=DReliable;
  mCBControl=DSynchronous;
  */
  // conveyor belt state
  mConveyorItem->Motor(ConveyorItem::MLeft);
  mConveyorItem->SensorS1(false);
  mConveyorItem->SensorS2(false);
  mConveyorItem->SensorS3(false);
  // distributor state
  mDistributorItem->Motor(DistributorItem::MOff);
  mDistributorItem->SetOrientation(0);
  mSmartDistributorStage=0;
  // clear work pieces
  foreach(WorkPieceItem* wpi,mWorkPieceList) {
    mGScene->removeItem(wpi);
    delete wpi;
  }
  mWorkPieceList.clear();
  mProcessWorkPiece=NULL;
  // process
  mProcessItem->WorkPiece(NULL);
  mProcessItem->Machine(ProcessItem::MOff);
  mProcessItem->DefectB(false);
  // stopper
  mStopperItem->SetPositionL(25); 
  mStopperItem->SetPositionR(0); 
  mStopperItem->MotorL(StopperItem::MOff); 
  mStopperItem->MotorR(StopperItem::MOff); 
  mSmartStopperStage=0;
  // wp insert
  mSmartWPInsertStage=0;
  // reset device
  mFaudesDev->Reset();
  // reset event log
  mEventLogger->clear();
  Logging("INITIALISE");
  // reset process log
  mProcessLogger->clear();
  mProcessLogger->setPlainText("__-__-__");
  mProcLogReq->movePosition(QTextCursor::Start);
  mProcLogEntry->movePosition(QTextCursor::Start);
  mProcLogEntry->movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,2);
  mProcLogProc->movePosition(QTextCursor::Start);
  mProcLogProc->movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,3);
  mProcLogExit->movePosition(QTextCursor::Start);
  mProcLogExit->movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,5);
  mProcLogAck->movePosition(QTextCursor::Start);
  mProcLogAck->movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,6);
  // enable breakb
  mBreakBButton->setEnabled(true);
  // visual
  Update();
  // get controls
  Trigger();
}

// update visual
void FtcPlant::Update(void) {
  // FF_DQM("FtcPlant::Update()");
  // pause
  if(mPause)  mPauseButton->setText("Resume");
  if(!mPause) mPauseButton->setText("Pause");
  // conveyor a
  if(mConveyorItem->Motor()==ConveyorItem::MOff) mCBCombo->setCurrentIndex(0); 
  if(mConveyorItem->Motor()==ConveyorItem::MLeft) mCBCombo->setCurrentIndex(1); 
  // distributor
  if(mDistributorItem->Motor()==DistributorItem::MOff) mDCombo->setCurrentIndex(0); 
  if(mDistributorItem->Motor()==DistributorItem::MRight) mDCombo->setCurrentIndex(1); 
  // stopper
  if(mStopperItem->MotorL()==StopperItem::MOff) mSMLCombo->setCurrentIndex(0); 
  if(mStopperItem->MotorL()==StopperItem::MOpen) mSMLCombo->setCurrentIndex(1); 
  if(mStopperItem->MotorL()==StopperItem::MClose) mSMLCombo->setCurrentIndex(2); 
  if(mStopperItem->MotorR()==StopperItem::MOff) mSMRCombo->setCurrentIndex(0); 
  if(mStopperItem->MotorR()==StopperItem::MOpen) mSMRCombo->setCurrentIndex(1); 
  if(mStopperItem->MotorR()==StopperItem::MClose) mSMRCombo->setCurrentIndex(2); 
}

// update state
void FtcPlant::Trigger(void) {
  FF_DQM("FtcPlant::Trigger()");
  // pause
  QObject* sb = qobject_cast<QPushButton*>(sender());
  if(sb==mPauseButton) {
    mPause = (mPauseButton->text()=="Pause");
    if(mPause)  mPauseButton->setText("Resume");
    if(!mPause) mPauseButton->setText("Pause");
  }
  // conveyor 
  if(mCBCombo->currentText()=="Off")   mConveyorItem->Motor(ConveyorItem::MOff);
  if(mCBCombo->currentText()=="On")  mConveyorItem->Motor(ConveyorItem::MLeft);
  // distributor
  if(mDCombo->currentText()=="Off")   mDistributorItem->Motor(DistributorItem::MOff);
  if(mDCombo->currentText()=="On")   mDistributorItem->Motor(DistributorItem::MRight);
  // stopper
  if(mSMLCombo->currentText()=="Off")     mStopperItem->MotorL(StopperItem::MOff);
  if(mSMLCombo->currentText()=="Open")   mStopperItem->MotorL(StopperItem::MOpen);
  if(mSMLCombo->currentText()=="Close")   mStopperItem->MotorL(StopperItem::MClose);
  if(mSMRCombo->currentText()=="Off")     mStopperItem->MotorR(StopperItem::MOff);
  if(mSMRCombo->currentText()=="Open")   mStopperItem->MotorR(StopperItem::MOpen);
  if(mSMRCombo->currentText()=="Close")   mStopperItem->MotorR(StopperItem::MClose);
  // controls
  /*
  if(mSLControlCombo->currentText()=="Automatic")  mSLControl=DAutomatic;
  if(mSLControlCombo->currentText()=="Manual")  mSLControl=DManual;
  if(mSRControlCombo->currentText()=="Automatic")  mSRControl=DAutomatic;
  if(mSRControlCombo->currentText()=="Manual")  mSRControl=DManual;
  if(mPRControlCombo->currentText()=="Reliable")  mPRControl=DReliable;
  if(mPRControlCombo->currentText()=="Unreliable")  mPRControl=DUnreliable;
  if(mCBControlCombo->currentText()=="Synchronous")  mCBControl=DSynchronous;
  if(mCBControlCombo->currentText()=="Variable")  mCBControl=DVariable;
  */
}

// simulator state
void FtcPlant::RequestWPInsert(void) {
  mSmartWPInsertStage++;
}

// query simulator state
bool FtcPlant::CanWPInsert(void) {
  if(mWorkPieceList.empty()) return true;
  WorkPieceItem* wpi=mWorkPieceList.last();
  if(wpi->X()<=460 ) return true; 
  return false;
}


// set simulator state
void FtcPlant::WPInsert(void) {
  FF_DQM("FtcPlant::WPInsert()");
  if(!CanWPInsert()) return;
  WorkPieceItem* wpi= new WorkPieceItem();
  wpi->SetPosition(535,0,0);
  mGScene->addItem(wpi);
  mWorkPieceList.append(wpi);
}

// set simulator state
void FtcPlant::WPRemove(void) {
  if(mWorkPieceList.empty()) return;
  WorkPieceItem* wpi=mWorkPieceList.first();
  if(wpi->x()>= - 550) return; 
  FF_DQM("FtcPlant::WPRemove(): #" << mWorkPieceList.size());
  mGScene->removeItem(wpi);
  mWorkPieceList.removeFirst();
  delete wpi;
}

// set simulator state
void FtcPlant::BreakB(void) {
  mBreakBButton->setEnabled(false);
  mProcessItem->DefectB(true);
}
// set simulator state
void FtcPlant::DStep1(void) {
  if(mSmartDistributorStage>0) return;
  mSmartDistributorStage=1;
}
// set simulator state
void FtcPlant::DStep3(void) {
  if(mSmartDistributorStage>0) return;
  mSmartDistributorStage=3;
}
// set simulator state
void FtcPlant::StartSmartStopper(void) {
  FF_DQM("StartSmartStopper");
  if(mSmartStopperStage>0) return;
  mSmartStopperStage=1;
}
// set simulator state
void FtcPlant::StartProcessA(void) {
  mProcessItem->Machine(ProcessItem::MProcA);
}
// set simulator state
void FtcPlant::StartProcessB(void) {
  mProcessItem->Machine(ProcessItem::MProcB);
}
// set simulator state
void FtcPlant::PlaceRequest1(void) {
  SensorEvent("P1");
  LogReq("P1");
}

void FtcPlant::ScreenShot(void) {
  FF_DQM("ScreenShot(file)");
  QSvgGenerator svggen;
  svggen.setFileName("/tmp/ftcplant.svg");
  svggen.setSize(QSize((int) mGScene->width()+1, (int) mGScene->height()+1));
  QPainter painter(&svggen);
  mGScene->render(&painter,QRectF(),mGScene->itemsBoundingRect());
  painter.end();
}



// set simulator state
void FtcPlant::PlaceRequest2(void) {
  SensorEvent("P2");
  LogReq("P2");
}
// set simulator state
void FtcPlant::PlaceRequest3(void) {
  SensorEvent("P3");
  LogReq("P3");
}

// run simulator
void FtcPlant::TimeStep(void) {
  if(mPause) return;
  // FF_DQM("FtcPlant::TimeStep()");

  // 1) ************ update actuator settings from external events
  QString actev;
  while((actev=ActuatorEvent())!="") {
    /*
    // conveyor belt a
    if(actev=="cba_stop")   mConveyorAItem->State(ConveyorItem::MOff);
    if(actev=="cba_right") mConveyorAItem->State(ConveyorItem::MInc);
    if(actev=="cba_left")  mConveyorAItem->State(ConveyorItem::MDec);
    */
    if(actev=="V")   StartSmartStopper();
    if(actev=="R")   DStep1();
    if(actev=="RR")  DStep3();
    if(actev=="A")   StartProcessA();
    if(actev=="B")   StartProcessB();
    if(actev=="p1")  LogAck("p1");
    if(actev=="p2")  LogAck("p2");
    if(actev=="p3")  LogAck("p3");
    if(actev=="pi")  LogAck("pi");
    if(actev=="pI")  LogReqCancel();
  }

  // 2) ***************** simulate stopper motors
  // record levels
  bool openl=mStopperItem->OpenL(); 
  bool openr=mStopperItem->OpenR(); 
  bool closedl=mStopperItem->ClosedL(); 
  bool closedr=mStopperItem->ClosedR();
  // run stopper motors 
  mStopperItem->TimeStep();
  // test collosion of stopper with workpiece
  for(int i=0; i<mWorkPieceList.size(); i++) {
    WorkPieceItem* wpi=mWorkPieceList.at(i);
    qreal x=wpi->X();
    //qreal y=wpi->Y();
    //qreal o=wpi->O();
    if(x<=191) continue;
    if(wpi->collidesWithItem(mStopperItem)) {
      mStopperItem->MotorL(StopperItem::MJammed);
      mStopperItem->MotorR(StopperItem::MJammed);
    }  
  } 
  // sense edges
  if(mStopperItem->OpenL() && !openl) SensorEvent("stp_lack"); 
  if(mStopperItem->OpenR() && !openr) SensorEvent("stp_rack"); 
  if(mStopperItem->ClosedL() && !closedl) SensorEvent("stp_lack"); 
  if(mStopperItem->ClosedR() && !closedr) SensorEvent("stp_rack"); 


  // 2) ***************** simulate belt transport
  // detect sensor edges
  bool s1 = mConveyorItem->SensorS1();
  bool s2 = mConveyorItem->SensorS2();
  bool s3 = mConveyorItem->SensorS3();
  bool s1n=false; 
  bool s2n=false; 
  bool s3n=false;
  // run conveyor belts motor
  mConveyorItem->TimeStep();
  // track workpiece transport
  for(int i=0; i<mWorkPieceList.size(); i++) {
    WorkPieceItem* wpi=mWorkPieceList.at(i);
    qreal x=wpi->X();
    qreal y=wpi->Y();
    qreal o=wpi->O();
    // figure sensor levels
    if(x>= 297.5  && x<=307.5) s1n=true;
    if(x>= 0  && x<=191 && y>=-2 && y<=2) s2n=true;
    if(x>= -300 && x<=-280) s3n=true;
    // do linear transport
    if(x>= -189 && x<=191) continue;
    if(y<=-2 || y>=2) continue;
    qreal nx = x+ mConveyorItem->Velocity();
    qreal ny = 0.8 * y;
    wpi->SetPosition(nx,ny,o);
    // test collision with previous work piece
    if(i>0) {
      WorkPieceItem* wpim1=mWorkPieceList.at(i-1);
      if(wpi->collidesWithItem(wpim1)) 
        wpi->SetPosition(x,y,o);
    }  
    // test collision with stopper
    if(wpi->collidesWithItem(mStopperItem)) {
      wpi->SetPosition(x,y,o);
    }  
    // test collision with distributor
    if(wpi->collidesWithItem(mDistributorItem)) {
      if(!mDistributorItem->Ready()) wpi->State(WorkPieceItem::WJammed);
      wpi->SetPosition(x,y,o);
    }  
  }
  // set sensor levels and detect sense edges
  if(s1n && !s1) {SensorEvent("s1_wpar"); SensorEvent("S1"); LogEntry();} 
  if(s2n && !s2) {SensorEvent("s2_wpar"); SensorEvent("S2");}
  if(s3n && !s3) {SensorEvent("s3_wpar"); SensorEvent("S3"); LogExit();}
  mConveyorItem->SensorS1(s1n);
  mConveyorItem->SensorS2(s2n);
  mConveyorItem->SensorS3(s3n);

  // 3)simulate distributor rotation
  // record velo and level
  bool rdy=mDistributorItem->Ready();
  qreal dvel = mDistributorItem->Velocity();
  mDistributorItem->TimeStep();
  // simulate workpiece transport on distributor
  for(int i=0; i<mWorkPieceList.size(); i++) {
    WorkPieceItem* wpi=mWorkPieceList.at(i);
    //if(wpi->State()==WorkPieceItem::WJammed) continue;
    qreal x=wpi->X();
    qreal y=wpi->Y();
    qreal o=wpi->O();
    qreal r=qSqrt(x*x + y*y);
    if(r<188 || r>194) continue;
    qreal nr=190; 
    if(mDistributorItem->Ready() && x<100 && y>=-2 && y <=2) nr=194;
    nr = 0.5*nr + 0.5*r;
    qreal no=o + dvel;
    qreal nx = nr * qCos(no / 360 * 2 * 3.141593);
    qreal ny = nr * qSin(no / 360 * 2 * 3.141593);
    wpi->SetPosition(nx,ny,no);
    // test collision with distributor
    if(wpi->collidesWithItem(mDistributorItem)) {
      //if(!mDistributorItem->Ready()) wpi->State(WorkPieceItem::WJammed);
      //wpi->SetPosition(x,y,o);
    }  
  }
  // detect sensor edges
  if(mDistributorItem->Ready() && !rdy) SensorEvent("rt_pos"); 
  // smart distributor
  if(mSmartDistributorStage>0) { 
    if(mDistributorItem->Ready() && !rdy) mSmartDistributorStage--; 
    if(mSmartDistributorStage==0) {
      SensorEvent("r"); 
      mDistributorItem->Motor(DistributorItem::MOff);
    }
    if(mSmartDistributorStage>0)
      mDistributorItem->Motor(DistributorItem::MRight);
  }



  // 4b) insert/remove workpiece 
  WPRemove();
  if(mSmartWPInsertStage >0 && CanWPInsert()) {
    WPInsert();
    mSmartWPInsertStage--;
  }


  // 5) simulate workpiece private dynamics
  //
  foreach(WorkPieceItem* wpi,mWorkPieceList) {
    wpi->TimeStep();
  }
  // figure wp at processing machine
  if(mProcessWorkPiece) {
    qreal x=mProcessWorkPiece->X();
    qreal y=mProcessWorkPiece->Y();
    if( x>1 || x <-1 || y<189 || y>191) {
      FF_DQM("FtcPlant::TimeStep(): wp left station");
      mProcessWorkPiece=NULL;
      mProcessItem->WorkPiece(NULL);
      LogProcNext();
    } 
  }
  if((!mProcessWorkPiece) && mDistributorItem->Ready()) {
    foreach(WorkPieceItem* wpi,mWorkPieceList) {
      qreal x=wpi->X();
      qreal y=wpi->Y();
      if( x<=1 && x >=-1 && y>=189 && y<=191) {
        FF_DQM("FtcPlant::TimeStep(): wp arrived at station");
        mProcessWorkPiece=wpi;
        mProcessItem->WorkPiece(wpi);
        break;
      }
    }
  }      
  // do process
  mProcessItem->TimeStep();
  // generate feedback events
  if(mProcessWorkPiece) { 
    if(mProcessWorkPiece->Complete()) {
      if(mProcessItem->Machine()==ProcessItem::MProcA) { 
        SensorEvent("prc_acmp"); 
        SensorEvent("a"); 
        LogProc("A");
        mProcessItem->Machine(ProcessItem::MOff);
      }
      if(mProcessItem->Machine()==ProcessItem::MProcB && !mProcessItem->DefectB()) { 
        SensorEvent("prc_bcmp"); 
        SensorEvent("b"); 
        LogProc("B");
        mProcessItem->Machine(ProcessItem::MOff);
      }
    }
  }
  if(mProcessWorkPiece) { 
    if(mProcessItem->Machine()==ProcessItem::MProcB && mProcessItem->DefectB()) {
      SensorEvent("prc_fault"); 
      SensorEvent("f");
      mProcessItem->Machine(ProcessItem::MOff);
    }
  }
  // 7) smart stopper
  // idle preparation: initial position
  if(mSmartStopperStage==-5) {
    mStopperItem->MotorL(StopperItem::MClose);
    mStopperItem->MotorR(StopperItem::MOpen);   
    if(mStopperItem->ClosedL()) 
    if(mStopperItem->OpenR())   
      mSmartStopperStage=-4;
  }
  // idle preparation: keep waiting for workpiece
  if(mSmartStopperStage==-4) {
    if(mConveyorItem->SensorS1()) mSmartStopperStage=-3;
  }
  // idle preparation: workpiece caught
  if(mSmartStopperStage==-3) {
    mStopperItem->MotorR(StopperItem::MClose);
    if(mStopperItem->ClosedR()) mSmartStopperStage=0;
  }
  // reject if no wp at s1
  if(mSmartStopperStage==1) {
    if(!mConveyorItem->SensorS1()) mSmartStopperStage=0;
  }
  // ensure initial position: left closed (this may jamm on the first turnaround)
  if(mSmartStopperStage==1) {
    if(mStopperItem->OpenL()) mStopperItem->MotorL(StopperItem::MClose);
    if(mStopperItem->ClosedL()) mSmartStopperStage=2;
  }
  // ensure initial position: right open when empty
  if(mSmartStopperStage==2) {
    if(mConveyorItem->SensorS1()) mSmartStopperStage=3;
    else mStopperItem->MotorR(StopperItem::MOpen);
    if(mStopperItem->OpenR())   mSmartStopperStage=3;
  }
  // wait for workpiece
  if(mSmartStopperStage==3) {
    if(mConveyorItem->SensorS1()) mSmartStopperStage=4;
  }
  // release workpiece
  if(mSmartStopperStage==4) {
    mStopperItem->MotorR(StopperItem::MClose);
    if(mStopperItem->ClosedR()) mSmartStopperStage=5;
  }
  if(mSmartStopperStage==5) {
    mStopperItem->MotorL(StopperItem::MOpen);
    if(mStopperItem->OpenL())   mSmartStopperStage=6;
  }
  // wait for ackowledge (edge on S2)
  if(mSmartStopperStage==6) {
    if(!mConveyorItem->SensorS2()) mSmartStopperStage=7;
  }
  if(mSmartStopperStage==7) {
    if(mConveyorItem->SensorS2()) mSmartStopperStage=-5;
  }


  // monitor net device status
  static faudes::vDevice::DeviceState recent=faudes::vDevice::Down;
  faudes::vDevice::DeviceState current=faudes::vDevice::Down;
  current=mFaudesDev->Status();
  if(recent!=current) {
    Logging("Device: "+FfStyle::QStrFromStr(mFaudesDev->StatusString()));
  }
  recent=current;
  // update visual (incl actuator signalss)
  Update();
}

// doit: close the window (reimplemet qt window)
void FtcPlant::closeEvent ( QCloseEvent * event ){
  // save window geometry
  QSettings settings("Faudes", "FtcPlant");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("state", saveState());
  // call base
  QMainWindow::closeEvent(event);
}


// autozoom: track resize
void FtcPlant::resizeEvent ( QResizeEvent * event ){
  // call base
  QMainWindow::resizeEvent(event);
  // zoom view
  if(mGView) AutoZoom();
}

// autozoom: doit
void FtcPlant::AutoZoom(void) {
  //qreal sx = mGView->viewport()->size().width();
  //qreal sy = mGView->viewport()->size().height();
  //FF_DQ("resize " << sx << sy);
  QRectF unirect = QRectF(-500,0,1000,1);  // ;-)
  mGView->fitInView(unirect,Qt::KeepAspectRatio);
  
}

// generate libfaudes event
void FtcPlant::SensorEvent(QString event) {
  FF_DQM("FtcPlant::SensorEvent(): " << FfStyle::StrFromQStr(event));
  faudes::Idx ev = mFaudesDev->Outputs().Index(FfStyle::StrFromQStr(event));
  if(!mFaudesDev->Outputs().Exists(ev)) return;
  Logging(event);
  mFaudesDev->WriteOutput(ev);
}

// sense libfaudes event
void FtcPlant::ScheduleEvent(QString event) {
 mScheduledEvent=event;
}

// sense libfaudes event
QString FtcPlant::ActuatorEvent(void) {
  QString res="";
  // scheduled by operator
  if(mScheduledEvent!="") {
     res= mScheduledEvent;
     mScheduledEvent="";
     FF_DQM("FtcPlant::ActuatorEvent(): " << FfStyle::StrFromQStr(res+ " [fake]"));
     Logging(res + " [fake]");
     return res;
  }
  // figure net device
  if(mFaudesDev->InputReady()) {
    faudes::Idx ev = mFaudesDev->ReadInput();
    res = FfStyle::QStrFromStr(mFaudesDev->Inputs().SymbolicName(ev));
    FF_DQM("FtcPlant::ActuatorEvent(): " << FfStyle::StrFromQStr(res));
    Logging(res);
    return res;
  }
  // nothing to report
  return res;
}


// event logging
void FtcPlant::Logging(QString message) {
  mEventLogger->textCursor().movePosition(QTextCursor::End);
  mEventLogger->textCursor().insertText(message);
  mEventLogger->textCursor().insertText("\n");
  // mEventLogger->appendPlainText(message);
  if(!mEventLogger->verticalScrollBar()->isSliderDown())
     mEventLogger->ensureCursorVisible();
}

// log entry
void FtcPlant::LogEntry(void) {
  mProcLogEntry->movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,1);
  mProcLogEntry->removeSelectedText();
  mProcLogEntry->insertText("*");
  mProcLogEntry->movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,1);
  if(!mProcLogEntry->movePosition(QTextCursor::Down)) {
   mProcessLogger->appendPlainText("__-__-__");
   mProcLogEntry->movePosition(QTextCursor::Down);
  }
}
// log request
void FtcPlant::LogReq(QString message) {
  mProcLogReq->movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,2);
  mProcLogReq->removeSelectedText();
  mProcLogReq->insertText(message);
  mProcLogReq->movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,2);
  if(!mProcLogReq->movePosition(QTextCursor::Down)) {
   mProcessLogger->appendPlainText("__-__-__");
   mProcLogReq->movePosition(QTextCursor::Down);
  }
}
// log request
void FtcPlant::LogReqCancel(void) {
  if(mProcLogReq->movePosition(QTextCursor::Up)) {
    mProcLogReq->movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,2);
    mProcLogReq->removeSelectedText();
    mProcLogReq->insertText("__");
    mProcLogReq->movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,2);
  }
}
// log process
void FtcPlant::LogProc(QString message) {
  mProcLogProc->movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,1);
  mProcLogProc->removeSelectedText();
  mProcLogProc->insertText(message);
}
// log process next wp
void FtcPlant::LogProcNext(void) {
  while(mProcLogProc->columnNumber()>3)
    mProcLogProc->movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,1);
  if(!mProcLogProc->movePosition(QTextCursor::Down)) {
    mProcessLogger->appendPlainText("__-__-__");
    mProcLogProc->movePosition(QTextCursor::Down);
  }
}
// log exit
void FtcPlant::LogExit(void) {
  mProcLogExit->movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,1);
  mProcLogExit->removeSelectedText();
  mProcLogExit->insertText("*");
  mProcLogExit->movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,1);
  if(!mProcLogExit->movePosition(QTextCursor::Down)) {
   mProcessLogger->appendPlainText("__-__-__");
   mProcLogExit->movePosition(QTextCursor::Down);
  }
}
// log request
void FtcPlant::LogAck(QString message) {
  mProcLogAck->movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,2);
  mProcLogAck->removeSelectedText();
  mProcLogAck->insertText(message);
  mProcLogAck->movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,2);
  if(!mProcLogAck->movePosition(QTextCursor::Down)) {
   mProcessLogger->appendPlainText("__-__-__");
   mProcLogAck->movePosition(QTextCursor::Down);
  }
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
  FF_DQ("FtcPlant:main()");

  // osx 11: fix fonts (nee to be before QApplication)
#ifdef Q_OS_MACX
  QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
  QFont::insertSubstitution(".Helvetica Neue DeskInterface", "Helvetica Neue");
  QFont::insertSubstitution(".SF NS Text", "Helvetica Neue");
#endif

  // let Qt see commandline
  QApplication app(argc, argv);
 

#ifdef Q_OS_WIN32  
  // redirect libfaudes error to file
  QTemporaryFile violog(QDir::tempPath()+ QDir::separator()+ "ftcplant_log_XXXXXX");
  violog.setAutoRemove(false);
  violog.open(); 
  QString viologname = violog.fileName(); 
  violog.close();
  faudes::ConsoleOut::G()->ToFile(FfStyle::StrFromQStr(viologname));
  FF_WARN("ftcplant logfile");
#endif

#ifdef Q_OS_WIN32  
#ifdef FAUDES_IODEVICE_SIMPLENET
  // if we have a net device on windows, initialise winsocks
  FF_WARN("Main(): init winsock");
  WSADATA wsaData; 
  if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0) {
    std::cout << "cannot open netdevice (network error)" << std::endl;  
    return 1;
  }
#endif
#endif


  // add my searchpath for plugins (obsolete?)
  QApplication::addLibraryPath(QCoreApplication::applicationDirPath() + QDir::separator()+ "plugins"); 

  // my id
  QApplication::setApplicationName("FtcPlant");
  QApplication::setOrganizationName("Faudes");
  QApplication::setOrganizationDomain("faudes.org");


  // initialize style
  FfStyle::Initialise();

  // load file and go
  FtcPlant *win = new FtcPlant();
  QSettings settings("Faudes", "FtcPlant");
  win->restoreGeometry(settings.value("geometry").toByteArray());
  win->restoreState(settings.value("state").toByteArray());
  win->show();

  //  initialize autozoom
  win->AutoZoom();


#ifdef Q_OS_MAC
#ifdef FF_OSXFULLSCREEN
  // add osx lion fullscreen decoration (for the fun of it)
  #include "osxfullscreen.h"
  OsxFullScreen(win);
#endif
#endif


  FF_DQ("FtcPlant:main(): do loop");
  return app.exec();
}
