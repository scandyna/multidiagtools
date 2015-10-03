/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_SQL_PRIMARY_KEY_H
#define MDT_SQL_PRIMARY_KEY_H

#include "mdtSqlIndexBase.h"

class QSqlDatabase;
class QSqlDriver;

/*! \brief Holds primary key information
 */
class mdtSqlPrimaryKey : public mdtSqlIndexBase
{
 public:

  /*! \brief Get SQL for primary key definition
   *
   * \param db Will be used to fetch driver type, and to escape field names
   * \pre db's driver must be loaded
   */
  QString getSql(const QSqlDatabase & db) const;

private:
  
  /*! \brief Get SQL - Sqlite implemetation
   */
  QString getSqlSqlite(const QSqlDriver * const driver) const;
};

#endif // #ifndef MDT_SQL_PRIMARY_KEY_H
