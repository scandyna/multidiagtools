/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "EditVehicleTypeClassTableModel.h"

using namespace Mdt::Entity;

namespace Mdt{ namespace Railway{

EditVehicleTypeClassTableModel::EditVehicleTypeClassTableModel(QObject* parent)
 : AbstractEditableCacheTableModel(parent)
{
  /// \todo See for a helper to make signal/slot connections
//   connect(&mCache, &EditVehicleTypeClassCache::cacheAboutToBeReset, this, &EditVehicleTypeClassTableModel::beginResetModel);
//   connect(&mCache, &EditVehicleTypeClassCache::cacheReset, this, &EditVehicleTypeClassTableModel::endResetModel);
}

// void EditVehicleTypeClassTableModel::setVehicleTypeClassRepository(const std::shared_ptr< VehicleTypeClassRepository >& repository)
// {
//   Q_ASSERT(repository.get() != nullptr);
// 
//   mCache.setVehicleTypeClassRepository(repository);
// }

// void EditVehicleTypeClassTableModel::setSelectQueryFactory(const QueryExpression::SelectQueryFactory& factory)
// {
//   Q_ASSERT(factory.isValid());
// 
//   mCache.setSelectQueryFactory(factory);
// }

// bool EditVehicleTypeClassTableModel::fetchAll()
// {
//   return mCache.fetchAll();
// }
// 
// int EditVehicleTypeClassTableModel::columnCountImpl() const
// {
//   return mCache.columnCount();
// }
// 
// int EditVehicleTypeClassTableModel::cachedRowCount() const
// {
//   return mCache.rowCount();
// }
// 
// QVariant EditVehicleTypeClassTableModel::horizontalHeaderDisplayRoleData(int column) const
// {
//   return cache()->horizontalHeaderName(column);
// }
// 
// QVariant EditVehicleTypeClassTableModel::displayRoleData(int row, int column) const
// {
//   return mCache.data(row, column);
// }
// 
// Container::TableCacheOperation EditVehicleTypeClassTableModel::operationAtRow(int row) const
// {
// 
// }

}} // namespace Mdt{ namespace Railway{
