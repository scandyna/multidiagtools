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
#ifndef MDT_SQL_DATABASE_H
#define MDT_SQL_DATABASE_H

#include <QSqlDatabase>
#include <QStringList>
#include <QFlags>

namespace mdt{ namespace sql{

  /*! \brief Offers some complements to QSqlDatabase
   */
  class Database
  {
   public:

    /*! \brief This enum type describes types of SQL tables.
     */
    enum TableType
    {
      Tables = QSql::Tables,              /*!< All the tables visible to the user. */
      SystemTables = QSql::SystemTables,  /*!< Internal tables used by the database. */
      Views = QSql::Views,                /*!< All the views visible to the user. */
      AllTables = QSql::AllTables         /*!< All of the above. */
    };
    Q_DECLARE_FLAGS(TableTypes, TableType)

    /*! \brief Get available tables for given database
     *
     * QSqlDatabase::tables() allready lists avaliable tables
     *  in a database. But, for SQLite, it also lists
     *  sqlite_* tables as normal table.
     *
     * \param db Database from which to get tables list
     * \param types Type of tables to get.
     *              For SQLite, tables beginning with sqlite_ are considered as QSql::SystemTables
     */
    static QStringList getTables(const QSqlDatabase & db, TableTypes types);

   private:

    /*! \brief SQLite implementation of getTables()
     */
    static QStringList getTablesSqlite(const QSqlDatabase & db, TableTypes types);

    /*! \brief Get tables and/or system table in a SQLite database
     */
    static QStringList getTablesSqlite(const QSqlDatabase & db, bool userTables, bool systemTables);
  };

  Q_DECLARE_OPERATORS_FOR_FLAGS(Database::TableTypes)

}} // namespace mdt{ namespace sql{

#endif // #ifndef MDT_SQL_DATABASE_H
