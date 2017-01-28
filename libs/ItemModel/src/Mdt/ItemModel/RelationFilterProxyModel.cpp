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
 : QSortFilterProxyModel(parent),
   mKeyCopier(new RelationKeyCopier)
{
  mKeyCopier->setChildModel(this);
  connect(this, &RelationFilterProxyModel::rowsInserted, this, &RelationFilterProxyModel::onRowsInserted);
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
  QSortFilterProxyModel::setDynamicSortFilter(enable);
}

void RelationFilterProxyModel::setParentModelMatchRow(int row)
{
  mParentModelRow = row;
  mKeyCopier->setParentModelCurrentRow(row);
  invalidateFilter();
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
  mInserting = false;
}

bool RelationFilterProxyModel::insertRows(int row, int count, const QModelIndex & parent)
{
  /*
   * QSortFilterProxyModel will insert rows into source model
   * without mapping row to source.
   * If we copy key data from here, we end up editing the wrong row.
   * But, QSortFilterProxyModel::rowsInserted() signal
   * reports expected first, last row for this proxy model.
   * So, we do key data copy in onRowsInserted() slot.
   */
  mInserting = true;
  if( !QSortFilterProxyModel::insertRows(row, count, parent) ){
    mInserting = false;
    return false;
  }
  return true;
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
  if(mInserting){
    return true;
  }
  return mFilterExpression.eval(sourceModel(), source_row, ParentModelEvalData(mParentModel, mParentModelRow), filterCaseSensitivity());
}

}} // namespace Mdt{ namespace ItemModel{
