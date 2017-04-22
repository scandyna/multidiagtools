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
#include "ItemSelectionModel.h"

// #include "Debug.h"

namespace Mdt{ namespace ItemEditor{

ItemSelectionModel::ItemSelectionModel(QAbstractItemModel* model)
 : QItemSelectionModel(model)
{
}

ItemSelectionModel::ItemSelectionModel(QAbstractItemModel* model, QObject* parent)
 : QItemSelectionModel(model, parent)
{
}

void ItemSelectionModel::setMultiRowSelectionAllowed(bool allow)
{
  mMultiRowSelectionAllowed = allow;
}

void ItemSelectionModel::updateCurrentRow(int row)
{
  if( (model() != nullptr) && (model()->rowCount() > 0) && (model()->columnCount() > 0) ){
    // Check if a specific column was requested
    int column;
    if(mRequestedColumn > -1){
      column = mRequestedColumn;
      mRequestedColumn = -1;
    }else{
      column = currentIndex().column();
    }
    // If current index was lost, we choose column 0
    if( (row >= 0) && (column < 0) ){
      column = 0;
    }
    auto index = model()->index(row, column);
    QItemSelectionModel::setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  }
}

void ItemSelectionModel::select(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
  if( mMultiRowSelectionAllowed || canChangeToIndex(index)){
    QItemSelectionModel::select(index, command);
  }
}

void ItemSelectionModel::select(const QItemSelection& selection, QItemSelectionModel::SelectionFlags command)
{
  if(mMultiRowSelectionAllowed){
    QItemSelectionModel::select(selection, command);
  }else{
    const auto indexList = selection.indexes();
    for(const auto & index : indexList){
      if(!canChangeToIndex(index)){
        return;
      }
    }
    QItemSelectionModel::select(selection, command);
  }
}

void ItemSelectionModel::setCurrentIndex(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
  if(canChangeToIndex(index)){
    QItemSelectionModel::setCurrentIndex(index, command);
  }else{
    mRequestedColumn = index.column();
    emit currentRowChangeRequested(index.row());
  }
}

bool ItemSelectionModel::canChangeToIndex(const QModelIndex& index) const
{
  return ( index.row() == currentIndex().row()  );
}

}} // namespace Mdt{ namespace ItemEditor{
