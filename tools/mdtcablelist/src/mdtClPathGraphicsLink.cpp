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
#include <QRectF>
#include <QStringList>
#include <QFont>
#include <QFontMetrics>

#include <QDebug>

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
}

int mdtClPathGraphicsLink::type() const 
{
  return Type;
}

QRectF mdtClPathGraphicsLink::boundingRect() const 
{
  QRectF rect;
  qreal penWidth = 1.0;
  qreal extra = (penWidth + 2.0) / 2.0;
  qreal width;
  qreal height;
  qreal xOffset;

  if((pvStartConnection == 0)||(pvEndConnection == 0)){
    return QRectF();
  }
  width = qAbs(pvEndPoint.x() - pvStartPoint.x());
  if(width < pvTextSize.width()){
    xOffset = (pvTextSize.width() - width) / 2.0;
    width = pvTextSize.width();
  }else{
    xOffset = 0.0;
  }
  ///width = qMax(width, pvTextSize.width());
  height = qAbs(pvEndPoint.y() - pvStartPoint.y());
  height = qMax(height, height/2.0 + pvTextSize.height());
  if(pvStartPoint.x() < pvEndPoint.x()){
    ///rect.setX(pvEndPoint.x() - width);
    rect.setX(pvStartPoint.x() - xOffset);
  }else{
    ///rect.setX(pvStartPoint.x() - width);
    rect.setX(pvEndPoint.x() - xOffset);
  }
  /**
  if(pvStartPoint.x() < pvEndPoint.x()){
    rect.setX(pvStartPoint.x() - width/2.0);
  }else{
    rect.setX(pvEndPoint.x() - width/2.0);
  }
  */
  ///rect.setY((pvEndPoint.y() - pvStartPoint.y()) / 2.0 + pvTextSize.height());
  if(pvStartPoint.y() < pvEndPoint.y()){
    rect.setY(pvEndPoint.y() - height);
  }else{
    rect.setY(pvStartPoint.y() - height);
  }
  rect.setWidth(width);
  ///rect.setHeight((pvEndPoint.y() - pvStartPoint.y()) / 2 + pvTextSize.height());
  rect.setHeight(height);
  rect = rect.normalized();
  rect = rect.adjusted(-extra, -extra, extra, extra);
  qDebug() << "boundingRect: " << rect;

  return rect;
  ///return QRectF(pvStartPoint, QSizeF(pvEndPoint.x() - pvStartPoint.x(), pvEndPoint.y() - pvStartPoint.y())).normalized().adjusted(-extra, -extra, extra, extra);
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
  ///qDebug() << "Start point: " << pvStartPoint;
  ///qDebug() << "End   point: " << pvEndPoint;
  ///qDebug() << "line dy: " << line.dy();
  textRect.setX(pvStartPoint.x() + (line.dx() - pvTextSize.width()) / 2.0);
  textRect.setY(pvStartPoint.y() + line.dy() / 2.0 - pvTextSize.height());
  textRect.setSize(pvTextSize);
  qDebug() << "textRect: " << textRect;
  qDebug() << "line: " << line;

  painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter->drawLine(line);
  painter->drawText(textRect, Qt::AlignHCenter | Qt::TextDontClip, pvText);
  painter->drawRect(boundingRect());
}
