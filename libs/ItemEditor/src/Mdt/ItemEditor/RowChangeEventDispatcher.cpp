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
#include "RowChangeEventDispatcher.h"
#include "AbstractController.h"
#include "ControllerStatePermission.h"

#include <QDebug>

namespace Mdt{ namespace ItemEditor{

RowChangeEventDispatcher::RowChangeEventDispatcher(AbstractController *controller)
 : QObject(controller),
   mController(controller)
{
  Q_ASSERT(!mController.isNull());
}

QAbstractItemModel* RowChangeEventDispatcher::model() const
{
  return pvModel;
}

void RowChangeEventDispatcher::setCurrentRow(int row)
{
  Q_ASSERT(row >= -1);
  Q_ASSERT(row < rowCount());

  updateCurrentRow(row);
}

int RowChangeEventDispatcher::rowCount() const
{
  if(pvModel.isNull()){
    return 0;
  }
  return pvModel->rowCount();
}

int RowChangeEventDispatcher::currentRow() const
{
  return pvRowState.currentRow();
}

void RowChangeEventDispatcher::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  if(!pvModel.isNull()){
    disconnect(pvModel, &QAbstractItemModel::modelReset, this, &RowChangeEventDispatcher::onModelReset);
    disconnect(pvModel, &QAbstractItemModel::rowsInserted, this, &RowChangeEventDispatcher::onRowsInserted);
//     disconnect(pvModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &RowChangeEventDispatcher::onRowsAboutToBeRemoved);
    disconnect(pvModel, &QAbstractItemModel::rowsRemoved, this, &RowChangeEventDispatcher::onRowsRemoved);
  }
  pvModel = model;
  connect(pvModel, &QAbstractItemModel::modelReset, this, &RowChangeEventDispatcher::onModelReset);
  connect(pvModel, &QAbstractItemModel::rowsInserted, this, &RowChangeEventDispatcher::onRowsInserted);
//   connect(pvModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &RowChangeEventDispatcher::onRowsAboutToBeRemoved);
  connect(pvModel, &QAbstractItemModel::rowsRemoved, this, &RowChangeEventDispatcher::onRowsRemoved);
  onModelReset();
}

void RowChangeEventDispatcher::onModelReset()
{
  Q_ASSERT(!pvModel.isNull());

  /*
   * If model has data, we go to first row
   */
  pvRowState.setRowCount(pvModel->rowCount());
  if(pvRowState.rowCount() > 0){
    pvRowState.setCurrentRow(0);
  }else{
    pvRowState.setCurrentRow(-1);
  }
  Q_ASSERT(pvRowState.rowCount() == pvModel->rowCount());
  /*
   * We must allways signal when a model was reset.
   */
  emit rowStateUpdated(pvRowState);
}

void RowChangeEventDispatcher::onRowsInserted(const QModelIndex& /*parent*/, int /*first*/, int last)
{
  Q_ASSERT(!mController.isNull());
  Q_ASSERT(!pvModel.isNull());

  pvRowState.setRowCount(pvModel->rowCount());
  if(ControllerStatePermission::canChangeCurrentRow(mController->controllerState())){
    pvRowState.setCurrentRow(last);
  }
  qDebug() << "RowChangeEventDispatcher::onRowsInserted() - rows: " << pvRowState.rowCount() << " , current: " << pvRowState.currentRow();

  emit rowStateUpdated(pvRowState);
}

void RowChangeEventDispatcher::onRowsRemoved(const QModelIndex& /*parent*/, int first, int last)
{
  Q_ASSERT(!pvModel.isNull());

  pvRowState.setRowCount(pvModel->rowCount());
  if(pvRowState.currentRow() >= pvRowState.rowCount()){
    pvRowState.setCurrentRow(pvRowState.rowCount()-1);
  }
  emit rowStateUpdated(pvRowState);
}

void RowChangeEventDispatcher::updateCurrentRow(int row)
{
  if(pvModel.isNull()){
    return;
  }
  Q_ASSERT(pvRowState.rowCount() == pvModel->rowCount());
  auto previousRowState = pvRowState;
  pvRowState.setCurrentRow(row);
  if(pvRowState != previousRowState){
    emit rowStateUpdated(pvRowState);
  }
}

}} // namespace Mdt{ namespace ItemEditor{
