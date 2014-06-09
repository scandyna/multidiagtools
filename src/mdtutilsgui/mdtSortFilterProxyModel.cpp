/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtSortFilterProxyModel.h"
#include "mdtAlgorithms.h"
#include <boost/graph/graph_concepts.hpp>
#include <QVariant>
#include <QSqlQueryModel>
#include <QSqlRecord>

#include <QDebug>

mdtSortFilterProxyModel::mdtSortFilterProxyModel(QObject *parent)
 : QSortFilterProxyModel(parent)
{
}

mdtSortFilterProxyModel::~mdtSortFilterProxyModel()
{
}

void mdtSortFilterProxyModel::clearColumnsSortOrder()
{
  pvColumnsSortOrder.clear();
}

void mdtSortFilterProxyModel::addColumnToSortOrder(int column, Qt::SortOrder order)
{
  // To sort columns in a given order, we simply sort each of them in reverse direction.
  // Here, we simply prepend each column. We do column index checking here.
  if(column < 0){
    return;
  }
  if(column >= columnCount()){
    return;
  }
  pvColumnsSortOrder.prepend(QPair<int, Qt::SortOrder>(column, order));
}

void mdtSortFilterProxyModel::addColumnToSortOrder(const QString &fieldName, Qt::SortOrder order)
{
  QSqlQueryModel *m;
  int column;

  m = dynamic_cast<QSqlQueryModel*>(sourceModel());
  if(m == 0){
    return;
  }
  column = m->record().indexOf(fieldName);
  if(column >= 0){
    addColumnToSortOrder(column, order);
  }
}

bool mdtSortFilterProxyModel::hasColumnToSort() const
{
  return (pvColumnsSortOrder.size() > 0);
}

QVector<int> mdtSortFilterProxyModel::sortedColumns() const
{
  QVector<int> lst;
  int i;

  for(i = (pvColumnsSortOrder.size() - 1); i >= 0; --i){
    lst.append(pvColumnsSortOrder.at(i).first);
  }

  return lst;
}

Qt::SortOrder mdtSortFilterProxyModel::sortOrder(int column) const
{
  int i;

  for(i = 0; i < pvColumnsSortOrder.size(); ++i){
    if(pvColumnsSortOrder.at(i).first == column){
      return pvColumnsSortOrder.at(i).second;
    }
  }

  return pvColumnsSortOrder.at(column).second;
}

bool mdtSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
  QVariant leftData = sourceModel()->data(left);
  QVariant rightData = sourceModel()->data(right);

  if((leftData.type() == QVariant::String) && (rightData.type() == QVariant::String)){
    return mdtAlgorithms::naturalCompareLessThan(leftData.toString(), rightData.toString(), sortCaseSensitivity());
  }

  return QSortFilterProxyModel::lessThan(left, right);
}

void mdtSortFilterProxyModel::sort(int column, Qt::SortOrder order)
{
  int i;
  QPair<int, Qt::SortOrder> p;

  qDebug() << "mdtSortFilterProxyModel::sort() - columns: " << pvColumnsSortOrder;
  
  if(pvColumnsSortOrder.isEmpty()){
    QSortFilterProxyModel::sort(column, order);
  }else{
    for(i = 0; i < pvColumnsSortOrder.size(); ++i){
      p = pvColumnsSortOrder.at(i);
      QSortFilterProxyModel::sort(p.first, p.second);
    }
  }
}
