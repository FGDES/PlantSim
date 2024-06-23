/* ftcplant.h  - productionline simulator */


#ifndef FAUDES_MANUPLANT_H
#define FAUDES_MANUPLANT_H

#include <QApplication>
#include <QtGui>
#include "libfaudes.h"
#include "ffstyle.h"
#include "ftcitems.h"




/*
************************************************
************************************************
  
FtcPlant main window

************************************************
************************************************
*/

class FtcPlant : public QMainWindow {

 Q_OBJECT

public:

  // constructor
  FtcPlant(void);


public slots:

  // gui slots
  void About(void);
  void Initialize(void);
  void BreakB(void);
  void RequestWPInsert(void);
  void DStep1(void);
  void DStep3(void);
  void StartSmartStopper(void);
  void StartProcessA(void);
  void StartProcessB(void);
  void PlaceRequest1(void);
  void PlaceRequest2(void);
  void PlaceRequest3(void);
  void ScreenShot(void);

  // update visual from state
  void Update();

  // update state from visual
  void Trigger();

  // run simulator
  void TimeStep();

  // event logger
  void Logging(QString message);

  // autozoom scene
  void AutoZoom(void);

  // schedule event
  void ScheduleEvent(QString event);

protected:

  // reimplement close event handler
  void closeEvent(QCloseEvent * event);

  // reimplement resize handler for autozoom
  void resizeEvent(QResizeEvent * event);

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
  QAction *mShotAct;

  // my layout
  QHBoxLayout* mHbox;
  QHBoxLayout* mVbox;

  // my layout items: smart gui
  QPushButton* mInitButton;
  QPushButton* mWPInsertButton;
  QPushButton* mBreakBButton;
  QPushButton* mDStep1Button;
  QPushButton* mDStep3Button;
  QPushButton* mStopperButton;
  QPushButton* mProcAButton;
  QPushButton* mProcBButton;
  QPushButton* mRequest1Button;
  QPushButton* mRequest2Button;
  QPushButton* mRequest3Button;
  QPushButton* mPauseButton;
  // my layout items: actuators
  QComboBox* mCBCombo;
  QComboBox* mDCombo;
  QComboBox* mSMLCombo;
  QComboBox* mSMRCombo;
  // my layout items: logger
  QPlainTextEdit* mEventLogger;  
  QPlainTextEdit* mProcessLogger;  

  // simulator parameters
  int mParaTimeStep;
  bool mPause;

  // simulator visual
  QGraphicsScene* mGScene;
  QGraphicsView* mGView;
  ConveyorItem* mConveyorItem;
  DistributorItem* mDistributorItem;
  StopperItem* mStopperItem;
  ProcessItem* mProcessItem;
  QList<WorkPieceItem*> mWorkPieceList;
  WorkPieceItem* mProcessWorkPiece;

  // my simulation state for smart controls
  int mSmartStopperStage;
  int mSmartDistributorStage;
  int mSmartWPInsertStage;

  // workpiece io
  void WPInsert(void);
  void WPRemove(void);
  bool CanWPInsert(void);

  // process logging
  void LogReq(QString message);
  void LogReqCancel(void);
  void LogEntry(void);
  void LogProc(QString message);
  void LogProcNext(void);
  void LogExit(void);
  void LogAck(QString message);
  QTextCursor* mProcLogReq;
  QTextCursor* mProcLogEntry;
  QTextCursor* mProcLogProc;
  QTextCursor* mProcLogAck;
  QTextCursor* mProcLogExit;

   
  // faudes devive
  faudes::vDevice* mFaudesDev;
  QString mScheduledEvent;


};

#endif
