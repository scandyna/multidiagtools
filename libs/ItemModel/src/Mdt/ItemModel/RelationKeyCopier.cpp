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
  reconnectSignalSlotsIfOk();
}

void RelationKeyCopier::setParentModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  mParentModel = model;
  clearKey();
  setParentModelCurrentRow(-1);
  reconnectSignalSlotsIfOk();
}

void RelationKeyCopier::setChildModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  mChildModel = model;
  clearKey();
  reconnectSignalSlotsIfOk();
}

void RelationKeyCopier::addColumnPair(int parentModelColumn, int childModelColumn)
{
  Q_ASSERT(!mParentModel.isNull());
  Q_ASSERT(!mChildModel.isNull());
  Q_ASSERT(parentModelColumn >= 0);
  Q_ASSERT(parentModelColumn < mParentModel->columnCount());
  Q_ASSERT(childModelColumn >= 0);
  Q_ASSERT(childModelColumn < mChildModel->columnCount());

  mKey.addColumnPair(parentModelColumn, childModelColumn);
}

void RelationKeyCopier::setKey(const PrimaryKey& parentModelPk, const ForeignKey& childModelFk)
{
  Q_ASSERT(!mParentModel.isNull());
  Q_ASSERT(!mChildModel.isNull());
  Q_ASSERT(!parentModelPk.isNull());
  Q_ASSERT(!childModelFk.isNull());
  Q_ASSERT(parentModelPk.columnCount() == childModelFk.columnCount());
  Q_ASSERT(parentModelPk.greatestColumn() < mParentModel->columnCount());
  Q_ASSERT(childModelFk.greatestColumn() < mChildModel->columnCount());

  mKey.setKey(parentModelPk, childModelFk);
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

void RelationKeyCopier::onChildModelRowsInserted(const QModelIndex & /*parent*/, int first, int last)
{
  Q_ASSERT(!mParentModel.isNull());
  Q_ASSERT(!mChildModel.isNull());

  qDebug() << "onChildModelRowsInserted() first: " << first << " , last " << last;
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

void RelationKeyCopier::copyKeyData(int childModelRow)
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
    mChildModel->setData( childModelIndex , mParentModel->data(parentModelIndex) );
  }
}

void RelationKeyCopier::reconnectSignalSlotsIfOk()
{
  if( mParentModel.isNull() || mChildModel.isNull() ){
    return;
  }
  disconnect(mChildModelRowsInsertedConnection);
  disconnect(mParentModelDataChangedConnection);
  if(mCopyTriggers & ChildModelRowsInserted){
    mChildModelRowsInsertedConnection = connect(mChildModel, &QAbstractItemModel::rowsInserted, this, &RelationKeyCopier::onChildModelRowsInserted);
  }
  if(mCopyTriggers & ParentModelDataChanged){
    mParentModelDataChangedConnection = connect(mParentModel, &QAbstractItemModel::dataChanged, this, &RelationKeyCopier::onParentModelDataChanged);
  }
}

void RelationKeyCopier::clearKey()
{
  mKey.clear();
}

}} // namespace Mdt{ namespace ItemModel{
