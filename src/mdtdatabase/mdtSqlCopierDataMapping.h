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
#ifndef MDT_SQL_COPIER_DATA_MAPPING_H
#define MDT_SQL_COPIER_DATA_MAPPING_H

#include <QVector>
#include <QVariant>
#include <memory>

namespace mdt{ namespace sql{ namespace copier{
  class TableMapping;
}}}
class QSqlRecord;

/*! \brief Helper class to build destination record when using mdtSqlCopierTableMapping
 *
 * Depending how table mapping is set, it happens that source and destination records contains not the same count of fields.
 *  This is for example the case if a fixed value must be copied to a destination field.
 *
 * The recommanded way to build a destination record is to use mdtSqlCopierDataMapping.
 *
 * Typical usage:
 * \code
 *  QSqlDatabase db;
 *  QSqlQuery sourceQuery;
 *  QSqlQuery destinationQuery;
 *  mdtSqlDatabaseCopierTableMapping mapping;
 *  QString sql;
 *  // Setup mapping
 *  ...
 *  // Fetch source data
 *  sql = mapping.getSqlForSourceTableSelect(db);
 *  if(!sourceQuery.exec(sql)){
 *    // Error handling..
 *  }
 *  // Copy
 *  while(sourceQuery.next()){
 *    // Build destination record
 *    mdtSqlCopierDataMapping dataMapping;
 *    dataMapping.setSourceRecord(sourceQuery.record(), mapping);
 *    // Prepare destination query
 *    ...
 *    // Bind values to destination query
 *    for(const auto & value : dataMapping.destinationRecord){
 *      destinationQuery.addBindValue(value);
 *    }
 *  }
 * \endcode
 */
struct mdtSqlCopierDataMapping
{
  /*! \brief Destination record
   */
  QVector<QVariant> destinationRecord;

  /*! \brief Set source record
   *
   * \param sourceRecord Record that contains data from source
   * \param tableMapping Used to map data
   */
  void setSourceRecord(const QSqlRecord & sourceRecord, const std::shared_ptr<mdt::sql::copier::TableMapping> & mapping);

  /*! \brief Add source record item
   *
   * Can be used if mdtSqlCopierDataMapping does not support source record type.
   *
   * \param sourceRecordIndex Index in source record
   * \param sourceRecordItem Data item in source record
   * \param tableMapping Used to map data
   */
  void addSourceRecordItem(int sourceRecordIndex, const QVariant & sourceRecordItem, const std::shared_ptr<mdt::sql::copier::TableMapping> & mapping);

  /*! \brief Get count of destination record fields
   */
  int size() const
  {
    return destinationRecord.size();
  }

  /*! \brief Check if destination record is empty
   */
  bool isEmpty() const
  {
    return destinationRecord.isEmpty();
  }

  /*! \brief Clear
   */
  void clear()
  {
    destinationRecord.clear();
  }
};

#endif // #ifndef MDT_SQL_COPIER_DATA_MAPPING_H
