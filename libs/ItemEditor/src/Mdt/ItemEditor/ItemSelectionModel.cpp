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
#include "ItemSelectionModel.h"

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

ItemSelectionModel::ItemSelectionModel(QAbstractItemModel* model)
 : QItemSelectionModel(model)
{
}

ItemSelectionModel::ItemSelectionModel(QAbstractItemModel* model, QObject* parent)
 : QItemSelectionModel(model, parent)
{
}

void ItemSelectionModel::setCurrentRowChangeAllowed(bool allow)
{
  pvCurrentRowChangeAllowed = allow;
}

void ItemSelectionModel::select(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
  if(canAcceptIndex(index)){
    QItemSelectionModel::select(index, command);
  }
}

void ItemSelectionModel::select(const QItemSelection& selection, QItemSelectionModel::SelectionFlags command)
{
  const auto indexList = selection.indexes();
  for(const auto & index : indexList){
    if(!canAcceptIndex(index)){
      return;
    }
  }
  QItemSelectionModel::select(selection, command);
}

void ItemSelectionModel::setCurrentIndex(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
  if(canAcceptIndex(index)){
    QItemSelectionModel::setCurrentIndex(index, command);
    pvPreviousCurrentRow = index.row();
  }
}

bool ItemSelectionModel::canAcceptIndex(const QModelIndex& index) const
{
  return ( pvCurrentRowChangeAllowed || (index.row() == pvPreviousCurrentRow) );
}

}} // namespace Mdt{ namespace ItemEditor{
