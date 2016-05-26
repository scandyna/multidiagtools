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

    /*! \brief Get collation definition
     */
    QString getCollationDefinition(const Collation & collation) const override;

    /*! \brief Get field definition
     */
    QString getFieldDefinition(const Field & field) const override;

   private:

    
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_DRIVER_SQLITE_H
