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
#include "ComparisonEval.h"
#include <QAbstractItemModel>
#include <QModelIndex>

#include <QDebug>

namespace Mdt{ namespace ItemModel{ namespace Expression{

QVariant getVariantValue(const FilterColumnData & col, const FilterEvalData & data)
{
  const auto * model = data.model();
  Q_ASSERT(model != nullptr);
  Q_ASSERT(col.columnIndex() < model->rowCount());

  qDebug() << "row: " << data.row();

  const auto index = model->index(data.row(), col.columnIndex());
  Q_ASSERT(index.isValid());

  return model->data(index);
}

bool CompareLikeTo::isLike(const FilterColumnData & col, const QString & like, const FilterEvalData & data) const
{
  qDebug() << "isLike(): expr: " << like;
  return false;
}

bool CompareEqualTo::isEqual(const FilterColumnData& col, const QString & value, const FilterEvalData & data)
{
  qDebug() << "isEqual(QString): value: " << value;
  
  return (QString::compare( getStringValue(col, data), value, data.caseSensitivity() ) == 0);
}

bool CompareEqualTo::isEqual(const FilterColumnData & col, int value, const FilterEvalData & data)
{
  qDebug() << "isEqual(int): value: " << value;
  return ( getVariantValue(col, data) == value );
}

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{
