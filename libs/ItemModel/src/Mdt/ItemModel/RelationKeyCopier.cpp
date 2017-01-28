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
#include "RelationKeyCopier.h"
#include "RelationColumnPair.h"
#include "PrimaryKey.h"
#include "ForeignKey.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVector>

#include <QDebug>

namespace Mdt{ namespace ItemModel{

RelationKeyCopier::RelationKeyCopier(QObject* parent)
 : QObject(parent)
{
}

void RelationKeyCopier::setCopyTriggers(CopyTriggers triggers)
{
  mCopyTriggers = triggers;
//   reconnectSignalSlotsIfOk();
}

void RelationKeyCopier::setChildModelRowsInsertedTriggerEnabled(bool enable)
{
  if(enable){
    setCopyTriggers(mCopyTriggers | ChildModelRowsInserted);
  }else{
    setCopyTriggers(mCopyTriggers & ~ChildModelRowsInserted);
  }
}

void RelationKeyCopier::setParentModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);
//   Q_ASSERT( mKey.isNull() || (mKey.greatestParentModelColumn() < model->columnCount()) );

  mParentModel = model;
  ///clearKey();
  setParentModelCurrentRow(-1);
//   reconnectSignalSlotsIfOk();
}

void RelationKeyCopier::setChildModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);
//   Q_ASSERT( mKey.isNull() || (mKey.greatestChildModelColumn() < model->columnCount()) );

  mChildModel = model;
  ///clearKey();
//   reconnectSignalSlotsIfOk();
}

void RelationKeyCopier::addColumnPair(int parentModelColumn, int childModelColumn)
{
  Q_ASSERT(parentModelColumn >= 0);
//   Q_ASSERT( mParentModel.isNull() || (parentModelColumn < mParentModel->columnCount()) );
  Q_ASSERT(childModelColumn >= 0);
//   Q_ASSERT( mChildModel.isNull() || (childModelColumn < mChildModel->columnCount()) );

  mKey.addColumnPair(parentModelColumn, childModelColumn);
}

void RelationKeyCopier::setKey(const PrimaryKey& parentModelPk, const ForeignKey& childModelFk)
{
  Q_ASSERT(!parentModelPk.isNull());
  Q_ASSERT(!childModelFk.isNull());
  Q_ASSERT(parentModelPk.columnCount() == childModelFk.columnCount());
//   Q_ASSERT( mParentModel.isNull() || (parentModelPk.greatestColumn() < mParentModel->columnCount()) );
//   Q_ASSERT( mChildModel.isNull() || (childModelFk.greatestColumn() < mChildModel->columnCount()) );

  mKey.setKey(parentModelPk, childModelFk);
}

void RelationKeyCopier::setKey(const RelationKey & key)
{
  Q_ASSERT(!key.isNull());
//   Q_ASSERT( mParentModel.isNull() || (key.greatestParentModelColumn() < mParentModel->columnCount()) );
//   Q_ASSERT( mChildModel.isNull() || (key.greatestChildModelColumn() < mChildModel->columnCount()) );

  mKey = key;
}

bool RelationKeyCopier::copyAllKeyData(RowRange childModelRowRange, const QModelIndex & parent)
{
  if(mParentModel.isNull() || mChildModel.isNull()){
    return false;
  }
  if(!childModelRowRange.isValid()){
    return false;
  }
  if(childModelRowRange.rowCount() > mChildModel->rowCount(parent)){
    return false;
  }
  if( (mParentModelCurrentRow < 0) || (mParentModelCurrentRow >= mParentModel->rowCount()) ){
    return false;
  }
  const int last = childModelRowRange.lastRow();
  for(int row = childModelRowRange.firstRow(); row <= last; ++row){
    if( !copyKeyDataForRow(row) ){
      return false;
    }
  }
  return true;
}

void RelationKeyCopier::setParentModelCurrentRow(int row)
{
  if(mParentModel.isNull()){
    return;
  }
  Q_ASSERT(row >= -1);
  Q_ASSERT(row < mParentModel->rowCount());
  
  qDebug() << "setParentModelCurrentRow() - row: " << row;
  
  mParentModelCurrentRow = row;
}

void RelationKeyCopier::onChildModelRowsInserted(const QModelIndex & /*parent*/, int first, int last)
{
  Q_ASSERT(!mParentModel.isNull());
  Q_ASSERT(!mChildModel.isNull());

  qDebug() << "onChildModelRowsInserted() first: " << first << " , last " << last << " - model: " << mChildModel;
  if(mParentModelCurrentRow < 0){
    return;
  }
  for(int row = first; row <= last; ++row){
    copyKeyData(row);
  }
}

void RelationKeyCopier::onParentModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> & roles)
{
  Q_ASSERT(!mParentModel.isNull());
  Q_ASSERT(!mChildModel.isNull());

  qDebug() << "onParentModelDataChanged() topLeft " << topLeft << " , bottomRight " << bottomRight << " , roles " << roles;
}

bool RelationKeyCopier::copyKeyDataForRow(int childModelRow)
{
  Q_ASSERT(!mParentModel.isNull());
  Q_ASSERT(!mChildModel.isNull());
  Q_ASSERT(childModelRow >= 0);
  Q_ASSERT(childModelRow < mChildModel->rowCount());
  Q_ASSERT(mParentModelCurrentRow >= 0);
  Q_ASSERT(mParentModelCurrentRow < mParentModel->rowCount());

  /** \todo Should fix which place what is checked:
   *  - Allaways check ranges ?
   *  - Simply check indexes ? (index will be invalid if range is invalid
   *  - Runtime check or assertions ?
   */

  qDebug() << "copyKeyData() - pairs: " << mKey.columnPairCount();
//   qDebug() << "copyKeyData() - chil model: " << mChildModel << " - row: " << childModelRow << " - pairs: " << mKey.columnPairCount();
  for(const auto columnPair : mKey){
    const auto parentModelIndex = mParentModel->index(mParentModelCurrentRow, columnPair.parentModelColumn());
    const auto childModelIndex = mChildModel->index(childModelRow, columnPair.childModelColumn());
    qDebug() << "copyKeyData() - childModelIndex: " << childModelIndex;
    qDebug() << "copyKeyData() - current: " << mChildModel->data(childModelIndex);
    if( !mChildModel->setData( childModelIndex , mParentModel->data(parentModelIndex) ) ){
      return false;
    }
    qDebug() << "copyKeyData() - updated: " << mChildModel->data(childModelIndex);
  }

  return true;
}

void RelationKeyCopier::copyKeyData(int childModelRow)
{
  Q_ASSERT(!mParentModel.isNull());
  Q_ASSERT(!mChildModel.isNull());
  Q_ASSERT(childModelRow >= 0);
  Q_ASSERT(childModelRow < mChildModel->rowCount());
  Q_ASSERT(mParentModelCurrentRow >= 0);
  Q_ASSERT(mParentModelCurrentRow < mParentModel->rowCount());

  qDebug() << "copyKeyData() - pairs: " << mKey.columnPairCount();
  for(const auto columnPair : mKey){
    const auto parentModelIndex = mParentModel->index(mParentModelCurrentRow, columnPair.parentModelColumn());
    const auto childModelIndex = mChildModel->index(childModelRow, columnPair.childModelColumn());
    qDebug() << "copyKeyData() - current: " << mChildModel->data(childModelIndex);
    mChildModel->setData( childModelIndex , mParentModel->data(parentModelIndex) );
    qDebug() << "copyKeyData() - updated: " << mChildModel->data(childModelIndex);
  }
}

// void RelationKeyCopier::reconnectSignalSlotsIfOk()
// {
//   if( mParentModel.isNull() || mChildModel.isNull() ){
//     return;
//   }
//   qDebug() << "Disconnect all if ...";
//   disconnect(mChildModelRowsInsertedConnection);
//   disconnect(mParentModelDataChangedConnection);
//   if(mCopyTriggers & ChildModelRowsInserted){
//     qDebug() << "connect ChildModelRowsInserted - child model: " << mChildModel;
//     mChildModelRowsInsertedConnection = connect(mChildModel, &QAbstractItemModel::rowsInserted, this, &RelationKeyCopier::onChildModelRowsInserted);
//   }
//   if(mCopyTriggers & ParentModelDataChanged){
//     mParentModelDataChangedConnection = connect(mParentModel, &QAbstractItemModel::dataChanged, this, &RelationKeyCopier::onParentModelDataChanged);
//   }
// }

void RelationKeyCopier::clearKey()
{
  mKey.clear();
}

}} // namespace Mdt{ namespace ItemModel{
