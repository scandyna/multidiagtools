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
#include "RowChangeEventMapper.h"
#include <QAbstractItemModel>
#include <QItemSelectionModel>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

RowChangeEventMapper::RowChangeEventMapper(QObject* parent)
 : QObject(parent)
{
}

void RowChangeEventMapper::setSelectionModel(QItemSelectionModel* model)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(!pvModel.isNull());

  if(!pvSelectionModel.isNull()){
    disconnect(pvSelectionModel, &QItemSelectionModel::currentChanged, this, &RowChangeEventMapper::setCurrentIndex);
    disconnect(pvSelectionModel, &QItemSelectionModel::modelChanged, this, &RowChangeEventMapper::setModel);
  }
  pvSelectionModel = model;
  connect(pvSelectionModel, &QItemSelectionModel::currentChanged, this, &RowChangeEventMapper::setCurrentIndex);
  connect(pvSelectionModel, &QItemSelectionModel::modelChanged, this, &RowChangeEventMapper::setModel);
  pvRowState.setCurrentRow(pvSelectionModel->currentIndex().row());
  emit selectionModelChanged(model);
  if(pvSelectionModel->model() != pvModel){
    setModel(pvSelectionModel->model());
  }else{
    pvRowState.setRowCount(pvModel->rowCount());
    emit rowStateChanged(pvRowState, RowChangeEventSource::ModelReset);
  }
}

void RowChangeEventMapper::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  if(!pvModel.isNull()){
    disconnect(pvModel, &QAbstractItemModel::modelReset, this, &RowChangeEventMapper::onModelReset);
    disconnect(pvModel, &QAbstractItemModel::rowsInserted, this, &RowChangeEventMapper::onRowsInserted);
    disconnect(pvModel, &QAbstractItemModel::rowsRemoved, this, &RowChangeEventMapper::onRowsRemoved);
  }
  pvModel = model;
  connect(pvModel, &QAbstractItemModel::modelReset, this, &RowChangeEventMapper::onModelReset);
  connect(pvModel, &QAbstractItemModel::rowsInserted, this, &RowChangeEventMapper::onRowsInserted);
  connect(pvModel, &QAbstractItemModel::rowsRemoved, this, &RowChangeEventMapper::onRowsRemoved);

  /*
   * We have to allways signal new row count here,
   *  so we not use setCurrentIndex().
   */
  pvRowState.setRowCount(model->rowCount());
  if(!pvSelectionModel.isNull()){
    pvRowState.setCurrentRow(pvSelectionModel->currentIndex().row());
  }
  emit rowStateChanged(pvRowState, RowChangeEventSource::ModelReset);
}

void RowChangeEventMapper::setCurrentIndex(const QModelIndex & current, const QModelIndex & previous)
{
  if(current.row() != previous.row()){
    pvRowState.setCurrentRow(current.row());
    emit rowStateChanged(pvRowState, RowChangeEventSource::ItemSelection);
  }
}

void RowChangeEventMapper::onModelReset()
{
  Q_ASSERT(!pvModel.isNull());

  /*
   * We have to allways signal new row count here,
   *  so we not use setCurrentIndex().
   */
  pvRowState.setRowCount(pvModel->rowCount());
  if(!pvSelectionModel.isNull()){
    pvRowState.setCurrentRow(pvSelectionModel->currentIndex().row());
  }
  emit rowStateChanged(pvRowState, RowChangeEventSource::ModelReset);
}

void RowChangeEventMapper::onRowsInserted(const QModelIndex & parent, int /*first*/, int /*last*/)
{
  Q_ASSERT(!pvModel.isNull());

  /*
   * We have to allways signal new row count here,
   *  so we not use setCurrentIndex().
   */
  pvRowState.setRowCount(pvModel->rowCount(parent));
  if(!pvSelectionModel.isNull()){
    pvRowState.setCurrentRow(pvSelectionModel->currentIndex().row());
  }
  /// \todo Check if specific source should be added
  emit rowStateChanged(pvRowState, RowChangeEventSource::ItemSelection);
}

void RowChangeEventMapper::onRowsRemoved(const QModelIndex & parent, int /*first*/, int /*last*/)
{
  Q_ASSERT(!pvModel.isNull());

  /*
   * We have to allways signal new row count here,
   *  so we not use setCurrentIndex().
   */
  pvRowState.setRowCount(pvModel->rowCount(parent));
  if(!pvSelectionModel.isNull()){
    pvRowState.setCurrentRow(pvSelectionModel->currentIndex().row());
  }
  /// \todo Check if specific source should be added
  emit rowStateChanged(pvRowState, RowChangeEventSource::ItemSelection);
}

}} // namespace Mdt{ namespace ItemEditor{
