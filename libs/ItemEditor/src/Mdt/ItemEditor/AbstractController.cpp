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
#include "ItemSelectionModel.h"
#include "RowChangeEventDispatcher.h"
#include <QAbstractItemModel>
#include <QItemSelectionModel>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

AbstractController::AbstractController(QObject* parent)
 : QObject(parent),
   pvModel(nullptr)
{
  pvRowChangeEventDispatcher = new RowChangeEventDispatcher(this);
  connect(pvRowChangeEventDispatcher, &RowChangeEventDispatcher::rowStateUpdated, this, &AbstractController::updateRowState);
}

void AbstractController::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  referenceItemModel(model);
  registerItemModel();
}

int AbstractController::rowCount() const
{
  return pvRowChangeEventDispatcher->rowCount();
}

int AbstractController::currentRow() const
{
  return pvRowChangeEventDispatcher->currentRow();
}

bool AbstractController::setCurrentRow(int row)
{
  Q_ASSERT(row >= -1);

  /**
   * \todo If row >= rowCount()
   *       we must try to fetch more data
   *       until we found row, or no more data is available,
   *       and consider this value as current row.
   */
  pvRowChangeEventDispatcher->setCurrentRow(row);

  return true;
}

void AbstractController::toFirst()
{
  if(rowCount() > 0){
    setCurrentRow(0);
  }else{
    setCurrentRow(-1);
  }
}

void AbstractController::toPrevious()
{
  /// \todo checks..
  setCurrentRow(currentRow()-1);
}

void AbstractController::toNext()
{
  /// \todo checks..
  setCurrentRow(currentRow()+1);
}

void AbstractController::toLast()
{
  setCurrentRow(rowCount()-1);
}

void AbstractController::referenceItemModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  if(model == pvModel){
    return;
  }
  pvModel = model;
  emit modelChanged(pvModel);
}

void AbstractController::registerItemModel()
{
  Q_ASSERT(pvModel != nullptr);

  if(pvModel == pvRowChangeEventDispatcher->model()){
    return;
  }
  pvRowChangeEventDispatcher->setModel(pvModel);
}

void AbstractController::registerSelectionModel(QItemSelectionModel* selectionModel)
{
  Q_ASSERT(selectionModel != nullptr);
  Q_ASSERT(pvRowChangeEventDispatcher->model() != nullptr);
  Q_ASSERT(selectionModel->model() == pvRowChangeEventDispatcher->model());

  if(selectionModel == pvRowChangeEventDispatcher->selectionModel()){
    return;
  }
  pvRowChangeEventDispatcher->setSelectionModel(selectionModel);
}

void AbstractController::updateRowState(RowState rs)
{
  emit currentRowChanged(rs.currentRow());
  emit rowStateChanged(rs);
}

}} // namespace Mdt{ namespace ItemEditor{
