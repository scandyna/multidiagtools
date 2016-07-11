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

bool RowChangeEventDispatcher::setCurrentRow(int row, RowChangeEventSource source)
{
  qDebug() << "RowChangeEventDispatcher::setCurrentRow() , row: " << row << " ...";
  
  auto rs = pvPreviousRowState;
  rs.setCurrentRow(row);
  setRowState(rs, source);
  
  emit currentRowChanged(row, source);
  /**
   * \todo If row >= rowCount()
   *       we must try to fetch more data
   *       until we found row, or no more data is available,
   *       and consider this value as current row.
   */

}

void RowChangeEventDispatcher::setRowState(RowState rs, RowChangeEventSource source)
{
  qDebug() << "RowChangeEventDispatcher::setRowState() , n: " << rs.rowCount() << ", row: " << rs.currentRow() << " ...";

  switch(source){
    case RowChangeEventSource::ModelReset:
      pvPreviousRowState = rs;
      emit modelReset();
      return;
    case RowChangeEventSource::Controller:
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
    emit currentRowChanged(rs.currentRow(), source);
  }else{
    emit rowStateChanged(rs);
  }
}


}} // namespace Mdt{ namespace ItemEditor{
