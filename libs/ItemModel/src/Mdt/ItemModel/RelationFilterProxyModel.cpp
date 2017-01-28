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

#include <QDebug>

using Mdt::ItemModel::Expression::ParentModelEvalData;

namespace Mdt{ namespace ItemModel{

RelationFilterProxyModel::RelationFilterProxyModel(QObject* parent)
 : QSortFilterProxyModel(parent),
   mKeyCopier(new RelationKeyCopier)
{
  mKeyCopier->setChildModel(this);  /// \todo Check: think it should be source model
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

bool RelationFilterProxyModel::insertRows(int row, int count, const QModelIndex & parent)
{
  /*
   * QSortFilterProxyModel::insertRows() is implemented something like this:
   * - Create a list of source rows that matches filterAcceptsRow() , if not allready done
   * - Check row on list size (looks like a bug: should check the highest source row in the list, not list size)
   * - Determine source_row:
   *   -> If row >= list size: source_row = liste size (Again, this seems wrong)
   *   -> Else: source_row = row at in list (Ok)
   * - Call sourceModel::insertRows(source_row, count, source_parent)
   *
   * This says that, if row > this proxy model's rowCount(),
   * the insert location will be wrong (= not as expected).
   * As workaround, we proceed this way:
   * - Turn mInserting flag ON to tell filterAcceptsRow() to accept all rows,
   *   so the size of QSortFilterProxyModel's list of source rows will be equal to source model's row count
   * - Invalidate filter to force QSortFilterProxyModel to recreate its mapping (the list of source rows)
   * - Call QSortFilterProxyModel::insertRows()
   * - Turn mInserting flag off to tell filterAcceptsRow() to proceed in normal way
   * - Invalidate filter ? Or not.
   * 
   * QSortFilterProxyModel will insert rows into source model
   * without mapping row to source.
   * If we copy key data from here, we end up editing the wrong row.
   * But, QSortFilterProxyModel::rowsInserted() signal
   * reports expected first, last row for this proxy model.
   * So, we do key data copy in onRowsInserted() slot.
   */
  qDebug() << "RFPM::insertRows() - row: " << row << " , row count: " << rowCount() << " , src row count: " << sourceModel()->rowCount();
  
  mInserting = true;
  
  /// If row > rowCount()
  invalidateFilter();
  
  if( !QSortFilterProxyModel::insertRows(row, count, parent) ){
    mInserting = false;
    return false;
  }
  mInserting = false;
  
  /// Maybe not invalidate here ?
  invalidateFilter();
  
  qDebug() << "RFPM::insertRows() - DONE";
  return true;
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
  qDebug() << "RFPM::onRowsInserted() - first: " << first;
  RowRange r;
  r.setFirstRow(first);
  r.setLastRow(last);
  mKeyCopier->copyAllKeyData(r, parent);

  qDebug() << "RFPM::onRowsInserted() - DONE";
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
  qDebug() << "RFPM::filterAcceptsRow() - eval..";
  if(mInserting){
    qDebug() << "RFPM::filterAcceptsRow() - mInserting -> accept";
    return true;
  }
  return mFilterExpression.eval(sourceModel(), source_row, ParentModelEvalData(mParentModel, mParentModelRow), filterCaseSensitivity());
}

}} // namespace Mdt{ namespace ItemModel{
