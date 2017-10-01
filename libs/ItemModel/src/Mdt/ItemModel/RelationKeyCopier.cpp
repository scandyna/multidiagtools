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

// #include <QDebug>

namespace Mdt{ namespace ItemModel{

void RelationKeyCopier::setParentModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  mParentModel = model;
  setParentModelCurrentRow(-1);
}

void RelationKeyCopier::setChildModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  mChildModel = model;
}

void RelationKeyCopier::addColumnPair(int parentModelColumn, int childModelColumn)
{
  Q_ASSERT(parentModelColumn >= 0);
  Q_ASSERT(childModelColumn >= 0);

  mKey.addColumnPair(parentModelColumn, childModelColumn);
}

void RelationKeyCopier::setKey(const PrimaryKey& parentModelPk, const ForeignKey& childModelFk)
{
  Q_ASSERT(!parentModelPk.isNull());
  Q_ASSERT(!childModelFk.isNull());
  Q_ASSERT(parentModelPk.columnCount() == childModelFk.columnCount());

  mKey.setKey(parentModelPk, childModelFk);
}

void RelationKeyCopier::setKey(const RelationKey & key)
{
  Q_ASSERT(!key.isNull());

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

bool RelationKeyCopier::copyKeyData(const RowList & childModelRowList, ColumnRange parentModelColumnRange)
{
  Q_ASSERT(!mParentModel.isNull());
  Q_ASSERT(!mChildModel.isNull());
  Q_ASSERT(parentModelColumnRange.isValid());
  Q_ASSERT(mParentModelCurrentRow >= 0);
  Q_ASSERT(mParentModelCurrentRow < mParentModel->rowCount());

  for(int row : childModelRowList){
    if( !copyKeyDataForRow(row, parentModelColumnRange) ){
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

  mParentModelCurrentRow = row;
}

bool RelationKeyCopier::copyKeyDataForRow(int childModelRow)
{
  Q_ASSERT(!mParentModel.isNull());
  Q_ASSERT(!mChildModel.isNull());
  Q_ASSERT(childModelRow >= 0);
  Q_ASSERT(childModelRow < mChildModel->rowCount());
  Q_ASSERT(mParentModelCurrentRow >= 0);
  Q_ASSERT(mParentModelCurrentRow < mParentModel->rowCount());

  for(const auto columnPair : mKey){
    const auto parentModelIndex = mParentModel->index(mParentModelCurrentRow, columnPair.parentModelColumn());
    const auto childModelIndex = mChildModel->index(childModelRow, columnPair.childModelColumn());
    if( !mChildModel->setData( childModelIndex , mParentModel->data(parentModelIndex) ) ){
      return false;
    }
  }

  return true;
}

bool RelationKeyCopier::copyKeyDataForRow(int childModelRow, ColumnRange parentModelColumnRange)
{
  Q_ASSERT(!mParentModel.isNull());
  Q_ASSERT(!mChildModel.isNull());
  Q_ASSERT(childModelRow >= 0);
  Q_ASSERT(childModelRow < mChildModel->rowCount());
  Q_ASSERT(mParentModelCurrentRow >= 0);
  Q_ASSERT(mParentModelCurrentRow < mParentModel->rowCount());
  Q_ASSERT(parentModelColumnRange.isValid());

  for(const auto columnPair : mKey){
    if( parentModelColumnRange.containsColumn(columnPair.parentModelColumn()) ){
      const auto parentModelIndex = mParentModel->index(mParentModelCurrentRow, columnPair.parentModelColumn());
      const auto childModelIndex = mChildModel->index(childModelRow, columnPair.childModelColumn());
      if( !mChildModel->setData( childModelIndex , mParentModel->data(parentModelIndex) ) ){
        return false;
      }
    }
  }

  return true;
}

}} // namespace Mdt{ namespace ItemModel{
