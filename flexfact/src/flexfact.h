/* flexfact.h  - productionline simulator */

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


#ifndef FAUDES_FLEXFACT_H
#define FAUDES_FLEXFACT_H

#include <QApplication>
#include <QtGui>
#include <QtXml>

#include "ffstyle.h"
#include "ffscene.h"
#include "ffoperator.h"
#include "ffmonitor.h"
#include "ffview.h"




/*
************************************************
************************************************
  
FlexFact main window

************************************************
************************************************
*/

class FlexFact : public QMainWindow {

 Q_OBJECT

public:

  // constructor
  FlexFact(void);


public slots:

  // std gui
  void About(void);
  void New(void);
  void Open(void);
  void Open(QString filename);
  void OpenRecent(void);
  void Save(void);
  void Save(QString filename);
  void SaveAs(void);
  void ExportSvg(void);
  void ExportSvg(QString filename);
  void ExportSimplenetFile(void);
  void ExportSimplenetFile(QString filename);
  void ExportSimplenetlocalFile(void);
  void ExportSimplenetlocalFile(QString filename);
  void ExportModbusFile(void);
  void ExportModbusFile(QString filename);

  // gui slots for simulation
  void SetOperatorPanel(int sz=-1);
  void ShowMonitor(bool show=true);
  void SetDevice(QString dev="");
  void ResetState(void);
  void Start(void);
  void Stop(void);
  void TimeStep(void);

  // write to logging widget
  void Logging(QString message);

  // adjust view
  void ShowLogger(void);
  void ShowImage(void);

  // editing
  void SetModified(bool m=true);
  bool Modified(void) { return mModified;};

protected slots:

  // menu updater
  void UpdateRecentFileActions(void);
  void ClearRecentFileActions(void);
  void CurrentFile(QString filename);


protected:

  // reimplement close event handler
  void closeEvent(QCloseEvent * event);

  // initialise
  void CreateGuiItems(void);
  void CreateActions(void);
  void CreateMenus(void);

  // helper
  QString StrippedName(const QString &fullFileName);

  // my actions
  QAction* mAboutAct;
  QAction* mNewAct;
  QAction* mOpenAct;
  QAction* mSaveAct;
  QAction* mSaveAsAct;
  QAction* mExportSimplenetlocalFileAct;
  QAction* mExportSimplenetFileAct;
  QAction* mExportModbusFileAct;
  QAction* mExportSvgAct;
  QAction* mExitAct;
  QAction* mZoomInAct;
  QAction* mZoomOutAct;
  QAction* mZoomFitAct;
  QAction* mClearLogAct;
  QAction* mShowLogAct;
  QAction* mShowImageAct;
  QAction* mResetStateAct;
  QAction* mStartSimAct;
  QAction* mStopSimAct;
  QAction* mModbusAct;
  QAction* mSimplenetAct;
  QAction* mSimplenetlocalAct;
  QAction* mMonitorAct;
  QList<QAction*> mOperatorActions;


  // recent file actions with mutable seperator
  QAction *mSeparatorRecentAct;
  QAction *mClearRecentAct;
  enum { MaxRecentFiles = 5 };
  QAction *mRecentFileActs[MaxRecentFiles];
  QString mFilename;

  // my layout
  QHBoxLayout* mHbox;
  QHBoxLayout* mVbox;
  QSplitter*   mSplitter;

  // my layout items
  QPlainTextEdit* mLogger;  
  QGroupBox* mLoggerBox;
  QTableView* mImageView;
  QGroupBox* mImageBox;
  QPushButton* mResetStateButton;
  QComboBox* mSetDeviceCombo;
  QPushButton* mStartSimButton;
  QPushButton* mStopSimButton;
  QGroupBox *mOperatorBox;
  QGroupBox *mMonitorBox;

  // simulator parameters
  bool mRunning;
  QString mDevice;

  // simulator visual
  FfScene* mGScene;
  FfView* mGView;
  FfOperatorPanel* mOperatorPanel;
  FfMonitorPanel* mMonitorPanel;

  // track user edit
  bool mModified;


};

#endif
