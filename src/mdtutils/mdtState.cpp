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
#include "mdtState.h"

mdtState::mdtState(int id, QState *parent)
 : QState(parent)
{
  pvId = id;
  pvLedColorId = 0;
  pvLedIsOn = false;
  pvNotifyEnteredToUi = false;
  connect(this, SIGNAL(entered()), this, SLOT(onStateEntered()));
}

mdtState::~mdtState()
{
}

void mdtState::setId(int id)
{
  pvId = id;
}

int mdtState::id() const
{
  return pvId;
}

void mdtState::setText(const QString & text)
{
  pvText = text;
}

QString mdtState::text() const
{
  return pvText;
}

void mdtState::setLedColorId(int colorId)
{
  pvLedColorId = colorId;
}

int mdtState::ledColorId() const
{
  return pvLedColorId;
}

void mdtState::setLedOn(bool on)
{
  pvLedIsOn = on;
}

bool mdtState::ledIsOn() const
{
  return pvLedIsOn;
}

void mdtState::setNotifyEnteredToUi(bool notify)
{
  pvNotifyEnteredToUi = notify;
}

bool mdtState::notifyEnteredToUi() const
{
  return pvNotifyEnteredToUi;
}

void mdtState::onStateEntered()
{
  emit entered(this);
}
