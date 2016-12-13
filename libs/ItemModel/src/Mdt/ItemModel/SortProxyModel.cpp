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
#include "SortProxyModel.h"
#include "ColumnSortOrder.h"
#include <algorithm>
#include <QVariant>
#include <QCollator>
#include <QLocale>

#include <QDebug>

namespace Mdt{ namespace ItemModel{

SortProxyModel::SortProxyModel(QObject* parent)
 : QSortFilterProxyModel(parent)
{
}

void SortProxyModel::addColumnToSortOrder(int column, Qt::SortOrder sortOrder)
{
//   qDebug() << "add(" << column << ")";
  mColumnSortOrderList.addColumn(column, sortOrder);
}

void SortProxyModel::addColumnToSortOrder(int column, StringNumericMode numericMode, Qt::SortOrder sortOrder, Qt::CaseSensitivity caseSensitivity)
{
  addColumnToSortOrder(column, sortOrder);
  setColumnStringSortAttributes(column, numericMode, caseSensitivity);
}

void SortProxyModel::clearColumnsSortOrder()
{
  mColumnSortOrderList.clear();
}

void SortProxyModel::setColumnStringSortAttributes(int column, StringNumericMode numericMode, Qt::CaseSensitivity caseSensitivity)
{
  mColumnSortStringAttributesList.setColumn(column, caseSensitivity, numericMode == StringNumericMode::Natural);
}

void SortProxyModel::clearColumnsStringSortAttributes()
{
  mColumnSortStringAttributesList.clear();
}

void SortProxyModel::sort(int column, Qt::SortOrder order)
{
  Q_ASSERT(column >= -1);
  Q_ASSERT(column < columnCount());

//   qDebug() << "sort(" << column << "," << order <<")";
  QSortFilterProxyModel::sort(column, order);
}

void SortProxyModel::sort()
{
//   qDebug() << "sort()";
  std::for_each(mColumnSortOrderList.crbegin(), mColumnSortOrderList.crend(), [this](const ColumnSortOrder & cso){
                                                                  sort(cso.column(), cso.sortOrder());
                                                                });
}

bool SortProxyModel::lessThan(const QModelIndex & source_left, const QModelIndex & source_right) const
{
  Q_ASSERT( sourceModel() != nullptr );

  if( (!source_left.isValid()) || (!source_right.isValid()) ){
    return QSortFilterProxyModel::lessThan(source_left, source_right);
  }
  const auto leftData = sourceModel()->data(source_left, sortRole());
  const auto rightData = sourceModel()->data(source_right, sortRole());
//   qDebug() << "lessThan, left: " << leftData << " , right: " << rightData;
  /*
   * Handle string comparisons
   */
  if( (leftData.type() == QVariant::String) && (rightData.type() == QVariant::String) ){
    Q_ASSERT( source_left.column() == source_right.column() );
//     qDebug() << "RESULT: " << lessThatString(leftData.toString(), rightData.toString(), source_left.column());
    return lessThatString(leftData.toString(), rightData.toString(), source_left.column());
  }
  return QSortFilterProxyModel::lessThan(source_left, source_right);
}

bool SortProxyModel::lessThatString(const QString & left, const QString & right, int column) const
{
  Qt::CaseSensitivity caseSensitivity;
  // If we have string sort attributes for current column, use them
  const auto attributes = mColumnSortStringAttributesList.attributesForColumn(column);
  if(attributes.isNull()){
    caseSensitivity = sortCaseSensitivity();
  }else{
    caseSensitivity = attributes.caseSensitivity();
  }
  // If sorting is not locale aware, use default implementation
//   qDebug() << "isSortLocaleAware(): " << isSortLocaleAware();
//   qDebug() << "numericMode: " << attributes.numericMode();
  if( !isSortLocaleAware() && !attributes.numericMode() ){
//     qDebug() << "Sorting RAW..";
    return (QString::compare(left, right, caseSensitivity) < 0);
  }
  // Compare locale aware
  mCollator.setCaseSensitivity(caseSensitivity); // Ignored on some platforms
  mCollator.setNumericMode(attributes.numericMode());
  return (mCollator.compare(left, right) < 0 );
}

}} // namespace Mdt{ namespace ItemModel{
