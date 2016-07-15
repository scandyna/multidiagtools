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

#include <QDebug>

namespace Mdt{ namespace ItemEditor{

RowChangeEventDispatcher::RowChangeEventDispatcher(QObject* parent)
 : QObject(parent)
{
}

void RowChangeEventDispatcher::setCurrentRow(int row)
{
  qDebug() << "Dispatcher: setCurrentRow() - row: " << row;

  bool currentRowHasChanged = (row != pvPreviousRowState.currentRow());
  pvPreviousRowState.setCurrentRow(row);
  auto rs = pvPreviousRowState;
  /*
   * If current row changed, we must tell it to item selection model,
   * which will later call setRowState().
   * Else, we must just signal new row state.
   */
  if(currentRowHasChanged){
    if( (!pvSelectionModel.isNull()) && (pvSelectionModel->model() != nullptr) ){
      auto column = pvSelectionModel->currentIndex().column();
      if( (row >= 0) && (column < 0) ){
        column = 0;
      }
      auto index = pvSelectionModel->model()->index(row, column);
      pvSelectionModel->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
    }
    emit currentRowChanged(row);
    emit rowStateChanged(rs);
  }else{
    qDebug() << "Dispatcher: emit currentRowChanged() - row " << row;
    emit currentRowChanged(row);
    emit rowStateChanged(rs);
  }
}

void RowChangeEventDispatcher::setSelectionModel(QItemSelectionModel* model)
{
  Q_ASSERT(model != nullptr);

  pvSelectionModel = model;
}

void RowChangeEventDispatcher::setRowState(RowState rs, RowChangeEventSource source)
{
  qDebug() << "Dispatecher: setRowState() - row: " << rs.currentRow() << " , source: " << (int)source;
  
  switch(source){
    case RowChangeEventSource::ModelReset:
      pvPreviousRowState = rs;
      emit modelReset();
      return;
    case RowChangeEventSource::ItemSelection:
      break;
  }
  Q_ASSERT(rs.rowCount() == pvPreviousRowState.rowCount());

  bool currentRowHasChanged = (rs.currentRow() != pvPreviousRowState.currentRow());
  pvPreviousRowState = rs;
  /*
   * If current row changed, we must tell it to the controller,
   * so it can update its states.
   * Controller will later call us back with the current row it has set.
   */
  if(currentRowHasChanged){
    emit currentRowChangedForController(rs.currentRow()/**, source*/);
  }else{
    qDebug() << "Dispatcher: emit currentRowChanged() - row " << rs.currentRow();
    emit currentRowChanged(rs.currentRow());
    emit rowStateChanged(rs);
  }
}


}} // namespace Mdt{ namespace ItemEditor{
