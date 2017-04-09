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
#include "RowChangeEventDispatcher.h"
// #include "ControllerStatePermission.h"

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

RowChangeEventDispatcher::RowChangeEventDispatcher(QObject *parent)
 : QObject(parent)
{
}

QAbstractItemModel* RowChangeEventDispatcher::model() const
{
  return mModel;
}

void RowChangeEventDispatcher::setCurrentRow(int row)
{
  Q_ASSERT(row >= -1);
  Q_ASSERT(row < rowCount());

  updateCurrentRow(row);
}

int RowChangeEventDispatcher::rowCount() const
{
  if(mModel.isNull()){
    return 0;
  }
  return mModel->rowCount();
}

int RowChangeEventDispatcher::currentRow() const
{
  return mRowState.currentRow();
}

bool RowChangeEventDispatcher::insertRowAtBeginning()
{
  Q_ASSERT(!mModel.isNull());

  return insertRowBefore(0);
}

bool RowChangeEventDispatcher::insertRowAtEnd()
{
  Q_ASSERT(!mModel.isNull());

  return insertRowBefore(mModel->rowCount());
}

void RowChangeEventDispatcher::setModel(QAbstractItemModel* model)
{
//   Q_ASSERT(model != nullptr);

  if(!mModel.isNull()){
    disconnect(mModel, &QAbstractItemModel::modelReset, this, &RowChangeEventDispatcher::onModelReset);
    disconnect(mModel, &QAbstractItemModel::rowsInserted, this, &RowChangeEventDispatcher::onRowsInserted);
//     disconnect(pvModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &RowChangeEventDispatcher::onRowsAboutToBeRemoved);
    disconnect(mModel, &QAbstractItemModel::rowsRemoved, this, &RowChangeEventDispatcher::onRowsRemoved);
  }
  mModel = model;
  if(mModel.isNull()){
    if(!mRowState.isNull()){
      const int previousCurrentRow = mRowState.currentRow();
      mRowState.clear();
      emit rowStateUpdated(mRowState);
      if(mRowState.currentRow() != previousCurrentRow){
        emit currentRowChanged(mRowState.currentRow());
      }
    }
  }else{
    connect(mModel, &QAbstractItemModel::modelReset, this, &RowChangeEventDispatcher::onModelReset);
    connect(mModel, &QAbstractItemModel::rowsInserted, this, &RowChangeEventDispatcher::onRowsInserted);
//     connect(pvModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &RowChangeEventDispatcher::onRowsAboutToBeRemoved);
    connect(mModel, &QAbstractItemModel::rowsRemoved, this, &RowChangeEventDispatcher::onRowsRemoved);
    onModelReset();
  }
}

void RowChangeEventDispatcher::onModelReset()
{
  Q_ASSERT(!mModel.isNull());

  const auto previousRowState = mRowState;
  /*
   * If model has data, we go to first row
   */
  mRowState.setRowCount(mModel->rowCount());
  if(mRowState.rowCount() > 0){
    mRowState.setCurrentRow(0);
  }else{
    mRowState.setCurrentRow(-1);
  }
  Q_ASSERT(mRowState.rowCount() == mModel->rowCount());
  if(mRowState != previousRowState){
    emit rowStateUpdated(mRowState);
  }
  if(mRowState.currentRow() != previousRowState.currentRow()){
    emit currentRowChanged(mRowState.currentRow());
  }
  /*
   * We must allways signal when a model was reset.
   */
//   qDebug() << "RCEvD:emit rowStateUpdated()";
//   emit rowStateUpdated(mRowState);
}

void RowChangeEventDispatcher::onRowsInserted(const QModelIndex& /*parent*/, int first, int last)
{
  Q_ASSERT(!mModel.isNull());

  const int previousCurrentRow = mRowState.currentRow();
  mRowState.setRowCount(mModel->rowCount());
  /*
   * If insertion was called from controller,
   * we must update current row.
   * If insertion was called outside of our controller, we must:
   * - Not update current row if insertion occured after current row
   * - Update current row to point to the same data if insertion occured before current row
   */
  /** \todo
   * If PK is enabled, also check that calculated current row matches PK data.
   * If not (for example because of filter/sorting)
   * search the row that contains the PK data.
   */
  if(mInsertingFromController){
    mRowState.setCurrentRow(last);
  }else{
    int row = mRowState.currentRow();
    if(row >= first){
      row = row + 1 + last - first;
      mRowState.setCurrentRow(row);
    }
  }
  emit rowStateUpdated(mRowState);
  if(mRowState.currentRow() != previousCurrentRow){
    emit currentRowChanged(mRowState.currentRow());
  }
  emit rowsInserted();
}

void RowChangeEventDispatcher::onRowsRemoved(const QModelIndex& /*parent*/, int first, int /*last*/)
{
  Q_ASSERT(!mModel.isNull());

  const int previousCurrentRow = mRowState.currentRow();
  mRowState.setRowCount(mModel->rowCount());
  /*
   * Define new current row:
   *  - If we remove before current row, we must update
   *  - Else, we let it as is
   */
  int row = mRowState.currentRow();
  if(first < row){
    --row;
  }
  // Also assure that we not go after last row in model
  if(row >= mRowState.rowCount()){
    row = mRowState.rowCount()-1;
  }
  mRowState.setCurrentRow(row);
//   if(pvRowState.currentRow() >= pvRowState.rowCount()){
//     pvRowState.setCurrentRow(pvRowState.rowCount()-1);
//   }
  emit rowsRemoved();
  emit rowStateUpdated(mRowState);
  if(mRowState.currentRow() != previousCurrentRow){
    emit currentRowChanged(mRowState.currentRow());
  }
}

bool RowChangeEventDispatcher::insertRowBefore(int row)
{
  Q_ASSERT(!mModel.isNull());
  Q_ASSERT(row >= 0);
  Q_ASSERT(row <= mModel->rowCount());

  mInsertingFromController = true;
  const bool ok = mModel->insertRow(row);
  mInsertingFromController = false;

  return ok;
}

void RowChangeEventDispatcher::updateCurrentRow(int row)
{
  if(mModel.isNull()){
    return;
  }
  Q_ASSERT(mRowState.rowCount() == mModel->rowCount());
  const auto previousRowState = mRowState;
  mRowState.setCurrentRow(row);
  if(mRowState != previousRowState){
    emit rowStateUpdated(mRowState);
  }
  if(mRowState.currentRow() != previousRowState.currentRow()){
    emit currentRowChanged(mRowState.currentRow());
  }
}

}} // namespace Mdt{ namespace ItemEditor{
