/* flexfact.cpp  - simulate plant */

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

#include "flexfact.h"

#ifdef FF_LIBFAUDES
#include "libfaudes.h"
#endif

#ifdef Q_OS_WIN32
#ifdef FF_LIBFAUDES
#include <winsock2.h>
#endif
#ifdef FF_WINCONSOLE
#include <windows.h>
#endif
#endif





/*
************************************************
************************************************
  
Implementation: FlexFact

************************************************
************************************************
*/


// construct
FlexFact::FlexFact() : QMainWindow(0)
{

  FF_DQ("FlexFact(): A");

  // invalidate my data
  mFilename="";

  // set qt window attributes
  setAttribute(Qt::WA_DeleteOnClose);
  setContentsMargins(0,0,0,0);
  setWindowTitle("Factory Simulator");

  // prepare unser interface
  CreateGuiItems();
  CreateActions();
  CreateMenus();
  
  // have my base layout (a hbox)
  QWidget* cwid= new QWidget();
  setCentralWidget(cwid);
  QHBoxLayout* mHbox = new QHBoxLayout(cwid);
  mHbox->setMargin(0);
  mHbox->setSpacing(0);

  // have a vbox (right column)
  QWidget* rwid = new QWidget();
  rwid->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
  QVBoxLayout* mVbox = new QVBoxLayout(rwid);
  mVbox->setMargin(0);
  mVbox->setSpacing(0);

  // have a proces image view
  mImageBox = new QGroupBox("Process Image");
  QHBoxLayout* imghbox = new QHBoxLayout(mImageBox);
  imghbox->setMargin(5);
  imghbox->setSpacing(5);
  imghbox->addWidget(mImageView);
  mVbox->addWidget(mImageBox);

  // have a logger
  mLoggerBox = new QGroupBox("Event Log");
  QHBoxLayout* loghbox = new QHBoxLayout(mLoggerBox);
  loghbox->setMargin(5);
  loghbox->setSpacing(5);
  loghbox->addWidget(mLogger);
  mVbox->addWidget(mLoggerBox);

  // have an operator panel
  mOperatorPanel= new FfOperatorPanel();
  mOperatorBox = new QGroupBox("Operator");
  QHBoxLayout* opphbox = new QHBoxLayout(mOperatorBox);
  opphbox->setMargin(5);
  opphbox->setSpacing(5);
  opphbox->addWidget(mOperatorPanel);
  mVbox->addWidget(mOperatorBox);

  // have a monitor panel
  mMonitorPanel= new FfMonitorPanel();
  mMonitorBox = new QGroupBox("Fault Monitor");
  QHBoxLayout* monhbox = new QHBoxLayout(mMonitorBox);
  monhbox->setMargin(5);
  monhbox->setSpacing(5);
  monhbox->addWidget(mMonitorPanel);
  mVbox->addWidget(mMonitorBox);

  // have a group layout: controls
  FF_DQ("FlexFact(): B5");
  QGroupBox* ctrgbox = new QGroupBox("Simulation");
  QGridLayout* ctrgrid = new QGridLayout(ctrgbox);
  ctrgrid->setMargin(5);
  ctrgrid->setSpacing(5);
  ctrgrid->setHorizontalSpacing(5);
  ctrgrid->addWidget(mStartSimButton,0,0,1,1);
  ctrgrid->addWidget(mStopSimButton,0,1,1,1);
  ctrgrid->addWidget(mResetStateButton,0,2,1,1);
  mVbox->addWidget(ctrgbox);

  // have a splitter for scene and right column
  FF_DQ("FlexFact(): B0");
  mSplitter = new QSplitter();
  mHbox->addWidget(mSplitter);
  mSplitter->addWidget(mGView);
  mSplitter->addWidget(rwid);

  // layout: balance scene with right columns
  QSizePolicy gpoly(QSizePolicy::Expanding,QSizePolicy::Preferred);
  gpoly.setHorizontalStretch(10);
  mGView->setSizePolicy(gpoly);
  mGView->setMinimumSize(200,200);
  QSizePolicy tpoly(QSizePolicy::Preferred,QSizePolicy::Preferred);
  tpoly.setHorizontalStretch(0);
  rwid->setSizePolicy(tpoly);

  // fix controls
  mRunning=false;
  mModified=false;
  mDevice="No Device";
  mStopSimButton->setEnabled(mRunning);
  mStopSimAct->setEnabled(mRunning);
  mStartSimButton->setEnabled(!mRunning);
  mStartSimAct->setEnabled(!mRunning);

  // fix view
  ShowLogger();
  SetOperatorPanel(0);
  ShowMonitor(false);

  // report from process image
  connect(mGScene->Image(),SIGNAL(NotifySensorEvent(const QString&)), this, SLOT(Logging(QString)));
  connect(mGScene->Image(),SIGNAL(NotifyActuatorEvent(const QString&)), this, SLOT(Logging(QString)));

  // connect panel
  connect(mOperatorPanel,SIGNAL(Switch(int, bool)),mGScene,SLOT(OperatorSwitch(int, bool)));
  connect(mGScene,SIGNAL(OperatorLight(int, bool)),mOperatorPanel,SLOT(Light(int, bool)));

  // connect monitor
  mGScene->SetMonitor(mMonitorPanel);

  // track editing
  connect(mGScene,SIGNAL(NotifyClearImage()),this,SLOT(SetModified()));

  // run simulator
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(TimeStep()));
  timer->start(1 / FfStyle::mFramesPerSecond * 1000); // 1/s ==> ms
}


// initialise: have guiitems
void FlexFact::CreateGuiItems() {
  FF_DQ("FlexFact::CreateGuiItems()");

  // gui item: the simulator
  mGScene = new FfScene(this);
  mGView = new FfView();
  mGView->setRenderHints(QPainter::Antialiasing);
  mGView->setScene(mGScene);

  // gui item: the logger
  mLogger = new QPlainTextEdit();
  mLogger->setReadOnly(true);
  mLogger->setCenterOnScroll(true);
  mLogger->setMaximumBlockCount(1000); // style this
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  mLogger->setFont(font);

  // Gui item: process image
  mImageView = new QTableView();
  mImageView->setModel(mGScene->ImageModel());
  mImageView->verticalHeader()->hide();
  mImageView->horizontalHeader()->hide();
  //mImageView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  mImageView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  mImageView->horizontalHeader()->setStretchLastSection(true);
  mImageView->horizontalHeader()->setSortIndicatorShown(false); 
  mImageView->setEditTriggers(0);
  mImageView->setShowGrid(false);
  QFont ifont;
  ifont.setFamily("Courier");
  ifont.setFixedPitch(true);
  mImageView->setFont(ifont);
}

// initialise: have actions
void FlexFact::CreateActions() {
  FF_DQ("FlexFact::CreateActions()");

  // about
  mAboutAct = new QAction(tr("About FlexFact"), this);
  connect(mAboutAct, SIGNAL(triggered()), this, SLOT(About()));

  // open
  mOpenAct = new QAction(tr("&Open ..."), this);
  mOpenAct->setShortcut(tr("Ctrl+O"));
  mOpenAct->setStatusTip(tr("Load configuration from file"));
  connect(mOpenAct, SIGNAL(triggered()), this, SLOT(Open()));

  // new
  mNewAct = new QAction(tr("&New"), this);
  mNewAct->setShortcut(tr("Ctrl+N"));
  mNewAct->setStatusTip(tr("Clear current configuration"));
  connect(mNewAct, SIGNAL(triggered()), this, SLOT(New()));

  // save
  mSaveAct = new QAction(tr("&Save"), this);
  mSaveAct->setShortcut(tr("Ctrl+S"));
  mSaveAct->setStatusTip(tr("Save configuration to file"));
  connect(mSaveAct, SIGNAL(triggered()), this, SLOT(Save()));

  // saveas
  mSaveAsAct = new QAction(tr("Save &As ..."), this);
  mSaveAsAct->setStatusTip(tr("Save configuration to another file"));
  connect(mSaveAsAct, SIGNAL(triggered()), this, SLOT(SaveAs()));

  // export
  mExportSimplenetFileAct = new QAction(tr("Export Simplenet Configuration ..."), this);
  mExportSimplenetFileAct->setEnabled(false);
  mExportSimplenetFileAct->setStatusTip(tr("Write configurationfile for libFAUDES supervisor device"));
  connect(mExportSimplenetFileAct, SIGNAL(triggered()), this, SLOT(ExportSimplenetFile()));

  // export
  mExportSimplenetlocalFileAct = new QAction(tr("Export Simplenet (local) Configuration ..."), this);
  mExportSimplenetlocalFileAct->setEnabled(false);
  mExportSimplenetlocalFileAct->setStatusTip(tr("Write configurationfile for libFAUDES supervisor device"));
  connect(mExportSimplenetlocalFileAct, SIGNAL(triggered()), this, SLOT(ExportSimplenetlocalFile()));

  // export
  mExportModbusFileAct = new QAction(tr("Export Modbus/TCP Configuration ..."), this);
  mExportModbusFileAct->setEnabled(false);
  mExportModbusFileAct->setStatusTip(tr("Write configurationfile for libFAUDES supervisor device"));
  connect(mExportModbusFileAct, SIGNAL(triggered()), this, SLOT(ExportModbusFile()));

  // export
  mExportSvgAct = new QAction(tr("Export Factory Layout as Graphic ..."), this);
  mExportSvgAct->setStatusTip(tr("Write configuration in SVG fromat"));
  connect(mExportSvgAct, SIGNAL(triggered()), this, SLOT(ExportSvg()));

  // open recenet
  for (int i = 0; i < MaxRecentFiles; ++i) {
    mRecentFileActs[i] = new QAction(this);
    mRecentFileActs[i]->setVisible(false);
    connect(mRecentFileActs[i], SIGNAL(triggered()),
	    this, SLOT(OpenRecent()));
  }

  // clear recent
  mClearRecentAct = new QAction(tr("Clear Recent"), this);
  mClearRecentAct->setStatusTip(tr("Clear list of recently visited filed"));
  connect(mClearRecentAct, SIGNAL(triggered()), this, SLOT(ClearRecentFileActions()));


  // zoom
  mZoomInAct=new QAction("Zoom In",this);
  mZoomInAct->setStatusTip("zoom graph view");
  mZoomInAct->setShortcut(tr("Ctrl++"));
  connect(mZoomInAct, SIGNAL(triggered()), mGView, SLOT(ZoomIn())); 
  mZoomOutAct=new QAction("Zoom Out",this);
  mZoomOutAct->setStatusTip("zoom graph view");
  mZoomOutAct->setShortcut(tr("Ctrl+-"));
  connect(mZoomOutAct, SIGNAL(triggered()), mGView, SLOT(ZoomOut())); 
  mZoomFitAct=new QAction("Fit in View",this);
  mZoomFitAct->setStatusTip("zoom graph view");
  mZoomFitAct->setShortcut(tr("Ctrl+*"));
  connect(mZoomFitAct, SIGNAL(triggered(bool)), mGView, SLOT(ZoomFit())); 

  // log
  mClearLogAct=new QAction("Clear Log",this);
  mClearLogAct->setStatusTip("clear event logger");
  mClearLogAct->setShortcut(tr("Ctrl+Shift+X"));
  connect(mClearLogAct, SIGNAL(triggered(bool)), mLogger, SLOT(clear())); 

  // show
  mShowLogAct=new QAction("Show Log",this);
  mShowLogAct->setStatusTip("show event logger");
  mShowLogAct->setCheckable(true);
  connect(mShowLogAct, SIGNAL(triggered(bool)), this, SLOT(ShowLogger())); 
  mShowImageAct=new QAction("Show Image",this);
  mShowImageAct->setStatusTip("show process image");
  mShowImageAct->setCheckable(true);
  connect(mShowImageAct, SIGNAL(triggered(bool)), this, SLOT(ShowImage())); 

  // exit
  mExitAct = new QAction(tr("&Quit"), this);
  mExitAct->setShortcut(tr("Ctrl+Q"));
  mExitAct->setStatusTip(tr("Quit"));
  connect(mExitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

  // reset state
  mResetStateAct = new QAction(tr("Reset"), this);
  mResetStateAct->setStatusTip(tr("reset simulator to initial state"));
  connect(mResetStateAct, SIGNAL(triggered()), this, SLOT(ResetState()));

  // start sim
  mStartSimAct = new QAction(tr("Start"), this);
  mStartSimAct->setStatusTip(tr("start simulator"));
  connect(mStartSimAct, SIGNAL(triggered()), this, SLOT(Start()));

  // stop sim
  mStopSimAct = new QAction(tr("Stop"), this);
  mStopSimAct->setStatusTip(tr("stop simulator"));
  connect(mStopSimAct, SIGNAL(triggered()), this, SLOT(Stop()));

  // device act
  mSimplenetAct = new QAction(tr("Simplenet"), this);
  mSimplenetAct->setStatusTip(tr("interface controller via Simplenet"));
  mSimplenetAct->setCheckable(true);
  mSimplenetAct->setChecked(false);
#ifndef FF_LIBFAUDES
  mSimplenetAct->setEnabled(false);
#endif
  connect(mSimplenetAct,SIGNAL(triggered()),this,SLOT(SetDevice()));
  mSimplenetlocalAct = new QAction(tr("Simplenet (local)"), this);
  mSimplenetlocalAct->setStatusTip(tr("interface controller via Simplenet restricted to localhost"));
  mSimplenetlocalAct->setCheckable(true);
  mSimplenetlocalAct->setChecked(false);
#ifndef FF_LIBFAUDES
  mSimplenetlocalAct->setEnabled(false);
#endif
  connect(mSimplenetlocalAct,SIGNAL(triggered()),this,SLOT(SetDevice()));
  mModbusAct = new QAction(tr("Modbus"), this);
  mModbusAct->setStatusTip(tr("interface controller via Modbus/TCP"));
  mModbusAct->setCheckable(true);
  mModbusAct->setChecked(false);
  connect(mModbusAct,SIGNAL(triggered()),this,SLOT(SetDevice()));

  // gui items: buttons for actions
  mResetStateButton=new QPushButton("Reset");
  connect(mResetStateButton,SIGNAL(pressed()),mResetStateAct,SLOT(trigger()));
  mStartSimButton=new QPushButton("Start");
  connect(mStartSimButton,SIGNAL(pressed()),mStartSimAct,SLOT(trigger()));
  mStopSimButton=new QPushButton("Stop");
  connect(mStopSimButton,SIGNAL(pressed()),mStopSimAct,SLOT(trigger()));
}

// initialise: menues
void FlexFact::CreateMenus(void) {
  FF_DQ("FlexFact::CreateMenus()");

  // have a file menu
  QMenu* menu = new QMenu(tr("&File"));
  menu->addAction(mAboutAct);
  menu->addSeparator();
  menu->addAction(mNewAct);
  menu->addAction(mOpenAct);
  menu->addAction(mSaveAct);
  menu->addAction(mSaveAsAct);
  menu->addSeparator();
  menu->addAction(mExportModbusFileAct);
  menu->addAction(mExportSimplenetFileAct);
  menu->addAction(mExportSimplenetlocalFileAct);
  menu->addAction(mExportSvgAct);
  mSeparatorRecentAct = menu->addSeparator();
  for(int i = 0; i < MaxRecentFiles; ++i)
    menu->addAction(mRecentFileActs[i]);
  // menu->addAction(mClearRecentAct); // I dislike this
#ifndef Q_WS_MAC  
  menu->addSeparator();
#endif
  menu->addAction(mExitAct);
  UpdateRecentFileActions();
 
  // install and connect file menu
  menuBar()->addMenu(menu);
  connect(menu, SIGNAL(aboutToShow()), this, SLOT(UpdateRecentFileActions()));

  // have a operation panel cfg menu
  QMenu* opmenu = new QMenu(tr("Operator Panel"));
  QAction* opact;
  opact=opmenu->addAction("none");
  opact->setProperty("cnt",QVariant(0));
  opact->setCheckable(true);
  connect(opact,SIGNAL(triggered()),this,SLOT(SetOperatorPanel()));
  mOperatorActions.append(opact);
  opact=opmenu->addAction("#4");
  opact->setProperty("cnt",QVariant(4));
  opact->setCheckable(true);
  connect(opact,SIGNAL(triggered()),this,SLOT(SetOperatorPanel()));
  mOperatorActions.append(opact);
  opact=opmenu->addAction("#8");
  opact->setProperty("cnt",QVariant(8));
  opact->setCheckable(true);
  connect(opact,SIGNAL(triggered()),this,SLOT(SetOperatorPanel()));
  mOperatorActions.append(opact);
  opact=opmenu->addAction("#12");
  opact->setProperty("cnt",QVariant(12));
  opact->setCheckable(true);
  connect(opact,SIGNAL(triggered()),this,SLOT(SetOperatorPanel()));
  mOperatorActions.append(opact);
  opact=opmenu->addAction("#16");
  opact->setProperty("cnt",QVariant(16));
  opact->setCheckable(true);
  connect(opact,SIGNAL(triggered()),this,SLOT(SetOperatorPanel()));
  mOperatorActions.append(opact);

  // have a monitor menu
  mMonitorAct = new QAction(tr("Fault Monitor"),this);
  mMonitorAct->setCheckable(true);
  mMonitorAct->setChecked(false);
  connect(mMonitorAct,SIGNAL(triggered(bool)),this,SLOT(ShowMonitor(bool)));

  // have a simulation menu
  menu = new QMenu(tr("Simulation"));
  menu->addAction(mStartSimAct);
  menu->addAction(mStopSimAct);
  menu->addAction(mResetStateAct);
  menu->addSeparator();
  menu->addAction(mModbusAct);
  menu->addAction(mSimplenetAct);
  menu->addAction(mSimplenetlocalAct);
  menu->addSeparator();
  menu->addMenu(opmenu);
  menu->addAction(mMonitorAct);
  menuBar()->addMenu(menu);

  // have a view menu
  menu = new QMenu(tr("View"));
  menu->addAction(mZoomInAct);
  menu->addAction(mZoomOutAct);
  menu->addAction(mZoomFitAct);
  menuBar()->addMenu(menu);
  menu->addSeparator();
  menu->addAction(mShowImageAct);
  menu->addAction(mShowLogAct);
  menu->addAction(mClearLogAct);


}

// fix open recent actions
void FlexFact::UpdateRecentFileActions(void) {

  // get from settings
  QSettings settings("Faudes", "FlexFact");
  QStringList files = settings.value("recentFileList").toStringList();

  // iterate list
  int i=0;  
  for(; i < files.size(); ++i) {
    if(i>(int)MaxRecentFiles) break;
    QString text = tr("&%1 %2").arg(i + 1).arg(StrippedName(files[i]));
    mRecentFileActs[i]->setText(text);
    mRecentFileActs[i]->setData(files[i]);
    mRecentFileActs[i]->setVisible(true);
  }
  for(; i < MaxRecentFiles; ++i)
    mRecentFileActs[i]->setVisible(false);

  // show/hide seperator
  mSeparatorRecentAct->setVisible(files.size() > 0);
  mClearRecentAct->setVisible(files.size() > 0);
}

// doit: fix clear recent actions
void FlexFact::ClearRecentFileActions(void) {

  // cler in  settings
  QSettings settings("Faudes", "FlexFact");
  QStringList files;
  settings.setValue("recentFileList", files);

  // update in menu
  UpdateRecentFileActions();

}

// record filename
void FlexFact::CurrentFile(QString filename) {
  // record
  mFilename=QFileInfo(filename).canonicalFilePath();
  // can export
#ifdef FF_LIBFAUDES
  mExportSimplenetFileAct->setEnabled(!mFilename.isEmpty());
  mExportSimplenetlocalFileAct->setEnabled(!mFilename.isEmpty());
  mExportModbusFileAct->setEnabled(!mFilename.isEmpty());
#endif
  // window title fallback
  if(mFilename.isEmpty()) {
    setWindowTitle(tr("Factory Simulator: Untitled[*]"));
    mGScene->SetName("FlexFact");
    return;
  } 
  // window title
  setWindowTitle(tr("Factory Simulator: %1[*]").arg(StrippedName(mFilename)));
  // have this name in scene
  mGScene->SetName(StrippedName(mFilename));
  // fix recent files incl settings
  QSettings settings("Faudes", "FlexFact");
  QStringList files = settings.value("recentFileList").toStringList();
  files.removeAll(filename);
  files.prepend(filename);
  while (files.size() > MaxRecentFiles)
    files.removeLast();
  settings.setValue("recentFileList", files);
}

// std gui slots: about
void FlexFact::About(void) {

  // confihuration string  
  QString conf="";
  if(mFilename!="") conf= QString("<p><b>Configuration:</b> %2</p> ").arg(StrippedName(mFilename)); 

  // use qt about box
  QMessageBox aboutbox;
  aboutbox.setText(QString(
    "<p><b>FlexFact - a configurable factory simulator</b></p>"));
  aboutbox.setInformativeText(QString(
    "<p><b>Version:</b> %1</p> "
    "%3" 
    "<p><b>Credits:</b> %4</p> "
    "%5 "
    "<p><b>(c) 2011-2016 Thomas Moor</b></p> "
    ).arg(
      FfStyle::VersionText(),
      conf,
      FfStyle::ContributorsText(),
      FfStyle::LicenseText()
    ));
    aboutbox.setWindowTitle("About FlexFact"),
    aboutbox.setIcon(QMessageBox::Information);
    aboutbox.addButton(QMessageBox::Ok);
    aboutbox.exec();
}

// std gui slot:  open
void FlexFact::Open(void) {
  FF_DQ("FlexFact::Open(): gui");

  // halt simulation
  Stop();
    
  // save changes 
  if(Modified()) {
    int ret = QMessageBox::warning(this, 
      tr("FlexFact"),
      tr("Do you want to save the current configuration?"),
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, 
      QMessageBox::Save);
    if (ret == QMessageBox::Save) Save();  
    if (ret == QMessageBox::Cancel) return;
  }

  // open dialog: restore from settings
  QFileDialog* fdiag = new QFileDialog();
  QSettings settings("Faudes", "FlexFact");
  fdiag->restoreState(settings.value("stateFileDialog").toByteArray());

  // open dialog: ffs files
  QStringList filters; filters 
    << "FlexFact files (*.ffs)"
    << "Any File (*.*)";
  fdiag->setFilters(filters);
  fdiag->setFileMode(QFileDialog::ExistingFile);
  fdiag->setWindowTitle(QString("Open Factory Configuration"));
  fdiag->setAcceptMode(QFileDialog::AcceptOpen);
  fdiag->setLabelText(QFileDialog::Accept,"Open");
  fdiag->setConfirmOverwrite(false);
  fdiag->setDefaultSuffix("ffs");

  // open dialog: run and save settings
  QString filename="";
  if(fdiag->exec()) {
    if(fdiag->selectedFiles().size()==1) 
      filename=fdiag->selectedFiles().at(0);
  }
  settings.setValue("stateFileDialog", fdiag->saveState());
  delete fdiag;

  // bail out
  filename=QFileInfo(filename).canonicalFilePath();
  if(filename=="") return;

  // do load the file
  Open(filename);
}

// flexfact load from file
void FlexFact::Open(QString filename) {
  FF_DQ("FlexFact::Open() " << filename);
  // read DOM from file
  QFile file(filename);
  file.open(QFile::ReadOnly);
  QDomDocument doc(StrippedName(filename));
  QString errMsg;
  int errLine;
  bool ok= doc.setContent(&file, &errMsg, &errLine);
  if(!ok) {
    FF_DQ("FlexFact::Open(): XML parse error " << errMsg);
    return;
  }
  // interpret DOM
  QDomElement root = doc.documentElement();
  if(root.tagName()!="FlexFactConfiguration") {
    FF_DQ("FlexFact::Open(): incompatible file ");
    return;
  }
  // halt simulation
  Stop();
  SetDevice("No Device");
  // record file (sets name, required for starting a simplenet device)
  CurrentFile(filename);
  // clear "all" ... TODO
  mGScene->Clear();
  QDomElement simpars = root.firstChildElement("SimulationParameters");
  // todo ... parse simparameters once there are such :-)
  mGScene->FromElement(root);
  // adjust
  mGView->ZoomFit();
  // set operator panel
  QDomElement oppanel=simpars.firstChildElement("OperatorPanel");
  QString cstr = oppanel.attribute("cnt");
  bool okc;
  int c= cstr.toInt(&okc); 
  if(!okc) c=0;  
  SetOperatorPanel(c);
  // set fault monitor
  QDomElement faultmon=simpars.firstChildElement("FaultMonitor");
  QString vstr = faultmon.attribute("val");
  bool okv;
  int v= vstr.toInt(&okv); 
  if(!okv) v=0;  
  ShowMonitor(v);
  // set device (incl. compile image)
  QDomElement dele=simpars.firstChildElement("Device");
  QString dev = dele.attribute("type");
  SetDevice(dev);
  // clear modified
  SetModified(false);
}


// flexfact new
void FlexFact::New(void) {
  FF_DQ("FlexFact::New() ");
  // halt simulation
  Stop();
  // save changes 
  if(Modified()) {
    int ret = QMessageBox::warning(this, 
      tr("FlexFact"),
      tr("Do you want to save the current configuration?"),
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, 
      QMessageBox::Save);
    if (ret == QMessageBox::Save) Save();  
    if (ret == QMessageBox::Cancel) return;
  }
  // record file (sets name, required for starting a simplenet device)
  CurrentFile("");
  // clear "all" ... TODO
  SetDevice("No Device");
  mGScene->Clear();
  SetOperatorPanel(0);
  // clear modified
  SetModified(false);
}

// std gio slot: save
void FlexFact::Save(void) {
  if(mFilename.isEmpty()) SaveAs();
  else Save(mFilename);
}

// flexfact save to file
void FlexFact::Save(QString filename) {
  FF_DQ("FlexFact::Save() " << filename);
  // halt simulation
  Stop();
  // build DOM
  QDomDocument doc(StrippedName(filename));
  QDomElement root = doc.createElement("FlexFactConfiguration");
  doc.appendChild(root);
  // simparameters
  QDomElement simpars = doc.createElement("SimulationParameters");
  root.appendChild(simpars);
  QDomElement oppanel = doc.createElement("OperatorPanel");
  oppanel.setAttribute("cnt",mOperatorPanel->Size());
  simpars.appendChild(oppanel);
  QDomElement monpanel = doc.createElement("FaultMonitor");
  monpanel.setAttribute("val",mMonitorBox->isVisible());
  simpars.appendChild(monpanel);
  if(mDevice!="No Device") {
    QDomElement dele = doc.createElement("Device");
    dele.setAttribute("type",mDevice);
    simpars.appendChild(dele);
  }
  // scene
  QDomElement scene = mGScene->ToElement(doc);
  root.appendChild(scene);
  // write to file
  QFile file(filename);
  file.open(QFile::WriteOnly | QFile::Truncate);
  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  doc.save(stream, 0, QDomNode::EncodingFromTextStream);
  file.close();
  // clear modified
  SetModified(false);
}

// std gio slot: save as
void FlexFact::SaveAs(void) {
  FF_DQ("FlexFact::SaveAs(): gui");

  // halt simulation
  Stop();

  // save dialog: restore from settings
  QFileDialog* fdiag = new QFileDialog();
  QSettings settings("Faudes", "FlexFact");
  fdiag->restoreState(settings.value("stateFileDialog").toByteArray());

  // save dialog: ffs files
  QStringList filters; filters 
    << "FlexFact files (*.ffs)"
    << "Any File (*.*)";
  fdiag->setFilters(filters);
  fdiag->setFileMode(QFileDialog::AnyFile);
  fdiag->setWindowTitle(QString("Save Factory Configuration"));
  fdiag->setAcceptMode(QFileDialog::AcceptSave);
  fdiag->setLabelText(QFileDialog::Accept,"Save");
  fdiag->setConfirmOverwrite(true);
  fdiag->setDefaultSuffix("ffs");

  // open dialog: run and save settings
  QString filename="";
  if(fdiag->exec()) {
    if(fdiag->selectedFiles().size()==1) 
      filename=fdiag->selectedFiles().at(0);
  }
  settings.setValue("stateFileDialog", fdiag->saveState());
  delete fdiag;

  // bail out on empty
  if(filename=="") return;

  // do save
  Save(filename);

  // record file
  CurrentFile(filename);
}

// file menu slots: open recent
void FlexFact::OpenRecent() {
  
  // bail out if not triggered by action
  QAction *action = qobject_cast<QAction *>(sender());
  if(!action) return;

  // figure filename
  QString filename = action->data().toString();
  if(filename=="") return;

  // save changes 
  if(Modified()) {
    int ret = QMessageBox::warning(this, 
      tr("FlexFact"),
      tr("Do you want to save the current configuration?"),
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, 
      QMessageBox::Save);
    if (ret == QMessageBox::Save) Save();  
    if (ret == QMessageBox::Cancel) return;
  }

  // do load the file
  Open(filename);

}

// track editing
void FlexFact::SetModified(bool m) {
  if(m) mGScene->Image()->CloseDevice();
  if(mModified == m) return;
  mModified=m;
  setWindowModified(m);
}


// flexfact save to file
void FlexFact::ExportSimplenetFile(QString filename) {
  FF_DQ("FlexFact::ExportDeviceFile() " << filename);
  // halt simulation
  Stop();
  // ensure valid image
  mGScene->CompileImage();
  // let image dump its configuration
  mGScene->Image()->ExportSimplenetConfiguration(filename);
}

// flexfact save to file
void FlexFact::ExportSimplenetlocalFile(QString filename) {
  FF_DQ("FlexFact::ExportDeviceFile() " << filename);
  // halt simulation
  Stop();
  // ensure valid image
  mGScene->CompileImage();
  // let image dump its configuration
  mGScene->Image()->ExportSimplenetlocalConfiguration(filename);
}

// flexfact save to file
void FlexFact::ExportModbusFile(QString filename) {
  FF_DQ("FlexFact::ExportDeviceFile() " << filename);
  // halt simulation
  Stop();
  // ensure valid image
  mGScene->CompileImage();
  // let image dump its configuration
  mGScene->Image()->ExportModbusConfiguration(filename);
}


// std gio slot: save as
void FlexFact::ExportSimplenetFile(void) {
  FF_DQ("FlexFact::ExportDeviceFile(): gui");

  // halt simulation
  Stop();

  // save dialog: restore from settings
  QFileDialog* fdiag = new QFileDialog();
  QSettings settings("Faudes", "FlexFact");
  fdiag->restoreState(settings.value("stateFileDialog").toByteArray());

  // save dialog: dev files
  QStringList filters; filters 
    << "libFAUDES device configurations (*.dev)"
    << "Any File (*.*)";
  fdiag->setFilters(filters);
  fdiag->setFileMode(QFileDialog::AnyFile);
  fdiag->setWindowTitle(QString("Export Device Configuration"));
  fdiag->setAcceptMode(QFileDialog::AcceptSave);
  fdiag->setLabelText(QFileDialog::Accept,"Save");
  fdiag->setConfirmOverwrite(true);
  fdiag->setDefaultSuffix("dev");

  // open dialog: run and save settings
  QString filename="";
  if(fdiag->exec()) {
    if(fdiag->selectedFiles().size()==1) 
      filename=fdiag->selectedFiles().at(0);
  }
  settings.setValue("stateFileDialog", fdiag->saveState());
  delete fdiag;

  // bail out on empty
  if(filename=="") return;

  // do save
  ExportSimplenetFile(filename);

}

// std gio slot: save as
void FlexFact::ExportSimplenetlocalFile(void) {
  FF_DQ("FlexFact::ExportDeviceFile(): gui");

  // halt simulation
  Stop();

  // save dialog: restore from settings
  QFileDialog* fdiag = new QFileDialog();
  QSettings settings("Faudes", "FlexFact");
  fdiag->restoreState(settings.value("stateFileDialog").toByteArray());

  // save dialog: dev files
  QStringList filters; filters 
    << "libFAUDES device configurations (*.dev)"
    << "Any File (*.*)";
  fdiag->setFilters(filters);
  fdiag->setFileMode(QFileDialog::AnyFile);
  fdiag->setWindowTitle(QString("Export Device Configuration"));
  fdiag->setAcceptMode(QFileDialog::AcceptSave);
  fdiag->setLabelText(QFileDialog::Accept,"Save");
  fdiag->setConfirmOverwrite(true);
  fdiag->setDefaultSuffix("dev");

  // open dialog: run and save settings
  QString filename="";
  if(fdiag->exec()) {
    if(fdiag->selectedFiles().size()==1) 
      filename=fdiag->selectedFiles().at(0);
  }
  settings.setValue("stateFileDialog", fdiag->saveState());
  delete fdiag;

  // bail out on empty
  if(filename=="") return;

  // do save
  ExportSimplenetlocalFile(filename);

}

// std gio slot: save as
void FlexFact::ExportModbusFile(void) {
  FF_DQ("FlexFact::ExportDeviceFile(): gui");

  // halt simulation
  Stop();

  // save dialog: restore from settings
  QFileDialog* fdiag = new QFileDialog();
  QSettings settings("Faudes", "FlexFact");
  fdiag->restoreState(settings.value("stateFileDialog").toByteArray());

  // save dialog: dev files
  QStringList filters; filters 
    << "libFAUDES Modbus configurations (*.dev)"
    << "Any File (*.*)";
  fdiag->setFilters(filters);
  fdiag->setFileMode(QFileDialog::AnyFile);
  fdiag->setWindowTitle(QString("Export Device Configuration"));
  fdiag->setAcceptMode(QFileDialog::AcceptSave);
  fdiag->setLabelText(QFileDialog::Accept,"Save");
  fdiag->setConfirmOverwrite(true);
  fdiag->setDefaultSuffix("dev");

  // open dialog: run and save settings
  QString filename="";
  if(fdiag->exec()) {
    if(fdiag->selectedFiles().size()==1) 
      filename=fdiag->selectedFiles().at(0);
  }
  settings.setValue("stateFileDialog", fdiag->saveState());
  delete fdiag;

  // bail out on empty
  if(filename=="") return;

  // do save
  ExportModbusFile(filename);

}

// flexfact save to file
void FlexFact::ExportSvg(QString filename) {
  FF_DQ("FlexFact::ExportSvg() " << filename);
  mGScene->ExportSvg(filename);
}

// std gio slot: save as
void FlexFact::ExportSvg(void) {
  FF_DQ("FlexFact::ExportSvg(): gui");

  // save dialog: restore from settings
  QFileDialog* fdiag = new QFileDialog();
  QSettings settings("Faudes", "FlexFact");
  fdiag->restoreState(settings.value("stateFileDialog").toByteArray());

  // save dialog: svg files
  QStringList filters; filters 
    << "SVG files (*.svg)"
    << "Any File (*.*)";
  fdiag->setFilters(filters);
  fdiag->setFileMode(QFileDialog::AnyFile);
  fdiag->setWindowTitle(QString("Export Configuration as SVG"));
  fdiag->setAcceptMode(QFileDialog::AcceptSave);
  fdiag->setLabelText(QFileDialog::Accept,"Save");
  fdiag->setConfirmOverwrite(true);
  fdiag->setDefaultSuffix("svg");

  // open dialog: run and save settings
  QString filename="";
  if(fdiag->exec()) {
    if(fdiag->selectedFiles().size()==1) 
      filename=fdiag->selectedFiles().at(0);
  }
  settings.setValue("stateFileDialog", fdiag->saveState());
  delete fdiag;

  // bail out on empty
  if(filename=="") return;

  // do save
  ExportSvg(filename);

}




// exit flexfact: close the window (reimplemet qt window)
void FlexFact::closeEvent(QCloseEvent * event){
  // save window geometry
  QSettings settings("Faudes", "FlexFact");
  settings.setValue("geometry", saveGeometry());
  // call base
  QMainWindow::closeEvent(event);
}


// gui slots: reset (implies start)
void FlexFact::ResetState(void) {
  FF_DQM("FlexFact::ResetState()");
  mGScene->Reset();
}

// gui slots: reset
void FlexFact::Start(void) {
  FF_DQM("FlexFact::Start()");
  mRunning=true;
  SetDevice(mDevice);
  mGScene->Run(mRunning);
  mStopSimButton->setEnabled(mRunning);
  mStopSimAct->setEnabled(mRunning);
  mStartSimButton->setEnabled(!mRunning);
  mStartSimAct->setEnabled(!mRunning);
  foreach(QAction* act, mOperatorActions) 
    act->setEnabled(!mRunning);
}

// adjust view
void FlexFact::ShowLogger(void) {
  mLoggerBox->show();
  mImageBox->hide();
  mShowLogAct->setChecked(true);
  mShowImageAct->setChecked(false);
}

// adjust view
void FlexFact::ShowImage(void) {
  mLoggerBox->hide();
  mImageBox->show();
  mShowLogAct->setChecked(false);
  mShowImageAct->setChecked(true);
}

// gui slots: reset
void FlexFact::Stop(void) {
  FF_DQM("FlexFact::Stop()");
  mRunning=false;
  mGScene->Run(mRunning);
  mStopSimButton->setEnabled(mRunning);
  mStopSimAct->setEnabled(mRunning);
  mStartSimButton->setEnabled(!mRunning);
  mStartSimAct->setEnabled(!mRunning);
  foreach(QAction* act, mOperatorActions) 
    act->setEnabled(!mRunning);
}

// run simulator
void FlexFact::TimeStep(void) {
  // sense and report device status
  static QString devrep="";
  const QString& nrep=mGScene->Image()->DeviceStatusString();
  if(nrep!=devrep) Logging(nrep);
  devrep=nrep;
  // sync image with device
  mGScene->Image()->DeviceSynchronize();
  // bail out on pause
  if(!mRunning) return;
  // do simulation
  mGScene->AdvanceTime( 1/FfStyle::mFramesPerSecond);  // 1/sec -> sec
}


// logging
void FlexFact::Logging(QString message) {
  mLogger->textCursor().movePosition(QTextCursor::End);
  mLogger->textCursor().insertText(message);
  mLogger->textCursor().insertText("\n");
  // mLogger->appendPlainText(message);
  if(!mLogger->verticalScrollBar()->isSliderDown())
     mLogger->ensureCursorVisible();
}

// initialise device
void FlexFact::SetDevice(QString dev) {
  // figure device
  if(dev=="") {
    if(QAction* act = qobject_cast<QAction*>(sender())) {
      if(act->isChecked()) dev=act->text();
    }
  }
  if(dev=="") dev="No Device";
  // set
  mGScene->Image()->CloseDevice();
  if(dev=="Simplenet") {
    mGScene->CompileImage();
    mGScene->Image()->InitializeSimplenetDevice();
    mGScene->Image()->StartDevice();
  } else if(dev=="Simplenet (local)") {
    mGScene->CompileImage();
    mGScene->Image()->InitializeSimplenetlocalDevice();
    mGScene->Image()->StartDevice();
  } else if(dev=="Modbus") {
    mGScene->CompileImage();
    mGScene->Image()->InitializeModbusDevice();
    mGScene->Image()->StartDevice();
  } else {
    dev="No Device";
    mGScene->Image()->StopDevice();
  }
  // update action status
  mDevice=dev;
  mSimplenetAct->setChecked(dev=="Simplenet");
  mSimplenetlocalAct->setChecked(dev=="Simplenet (local)");
  mModbusAct->setChecked(dev=="Modbus");
}

// initialise panel
void FlexFact::SetOperatorPanel(int sz) {
  // stop
  Stop();
  mGScene->Image()->CloseDevice();
  // figure
  FF_DQM("FlexFact::SetOperatorPanel()");
  if(sz<0) {
    if(QAction* act = qobject_cast<QAction*>(sender())) {
      QVariant cnt = act->property("cnt");
      sz=cnt.toInt();
    }
  }
  FF_DQM("FlexFact::SetOperatorPanel(): " << sz);
  // normalize
  if(sz<0) sz =0;
  sz = (sz/4) * 4;
  if(sz>16) sz=16;
  // set panel
  mOperatorPanel->SetSize(sz);
  mGScene->OperatorPanelSize(sz);
  // set gui feedback
  foreach(QAction* act, mOperatorActions) {
    QVariant cnt = act->property("cnt");
    if(!cnt.isValid()) continue;
    FF_DQM("FlexFact::SetOperatorPanel(): action " << cnt.toInt());
    act->setChecked(cnt.toInt()==sz);
  }
  // vissible box
  mOperatorBox->setVisible(sz>0);
}

// initialise monitor
void FlexFact::ShowMonitor(bool show) {
  FF_DQM("FlexFact::ShowMonitor(): " << show);
  mMonitorBox->setVisible(show);
  mMonitorAct->setChecked(show);
}

// helper
QString FlexFact::StrippedName(const QString &fullFileName) {
  return QFileInfo(fullFileName).baseName();
}

/*
************************************************
************************************************
  
Implementation: main()

************************************************
************************************************
*/

// static stream to receive for debugging messages (only windows)
QTextStream mainLogStream;

// handler to redirect redirect debug messages to file (only windows)
void mainMessageHandler(QtMsgType mtype, const char *msg) {
  (void) mtype;
  mainLogStream << QString(msg) << endl;
}


// application entry
int main(int argc, char *argv[]) {

  // osx 11: fix fonts (nee to be before QApplication)
#ifdef Q_OS_MACX
  QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
  QFont::insertSubstitution(".Helvetica Neue DeskInterface", "Helvetica Neue");
  QFont::insertSubstitution(".SF NS Text", "Helvetica Neue");
#endif

  // let Qt see commandline
  QApplication app(argc, argv);

  // lazy commandline, get filename and options
  bool dook=true;
  bool dowincon=false;
  QStringList qargs = QCoreApplication::arguments();  
  for(int i=1; i<qargs.size(); i++) {
    QString option = qargs.at(i);
    // win console
    if(option=="-d") { 
      dowincon=true;
      continue;
    }
    // break on no option
    dook=false;
    break;
  }

#ifdef FF_WINCONSOLE
  // console debugging
  if(dowincon)
  if(AllocConsole()) {
    freopen("CONOUT$","wb",stdout);
    freopen("CONOUT$","wb",stderr);
  }
  // redirect console-out to file  
  if(!dowincon){ 
    QTemporaryFile* mainLogFile= new QTemporaryFile(QDir::tempPath()+ QDir::separator()+ "flexfact_log_XXXXXX");
    mainLogFile->setAutoRemove(false);
    mainLogFile->open();
    mainLogStream.setDevice(mainLogFile);
    FF_WARN("main(): redirect to logfile at " << mainLogFile->fileName());
    qInstallMsgHandler(mainMessageHandler);
    FF_WARN("flexfact logfile");
  }

#endif

  // report commandline errors
  if(!dook) {
    std::cout << "usage: flexfact [-d]" << std::endl;  
    return 1;
  }


#ifdef Q_OS_WIN32  
#ifdef FF_LIBFAUDES
  // if we have a net device on windows, initialise winsocks
  FF_WARN("main(): about to init winsock");
  WSADATA wsaData;
  if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0) {
    FF_WARN("cannot open netdevice (network error)");
    return 1;
  }
  FF_WARN("main(): init winsock: ok");
#endif
#endif


  // add my searchpath for plugins (obsolete?)
  QApplication::addLibraryPath(QCoreApplication::applicationDirPath() + QDir::separator()+ "plugins"); 

  // set application id
  QApplication::setApplicationName("FlexFact");
  QApplication::setOrganizationName("Faudes");
  QApplication::setOrganizationDomain("faudes.org");

  // initialize global style parameters
  FfStyle::Initialise();

  // have my main window restored
  FlexFact *win = new FlexFact();
  QSettings settings("Faudes", "FlexFact");
  win->restoreGeometry(settings.value("geometry").toByteArray());
  win->show();


#ifdef Q_OS_MAC
#ifdef FF_OSXFULLSCREEN
  // add osx lion fullscreen decoration (for the fun of it)
  #include "osxfullscreen.h"
  OsxFullScreen(win);
#endif
#endif

  // load recent file if there is such
  QStringList files = settings.value("recentFileList").toStringList();
  if(files.size()>0) {
    QString recent=files.at(0);
    if(QFileInfo(recent).isReadable()) win->Open(recent);
  }
  int res=0;
#ifdef FF_LIBFAUDES
  try { 
    FF_WARN("main(): enter qt event loop");
    res= app.exec();
  } catch (faudes::Exception& fexcep) {
    QString err=QString("Details: ")+FfStyle::QStrFromStr(fexcep.What());
    QMessageBox errbox;
    errbox.setText(QString("<p><b>libFAUDES/FlexFact Exception</b></p>"));
    errbox.setInformativeText(QString("<p>%1</p><p><b>This indicates a fatal error. Sorry.</b></p>").arg(err));
    errbox.setWindowTitle("FlexFact");
    errbox.setIcon(QMessageBox::Information);
    errbox.addButton(
      "Ok", QMessageBox::YesRole);
    errbox.exec();
    exit(1);
  }
#else
  res= app.exec();
#endif
  return res;
}
