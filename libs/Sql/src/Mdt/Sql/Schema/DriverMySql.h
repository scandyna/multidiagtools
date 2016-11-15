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
#ifndef MDT_SQL_SCHEMA_DRIVER_MYSQL_H
#define MDT_SQL_SCHEMA_DRIVER_MYSQL_H

#include "DriverImplementationInterface.h"

namespace Mdt{ namespace Sql{ namespace Schema{

  /*!\brief MySql schema driver implementation
   */
  class DriverMySql : public DriverImplementationInterface
  {
   public:

    /*! \brief Constructor
     */
    DriverMySql(const QSqlDatabase & db);

    /*! \brief Get driver type
     */
    DriverType type() const override
    {
      return DriverType::MySQL;
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

    /*! \brief Get field definition of a auto increment primary key
     */
    QString getPrimaryKeyFieldDefinition(const AutoIncrementPrimaryKey & pk) const override;

    /*! \brief Get primary key for table from database
     */
    Mdt::Expected<PrimaryKeyContainer> getTablePrimaryKeyFromDatabase(const QString & tableName) const override;

    /*! \brief Get list of foreign keys for table from database
     */
    Mdt::Expected<ForeignKeyList> getTableForeignKeyListFromDatabase(const QString & tableName) const override;

    /*! \brief Get a list of idexes for table from database
     */
    Mdt::Expected<IndexList> getTableIndexListFromDatabase(const QString & tableName) const override;

   private:

    
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_DRIVER_MYSQL_H
