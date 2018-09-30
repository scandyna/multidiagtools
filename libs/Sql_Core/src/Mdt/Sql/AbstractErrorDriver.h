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
#ifndef MDT_SQL_ABSTRACT_ERROR_DRIVER_H
#define MDT_SQL_ABSTRACT_ERROR_DRIVER_H

#include "Mdt/Error.h"
#include "Mdt/ErrorCode.h"
#include "MdtSql_CoreExport.h"
#include <QSqlError>
#include <QtPlugin>

#define MdtSqlAbstractErrorDriver_iid "Mdt.Sql.AbstractErrorDriver"

namespace Mdt{ namespace Sql{

  /*! \brief Base class to implement a error driver
   *
   * When implementing a error driver for a specific SQL driver,
   *  the driver name used in QSqlDatabase::addDatabase() must be known by PluginLoader .
   *
   * See the ErrorDriverSQLite for more details.
   *
   * \sa ErrorDriver
   */
  class MDT_SQL_CORE_EXPORT AbstractErrorDriver
  {
   public:

    /*! \brief Get error code from \a sqlError
     */
    virtual Mdt::ErrorCode errorCode(const QSqlError & sqlError) const = 0;

    /*! \brief Get error level from \a sqlError
     */
    virtual Mdt::Error::Level errorLevel(const QSqlError & sqlError) const = 0;
  };

}} // namespace Mdt{ namespace Sql{
Q_DECLARE_INTERFACE(Mdt::Sql::AbstractErrorDriver, MdtSqlAbstractErrorDriver_iid)

#endif // #ifndef MDT_SQL_ABSTRACT_ERROR_DRIVER_H
