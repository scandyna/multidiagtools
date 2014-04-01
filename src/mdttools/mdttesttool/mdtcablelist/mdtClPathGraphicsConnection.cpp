/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtClPathGraphicsConnection.h"
#include "mdtClPathGraphicsLink.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QTransform>
#include <QFont>
#include <QFontMetrics>
#include <QStringList>
#include <cmath>
#ifndef M_PI
 #define M_PI (3.14159265358979323846)
#endif

//#include <QDebug>

mdtClPathGraphicsConnection::mdtClPathGraphicsConnection()
{
  setFlag(ItemIsMovable);
  setFlag(ItemSendsGeometryChanges);
  setCacheMode(DeviceCoordinateCache);
  setZValue(-1);
  ///setConnectionType(Terminal);
  setConnectionType(Socket);
}

mdtClPathGraphicsConnection::~mdtClPathGraphicsConnection()
{
}

void mdtClPathGraphicsConnection::setConnectionType(mdtClPathGraphicsConnection::ConnectionType t)
{
  pvConnectionType = t;
  switch(pvConnectionType){
    case Terminal:
      setTerminalDiameter(15);
      break;
    case Pin:
      setPinSize(10, 5);
      break;
    case Socket:
      setSocketSize(10, 10);
      break;
  }
}

void mdtClPathGraphicsConnection::setText(const QString &text)
{
  QFont font; // Will get current application font
  QFontMetrics fontMetrics(font);
  qreal width, height;
  qreal space = 5.0;
  QStringList lines;
  int i;

  pvText = text;
  lines = text.split("\n");
  width = 0.0;
  height = 0.0;
  for(i = 0; i < lines.size(); ++i){
    width = qMax(width, (qreal)fontMetrics.width(lines.at(i)));
    height += fontMetrics.height();
  }
  pvTextRect.setX(-width/2.0);
  pvTextRect.setY(pvConnectionRect.y() - space - height);
  pvTextRect.setWidth(width);
  pvTextRect.setHeight(height);
  updateBoundingRect();
}

///void mdtClPathGraphicsConnection::setCircleDiameter(qreal diameter)
void mdtClPathGraphicsConnection::setTerminalDiameter(qreal diameter)
{
  pvConnectionRect.setX(-diameter/2.0);
  pvConnectionRect.setY(-diameter/2.0);
  pvConnectionRect.setWidth(diameter);
  pvConnectionRect.setHeight(diameter);
  // Update text rect
  setText(pvText);
  ///updateBoundingRect();
}

void mdtClPathGraphicsConnection::setPinSize(qreal width, qreal height)
{
  pvConnectionRect.setX(-width/2.0);
  pvConnectionRect.setY(-height/2.0);
  pvConnectionRect.setWidth(width);
  pvConnectionRect.setHeight(height);
  // Update text rect
  setText(pvText);
}

void mdtClPathGraphicsConnection::setSocketSize(qreal width, qreal height)
{
  pvConnectionRect.setX(-width/2.0);
  pvConnectionRect.setY(-height/2.0);
  pvConnectionRect.setWidth(width);
  pvConnectionRect.setHeight(height);
  // Update text rect
  setText(pvText);
}

QPointF mdtClPathGraphicsConnection::nextPos(bool reverse, qreal linkLength, qreal dAlpha) const
{
  qreal alpha;
  qreal x, y;
  qreal newX, newY;
  qreal pi = M_PI;

  if(scene() == 0){
    return QPointF();
  }
  // Get scene position of this item
  x = mapToScene(pvConnectionRect.center()).x();
  y = mapToScene(pvConnectionRect.center()).y();
  // Try to find a point directly on the right (or the left if reverse)
  newY = y;
  if(reverse){
    newX = x - linkLength;
    if(scene()->itemAt(newX, newY, QTransform()) == 0){
      // No item here, we found a point
      return QPointF(newX, newY);
    }
  }else{
    newX = x + linkLength;
    if(scene()->itemAt(newX, newY, QTransform()) == 0){
      // No item here, we found a point
      return QPointF(newX, newY);
    }
  }
  // Try to find a free point around us, alternating with alpha 0, alpha > and alpha < 0
  alpha = dAlpha;
  while((alpha > -pi)&&(alpha < pi)){
    // Try with current alpha
    newX = x + linkLength * cos(alpha);
    newY = y + linkLength * sin(alpha);
    if(scene()->itemAt(newX, newY, QTransform()) == 0){
      // No item here, we found a point
      return QPointF(newX, newY);
    }
    // Try with next alpha
    alpha += dAlpha;
    newX = x + linkLength * cos(alpha);
    newY = y + linkLength * sin(alpha);
    if(scene()->itemAt(newX, newY, QTransform()) == 0){
      // No item here, we found a point
      return QPointF(newX, newY);
    }
    // Try with opposite alpha
    alpha = -alpha;
    newX = x + linkLength * cos(alpha);
    newY = y + linkLength * sin(alpha);
    if(scene()->itemAt(newX, newY, QTransform()) == 0){
      // No item here, we found a point
      return QPointF(newX, newY);
    }
    // No free point here, continue
    alpha = -alpha;
    alpha += dAlpha;
  }

  return QPointF();
}

void mdtClPathGraphicsConnection::addLink(mdtClPathGraphicsLink *link) 
{
  Q_ASSERT(link != 0);

  pvLinkList.append(link);
  link->adjust();
}

QList<mdtClPathGraphicsLink*> mdtClPathGraphicsConnection::links() const
{
  return pvLinkList;
}


int mdtClPathGraphicsConnection::type() const 
{
  return Type;
}

QRectF mdtClPathGraphicsConnection::boundingRect() const 
{
  return pvBoundingRect;
}

QPainterPath mdtClPathGraphicsConnection::shape() const 
{
  QPainterPath path;

  path.addEllipse(pvConnectionRect);

  return path;
}

void mdtClPathGraphicsConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) 
{
  Q_ASSERT(painter != 0);
  Q_ASSERT(option != 0);

  ///painter->setPen(Qt::NoPen);
  ///painter->setPen(QPen(QBrush(Qt::black), 1.0));
  ///painter->setBrush(Qt::blue);
  ///painter->setBrush(Qt::white);
  ///painter->drawEllipse(pvConnectionRect);
  paintConnection(painter, option, widget);
  painter->setPen(Qt::black);
  painter->setBrush(Qt::red);
  painter->drawText(pvTextRect, Qt::AlignHCenter | Qt::TextDontClip, pvText);
}

QVariant mdtClPathGraphicsConnection::itemChange(GraphicsItemChange change, const QVariant &value)
{
  int i;

  switch(change){
    case ItemPositionHasChanged:
      for(i = 0; i < pvLinkList.size(); ++i){
        Q_ASSERT(pvLinkList.at(i) != 0);
        pvLinkList.at(i)->adjust();
      }
      break;
    default:
      break;
  }

  return QGraphicsItem::itemChange(change, value);
}

void mdtClPathGraphicsConnection::paintConnection(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_ASSERT(painter != 0);
  Q_ASSERT(option != 0);

  switch(pvConnectionType){
    case Terminal:
      paintTerminal(painter, option, widget);
      break;
    case Pin:
      paintPin(painter, option, widget);
      break;
    case Socket:
      paintSocket(painter, option, widget);
      break;
  }
}

void mdtClPathGraphicsConnection::paintTerminal(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_ASSERT(painter != 0);
  Q_ASSERT(option != 0);

  painter->setPen(QPen(QBrush(Qt::black), 1.0));
  painter->setBrush(Qt::white);
  painter->drawEllipse(pvConnectionRect);
}

void mdtClPathGraphicsConnection::paintPin(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_ASSERT(painter != 0);
  Q_ASSERT(option != 0);

  painter->setPen(Qt::NoPen);
  painter->setBrush(Qt::black);
  painter->drawRect(pvConnectionRect);
}

void mdtClPathGraphicsConnection::paintSocket(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_ASSERT(painter != 0);
  Q_ASSERT(option != 0);

  painter->setPen(Qt::black);
  ///painter->setBrush(Qt::black);
  ///painter->drawRect(pvConnectionRect);
  painter->drawArc(pvConnectionRect, 1440.0, 2880.0);
}

void mdtClPathGraphicsConnection::updateBoundingRect()
{
  qreal x, y, width, height;
  qreal margin = 2.0;

  x = qMin(-pvConnectionRect.width()/2.0, -pvTextRect.width()/2.0) - margin;
  y = -pvConnectionRect.height()/2.0 - pvTextRect.height() - margin;
  width = qMax(pvConnectionRect.width(), pvTextRect.width()) + 2.0*margin;
  height = pvConnectionRect.height() + pvTextRect.height() + 2.0*margin;
  pvBoundingRect.setX(x);
  pvBoundingRect.setY(y);
  pvBoundingRect.setWidth(width);
  pvBoundingRect.setHeight(height);
}
