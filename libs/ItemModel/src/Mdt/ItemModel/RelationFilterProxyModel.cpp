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
#include "Expression/ParentModelEvalData.h"

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

  mParentModel = model;
  mParentModelRow = -1;
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

void RelationFilterProxyModel::setDynamicSortFilter(bool enable)
{
  SortFilterProxyModel::setDynamicSortFilter(enable);
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
  mParentModelRow = row;
  mKeyCopier->setParentModelCurrentRow(row);
  invalidateFilter();
}

void RelationFilterProxyModel::onSourceModelChanged()
{
  disconnect(mRowsInsertedConnection);
  auto *model = sourceModel();
  if(model == nullptr){
    return;
  }
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

bool RelationFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
{
  if(source_parent.isValid()){
    return false;
  }
  if(mParentModel.isNull() || (sourceModel() == nullptr)){
    return false;
  }
  if( (mParentModelRow < 0) || (mParentModelRow >= mParentModel->rowCount()) ){
    return false;
  }
  if(mFilterExpression.isNull()){
    return true;
  }
  return mFilterExpression.eval(sourceModel(), source_row, ParentModelEvalData(mParentModel, mParentModelRow), filterCaseSensitivity());
}

}} // namespace Mdt{ namespace ItemModel{
