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
#include "GreatestColumnTransform.h"
#include <algorithm>

#include <QDebug>

namespace Mdt{ namespace ItemModel{ namespace Expression{

void GetGreatestColumn::setGreatestColumn(const FilterColumnData & columnData, const QVariant &, GreatestColumnData & currentGreatestColumn)
{
  qDebug() << "column: " << columnData.columnIndex() << " , current: " << currentGreatestColumn.column;
  currentGreatestColumn.column = std::max(columnData.columnIndex(), currentGreatestColumn.column);
  qDebug() << " -> new current: " << currentGreatestColumn.column;
}

void GetGreatestColumn::setGreatestColumn(const FilterColumnData& columnData, const ParentModelColumnData& parentModelColumnData, GreatestColumnData& currentGreatestColumn)
{
  qDebug() << "column: " << columnData.columnIndex();
  qDebug() << "current: " << currentGreatestColumn.column;
  qDebug() << "PMCD: " << parentModelColumnData.columnIndex();
  qDebug() << "current PMC: " << currentGreatestColumn.parentModelColumn;
  currentGreatestColumn.column = std::max(columnData.columnIndex(), currentGreatestColumn.column);
  currentGreatestColumn.parentModelColumn = std::max(parentModelColumnData.columnIndex(), currentGreatestColumn.parentModelColumn);
}

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{
