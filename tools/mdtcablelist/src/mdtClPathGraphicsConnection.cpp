/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include <QStyleOptionGraphicsItem>
#include <QFont>
#include <QFontMetrics>
#include <QStringList>

#include <QDebug>

mdtClPathGraphicsConnection::mdtClPathGraphicsConnection()
{
  setFlag(ItemIsMovable);
  setFlag(ItemSendsGeometryChanges);
  setCacheMode(DeviceCoordinateCache);
  setZValue(-1);
  setCircleDiameter(15);
}

mdtClPathGraphicsConnection::~mdtClPathGraphicsConnection()
{
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
  pvTextRect.setY(pvCircleRect.y() - space - height);
  pvTextRect.setWidth(width);
  pvTextRect.setHeight(height);
  updateBoundingRect();
}

void mdtClPathGraphicsConnection::setCircleDiameter(qreal diameter)
{
  pvCircleRect.setX(-diameter/2.0);
  pvCircleRect.setY(-diameter/2.0);
  pvCircleRect.setWidth(diameter);
  pvCircleRect.setHeight(diameter);
  // Update text rect
  setText(pvText);
  updateBoundingRect();
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

  path.addEllipse(pvCircleRect);
  ///path.addText(pvTextRect.topLeft(), QFont(), pvText);

  return path;
}

void mdtClPathGraphicsConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) 
{
  Q_ASSERT(painter != 0);
  Q_ASSERT(option != 0);

  painter->setPen(Qt::NoPen);
  painter->setBrush(Qt::blue);
  painter->drawEllipse(pvCircleRect);
  painter->setPen(Qt::black);
  painter->setBrush(Qt::red);
  painter->setFont(QFont("Arial", 10));
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

void mdtClPathGraphicsConnection::updateBoundingRect()
{
  qreal x, y, width, height;
  qreal margin = 2.0;

  x = qMin(-pvCircleRect.width()/2.0, -pvTextRect.width()/2.0) - margin;
  y = -pvCircleRect.height()/2.0 - pvTextRect.height() - margin;
  width = qMax(pvCircleRect.width(), pvTextRect.width()) + 2.0*margin;
  height = pvCircleRect.height() + pvTextRect.height() + 2.0*margin;
  pvBoundingRect.setX(x);
  pvBoundingRect.setY(y);
  pvBoundingRect.setWidth(width);
  pvBoundingRect.setHeight(height);
}
