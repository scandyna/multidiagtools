/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtCsvTableViewDataMapper.h"
#include <QTableView>
#include <QHeaderView>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QString>

mdtCsvTableViewDataMapper::mdtCsvTableViewDataMapper()
 : pvCurrentRow(-1)
{
}

void mdtCsvTableViewDataMapper::setView(QTableView* view)
{
  Q_ASSERT(view != nullptr);
  Q_ASSERT(view->model() != nullptr);
  Q_ASSERT(view->horizontalHeader() != nullptr);
  Q_ASSERT(view->verticalHeader() != nullptr);

  pvView = view;
  pvCurrentRow = -1;
}

bool mdtCsvTableViewDataMapper::next()
{
  Q_ASSERT(pvView != nullptr);

  auto *model = pvView->model();
  Q_ASSERT(model != nullptr);

  // If we are at last row, try to fetch more data
  if(pvCurrentRow == (model->rowCount() - 1)){
    if(!model->canFetchMore(QModelIndex())){
      return false;
    }
    model->fetchMore(QModelIndex());
  }
  // Check if we are at last row (maybe fetchMore() did nothing, typical if model has no data)
  if(pvCurrentRow == (model->rowCount() - 1)){
    return false;
  }
  ++pvCurrentRow;
  // If current row is hidden, try to get next row again
  if(pvView->isRowHidden(pvCurrentRow)){
    return next();
  }

  return true;
}

mdtCsvRecord mdtCsvTableViewDataMapper::getCurrentRecord() const
{
  Q_ASSERT(pvView != nullptr);

  mdtCsvRecord record;
  auto *model = pvView->model();
  Q_ASSERT(model != nullptr);
  Q_ASSERT(pvCurrentRow > -1);
  Q_ASSERT(pvCurrentRow < model->rowCount());
  Q_ASSERT(!pvView->isRowHidden(pvCurrentRow));
  auto *hHeader = pvView->horizontalHeader();
  Q_ASSERT(hHeader != nullptr);
  auto *vHeader = pvView->verticalHeader();
  Q_ASSERT(vHeader != nullptr);

  record.columnDataList.reserve(model->columnCount());
  for(int col = 0; col < model->columnCount(); ++col){
    if(pvView->isColumnHidden(col)){
      continue;
    }
    auto index = model->index(vHeader->logicalIndex(pvCurrentRow), hHeader->logicalIndex(col));
    record.columnDataList.append(model->data(index));
  }

  return record;
}
