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
#include "RelationFilterProxyModel.h"
#include "RelationKeyCopier.h"
#include "RowRange.h"
#include "ColumnRange.h"
#include "Expression/ParentModelEvalData.h"
#include <algorithm>

// #include <QDebug>

using Mdt::ItemModel::Expression::ParentModelEvalData;

namespace Mdt{ namespace ItemModel{

RelationFilterProxyModel::RelationFilterProxyModel(QObject* parent)
 : SortFilterProxyModel(parent),
   mKeyCopier(new RelationKeyCopier)
{
  connect(this, &RelationFilterProxyModel::sourceModelChanged, this, &RelationFilterProxyModel::onSourceModelChanged);
}

RelationFilterProxyModel::~RelationFilterProxyModel()
{
}

void RelationFilterProxyModel::setParentModel(QAbstractItemModel *model)
{
  Q_ASSERT(model != nullptr);

//   qDebug() << "RFPM: setParentModel() " << model;

  disconnect(mParentModelDataChangedConnection);
  mParentModel = model;
  mParentModelDataChangedConnection = connect(model, &QAbstractItemModel::dataChanged, this, &RelationFilterProxyModel::onParentModelDataChanged);
//   mParentModelRow = -1;
  setParentModelMatchRow(mParentModelRow);
  mKeyCopier->setParentModel(model);
  invalidateFilter();
}

void RelationFilterProxyModel::setFilter(const RelationFilterExpression & expression)
{
  Q_ASSERT(!expression.isNull());

  mFilterExpression = expression;
  mKeyCopier->setKey(expression.getRelationKeyForEquality());
  invalidateFilter();
}

void RelationFilterProxyModel::setFilter(const RelationKey & relationKey)
{
  Q_ASSERT(!relationKey.isNull());
  Q_ASSERT(relationKey.columnPairCount() <= 4);

  mFilterExpression = RelationFilterExpression::fromRelationKey(relationKey);
  mKeyCopier->setKey(relationKey);
  invalidateFilter();
}

void RelationFilterProxyModel::setFilter(const PrimaryKey & parentModelPk, const ForeignKey & childModelFk)
{
  Q_ASSERT(!parentModelPk.isNull());
  Q_ASSERT(!childModelFk.isNull());
  Q_ASSERT(parentModelPk.columnCount() == childModelFk.columnCount());
  Q_ASSERT(parentModelPk.columnCount() <= 4);

  RelationKey key;
  key.setKey(parentModelPk, childModelFk);
  setFilter(key);
}

RelationKey RelationFilterProxyModel::relationKeyForEquality() const
{
  return mKeyCopier->key();
}

void RelationFilterProxyModel::setDynamicSortFilter(bool enable)
{
  SortFilterProxyModel::setDynamicSortFilter(enable);
}

RowList RelationFilterProxyModel::getCurrentSourceModelRowList() const
{
  RowList list;

  if(columnCount() < 1){
    return list;
  }
  const int n = rowCount();
  for(int row = 0; row < n; ++row){
    const auto idx = index(row, 0);
    list.append( mapToSource(idx).row() );
  }

  return list;
}

bool RelationFilterProxyModel::insertRows(int row, int count, const QModelIndex & parent)
{
  mInserting = true;
  const bool ok = SortFilterProxyModel::insertRows(row, count, parent);
  mInserting = false;

  return ok;
}

void RelationFilterProxyModel::setParentModelMatchRow(int row)
{
  Q_ASSERT(row >= -1);

//   qDebug() << "RFPM::setParentModelMatchRow() - REQ row " << row;
  if(mParentModel.isNull()){
    mParentModelRow = -1;
  }else{
//     qDebug() << " -> PM rows: " << mParentModel->rowCount();
    if(row < mParentModel->rowCount()){
      mParentModelRow = row;
    }else{
      mParentModelRow = -1;
    }
  }
//   qDebug() << "RFPM::setParentModelMatchRow() - row set to " << mParentModelRow;
  mKeyCopier->setParentModelCurrentRow(mParentModelRow);
  invalidateFilter();
}

void RelationFilterProxyModel::onSourceModelChanged()
{
  auto *model = sourceModel();
  Q_ASSERT(model != nullptr);
  disconnect(mRowsInsertedConnection);
  mKeyCopier->setChildModel(model);
  mRowsInsertedConnection = connect(model, &QAbstractItemModel::rowsInserted, this, &RelationFilterProxyModel::onRowsInserted);
}

void RelationFilterProxyModel::onRowsInserted(const QModelIndex& parent, int first, int last)
{
  if(!mInserting){
    return;
  }
  RowRange r;
  r.setFirstRow(first);
  r.setLastRow(last);
  mKeyCopier->copyAllKeyData(r, parent);
}

void RelationFilterProxyModel::onParentModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> & roles)
{
  if( (!roles.isEmpty()) && (!roles.contains(Qt::EditRole)) && (!roles.contains(Qt::DisplayRole)) ){
    return;
  }
  /*
   * We only act on currently filtered rows.
   * Those are filtered regarding parent model row
   */
  RowRange r;
  r.setFirstIndex(topLeft);
  r.setLastIndex(bottomRight);
  if(!r.containsRow(mParentModelRow)){
    return;
  }
  ColumnRange c;
  c.setFirstIndex(topLeft);
  c.setLastIndex(bottomRight);
  mKeyCopier->copyKeyData( getCurrentSourceModelRowList(), c );
  invalidateFilter();
}

bool RelationFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
{
  if(mParentModelRow < 0){
    return false;
  }
  if(source_parent.isValid()){
    return false;
  }
  Q_ASSERT(!mParentModel.isNull());
  Q_ASSERT(sourceModel() != nullptr);
  Q_ASSERT(mParentModelRow >= 0);
  Q_ASSERT(mParentModelRow < mParentModel->rowCount());
//   if(mParentModel.isNull() || (sourceModel() == nullptr)){
//     return false;
//   }
//   if( (mParentModelRow < 0) || (mParentModelRow >= mParentModel->rowCount()) ){
//     return false;
//   }
  if(mFilterExpression.isNull()){
    return true;
  }
  /*
   * After a model reset, possibly due to changing source model,
   * filter expression possibly does not match until setFilter() is called again.
   * We don't want to constraint setup order, so do bound checking here.
   */
  if( sourceModel()->columnCount() < (mFilterExpression.greatestColumn()+1) ){
    return false;
  }
  return mFilterExpression.eval(sourceModel(), source_row, ParentModelEvalData(mParentModel, mParentModelRow), filterCaseSensitivity());
}

}} // namespace Mdt{ namespace ItemModel{
