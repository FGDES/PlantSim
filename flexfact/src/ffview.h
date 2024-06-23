/*ffview.h  - qgraphicsview for ggscene */

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

#ifndef FAUDES_FFVIEW_H
#define FAUDES_FFVIEW_H

#include <QtGui>

#include "ffscene.h"

/*
 ************************************************
 ************************************************

 A FfView is a QGraphicsView intended to show a 
 FfScene.  It adds zoom via mouse wheel and 
 other view-dependant features.

 ************************************************
 ************************************************
 */


 
class FfView : public QGraphicsView {
  Q_OBJECT

public:
  // construct from scene
  FfView(QWidget* parent=0);

  // zoom helpers
  void Reset(void);
  void Scale(qreal sc);
  qreal Scale(void);

public slots:

  // user zoom
  void ZoomFit(void);
  void ZoomIn(qreal sf=1.2);
  void ZoomOut(qreal sf=1/1.2);

signals:

  // report user zoom
  void NotifyZoom(qreal sf);



protected:

  // reimplement mouse events
  void wheelEvent(QWheelEvent *event);
  void mousePressEvent(QMouseEvent * event);
  void mouseReleaseEvent(QMouseEvent * event);
  void mouseDoubleClickEvent(QMouseEvent * event);
  void mouseMoveEvent(QMouseEvent * event);



};


#endif
