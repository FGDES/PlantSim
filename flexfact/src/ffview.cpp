/* ffview.cpp  - qgraphicsview for ffscene */

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

#include "ffview.h"

#define MAXSC 1
#define MINSC 0.1

// construct
FfView::FfView(QWidget* parent) :  QGraphicsView(parent) {
  FF_DQ("FfView::FfView(" << parent << ")");
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  setCacheMode(QGraphicsView::CacheBackground); // need to resilve issues with backgound
  setDragMode(RubberBandDrag);
  setMouseTracking(true);
  FF_DQ("FfView::FfView(): done");
}

// Scale
void FfView::Scale(qreal sc) {  
  setMatrix(QMatrix(sc,0,0,sc,0,0));
}

// Scale
qreal FfView::Scale(void) {  
  return matrix().m11();
}


// Reset Matrix
void FfView::Reset(void) {
  setMatrix(QMatrix());
}

// fit scene
void FfView::ZoomFit(void){
  FfScene* gs = qobject_cast<FfScene*>(scene());
  if(!gs) return;
  // set scene rectangle
  QRectF rect=gs->itemsBoundingRect();
  rect.adjust(-0.01*gs->width(),-0.01*gs->height(),
    0.01*gs->width(),0.01*gs->height());
  gs->setSceneRect(rect);
  // default: fit entire scene
  QRectF target = gs->sceneRect();
  // alt: fit selection
  if(gs->selectedItems().size() >= 1) {
    target= QRect();
    foreach(QGraphicsItem* item, gs->selectedItems()) {
      QPolygonF itempoly =item->mapToScene(item->boundingRect());
      target |=itempoly.boundingRect();
    }
  }
  // doit
  fitInView(target, Qt::KeepAspectRatio); 
  // fix bounds
  qreal sc = Scale();
  if(sc>MAXSC)  sc=MAXSC;
  if(sc<MINSC) sc=MINSC;
  Scale(sc);
}

// ZoomIn()
void FfView::ZoomIn(qreal sf) {
  FF_DQ("FfView::Zoom(): old scale " << Scale() << " factor " << sf);
  qreal sc = Scale();
  sc *=sf;
  if(sc>MAXSC)  sc=MAXSC;
  if(sc<MINSC) sc=MINSC;
  Scale(sc);
}

// ZoomOut()
void FfView::ZoomOut(qreal sf) {
  ZoomIn(sf);
}


// wheel event
void FfView::wheelEvent(QWheelEvent *event) {
  FF_DQ("FfView::wheelEvent(..) at (" << event->pos().x() << ", " << event->pos().y()
	<< ") with value " << event->delta());
  // accept the event at any rate
  event->accept();
  // scale factor
  qreal degree= ((qreal) event->delta()) / 8.0; 
  if(degree < -30) degree = -30.0;
  if(degree >  30) degree = 30.0;
  qreal sc = 1.0 + ( 5.0 * degree / 3000.0 ); // 5% steps per 30 degree
  // fix bounds 
  if(sc * Scale() >MAXSC) return;
  if(sc * Scale() <MINSC) return;
  // where we are before scalung
  QPointF opos=mapToScene(event->pos());
  FF_DQ("FfView::wheelEvent(..) at scenepos (" << opos.x() << ", " << opos.y() << ")");
  // do the scale
  setResizeAnchor(AnchorUnderMouse);
  setTransformationAnchor(AnchorUnderMouse);
  scale(sc,sc);
  setTransformationAnchor(NoAnchor);
  // translate
  QPointF npos=mapToScene(event->pos());
  QPointF diff=npos-opos;
  QPointF vdiff=mapFromScene(diff);
  translate(diff.x(),diff.y());
  // notify
  emit NotifyZoom(Scale());
};


// handle my events: mouse press
// note: when rubber band drag ends, press void deselects but is not passed to scene
// note: the rubber band also gets irritated when the scene has a context menu
void FfView::mousePressEvent(QMouseEvent * event) {
   FF_DQ("FfView::mousePress()");
   //if(event->button() == Qt::RightButton) return;
   QGraphicsView::mousePressEvent(event);
   /*
   if(event->isAccepted())
     if(FfScene* sc=qobject_cast<FfScene*>(scene())) sc->fixSelection();
   */
}


// handle my events: mouse release
// note: when rubber band drag ends, release is not passed to scene
void FfView::mouseReleaseEvent(QMouseEvent * event) {
   FF_DQ("FfView::mouseRelease()");
   QGraphicsView::mouseReleaseEvent(event);
   if(event->isAccepted())
   if(FfScene* sc=qobject_cast<FfScene*>(scene())) {
      sc->UserSelectionUpdate(); 
   }
   if(event->isAccepted())
   if(FfScene* sc=qobject_cast<FfScene*>(scene())) {
     if(sc->MoveSelection()) event->ignore();
   }
}


// handle my events: mouse double click
void FfView::mouseDoubleClickEvent(QMouseEvent * event) {
   //FF_DQ("FfView::mouseDoubleClickEvent()");
   // pass on
   QGraphicsView::mouseDoubleClickEvent(event);
}

// handle my events: mouse move
void FfView::mouseMoveEvent(QMouseEvent * event) {
  //FF_DQG("FfView::mouseMove()");
   QGraphicsView::mouseMoveEvent(event);
}
