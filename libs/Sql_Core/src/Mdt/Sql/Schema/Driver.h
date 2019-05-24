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
#ifndef MDT_SQL_SCHEMA_DRIVER_H
#define MDT_SQL_SCHEMA_DRIVER_H

#include "DriverType.h"
#include "FieldType.h"
#include "FieldTypeList.h"
#include "PrimaryKeyContainer.h"
// #include "ForeignFieldList.h"
#include "../Connection.h"
#include "Mdt/Error.h"
#include "Mdt/Expected.h"
#include <QSqlDriver>
#include <QString>
#include <QMetaType>
#include <QVariant>
#include <memory>

class QSqlDatabase;

namespace Mdt{ namespace Sql{ namespace Schema{

  class DriverImplementationInterface;
//   class PrimaryKeyContainer;
  class ForeignKeyList;
  class Table;
  class View;
  class TablePopulation;
  class Schema;

  template<typename T>
  class TableTemplate;

  template<typename T>
  class ViewTemplate;

  template<typename T>
  class SchemaTemplate;

  template<typename T>
  class TablePopulationTemplate;

  /*! \brief SQL Schema driver
   */
  class Driver
  {
   public:

    /*! \brief Create a driver based on the database referenced by \a connection
     *
     * The mapping is done by typeFromDbmsType()
     *  If \a connection refers to a database handle that has a driver that is not supported,
     *  isValid() will return false and lastError()
     *  also contains a description.
     *
     * \pre \a connection must refer to a valid database handle
     */
    explicit Driver(const Connection & connection);

    /*! \brief Destructor
     */
    // Cannot be inlined because unique_ptr must have full definition of driver implementation object
    ~Driver();

    // Copy disabled
    Driver(const Driver &) = delete;
    Driver & operator=(const Driver &) = delete;
    // Move disabled
    Driver(Driver &&) = delete;
    Driver & operator=(Driver &&) = delete;

    /*! \brief Get driver type
     */
    DriverType type() const;

    /*! \brief Check if driver is valid
     *
     * Driver is valid if type() is different than DriverType::Unknown
     */
    bool isValid() const
    {
      return (type() != DriverType::Unknown);
    }

    /*! \brief Get a list of avaliable field type
     *
     * \pre Driver must be valid
     */
    FieldTypeList getAvailableFieldTypeList() const;

    /*! \brief Create table in database
     *
     * \pre Driver must be valid
     */
    bool createTable(const Table & table);

    /*! \brief Create table in database
     *
     * \pre Driver must be valid
     */
    template<typename T>
    bool createTable(const TableTemplate<T> & table)
    {
      return createTable(table.toTable());
    }

    /*! \brief Drop table
     *
     * \pre Driver must be valid
     */
    bool dropTable(const Table & table);

    /*! \brief Drop table
     *
     * \pre Driver must be valid
     */
    /*
     * This could be somewhat optimized
     * if the whole definition of table is required.
     * For example, implement a private function
     * (if needed, adapt TableTemplate)
     * that takes only what is needed.
     * Note: does TableTemplate::toTable() make a copy ?
     */
    template<typename T>
    bool dropTable(const TableTemplate<T> & table)
    {
      return dropTable(table.toTable());
    }

    /*! \brief Get primary key for table from database
     *
     * Returned primary key type depends on how it was defined:
     *  - For a auto increment primary key, type will be AutoIncrementPrimaryKeyType
     *  - For other cases, type will be PrimaryKeyType
     *
     * \pre Driver must be valid
     */
    Mdt::Expected<PrimaryKeyContainer> getTablePrimaryKeyFromDatabase(const QString & tableName) const;

//     /*! \brief Get primary key for table from database
//      *
//      * \pre Driver must be valid
//      * \sa getTablePrimaryKeyFromDatabase(const QString&)
//      *
//      * \todo Has this overload any sense ?
//      */
//     Mdt::Expected<PrimaryKeyContainer> getTablePrimaryKeyFromDatabase(const Table & table) const;

//     /*! \brief Get primary key for table from database
//      *
//      * \pre Driver must be valid
//      * \sa getTablePrimaryKeyFromDatabase(const QString&)
//      *
//      * \todo Has this overload any sense ?
//      */
//     template<typename T>
//     Mdt::Expected<PrimaryKeyContainer> getTablePrimaryKeyFromDatabase(const TableTemplate<T> & table) const
//     {
//       return getTablePrimaryKeyFromDatabase(table.tableName());
//     }

    /*! \brief Get list of foreign keys for table from database
     *
     * \pre Driver must be valid
     */
    Mdt::Expected<ForeignKeyList> getTableForeignKeyListFromDatabase(const QString & tableName) const;

    /*! \brief Create view in database
     *
     * \pre Driver must be valid
     */
    bool createView(const View & view);

    /*! \brief Create view in database
     *
     * \pre Driver must be valid
     */
    template<typename T>
    bool createView(const ViewTemplate<T> & view)
    {
      return createView(view.toView());
    }

    /*! \brief Drop view from database
     *
     * \pre Driver must be valid
     */
    bool dropView(const View & view);

    /*! \brief Drop view from database
     *
     * \pre Driver must be valid
     */
    template<typename T>
    bool dropView(const ViewTemplate<T> & view)
    {
      return dropView(view.toView());
    }

    /*! \brief Populate a table
     *
     * \pre Driver must be valid
     */
    bool populateTable(const TablePopulation & tp);

    /*! \brief Populate a table
     *
     * \pre Driver must be valid
     */
    template<typename T>
    bool populateTable(const TablePopulationTemplate<T> & tp)
    {
      return populateTable(tp.toTablePopulation());
    }

    /*! \brief Create schema in database
     *
     * \pre Driver must be valid
     */
    bool createSchema(const Schema & schema);

    /*! \brief Create schema in database
     *
     * \pre Driver must be valid
     */
    template<typename T>
    bool createSchema(const SchemaTemplate<T> & schema)
    {
      return createSchema(schema.toSchema());
    }

    /*! \brief Drop schema from database
     *
     * \pre Driver must be valid
     */
    bool dropSchema(const Schema & schema);

    /*! \brief Drop schema from database
     *
     * \pre Driver must be valid
     */
    template<typename T>
    bool dropSchema(const SchemaTemplate<T> & schema)
    {
      return dropSchema(schema.toSchema());
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

    /*! \brief Get driver type from QSqlDriver's Dbms Type
     */
    static DriverType typeFromDbmsType(QSqlDriver::DbmsType dbmsType)
    {
      switch(dbmsType){
        case QSqlDriver::MySqlServer:
          return DriverType::MySQL;
        case QSqlDriver::SQLite:
          return DriverType::SQLite;
        case QSqlDriver::PostgreSQL:
          return DriverType::PostgreSQL;
        case QSqlDriver::MSSqlServer:
        case QSqlDriver::Oracle:
        case QSqlDriver::Sybase:
        case QSqlDriver::Interbase:
        case QSqlDriver::DB2:
        case QSqlDriver::UnknownDbms:
          break;
      }
      return DriverType::Unknown;
    }

   private:

    /*! \brief Call QObject::tr()
     */
    QString tr(const char *sourceText);

    std::unique_ptr<DriverImplementationInterface> mImpl;
    mutable Mdt::Error mLastError;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_DRIVER_H
