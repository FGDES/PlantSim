/* ffstyle.h  - flexfact configuration */

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

#ifndef FAUDES_FFSTYLE_H
#define FAUDES_FFSTYLE_H


#include <cmath>
#include <string>
#include <iostream>
#include <bitset>


#include <QtGui>
#include <QPalette>
#include <QDebug>

// version (is set in project file)
#ifndef FF_VERSION
#define FF_VERSION "0.7x"
#endif

// debugging: verry verbose level
#ifdef FF_DEBUG_VIO
#define FF_DQ(message) qDebug() << "FAUDES_VERBOSE: " << message
#else
#define FF_DQ(message) { };
#endif

// debugging: plant simulation
#ifdef FF_DEBUG_PLANT
#define FF_DQM(message) qDebug() << "FAUDES_PLANTSIM: " << message
#else
#define FF_DQM(message)
#endif

// debugging: networking
#ifdef FF_DEBUG_NETWORK
#define FF_DQN(message) qDebug() << "FAUDES_NETWORK: " << message
#else
#define FF_DQN(message)
#endif

// debugging: warning (not mutable)
#define FF_WARN(message) qWarning() << "FAUDES_WARNING: " << message

// convenience typedef: symbolic built-in colors
typedef enum { 
  VioNoColor, 
  VioBlack, VioDarkGrey, VioGrey, VioLightGrey, VioLighterGrey, VioAllmostWhite, VioWhite, 
  VioOldLace, VioBabyBlue,
  VioFirstColor, 
  VioRed=VioFirstColor, VioGreen, VioBlue, 
  VioYellow, VioMagents, VioCyan,
  VioDarkRed, VioDarkGreen, VioDarkBlue, 
  VioLightRed, VioLightGreen, VioLightBlue, 
} VioColor;

/*
 ************************************************
 ************************************************

 Container for static parameters and helper
 functions; e.g. colors and drawing primitives,
 simulation framerate etc.

 Note: this is a stripped down version from
 the viodes/DESTool configuration base class VioStyle;
 it still contains some artefacts from its origin
 and needs clean up.


 ************************************************
 ************************************************
 */


class FfStyle {

public:

  // construct/destruct
  FfStyle(void);
  virtual ~FfStyle(void) {};

  // load hard-coded default
  static void Initialise();

  // global configuration  from file
  // notes: when first invoked, the filename is recorded and used for further invocations
  // as a default; thus, derived classes can read the file again to find their additional
  // sections. when first invoked with no filename specified, a platform dependent 
  // default path is used eg "(executablepath)/vioconfig.txt"; this is a leftover
  // from DESTool and was never used for FlexFact
  static void ReadFile(const QString& filename="");

  // viodes: configuration name and file
  static const QString& ConfigFile(void) { return mConfigFile; };
  static const QString& ConfigName(void) { return mConfigName; };


  // license/copyright text
  static const QString& LicenseText(void) { return mLicenseText; };
  static const QString& ContributorsText(void) { return mContributorsText; };
  static const QString& VersionText(void) { return mVersionText; };

  // faudes: convert plain strings
  static std::string StrFromQStr(const QString& qstr);
  static std::string LfnFromQStr(const QString& qstr);
  static QString QStrFromStr(const std::string& str);

  // drawing: default pen/font
  static const QFont& DefaultFont(void) { return *mpDefaultFont; };
  static const QFontMetricsF& DefaultFontMetrics(void) { return *mpDefaultFontMetrics; };
  static const QFont& DefaultHugeFont(void) { return *mpDefaultHugeFont; };
  static const QFontMetricsF& DefaultHugeFontMetrics(void) { return *mpDefaultHugeFontMetrics; };

  // drawing pens
  static const QPen& DefaultPen(void) { return *mpDefaultPen; };
  static const QBrush& DefaultBrush(void) { return *mpDefaultBrush; };
  static const QPen& HighlitePen(void) { return *mpHighlitePen; };
  static const QPen& WarningPen(void) { return *mpWarningPen; };
  static const QBrush& WarningBrush(void) { return *mpWarningBrush; };

  // drawing: static parameters
  static const qreal& LineShapeSize(void) { return mLineShapeSize;};
  static const qreal& CtrlTolerance(void) { return mCtrlTolerance;};
  static const qreal& GridWidth(void) { return mGridWidth;};
  static const qreal& ExportResolution(void) { return mExportResolution;};
  static const qreal& ExportMaxSize(void) { return mExportMaxSize;};
  
  // drawing: translate color
  static int Colors(void);
  static void Color(const QString& name, const QColor& color);
  static const QColor& Color(int color);
  static const QColor& Color(const QString& color);
  static int ColorIndex(const QString& color);
  static const QString& ColorName(int idx);
  static QList<QString> ColorNames(void);

  // drawing: manipulate colors
  static QColor ColorTaint(const QColor& value, const QColor& huecol, qreal sat);
  static QColor ColorGrey(const QColor& value);

  // drawing: 2D vectors
  static qreal NormF(const QPointF& point);
  static QPointF NormalizeF(const QPointF& point);
  static QPointF NormalF(const QPointF& point);
  static QPointF LotF(const QPointF& base, const QPointF& point);
  static qreal ScalarF(const QPointF& a, const QPointF& b);

  // drawing: draw text
  static void TextLL(QPainterPath& path, const QPointF& pos, const QString& text);
  static void TextLL(QRectF& drect, const QPointF& pos, const QString& text);
  static void TextCR(QPainterPath& path, const QRectF& rect, const QString& text);
  static void TextCR(QRectF& drect, const QRectF& rect, const QString& text);
  static void TextCP(QPainterPath& path, const QPointF& center, const QString& text);
  static void TextCP(QRectF& drect, const QPointF& center, const QString& text);


  // drawing: magnetic grid points
  static QPointF Grid200(const QPointF& point);
  static QPointF Grid400(const QPointF& point);
  static QPoint  Grid200I(const QPointF& point);
  
  // how many frames per second to render
  // (since graphcis and simulation are implemented in sync,
  // you can not get too low here, otherwise blocking will be missdetected)
  static qreal mFramesPerSecond;

  // velocity of items is set up sucht that operations take approx a second
  // use pitch to globally slow things down or speen them up
  static qreal mPitch;



private:

  // color defs
  static QList<QColor> mColorDefs;
  static QMap<QString,int>  mColorIndexes;
  static QMap<int,QString>  mColorNames;

  // static member declaration
  static QString mConfigFile;
  static QString mConfigName;
  static QString mLicenseText;
  static QString mContributorsText;
  static QString mVersionText;
  static qreal mLineShapeSize;
  static qreal mCtrlTolerance;
  static qreal mGridWidth;
  static qreal mExportResolution;
  static qreal mExportMaxSize;
  static QPen*   mpDefaultPen;
  static QBrush* mpDefaultBrush;
  static QPen*   mpHighlitePen;
  static QPen*   mpWarningPen;
  static QBrush*   mpWarningBrush;
  static QFontMetricsF* mpDefaultFontMetrics;
  static QFont* mpDefaultFont;
  static QFontMetricsF* mpDefaultHugeFontMetrics;
  static QFont* mpDefaultHugeFont;

protected:


};


/*
operator < for qpoints
*/

bool operator<(const QPoint& a, const QPoint& b);

/*
operator << for c++ strings
*/

QDebug& operator<<(QDebug& qdbg, const std::string &str);

#endif
