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
#include "AbstractStlTableModel.h"

namespace Mdt{ namespace ItemModel{

AbstractStlTableModel::AbstractStlTableModel(QObject* parent)
 : QAbstractTableModel(parent)
{
}

void AbstractStlTableModel::emitDataChangedSignal(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
  emit dataChanged(topLeft, bottomRight, roles);
}

void AbstractStlTableModel::emitVerticalHeaderDataChanged(int firstRow, int lastRow)
{
  emit headerDataChanged(Qt::Vertical, firstRow, lastRow);
}

}} // namespace Mdt{ namespace ItemModel{
