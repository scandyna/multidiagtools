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
#ifndef MDT_SQL_DATABASE_SCHEMA_H
#define MDT_SQL_DATABASE_SCHEMA_H

#include "mdtSqlSchemaTable.h"
#include "mdtSqlTablePopulationSchema.h"
#include "mdtSqlViewSchema.h"
#include <QList>

class QSqlDatabase;

/*! \brief Container for a database schema
 *
 * This class is mostly a container for database schema information objects,
 *  such as SQL table schema, SQL view schema, ...
 *
 * Must of functional part, typically creation of tables, views, etc,
 *  is done by separate classes.
 */
class mdtSqlDatabaseSchema
{
 public:

  /*! \brief Add a table schema
   */
  void addTable(const mdtSqlSchemaTable & st);

  /*! \brief Get tables count
   */
  int tableCount() const
  {
    return pvTableList.size();
  }

  /*! \brief Get table list
   */
  QList<mdtSqlSchemaTable> tableList() const
  {
    return pvTableList;
  }

  /*! \brief Get table name for given index
   *
   * \pre index must be in a valid range
   */
  QString tableName(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvTableList.size());
    return pvTableList.at(index).tableName();
  }

  /*! \brief Add a view schema
   */
  void addView(const mdtSqlViewSchema::Schema & vs);

  /*! \brief Get view schema count
   */
  int viewCount() const
  {
    return pvViewList.size();
  }

  /*! \brief Get view schema list
   */
  QList<mdtSqlViewSchema::Schema> viewList() const
  {
    return pvViewList;
  }

  /*! \brief Get view name for given index
   *
   * \pre index must be in a valid range
   */
  QString viewName(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvViewList.size());
    return pvViewList.at(index).name();
  }

  /*! \brief Add a table population schema
   */
  void addTablePopulation(const mdtSqlTablePopulationSchema & tps);

  /*! \brief Get count of table population schema
   */
  int tablePopulationCount() const
  {
    return pvTablePopulationSchemaList.size();
  }

  /*! \brief Get table population schema list
   */
  QList<mdtSqlTablePopulationSchema> tablePopulationSchemaList() const
  {
    return pvTablePopulationSchemaList;
  }

  /*! \brief Get table population schema name
   *
   * \pre index must be in a valid range
   */
  QString tablePopulationName(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvTablePopulationSchemaList.size());
    return pvTablePopulationSchemaList.at(index).name();
  }

  /*! \brief Create given database schema
   *
   * Given database object is directly used for database creation.
   *  It will be open by this function if it is not allready.
   *
   * \note When creating GUI applications, consider using
   *        mdtSqlDatabaseSchemaDialog.
   * \pre db must be valid (its driver must be loaded) and open
   * \sa mdtSqlDatabaseSchemaDialog , mdtSqlDatabaseSchemaThread , mdtSqlDatabaseSchemaModel .
   */
  bool createSchema(const QSqlDatabase & db);

 private:

  QList<mdtSqlSchemaTable> pvTableList;
  QList<mdtSqlViewSchema::Schema> pvViewList;
  QList<mdtSqlTablePopulationSchema> pvTablePopulationSchemaList;
};

#endif // #ifndef MDT_SQL_DATABASE_SCHEMA_H
