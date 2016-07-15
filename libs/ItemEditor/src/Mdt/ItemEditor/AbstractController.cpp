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
#include "RowChangeEventMapper.h"
#include "RowChangeEventDispatcher.h"
#include <QAbstractItemModel>
#include <QItemSelectionModel>

#include <QDebug>

namespace Mdt{ namespace ItemEditor{

AbstractController::AbstractController(QObject* parent)
 : QObject(parent),
   pvCurrentRow(-1),
   pvRowChangeEventMapper(new RowChangeEventMapper(this)),
   pvRowChangeEventDispatcher(new RowChangeEventDispatcher(this))
{
  connect(pvRowChangeEventMapper, &RowChangeEventMapper::selectionModelChanged, pvRowChangeEventDispatcher, &RowChangeEventDispatcher::setSelectionModel);
  connect(pvRowChangeEventMapper, &RowChangeEventMapper::rowStateChanged, pvRowChangeEventDispatcher, &RowChangeEventDispatcher::setRowState);
  connect(pvRowChangeEventDispatcher, &RowChangeEventDispatcher::modelReset, this, &AbstractController::toFirst);
  connect(pvRowChangeEventDispatcher, &RowChangeEventDispatcher::rowStateChanged, this, &AbstractController::rowStateChanged);
  connect(pvRowChangeEventDispatcher, &RowChangeEventDispatcher::currentRowChanged, this, &AbstractController::currentRowChanged);
  connect(pvRowChangeEventDispatcher, &RowChangeEventDispatcher::currentRowChangedForController, this, &AbstractController::setCurrentRow);
}

void AbstractController::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  referenceItemModel(model);
  registerItemModel();
}

int AbstractController::rowCount() const
{
  if(pvModel.isNull()){
    return 0;
  }
  return pvModel->rowCount();
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
  qDebug() << "Controller: set current row " << row;
  pvCurrentRow = row;
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
  setCurrentRow(pvCurrentRow-1);
}

void AbstractController::toNext()
{
  /// \todo checks..
  setCurrentRow(pvCurrentRow+1);
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
  Q_ASSERT(!pvModel.isNull());

  if(pvModel == pvRowChangeEventMapper->model()){
    return;
  }
  pvRowChangeEventMapper->setModel(pvModel);
}

void AbstractController::registerSelectionModel(QItemSelectionModel* model)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(pvRowChangeEventMapper->model() != nullptr);

  if(model == pvRowChangeEventMapper->selectionModel()){
    return;
  }
  pvRowChangeEventMapper->setSelectionModel(model);
}

// void AbstractController::setRowState(RowState rs)
// {
//   qDebug() << "AbstractController::setRowState() ...";
//   emit rowStateChanged(rs);
// }


}} // namespace Mdt{ namespace ItemEditor{
