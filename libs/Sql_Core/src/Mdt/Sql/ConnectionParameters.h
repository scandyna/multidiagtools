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
#ifndef MDT_SQL_CONNECTION_PARAMETERS_H
#define MDT_SQL_CONNECTION_PARAMETERS_H

#include "MdtSql_CoreExport.h"
#include <QSqlDatabase>
#include <QString>

namespace Mdt{ namespace Sql{

  /*! \brief Parameters for a SQL database connection
   */
  class MDT_SQL_CORE_EXPORT ConnectionParameters
  {
   public:

    /*! \brief Construct empty parameters
     */
    ConnectionParameters() noexcept = default;

    /*! \brief Construct parameters
     */
    explicit ConnectionParameters(const QString & driverName);

    /*! \brief Copy construct parametrs from \a other
     */
    ConnectionParameters(const ConnectionParameters & other) = default;

    /*! \brief Copy assign \a other  to this parametrs
     */
    ConnectionParameters & operator=(const ConnectionParameters & other) = default;

    /*! \brief Move construct parametrs from \a other
     */
    ConnectionParameters(ConnectionParameters && other) = default;

    /*! \brief Move assign \a other  to this parametrs
     */
    ConnectionParameters & operator=(ConnectionParameters && other) = default;

    /*! \brief Get the driver name
     */
    QString driverName() const
    {
      return mDriverName;
    }

    /*! \brief Set database name
     */
    void setDatabaseName(const QString & name);

    /*! \brief Get database name
     */
    QString databaseName() const
    {
      return mDatabaseName;
    }

    /*! \brief Setup \a db with this parameters
     *
     * Will setup all attributes of \a db except the driver.
     *
     * \pre \a db must have the same driver name than driverName()
     */
    void setupDatabase(QSqlDatabase & db) const;

   private:

    QString mDriverName;
    QString mDatabaseName;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_CONNECTION_PARAMETERS_H
