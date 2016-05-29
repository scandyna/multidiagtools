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
#ifndef MDT_SQL_SCHEMA_DRIVER_H
#define MDT_SQL_SCHEMA_DRIVER_H

#include "DriverType.h"
#include "FieldType.h"
#include "FieldTypeList.h"
#include "Mdt/Error.h"
#include <QSqlDriver>
#include <QString>
#include <QMetaType>
#include <memory>

class QSqlDatabase;

namespace Mdt{ namespace Sql{ namespace Schema{

  class DriverImplementationInterface;

  /*! \brief SQL Schema driver
   *
   * Driver 
   */
  class Driver
  {
   public:

    /*! \brief Create a driver based on QSqlDatabase object
     *
     * The mapping is done by typeFromDbmsType()
     *  If db has a driver that is not supported,
     *  isValid() will return false and lastError()
     *  also contains a description.
     *
     * \pre db must be valid (must have a driver loaded)
     */
    Driver(const QSqlDatabase & db);

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

    /*! \brief Get SQL field type from QMetaType::Type
     *
     * \pre Driver must be valid
     */
    FieldType fieldTypeFromQMetaType(QMetaType::Type qmt) const;

    /*! \brief Get QMetaType::Type from SQL field type
     *
     * \pre Driver must be valid
     */
    QMetaType::Type fieldTypeToQMetaType(FieldType ft) const;

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return pvLastError;
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
        case QSqlDriver::MSSqlServer:
        case QSqlDriver::PostgreSQL:
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

    std::unique_ptr<DriverImplementationInterface> pvImpl;
    Mdt::Error pvLastError;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_DRIVER_H
