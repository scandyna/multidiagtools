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

  qDebug() << "sort(" << column << ")";
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
//   Q_ASSERT( source_left.column() == source_right.column() );
//   const auto column = source_left.column();
  const auto leftData = sourceModel()->data(source_left);
  const auto rightData = sourceModel()->data(source_right);
//   qDebug() << "column: " << column;
  qDebug() << "lessThan, left: " << leftData << " , right: " << rightData;
  /*
   * Handle string comparisons
   */
  if( (leftData.type() == QVariant::String) && (rightData.type() == QVariant::String) ){
    Q_ASSERT( source_left.column() == source_right.column() );
    return lessThatString(leftData.toString(), rightData.toString(), source_left.column());
//     /// \todo QCollator can be expensive to construct, due to QLocale construction. Bench this
//     QCollator collator;
//     ///collator.setLocale( QLocale(QLocale::English, QLocale::AnyCountry) );
//     // If we have string sort attributes for current column, use them
//     auto csa = mColumnSortStringAttributesList.attributesForColumn(column);
//     if(csa.isNull()){
//       qDebug() << "Taking global attributes";
//       collator.setCaseSensitivity(sortCaseSensitivity());
//       collator.setNumericMode(false);
//     }else{
//       collator.setCaseSensitivity(csa.caseSensitivity());
//       collator.setNumericMode(csa.numericMode());
//     }
//     qDebug() << "CS: " << collator.caseSensitivity();
//     qDebug() << "Local: " << isSortLocaleAware();
//     ///qDebug() << "less: " << (collator.compare(leftData.toString(), rightData.toString()) < 0);
//     return QSortFilterProxyModel::lessThan(source_left, source_right);
//     ///return collator.compare(leftData.toString(), rightData.toString()) < 0;
  }
  
  return QSortFilterProxyModel::lessThan(source_left, source_right);
}

bool SortProxyModel::lessThatString(const QString & left, const QString & right, int column) const
{
  Qt::CaseSensitivity caseSensitivity;
  // If we have string sort attributes for current column, use them
  const auto attributes = mColumnSortStringAttributesList.attributesForColumn(column);
  if(attributes.isNull()){
    qDebug() << "Take CS global";
    caseSensitivity = sortCaseSensitivity();
  }else{
    qDebug() << "Take CS from attributes: " << attributes.caseSensitivity();
    caseSensitivity = attributes.caseSensitivity();
  }
  // If sorting is not locale aware, use default implementation
  qDebug() << "CS: " << caseSensitivity;
  qDebug() << "isSortLocaleAware(): " << isSortLocaleAware();
  qDebug() << "numericMode: " << attributes.numericMode();
  if( !isSortLocaleAware() && !attributes.numericMode() ){
    qDebug() << "Sorting RAW..";
    return (QString::compare(left, right, caseSensitivity) < 0);
  }
  // Compare locale aware
  /// \todo QCollator can be expensive to construct, due to QLocale construction. Bench this
  qDebug() << "Sorting local aware..";
  QCollator collator;
  collator.setCaseSensitivity(caseSensitivity); // Ignored on some platforms
  collator.setNumericMode(attributes.numericMode());
  return (collator.compare(left, right) < 0 );
}

}} // namespace Mdt{ namespace ItemModel{
