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

#include <QDebug>

namespace Mdt{ namespace ItemEditor{

RowChangeEventMapper::RowChangeEventMapper(QObject* parent)
 : QObject(parent)
{
}

void RowChangeEventMapper::setSelectionModel(QItemSelectionModel* model)
{
  Q_ASSERT(model != nullptr);

  if(!pvSelectionModel.isNull()){
    disconnect(pvSelectionModel, &QItemSelectionModel::currentChanged, this, &RowChangeEventMapper::setCurrentIndex);
  }
  pvSelectionModel = model;
  connect(pvSelectionModel, &QItemSelectionModel::currentChanged, this, &RowChangeEventMapper::setCurrentIndex);
  if(pvSelectionModel->model() != pvModel){
    setModel(pvSelectionModel->model());
  }

  emit currentRowChanged(pvSelectionModel->currentIndex().row());
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

  emit rowCountChanged(model->rowCount());
}

void RowChangeEventMapper::setCurrentIndex(const QModelIndex & current, const QModelIndex & previous)
{
  qDebug() << "Current index changed " << current;

  if(current.row() != previous.row()){
    emit currentRowChanged(current.row());
  }
}

void RowChangeEventMapper::onModelReset()
{
  Q_ASSERT(!pvModel.isNull());

  qDebug() << "Model reset ..";
  emit rowCountChanged(pvModel->rowCount());
  // QItemSelectionModel does not signal current changed after model reset
  /// \todo see comment in unit test
//   if(!pvSelectionModel.isNull()){
//     setCurrentIndex(pvSelectionModel->currentIndex(), QModelIndex());
//   }
}

void RowChangeEventMapper::onRowsInserted(const QModelIndex & parent, int /*first*/, int /*last*/)
{
  Q_ASSERT(!pvModel.isNull());

  qDebug() << "Model rows inserted ..";
  emit rowCountChanged(pvModel->rowCount(parent));
  // QItemSelectionModel does not signal current changed after insert
  /// \todo see comment in unit test
//   if(!pvSelectionModel.isNull()){
//     setCurrentIndex(pvSelectionModel->currentIndex(), QModelIndex());
//   }
}

void RowChangeEventMapper::onRowsRemoved(const QModelIndex & parent, int /*first*/, int /*last*/)
{
  Q_ASSERT(!pvModel.isNull());

  qDebug() << "Model rows removed ..";
  emit rowCountChanged(pvModel->rowCount(parent));
  // QItemSelectionModel does not signal current changed after remove
  /// \todo see comment in unit test
//   if(!pvSelectionModel.isNull()){
//     setCurrentIndex(pvSelectionModel->currentIndex(), QModelIndex());
//   }
}

}} // namespace Mdt{ namespace ItemEditor{
