/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "ItemModel.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QDebug>

namespace Mdt{ namespace TestLib{

QVariant getModelData(const QAbstractItemModel* model, int row, int column, Qt::ItemDataRole role)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < model->rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < model->columnCount());

  auto index = model->index(row, column);
  if(!index.isValid()){
    qDebug() << "Mdt::TestLib::getModelData() - index is not valid: " << index;
    return QVariant();
  }

  return model->data(index, role);
}

QVariant getModelData(const QAbstractItemModel& model, int row, int column, Qt::ItemDataRole role)
{
  return getModelData(&model, row, column, role);
}

bool setModelData(QAbstractItemModel* model, int row, int column, const QVariant& value, Qt::ItemDataRole role)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < model->rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < model->columnCount());

  auto index = model->index(row, column);
  if(!index.isValid()){
    qDebug() << "Mdt::TestLib::setModelData() - index is not valid: " << index;
    return false;
  }

  return model->setData(index, value, role);
}

bool setModelData(QAbstractItemModel& model, int row, int column, const QVariant& value, Qt::ItemDataRole role)
{
  return setModelData(&model, row, column, value, role);
}

bool prependRowToModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  return model->insertRows(0, 1);
}

bool prependRowToModel(QAbstractItemModel& model)
{
  return prependRowToModel(&model);
}

bool appendRowToModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  return model->insertRows(model->rowCount(), 1);
}

bool appendRowToModel(QAbstractItemModel& model)
{
  return appendRowToModel(&model);
}

}} // namespace Mdt{ namespace TestLib{
