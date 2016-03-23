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
#ifndef MDT_SQL_COPIER_CSV_FILE_IMPORT_MAPPING_H
#define MDT_SQL_COPIER_CSV_FILE_IMPORT_MAPPING_H

#include "CsvFileImportTableMapping.h"
#include "mdtError.h"
#include "mdtExpected.h"
#include <QSqlDatabase>
#include <QVector>
#include <QStringList>
#include <memory>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Store CSV file import mapping
   */
  class CsvFileImportMapping
  {
   public:

    /*! \brief Get count of table mapping
     */
    int tableMappingCount() const
    {
      return pvTableMappingList.size();
    }

    /*! \brief Set destination database
     *
     * Will also reset mapping.
     *
     * \sa resetMapping()
     */
    mdtExpected<bool> setDestinationDatabase(const QSqlDatabase & db);

    /*! \brief Get destination database
     */
    QSqlDatabase destinationDatabase() const
    {
      return pvDestinationDatabase;
    }

    /*! \brief Reset mapping
     *
     * Will clear mapping,
     *  then, generate it for each available table in destination database.
     *  After that, no valid mapping is set to source database.
     */
    mdtExpected<bool> resetMapping();

    /*! \brief Get source table name for given mapping index
     *
     * \pre index must be in a valid range
     */
    QString sourceTableName(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvTableMappingList.size());
      return pvTableMappingList.at(index)->sourceTableName();
    }

    /*! \brief Get destination table name for given mapping index
     *
     * \pre index must be in a valid range
     */
    QString destinationTableName(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvTableMappingList.size());
      return pvTableMappingList.at(index)->destinationTableName();
    }

    /*! \brief Get table mapping at given index
     *
     * \pre index must be in a valid range
     */
    const std::shared_ptr<CsvFileImportTableMapping> tableMapping(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvTableMappingList.size());
      return pvTableMappingList.at(index);
    }

   private:

    QSqlDatabase pvDestinationDatabase;
    QVector<std::shared_ptr<CsvFileImportTableMapping>> pvTableMappingList;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_CSV_FILE_IMPORT_MAPPING_H

