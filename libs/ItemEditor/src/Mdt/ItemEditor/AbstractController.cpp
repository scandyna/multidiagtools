/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "AbstractController.h"
#include <QAbstractItemModel>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

AbstractController::AbstractController(QObject* parent)
 : QObject(parent),
   pvCurrentRow(-1)
{
}

void AbstractController::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  pvModel = model;
  emit modelChanged(model);
}

int AbstractController::rowCount() const
{
  if(pvModel.isNull()){
    return 0;
  }
  return pvModel->rowCount();
}

void AbstractController::toFirst()
{

}

void AbstractController::toPrevious()
{

}

void AbstractController::toNext()
{

}

void AbstractController::toLast()
{

}

void AbstractController::setRowState(RowState rs)
{

}


}} // namespace Mdt{ namespace ItemEditor{
