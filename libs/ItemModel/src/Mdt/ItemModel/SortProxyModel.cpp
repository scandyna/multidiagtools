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

// #include <QDebug>

namespace Mdt{ namespace ItemModel{

SortProxyModel::SortProxyModel(QObject* parent)
 : QSortFilterProxyModel(parent)
{
}

void SortProxyModel::addColumnToSortOrder(int column, Qt::SortOrder sortOrder)
{
  Q_ASSERT(column >= 0);

  mColumnSortOrderList.addColumn(column, sortOrder);
}

void SortProxyModel::addColumnToSortOrder(int column, StringNumericMode numericMode, Qt::SortOrder sortOrder, Qt::CaseSensitivity caseSensitivity)
{
  Q_ASSERT(column >= 0);

  addColumnToSortOrder(column, sortOrder);
  setColumnStringSortAttributes(column, numericMode, caseSensitivity);
}

void SortProxyModel::clearColumnsSortOrder()
{
  mColumnSortOrderList.clear();
}

void SortProxyModel::setColumnStringSortAttributes(int column, StringNumericMode numericMode, Qt::CaseSensitivity caseSensitivity)
{
  Q_ASSERT(column >= 0);

  mColumnSortStringAttributesList.setColumn(column, caseSensitivity, numericMode == StringNumericMode::Natural);
}

void SortProxyModel::clearColumnsStringSortAttributes()
{
  mColumnSortStringAttributesList.clear();
}

void SortProxyModel::sort(int column, Qt::SortOrder order)
{
  Q_ASSERT(column >= -1);

  QSortFilterProxyModel::sort(column, order);
  emit columnSorted(column);
}

void SortProxyModel::sort()
{
  std::for_each(mColumnSortOrderList.crbegin(), mColumnSortOrderList.crend(), [this](const ColumnSortOrder & cso){
                                                                  sort(cso.column(), cso.sortOrder());
                                                                });
  emit modelSorted();
}

void SortProxyModel::setDynamicSortFilter(bool enable)
{
  /*
   * Only resort when enabling
   */
  const bool resort = ( (enable != dynamicSortFilter()) && enable );
  QSortFilterProxyModel::setDynamicSortFilter(enable);
  if(resort){
    sort();
  }
}

void SortProxyModel::setSortCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
  const bool resort = caseSensitivity != sortCaseSensitivity();
  QSortFilterProxyModel::setSortCaseSensitivity(caseSensitivity);
  if(resort){
    sort();
  }
}

void SortProxyModel::setSortLocaleAware(bool on)
{
  const bool resort = on != isSortLocaleAware();
  QSortFilterProxyModel::setSortLocaleAware(on);
  if(resort){
    sort();
  }
}

void SortProxyModel::setSortRole(int role)
{
  const bool resort = role != sortRole();
  QSortFilterProxyModel::setSortRole(role);
  if(resort){
    sort();
  }
}

void SortProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
  Q_ASSERT(sourceModel != nullptr);
  /*
   * If dynamic sort is enabled, we must sort once the model was set.
   * QSortFilterProxyModel handles this.
   * If QSortFilterProxyModel::sort() was previously called with a column > -1,
   * sorting will be processed after model was set.
   * To prevent sorting 2x , disable dynamic sort while setting source model
   */
  const bool dynSortEnabled = dynamicSortFilter();
  setDynamicSortFilter(false);
  disconnect( sourceModel, &QAbstractItemModel::rowsInserted, this, &SortProxyModel::onRowsInserted );
  disconnect( sourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &SortProxyModel::onModelAboutToBeReset );
  disconnect( sourceModel, &QAbstractItemModel::modelReset, this, &SortProxyModel::onModelReset );
  disconnect( sourceModel, &QAbstractItemModel::dataChanged, this, &SortProxyModel::onDataChanged );
  QSortFilterProxyModel::setSourceModel(sourceModel);
  connect( sourceModel, &QAbstractItemModel::rowsInserted, this, &SortProxyModel::onRowsInserted );
  connect( sourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &SortProxyModel::onModelAboutToBeReset );
  connect( sourceModel, &QAbstractItemModel::modelReset, this, &SortProxyModel::onModelReset );
  connect( sourceModel, &QAbstractItemModel::dataChanged, this, &SortProxyModel::onDataChanged );
  // setDynamicSortFilter() will call sort() itself if dynamic sort is enabled
  setDynamicSortFilter(dynSortEnabled);
}

void SortProxyModel::onRowsInserted(const QModelIndex &, int, int)
{
  if(dynamicSortFilter()){
    sort();
  }
}

void SortProxyModel::onModelAboutToBeReset()
{
  /*
   * QSortFilterProxyModel uses its private sort methods.
   * To prevent sorting 2x the model, disable dynamic sort
   */
  pvDynamicSortWasEnabled = dynamicSortFilter();
  setDynamicSortFilter(false);
}

void SortProxyModel::onModelReset()
{
  /*
   * Dynamic sort was disabled by onModelAboutToBeReset().
   * Re-enable it if it was before.
   * This will also call sort() if dynamic sort was enabled.
   */
  setDynamicSortFilter(pvDynamicSortWasEnabled);
}

void SortProxyModel::onDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &)
{
  /*
   * If dynamic sort is enabled, QSortFilterProxyModel will sort using its private implementation
   * (which seems optimized to not sort all rows).
   * To have a minimum to resort, we should:
   *  - Implement a function that proceeds as QSortFilterProxyModel's private implementation, which looks not easy
   *  - Filter dataChanged() , which looks ugly
   *
   * For instance, sort the model if range topLeft,bottomRight contains a column that exists in sortOrderList.
   */
  if(dynamicSortFilter()){
    if( isColumnInSortOrder(topLeft.column(), bottomRight.column()) ){
      sort();
    }
  }
}

bool SortProxyModel::lessThan(const QModelIndex & source_left, const QModelIndex & source_right) const
{
  Q_ASSERT( sourceModel() != nullptr );

  if( (!source_left.isValid()) || (!source_right.isValid()) ){
    return QSortFilterProxyModel::lessThan(source_left, source_right);
  }
  const auto leftData = sourceModel()->data(source_left, sortRole());
  const auto rightData = sourceModel()->data(source_right, sortRole());
  /*
   * Handle string comparisons
   */
  if( (leftData.type() == QVariant::String) && (rightData.type() == QVariant::String) ){
    Q_ASSERT( source_left.column() == source_right.column() );
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
  if( !isSortLocaleAware() && !attributes.numericMode() ){
    return (QString::compare(left, right, caseSensitivity) < 0);
  }
  // Compare locale aware
  mCollator.setCaseSensitivity(caseSensitivity); // Ignored on some platforms
  mCollator.setNumericMode(attributes.numericMode());
  return (mCollator.compare(left, right) < 0 );
}

bool SortProxyModel::isColumnInSortOrder(int left, int right) const
{
  return ( std::find_if(mColumnSortOrderList.crbegin(), mColumnSortOrderList.crend(), [left, right](const ColumnSortOrder & cso){
    return ( (cso.column() >= left) && (cso.column() <= right) ); 
  }) != mColumnSortOrderList.crend() );
}

}} // namespace Mdt{ namespace ItemModel{
