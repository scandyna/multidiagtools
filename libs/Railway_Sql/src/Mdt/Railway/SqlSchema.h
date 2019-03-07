/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_RAILWAY_SQL_SCHEMA_H
#define MDT_RAILWAY_SQL_SCHEMA_H

#include "MdtRailway_SqlExport.h"
#include <QSqlDatabase>

namespace Mdt{ namespace Railway{

  /*! \brief Railway SQL schema
   */
  class MDT_RAILWAY_SQL_EXPORT SqlSchema
  {
   public:

    /*! \brief Constructor
     */
    SqlSchema();

    /*! \brief Create railway SQL schema
     *
     * \a dbConnection must be valid (must have a driver loaded)
     */
    bool createSchema(const QSqlDatabase & dbConnection);
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_SQL_SCHEMA_H
