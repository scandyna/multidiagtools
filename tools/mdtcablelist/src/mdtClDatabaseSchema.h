/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_CL_DATABASE_SCHEMA_H
#define MDT_CL_DATABASE_SCHEMA_H

#include "mdtSqlSchemaTable.h"
#include "mdtError.h"
#include <QList>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QFileInfo>
#include <QDir>
#include <QSqlDatabase>

class mdtSqlDatabaseManager;

/*! \brief Build database schema
 */
class mdtClDatabaseSchema
{
 public:

  /*! \brief Constructor
   */
  mdtClDatabaseSchema(mdtSqlDatabaseManager *dbManager);

  /*! \brief Destructor
   */
  ~mdtClDatabaseSchema();

  /*! \brief Create schema using Sqlite
   *
   * Ask the user to enter a path and create schema .
   * The startDirectory is used as directory to go when displaying the file dialog .
   */
  bool createSchemaSqlite(const QDir & startDirectory);

  /*! \brief Create schema using Sqlite
   *
   * Will overwrite database if exists .
   */
  bool createSchemaSqlite(const QFileInfo & dbFileInfo);

  /*! \brief Create schema using Sqlite
   *
   * \pre A database must allready be open .
   */
  bool createSchemaSqlite();

  /*! \brief Import a database
   *
   * Will ask the user to choose a source database and import it to currently open database .
   * The startDirectory is used as directory to go when displaying the file dialog .
   *
   * \pre A database (witch is destination database) must allready be open .
   */
  bool importDatabase(const QDir & startDirectory);

  /*! \brief Import a database
   *
   * Will import database given by sourceDbFileInfo into current open database .
   *
   * \pre A database (witch is destination database) must allready be open .
   */
  bool importDatabase(const QFileInfo sourceDbFileInfo);

  /*! \brief Check validity of a schema
   *
   * \todo Current version only checks that all expected tables exists .
   *
   * \pre A database must allready be open .
   */
  bool checkSchema();

  /*! \brief Get last error
   */
  mdtError lastError() const;

 private:

  /*! \brief
   */
  bool setupTables();

  /*! \brief
   */
  bool createTablesSqlite();

  /*! \brief
   */
  bool createViews();

  /*! \brief Populate database with base data
   */
  bool populateTables();

  /*! \brief
   */
  bool setupVehicleTypeTable();

  /*! \brief
   */
  bool setupVehicleTypeUnitTable();

  /*! \brief
   */
  bool setupVehicleTypeLinkTable();

  /*! \brief Setup connector table
   */
  bool setupConnectorTable();

  /*! \brief Setup Connector contact table
   */
  bool setupConnectorContactTable();

  /*! \brief
   */
  bool setupArticleTable();

  /*! \brief
   */
  bool setupArticleComponentTable();

  /*! \brief Setup article connector table
   */
  bool setupArticleConnectorTable();

  /*! \brief
   */
  bool setupArticleConnectionTable();

  /*! \brief
   */
  bool setupArticleLinkTable();

  /*! \brief
   */
  bool setupUnitTable();

  /*! \brief Setup unit connector table
   */
  bool setupUnitConnectorTable();

  /*! \brief
   */
  bool setupUnitConnectionTable();

  /*! \brief
   */
  bool setupLinkTable();

  /*! \brief
   */
  bool setupLinkDirectionTable();

  /*! \brief Setup test node table
   *
   * \todo Move to test tool when created
   */
  bool setupTestNodeTable();

  /*! \brief Setup test node unit table
   *
   * \todo Move to test tool when created
   */
  bool setupTestNodeUnitTable();

  /*! \brief Setup test node unit type table
   *
   * \todo Move to test tool when created
   */
  bool setupTestNodeUnitTypeTable();

  /*! \brief
   */
  bool setupLinkTypeTable();

  /*! \brief Create a view
   *
   * Will also drop it first if extits
   */
  bool createView(const QString & viewName, const QString & sql);

  /*! \brief
   */
  bool createVehicleTypeUnitView();

  /*! \brief
   */
  bool createArticleComponentUsageView();

  /*! \brief
   */
  bool createArticleComponentView();

  /*! \brief Create article connection view
   */
  bool createArticleConnectionView();

  /*! \brief
   */
  bool createArticleLinkView();

  /*! \brief
   */
  bool createUnitView();

  /*! \brief
   */
  bool createUnitComponentView();

  /*! \brief
   */
  bool createUnitConnectionView();

  /*! \brief Create a view that joins article link table with unit connections
   *
   * When creating a article, witch is a model that can be re-used in units,
   *  its possible to add links to this article. These links are added to ArticleLink_tbl.
   *
   * To find a path in the link table, article links must be followed.
   *  This can be done by:
   *   - Finding witch unit connection is linked to a article connection
   *   - Finding links between these article connections in article link table
   *   - Include the result in link list
   *
   * This view will give article links that are used by a unit connection .
   */
  bool createArticleLinkUnitConnectionView();

  /*! \brief
   */
  bool createUnitLinkView();

  /*! \brief
   */
  bool createUnitVehicleTypeView();

  /*! \brief
   */
  bool createLinkListView();

  /*! \brief Check if given PK data exists in given table name
   */
  bool pkExistsInTable(const QString & tableName, const QString & pkField, const QVariant & pkData);

  /*! \brief Insert data into a table
   *
   * Note: first field must be primary key
   */
  bool insertDataIntoTable(const QString & tableName, const QStringList & fields, const QList<QVariant> & data);

  /*! \brief Get SQL statement for data insertion
   */
  QString sqlForDataInsertion(const QString & tableName, const QStringList & fields, const QList<QVariant> & data);

  /*! \brief Get SQL statement for data edition
   */
  QString sqlForDataEdition(const QString & tableName, const QStringList & fields, const QList<QVariant> & data);

  /*! \brief Populate LinkType table
   */
  bool populateLinkTypeTable();

  /*! \brief Populate LinkType table
   */
  bool populateLinkDirectionTable();

  /*! \brief Populate TestNodeUnitType table
   *
   * \todo Move to test tool when created
   */
  bool populateTestNodeUnitTypeTable();

  Q_DISABLE_COPY(mdtClDatabaseSchema);

  mdtSqlDatabaseManager *pvDatabaseManager;
  QList<mdtSqlSchemaTable> pvTables;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CL_DATABASE_SCHEMA_H
