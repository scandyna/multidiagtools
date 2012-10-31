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
#include "mdtDigitalIo.h"

mdtDigitalIo::mdtDigitalIo(QObject *parent)
 : mdtAbstractIo(parent)
{
  pvIsOn = false;
}

mdtDigitalIo::~mdtDigitalIo()
{
}

bool mdtDigitalIo::isOn()
{
  return pvIsOn;
}

void mdtDigitalIo::setOn(bool on)
{
  if(pvIsOn != on){
    pvIsOn = on;
    emit(stateChanged(pvIsOn));
    emit(stateChangedForUi(pvIsOn));
  }
}

void mdtDigitalIo::setStateFromUi(bool on)
{
  if(pvIsOn != on){
    pvIsOn = on;
    emit(stateChanged(pvIsOn));
  }
}
