/* FfStyle.cpp  - flexfact configuration */

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

#include "ffstyle.h"


/*
 *************************************
 *************************************
 *************************************

 Implementation of FfStyle: static vars

 *************************************
 *************************************
 *************************************
 */

// static instance
FfStyle* mpStaticInstance=0;

// static default: name of this configuration
QString FfStyle::mConfigName = "VioStdConfig";
QString FfStyle::mConfigFile = "";

// static: license 
QString FfStyle::mLicenseText =
   "<p><b>License:</b> "
   "This copy of FlexFact is distributed under the terms of the GNU General Public License "
    "as published by the Free Software Foundation, either version 3 of the License, or "
    "any later version. FlexFact is distributed in the hope that it will be useful, "
    "but without any warranty. See the provided file LICENSE.txt for more details.</p>";

// static: contributors
QString FfStyle::mContributorsText =
   "Christine Baier, Klaus Schmidt, Zhengying Wang, Thomas Wittmann, et al";

// static: version
QString FfStyle::mVersionText =
   FF_VERSION;

// static default: tolerance in selection
qreal FfStyle::mLineShapeSize= 10.0;
qreal FfStyle::mCtrlTolerance = 7;

// static default: mesh width of grid 
qreal FfStyle::mGridWidth=10;

// static default: bitmap for graphics export
qreal FfStyle::mExportResolution = 10; // pixel per point
qreal FfStyle::mExportMaxSize= 3000;   // pixels

// static pens
QPen*   FfStyle::mpDefaultPen;
QBrush* FfStyle::mpDefaultBrush;
QPen*   FfStyle::mpHighlitePen;
QPen*   FfStyle::mpWarningPen;
QBrush* FfStyle::mpWarningBrush;

// static fonts
QFontMetricsF* FfStyle::mpDefaultFontMetrics;
QFont* FfStyle::mpDefaultFont;
QFontMetricsF* FfStyle::mpDefaultHugeFontMetrics;
QFont* FfStyle::mpDefaultHugeFont;

// static color defs
QList<QColor> FfStyle::mColorDefs;
QMap<QString,int>  FfStyle::mColorIndexes;
QMap<int,QString>  FfStyle::mColorNames;



/*
 *************************************
 *************************************
 *************************************

 Implementation of FfStyle: setup

 *************************************
 *************************************
 *************************************
 */



// constructor (used for static instance)
FfStyle::FfStyle(void) {}

// hard-coded defaults
void FfStyle::Initialise(void) {
  // we are initialized
  if(mpStaticInstance) return;
  // report
  FF_DQ("FfStyle::Initialise() ");
  // static instance 
  mpStaticInstance=new FfStyle();
  // clear colors
  mColorDefs.clear();
  mColorNames.clear();
  // built in colors (order must match enum ... )
  Color("NoColor",QColor(255, 255, 255));
  Color("Black",  QColor(0, 0, 0));
  Color("DarkGrey",   QColor(50, 50, 50));
  Color("Grey",   QColor(80, 80, 80));
  Color("LightGrey",   QColor(175, 175, 175));
  Color("LighterGrey",   QColor(210, 210, 210));
  Color("AllmostWhite",  QColor(230, 230, 230));
  Color("White",  QColor(255, 255, 255));
  Color("OldLace",  QColor(240, 240, 160));
  Color("BabyBlue",  QColor(180, 180, 240));
  Color("Red",    QColor(180, 0, 0));
  Color("Green",  QColor(0, 180, 0));
  Color("Blue",   QColor(0, 0, 180));
  Color("Yellow", QColor(128, 128, 0));
  Color("Magenta",QColor(96, 0, 96));
  Color("Cyan",   QColor(0, 96, 96));
  Color("DarkRed",    QColor(80, 0, 0));
  Color("DarkGreen",  QColor(0, 80, 0));
  Color("DarkBlue",   QColor(0, 0, 80));
  Color("LightRed",    QColor(255, 80, 80));
  Color("LightGreen",  QColor(80, 255, 80));
  Color("LightBlue",   QColor(80, 80, 255));
  // pen
  mpDefaultPen=new QPen();
  mpDefaultPen->setStyle(Qt::SolidLine);
  mpDefaultPen->setBrush(Qt::SolidPattern);
  mpDefaultPen->setBrush(Qt::black);
  mpDefaultPen->setWidthF(1);
  // brush
  mpDefaultBrush= new QBrush();
  mpDefaultBrush->setStyle(Qt::SolidPattern);
  mpDefaultBrush->setColor(Qt::black);
  // highlight pen   
  mpHighlitePen=new QPen();
  mpHighlitePen->setStyle(Qt::SolidLine);
  mpHighlitePen->setBrush(Qt::SolidPattern);
  mpHighlitePen->setBrush(Qt::red);
  mpHighlitePen->setWidthF(10);
  // warning pen   
  mpWarningPen=new QPen();
  mpWarningPen->setStyle(Qt::SolidLine);
  mpWarningPen->setBrush(FfStyle::Color("Red"));
  mpWarningPen->setWidthF(1);
  // brush
  mpWarningBrush= new QBrush();
  mpWarningBrush->setStyle(Qt::SolidPattern);
  mpWarningBrush->setColor(FfStyle::Color("Red"));
  // fonts
  mpDefaultFont = new QFont();
  mpDefaultFontMetrics = new QFontMetricsF(*mpDefaultFont,0);
  mpDefaultHugeFont = new QFont();
  mpDefaultHugeFont->setPointSize(48);
  mpDefaultHugeFont->setWeight(QFont::Bold);
  mpDefaultHugeFontMetrics = new QFontMetricsF(*mpDefaultFont,0);
}

// read config file 
void FfStyle::ReadFile(const QString& filename) {
  // start with defaults
  Initialise();
  // record filename
  if(filename!="") mConfigFile=filename;
  // built in default
  if(mConfigFile=="") mConfigFile=QCoreApplication::applicationDirPath() + "/vioconfig.txt";
  // have token reader
  std::string fname=FfStyle::StrFromQStr(mConfigFile);
  FF_DQ("FfStyle::Config: read file " << fname);
  /*
  note: if this ever gets implemented, we use Qt/XML rather than
  libfaufes tokenreader

  faudes::TokenReader tr(fname);
  // find my section
  tr.SeekBegin("VioConfig");
  // record name of configuration
  mConfigName=QStrFromStr(tr.ReadString());
  // loop my section
  while(!tr.Eos("VioConfig")) {
    faudes::Token token;
    tr.Peek(token);
    if(token.Type()!=faudes::Token::Begin) { tr.Get(token); continue; }
    // dotpath
    if(token.StringValue()=="DotPath") {
      tr.ReadBegin("DotPath");
      mDotExecutable=tr.ReadString();
      tr.ReadEnd("DotPath");
      continue;
    }
    // ... add more sections here (todo: fonts, colors etc)
    // skip unkown section
    tr.ReadBegin(token.StringValue());
    tr.ReadEnd(token.StringValue());
  }
  // done
  tr.ReadEnd("VioConfig");
  */
  FF_DQ("FfStyle::Config:done ");
}






// string conversion
std::string FfStyle::StrFromQStr(const QString& qstr) {
    return std::string(qstr.toUtf8().constData()); };
std::string FfStyle::LfnFromQStr(const QString& qstr) {
    return std::string(qstr.toLocal8Bit().constData()); };
QString FfStyle::QStrFromStr(const std::string& str) {
  return QString::fromUtf8(str.c_str());}



/*
 *************************************
 *************************************
 *************************************

 Implementation of FfStyle: graphics

 *************************************
 *************************************
 *************************************
 */




// helper: fix hue and sat, keep val
QColor FfStyle::ColorTaint(const QColor& valcol, const QColor& huecol, qreal sat) {
  qreal hue    =  huecol.hueF();
  qreal val    =  valcol.valueF();
  //qreal valsat =  valcol.saturationF();
  QColor result(0,0,0);
  result.setHsvF(hue,sat,val);
  return result;
}

// helper: mute saturation
QColor FfStyle::ColorGrey(const QColor& valcol) {
  qreal val    =  valcol.valueF();
  static qreal minval = .2;
  static qreal maxval = .5;
  val = val* (maxval-minval)+minval;
  QColor result(0,0,0);
  result.setHsvF(0,0,val);
  return result;
}


// helper: euclidian norm (or zero if too small)
qreal FfStyle::NormF(const QPointF& point) {
  qreal len=sqrt(pow(point.x(),2)+pow(point.y(),2));
  if(len < 1.0) len=0;
return len;
} 

// helper: scale to euclidian norm (or zero if too small)
QPointF FfStyle::NormalizeF(const QPointF& point) {
  qreal len = NormF(point);
  if(len==0.0) return QPointF(0,0);
  return 1/len * point;
}

// helper: normal aka orthogonal unit length (or zero if too small)
QPointF FfStyle::NormalF(const QPointF& point) {
  qreal len = NormF(point);
  if(len==0.0) return QPointF(0,0);
  QPointF res;
  res.ry()=point.x();
  res.rx()=-point.y();
  return 1/len * res;
}

// helper: extend base to "lot (EL?)" of x (base being normalized)
QPointF FfStyle::LotF(const QPointF& base, const QPointF& point) {
  return base * (base.x()*point.x()+base.y()*point.y());
} 

// helper: scalar product
qreal FfStyle::ScalarF(const QPointF& b, const QPointF& a) {
  return a.x()*b.x()+a.y()*b.y();
} 

// add color to Qt color
void FfStyle::Color(const QString& name, const QColor& color) {
  int pos;
  if(mColorIndexes.contains(name)) {
    FF_DQ("FfStyle::Color(): redefining " << FfStyle::StrFromQStr(name));
    pos=mColorIndexes[name];
    mColorDefs[pos]=color;
  } else {
    pos=mColorDefs.size();
    FF_DQ("FfStyle::Color(): defining " << FfStyle::StrFromQStr(name) << " with idx " << pos);
    mColorIndexes[name]=pos;
    mColorNames[pos]=name;
    mColorDefs.append(color);
 }    
}

// number of colors defined
int FfStyle::Colors(void) {
  return mColorDefs.size();
}

// translate symbolic color to Qt color
const QColor& FfStyle::Color(int color) {
  //FF_DQ("FfStyle::Color(): looking up " << color);
  if(color>=0 && color < mColorDefs.size()) return mColorDefs.at(color);
  return mColorDefs.at((int) VioBlack);
}

// translate color name to Qt color
const QColor& FfStyle::Color(const QString& color) {
  //FF_DQ("FfStyle::Color(): looking up " << FfStyle::StrFromQStr(color));
  if(mColorIndexes.contains(color)) return Color(mColorIndexes[color]);
  return mColorDefs.at((int) VioBlack);
}

// translate color name to color index
int FfStyle::ColorIndex(const QString& color) {
  //FF_DQ("FfStyle::Color(): looking up " << FfStyle::StrFromQStr(color));
  if(!mColorIndexes.contains(color)) return VioNoColor;
  return mColorIndexes[color];
}

// translate color index to color name
const QString& FfStyle::ColorName(int idx) {
  //FF_DQ("FfStyle::Color(): looking up " << idx );
  if(!mColorNames.contains(idx)) return ColorName(VioNoColor);
  return mColorNames[idx];
}

// return all known colors
QList<QString> FfStyle::ColorNames(void) {
  QList<QString> res;
  foreach(const QString& name, mColorNames)
    res.append(name);
  return res;
}


// helper: magnetic grid
QPointF FfStyle::Grid200(const QPointF& point) {
  QPointF res;
  if(point.x()>=0) 
    res.rx()=((long) (point.x()/200+0.5))*200;
  else
    res.rx()=-((long) (-point.x()/200+0.5))*200;
  if(point.y()>=0) 
    res.ry()=((long) (point.y()/200+0.5))*200;
  else
    res.ry()=-((long) (-point.y()/200+0.5))*200;
  return res;
}

// helper: magnetic grid
QPoint FfStyle::Grid200I(const QPointF& point) {
  QPoint res;
  if(point.x()>=0) 
    res.rx()=(long) ((point.x()+100)/200);
  else
    res.rx()=-(long) ((-point.x()+100)/200);
  if(point.y()>=0) 
    res.ry()=(long) ((point.y()+100)/200);
  else
    res.ry()=-(long) ((-point.y()+100)/200);
  return res;
}

// helper: magnetic grid
QPointF FfStyle::Grid400(const QPointF& point) {
  QPointF res;
  if(point.x()>=0) 
    res.rx()=((long) (point.x()/400+0.5))*400;
  else
    res.rx()=-((long) (-point.x()/400+0.5))*400;
  if(point.y()>=0) 
    res.ry()=((long) (point.y()/400+0.5))*400;
  else
    res.ry()=-((long) (-point.y()/400+0.5))*400;
  return res;
}

// draw text, lower left point given
void FfStyle::TextLL(QPainterPath& path, const QPointF& pos, const QString& text) {
  path.addText(pos,*mpDefaultFont,text);
}

// text rect, lower left point given
void FfStyle::TextLL(QRectF& drect, const QPointF& pos, const QString& text) {
  drect=mpDefaultFontMetrics->boundingRect(text);
  drect.translate(pos);
}

// draw text, center point given
void FfStyle::TextCP(QPainterPath& path, const QPointF& center, const QString& text) {
  QRectF outer;
  TextLL(outer,center,text);
  QPointF offset=center - outer.center(); 
  TextLL(path,center+offset,text);
}

// text rect, center point given
void FfStyle::TextCP(QRectF& drect, const QPointF& center, const QString& text) {
  TextLL(drect,center,text);
  QPointF offset=center - drect.center(); 
  drect.translate(offset);
}

// draw text, center given by rect
void FfStyle::TextCR(QPainterPath& path, const QRectF& rect, const QString& text) {
  TextCP(path,rect.center(),text);
}

//  text rect, center given by rect
void FfStyle::TextCR(QRectF& drect, const QRectF& rect, const QString& text) {
  TextCP(drect,rect.center(),text);
}



// simulation parameters
qreal FfStyle::mPitch=0.5;
qreal FfStyle::mFramesPerSecond=50;



/*
 *************************************
 *************************************
 *************************************

 Operators

 *************************************
 *************************************
 *************************************
 */






/*
operator < for qpoints
*/

bool operator<(const QPoint& a, const QPoint& b) {
  if(a.x()<b.x()) return true;
  if(a.x()>b.x()) return false;
  if(a.y()<b.y()) return true;
  return false;
}


/*
operator << for c++ strings
*/

QDebug& operator<<(QDebug& qdbg, const std::string &str) {
  return ( qdbg <<  FfStyle::QStrFromStr(str) );
}



