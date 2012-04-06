/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtLed.h"

#include <QRadialGradient>
#include <QPointF>
#include <QSizePolicy>

mdtLed::mdtLed(QWidget *parent)
  : QWidget(parent)
{
  // Set default borders
  setBorders(5, 5);

  pvSizeHint.setHeight(15);
  pvSizeHint.setWidth(15);

  // LED size - default -1 for auto resize
  pvFixedSize.setHeight(-1);
  pvFixedSize.setWidth(-1);

  // Default LED
  pvAntialiasing = true;
  setGlint();
  setGreen();
  setOff();
  pvTextMode = false;
}

void mdtLed::setBorders(int horizontalBorder, int verticalBorder)
{
  pvHorzontalBorder = horizontalBorder;
  pv2xHorzontalBorder = 2*pvHorzontalBorder;
  pvVerticalBorder = verticalBorder;
  pv2xVerticalBorder = 2*pvVerticalBorder;
}

void mdtLed::setGreen()
{
  // Colors
  pvOnColor.setRgb(0, 255, 0);
  pvOnGlance.setRgb(220, 255, 220);
  pvOffColor = pvOnColor.darker();
  pvOffGlance = pvOnGlance.darker();
}

void mdtLed::setOrange()
{
  // Colors
  pvOnColor.setRgb(255, 180, 0);
  pvOnGlance.setRgb(245, 230, 150);
  pvOffColor = pvOnColor.darker();
  pvOffGlance = pvOnGlance.darker();
}

void mdtLed::setRed()
{
  // Colors
  pvOnColor.setRgb(255, 0, 0);
  pvOnGlance.setRgb(255, 120, 120);
  pvOffColor = pvOnColor.darker();
  pvOffGlance = pvOnGlance.darker();
}

void mdtLed::setFlat()
{
  pvGlint = false;
  pvFlatBrush.setStyle(Qt::SolidPattern);
}

void mdtLed::setGlint()
{
  pvGlint = true;
}

void mdtLed::setTextMode()
{
  pvTextMode = true;
}

void mdtLed::setAntialiasing(bool enabled)
{
  pvAntialiasing = enabled;
}

void mdtLed::setFixedSize(int width, int height)
{
  if(width < 10){
    width = 10;
  }
  if(height < 10){
    height = 10;
  }

  pvFixedSize.setWidth(width);
  pvFixedSize.setHeight(height);

  // Adjust size hints
  pvSizeHint.setWidth(pvFixedSize.width() + pvHorzontalBorder);
  pvSizeHint.setHeight(pvFixedSize.height() + pvVerticalBorder);
}

void mdtLed::setAutoSize()
{
  pvFixedSize.setWidth(-1);
  pvFixedSize.setHeight(-1);

  pvSizeHint.setWidth(15);
  pvSizeHint.setHeight(15);
}

QSize mdtLed::sizeHint() const
{
  return pvSizeHint;
}

QSize mdtLed::minimumSizeHint() const
{
  return pvSizeHint;
}

void mdtLed::setOn()
{
  pvCurrentColor = pvOnColor;
  pvCurrentGlance = pvOnGlance;
  pvText = "ON";
  pvIsOn = true;
  update();
}

void mdtLed::setOff()
{
  pvCurrentColor = pvOffColor;
  pvCurrentGlance = pvOffGlance;
  pvText = "OFF";
  pvIsOn = false;
  update();
}

void mdtLed::setOn(bool on)
{
  if(on){
    setOn();
  }else{
    setOff();
  }
}

void mdtLed::toggleOnOff()
{
  if(pvIsOn){
    setOff();
  }else{
    setOn();
  }
}

void mdtLed::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  QPointF center( (qreal)size().width()/2.0  , (qreal)size().height()/2.0 );

  painter.setPen(pvPen);

  // Set brush parameters
  if(!pvTextMode){
    if(pvGlint){
      QRadialGradient gradient( center , center.x() , center);
      gradient.setColorAt(0.0 , pvCurrentGlance);
      gradient.setColorAt(1.0 , pvCurrentColor);
      painter.setBrush(gradient);
    }else{
      pvFlatBrush.setColor(pvCurrentColor);
      painter.setBrush(pvFlatBrush);
    }
  }

  if(pvAntialiasing){
    painter.setRenderHint(QPainter::Antialiasing, true);
  }else{
    painter.setRenderHint(QPainter::Antialiasing, false);
  }

  // Draw
  if(pvFixedSize.isValid()){
    int leftBorder = ( size().width() - pvFixedSize.width() ) / 2 ;
    int topBorder = ( size().height() - pvFixedSize.height() ) / 2;
    painter.drawEllipse(leftBorder , topBorder , pvFixedSize.width() , pvFixedSize.height());
    if(pvTextMode){
      painter.drawText(leftBorder, topBorder, pvFixedSize.width(), pvFixedSize.height(), Qt::AlignHCenter | Qt::AlignVCenter , pvText);
    }
  }else{
    painter.drawEllipse(pvHorzontalBorder , pvVerticalBorder , size().width()-pv2xHorzontalBorder , size().height()-pv2xVerticalBorder);
    if(pvTextMode){
      painter.drawText(pvHorzontalBorder, pvVerticalBorder, size().width()-pv2xHorzontalBorder, size().height()-pv2xVerticalBorder, Qt::AlignHCenter | Qt::AlignVCenter , pvText);
    }
  }
}

/* Main */
/*
#include <QApplication>
#include <QGridLayout>
int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  QWidget *w = new QWidget;
  QGridLayout *layout = new QGridLayout;
  int x, y;

  for(x=0; x<20; x++){
    for(y=0; y<10; y++){
      layout->addWidget(new mdtLed, y, x);
    }
  }

  w->setLayout(layout);
  w->resize(400,200);
  w->show();

  return app.exec();
}
*/
