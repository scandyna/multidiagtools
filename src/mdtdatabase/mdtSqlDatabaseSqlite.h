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
#ifndef MDT_SQL_DATABASE_SQLITE_H
#define MDT_SQL_DATABASE_SQLITE_H

#include <QSqlDatabase>

/*! \brief Helper class for SQLite database connection and creation
 *
 * To handle database connections, QSqlDatabase provides all the needed stuff.
 *  mdtSqlDatabaseSqlite is a helper class the adds some logic to deal with common cases
 *  when creating or opening a SQLite database (behaviour whenn opening a non existing file
 *  or creating a existing file, etc...)
 *
 * \sa mdtSqlDatabaseDialogSqlite
 */
class mdtSqlDatabaseSqlite
{
};

#endif // #ifndef MDT_SQL_DATABASE_SQLITE_H
