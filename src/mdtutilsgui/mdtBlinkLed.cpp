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
#include "mdtBlinkLed.h"

mdtBlinkLed::mdtBlinkLed(QWidget *parent)
 : mdtLed(parent)
{
  setBlinkPeriod(1000);
  connect(&pvTimer, SIGNAL(timeout()), this, SLOT(toggleOnOff()));
}

void mdtBlinkLed::setBlinkPeriod(int T)
{
  if(T <= 160){
    T = 160;
  }
  pvBlinkHalfPeriod = T/2;
}

void mdtBlinkLed::setBlinking(bool on)
{
  if(on){
    pvTimer.setSingleShot(false);
    pvTimer.start(pvBlinkHalfPeriod);
  }else{
    pvTimer.stop();
  }
}

void mdtBlinkLed::setOn()
{
  setBlinking(false);
  mdtLed::setOn();
}

void mdtBlinkLed::setOn(bool on)
{
  setBlinking(false);
  mdtLed::setOn(on);
}

void mdtBlinkLed::setOn(int holdTime, bool priority)
{
  if(pvTimer.isActive()){
    if(priority){
      pvTimer.stop();
    }else{
      return;
    }
  }
  pvTimer.setSingleShot(true);
  mdtLed::setOn();
  pvTimer.start(holdTime);
}

void mdtBlinkLed::setOff()
{
  setBlinking(false);
  mdtLed::setOff();
}
