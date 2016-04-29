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
#ifndef MDT_SQL_DATABASE_SCHEMA_H
#define MDT_SQL_DATABASE_SCHEMA_H

#include "mdtSqlTableSchema.h"
#include "mdtSqlTablePopulationSchema.h"
#include "mdtSqlViewSchema.h"
#include "mdtSqlTriggerSchema.h"
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
  void addTable(const mdtSqlTableSchema & st);

  /*! \brief Get tables count
   */
  int tableCount() const
  {
    return pvTableList.size();
  }

  /*! \brief Get table schema for requested table name
   */
  mdtSqlTableSchema table(const QString & tableName) const;

  /*! \brief Get table list
   */
  QList<mdtSqlTableSchema> tableList() const
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

  /*! \brief Get clause to join 2 tables
   *
   * This function works if both specified tables
   *  are defined in database schema and related.
   *  The table to join must contain a foreign key
   *  that refers to main table.
   *
   * \param mainTable Main table of the join clause.
   * \param tableToJoin Table to join. Must contain a foreign key to main table.
   * \return If all requirements are met, a join clause with tables and keys set.
   *         Else, a Null clause is returned.
   * \pre Main table and table to join must exist in database schema.
   * \pre Table to join must have a foreign key refrencing main table.
   */
  mdtSqlViewSchema::JoinClause joinClause(const mdtSqlViewSchema::Table & mainTable, const mdtSqlViewSchema::Table & tableToJoin) const;

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

  /*! \brief Add a trigger schema
   */
  void addTrigger(const mdtSqlTriggerSchema & trigger);

  /*! \brief Get trigger schema count
   */
  int triggerCount() const
  {
    return pvTriggerList.size();
  }

  /*! \brief Get trigger schema list
   */
  QList<mdtSqlTriggerSchema> triggerList() const
  {
    return pvTriggerList;
  }

  /*! \brief Get trigger schema name
   *
   * \pre index must be in a valid range
   */
  QString triggerName(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvTriggerList.size());
    return pvTriggerList.at(index).name();
  }

  /*! \brief Add a schema to this one
   */
  void addSchema(const mdtSqlDatabaseSchema & s);

  /*! \brief Create schema in database
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

  QList<mdtSqlTableSchema> pvTableList;
  QList<mdtSqlViewSchema::Schema> pvViewList;
  QList<mdtSqlTablePopulationSchema> pvTablePopulationSchemaList;
  QList<mdtSqlTriggerSchema> pvTriggerList;
};

#endif // #ifndef MDT_SQL_DATABASE_SCHEMA_H
