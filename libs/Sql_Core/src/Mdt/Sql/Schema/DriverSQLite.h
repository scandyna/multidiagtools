/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_SQL_SCHEMA_DRIVER_SQLITE_H
#define MDT_SQL_SCHEMA_DRIVER_SQLITE_H

#include "DriverImplementationInterface.h"

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief SQLite schema driver
   */
  class DriverSQLite : public DriverImplementationInterface
  {
   public:

    /*! \brief Constructor
     */
    DriverSQLite(const QSqlDatabase & db);

    /*! \brief Get driver type
     */
    DriverType type() const override
    {
      return DriverType::SQLite;
    }

    /*! \brief Get default charset for current database/schema
     */
    Charset getDatabaseDefaultCharset() const override;

    /*! \brief Get collation definition
     */
    QString getCollationDefinition(const Collation & collation) const override;

    /*! \brief Get field definition
     */
    QString getFieldDefinition(const Field & field) const override;

    /*! \brief Get FieldList for table from database
     */
    virtual Mdt::Expected<FieldList> getTableFieldListFromDatabase(const QString & tableName) const override;

    /*! \brief Get field definition of a auto increment primary key
     */
    QString getPrimaryKeyFieldDefinition(const AutoIncrementPrimaryKey & pk) const override;

    /*! \brief Get primary key for table from database
     */
    virtual Mdt::Expected<PrimaryKeyContainer> getTablePrimaryKeyFromDatabase(const QString & tableName) const override;

    /*! \brief Get list of foreign keys for table from database
     */
    Mdt::Expected<ForeignKeyList> getTableForeignKeyListFromDatabase(const QString & tableName) const override;

    /*! \brief Get a list of idexes for table from database
     */
    Mdt::Expected<IndexList> getTableIndexListFromDatabase(const QString & tableName) const override
    {
      return getTableIndexListFromDatabase(tableName, QStringLiteral("c"));
    }

   private:

    /*! \brief Get a list of idexes for table from database for origin
     */
    Mdt::Expected<IndexList> getTableIndexListFromDatabase(const QString & tableName, const QString & origin) const;

    /*! \brief Add columns to index
     */
    bool addColumnsToIndex(Index & index) const;

    /*! \brief Get case sensitivity for index from database for fieldName
     */
    Mdt::Expected<CaseSensitivity> getIndexFieldCaseSensitivityFromDatabase(const Index & index, const QString & fieldName) const;

    /*! \brief Get default value
     */
    QVariant fieldDefaultValue(const QVariant & v) const;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_DRIVER_SQLITE_H
