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
#include "mdtClPathGraphicsLink.h"
#include "mdtClPathGraphicsConnection.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QLineF>
#include <QStringList>
#include <QFont>
#include <QFontMetrics>

//#include <QDebug>

mdtClPathGraphicsLink::mdtClPathGraphicsLink(mdtClPathGraphicsConnection *startConnection, mdtClPathGraphicsConnection *endConnection) 
{
  Q_ASSERT(startConnection != 0);
  Q_ASSERT(endConnection != 0);

  setAcceptedMouseButtons(0);
  pvStartConnection = startConnection;
  pvEndConnection = endConnection;
  pvStartConnection->addLink(this);
  pvEndConnection->addLink(this);
  adjust();
}

void mdtClPathGraphicsLink::setText(const QString &text)
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
  pvTextSize.setWidth(width);
  pvTextSize.setHeight(height + space);
  updateBoundingRect();
}

mdtClPathGraphicsConnection *mdtClPathGraphicsLink::startConnection() 
{
  return pvStartConnection;
}

mdtClPathGraphicsConnection *mdtClPathGraphicsLink::endConnection() 
{
  return pvEndConnection;
}

void mdtClPathGraphicsLink::adjust() 
{
  if((pvStartConnection == 0)||(pvEndConnection == 0)){
    return;
  }
  // Get central point of start and end connection
  QLineF line(mapFromItem(pvStartConnection, 0.0, 0.0), mapFromItem(pvEndConnection, 0.0, 0.0));
  // Recommanded in Qt's doc, because we will affect boundingRect() here
  prepareGeometryChange();
  /// \todo Implement a method in mdtClPathGraphicsConnection to get a valid attachement point
  pvStartPoint = line.p1();
  pvEndPoint = line.p2();
  updateBoundingRect();
}

int mdtClPathGraphicsLink::type() const 
{
  return Type;
}

QRectF mdtClPathGraphicsLink::boundingRect() const 
{
  if((pvStartConnection == 0)||(pvEndConnection == 0)){
    return QRectF();
  }
  return pvBoundingRect;
}

void mdtClPathGraphicsLink::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) 
{
  QRectF textRect;

  if((pvStartConnection == 0)||(pvEndConnection == 0)){
    return;
  }
  QLineF line(pvStartPoint, pvEndPoint);
  if(qFuzzyCompare(line.length(), qreal(0.0))){
    return;
  }
  textRect.setX(pvStartPoint.x() + (line.dx() - pvTextSize.width()) / 2.0);
  textRect.setY(pvStartPoint.y() + line.dy() / 2.0 - pvTextSize.height());
  textRect.setSize(pvTextSize);
  painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter->drawLine(line);
  painter->drawText(textRect, Qt::AlignHCenter | Qt::TextDontClip, pvText);
}

void mdtClPathGraphicsLink::updateBoundingRect()
{
  qreal penWidth = 1.0;
  qreal extra = (penWidth + 2.0) / 2.0;
  qreal width;
  qreal height;
  qreal xOffset;

  width = qAbs(pvEndPoint.x() - pvStartPoint.x());
  if(width < pvTextSize.width()){
    xOffset = (pvTextSize.width() - width) / 2.0;
    width = pvTextSize.width();
  }else{
    xOffset = 0.0;
  }
  height = qAbs(pvEndPoint.y() - pvStartPoint.y());
  height = qMax(height, height/2.0 + pvTextSize.height());
  if(pvStartPoint.x() < pvEndPoint.x()){
    pvBoundingRect.setX(pvStartPoint.x() - xOffset);
  }else{
    pvBoundingRect.setX(pvEndPoint.x() - xOffset);
  }
  if(pvStartPoint.y() < pvEndPoint.y()){
    pvBoundingRect.setY(pvEndPoint.y() - height);
  }else{
    pvBoundingRect.setY(pvStartPoint.y() - height);
  }
  pvBoundingRect.setWidth(width);
  pvBoundingRect.setHeight(height);
  pvBoundingRect = pvBoundingRect.normalized();
  pvBoundingRect = pvBoundingRect.adjusted(-extra, -extra, extra, extra);
}
