/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_SQL_ERROR_DRIVER_H
#define MDT_SQL_ERROR_DRIVER_H

#include "Mdt/Error.h"
#include "Mdt/ErrorCode.h"
#include "MdtSql_CoreExport.h"
#include <QObject>
#include <QString>
#include <QSqlDriver>
#include <memory>

class QSqlDatabase;
class QSqlError;

namespace Mdt{ namespace Sql{

  class AbstractErrorDriver;
  class PluginInfo;

  /*! \brief Handle on a error driver
   *
   * \code
   * using namespace Mdt::Sql;
   *
   * auto db = QSqlDatabase::addDatabase("QSQLITE");
   * ErrorDriver errorDriver(db);
   * \endcode
   *
   * errorDriver will be a handle to a ErrorDriverSQLite
   *
   * \sa Mdt::Sql::Error
   */
  class MDT_SQL_CORE_EXPORT ErrorDriver : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Construct a error driver
     *
     * If \a sqlDriver is a nullptr, or is not supported,
     *  this error driver will be invalid.
     *
     * \sa isValid()
     * \sa initError()
     */
    explicit ErrorDriver(const QSqlDriver * const sqlDriver, QObject *parent = nullptr);

    /*! \brief Construct a error driver
     *
     * If \a db has no driver loaded,
     *  or not supported driver is loaded,
     *  a warning will be outputed to the debug log,
     *  and unknown errors will be returned.
     */
    explicit ErrorDriver(const QSqlDatabase & db, QObject *parent = nullptr);

    /*! \brief Construct a error driver
     *
     * \sa ErrorDriver(const QSqlDatabase &)
     */
    explicit ErrorDriver(const QString & sqlDriverName, QObject *parent = nullptr);

    ~ErrorDriver();

    ErrorDriver(const ErrorDriver &) = delete;
    ErrorDriver & operator=(const ErrorDriver &) = delete;
    ErrorDriver(ErrorDriver &&) = delete;
    ErrorDriver & operator=(ErrorDriver &&) = delete;

    /*! \brief Check if this error driver is valid
     *
     * This driver is valid if a implementation
     *  have been loaded
     *
     * \sa initError()
     */
    bool isValid() const noexcept
    {
      return (mImpl.get() != nullptr);
    }

    /*! \brief Get init error
     */
    Mdt::Error initError() const
    {
      return mInitError;
    }

    /*! \brief Get error code from \a sqlError
     */
    Mdt::ErrorCode errorCode(const QSqlError & sqlError) const;

    /*! \brief Get error level from \a sqlError
     */
    Mdt::Error::Level errorLevel(const QSqlError & sqlError) const;

   private:

    void init(const QString & sqlDriverName);
    void setInitError(const Mdt::Error & error);

    /*! \brief Get the database driver name from Dbms type
     */
    static QString driverNameFromDbmsType(QSqlDriver::DbmsType dbmsType);

    std::unique_ptr<AbstractErrorDriver> mImpl;
    Mdt::Error mInitError;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ERROR_DRIVER_H
