/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "CurrentRowToBeSetSpy.h"
#include "Mdt/ItemEditor/RowChangeEventDispatcher.h"
#include "Mdt/ItemEditor/AbstractController.h"

// #include <QDebug>

using namespace Mdt::ItemEditor;

CurrentRowToBeSetSpy::CurrentRowToBeSetSpy(const RowChangeEventDispatcher & dispatcher, QObject* parent)
 : QObject(parent)
{
  connect(&dispatcher, &RowChangeEventDispatcher::currentRowToBeSet, this, &CurrentRowToBeSetSpy::setCurrentRow);
}

CurrentRowToBeSetSpy::CurrentRowToBeSetSpy(const AbstractController& controller, QObject* parent)
 : QObject(parent)
{
  connect(&controller, &AbstractController::currentRowToBeSet, this, &CurrentRowToBeSetSpy::setCurrentRow);
}

int CurrentRowToBeSetSpy::takeCurrentRow()
{
  int row = mCurrentRow;
  clear();
  return row;
}

void CurrentRowToBeSetSpy::clear()
{
  mCount = 0;
  mCurrentRow = -2;
}

void CurrentRowToBeSetSpy::setCurrentRow(int row)
{
  ++mCount;
  mCurrentRow = row;
}
