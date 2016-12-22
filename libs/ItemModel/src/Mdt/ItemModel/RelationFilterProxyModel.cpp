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
#include "RelationFilterProxyModel.h"
#include "Expression/ParentModelEvalData.h"

#include <QDebug>

using Mdt::ItemModel::Expression::ParentModelEvalData;

namespace Mdt{ namespace ItemModel{

RelationFilterProxyModel::RelationFilterProxyModel(QObject* parent)
 : QSortFilterProxyModel(parent)
{
}

void RelationFilterProxyModel::setParentModel(QAbstractItemModel *model)
{
  Q_ASSERT(model != nullptr);

  mParentModel = model;
  invalidateFilter();
}

void RelationFilterProxyModel::setParentModelMatchRow(int row)
{
  mParentModelRow = row;
  qDebug() << "RelationFilterProxyModel::setParentModelMatchRow(" << row << ")";
  qDebug() << "- parent model: " << mParentModel << " , source model: " << sourceModel();
  qDebug() << "- expression set: " << !mFilterExpression.isNull();
  
  invalidateFilter();
  
  qDebug() << "- parent rows: " << mParentModel->rowCount();
  qDebug() << "- source rows: " << sourceModel()->rowCount();
  qDebug() << "- this   rows: " << rowCount();
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
  qDebug() << "-> eval() ...";
  return mFilterExpression.eval(sourceModel(), source_row, ParentModelEvalData(mParentModel, mParentModelRow), filterCaseSensitivity());
}

}} // namespace Mdt{ namespace ItemModel{
