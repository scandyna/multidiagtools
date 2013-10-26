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
#include <QFileInfo>

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
   */
  bool createSchemaSqlite(const QFileInfo & dbFileInfo);

  private:

  /*! \brief
   */
  void setupTables();

  /*! \brief
   */
  bool createTablesSqlite();

  /*! \brief
   */
  bool createViews();

  /*! \brief
   */
  void setupVehicleTypeTable();

  /*! \brief
   */
  void setupVehicleTypeUnitTable();

  /*! \brief
   */
  void setupVehicleTypeLinkTable();

  /*! \brief
   */
  void setupArticleTable();

  /*! \brief
   */
  void setupArticleComponentTable();

  /*! \brief
   */
  void setupArticleConnectionTable();

  /*! \brief
   */
  void setupArticleLinkTable();

  /*! \brief
   */
  void setupUnitTable();

  /*! \brief
   */
  void setupUnitConnectionTable();

  /*! \brief
   */
  void setupLinkTable();

  /*! \brief
   */
  void setupLinkDirectionTable();

  /*! \brief
   */
  void setupLinkTypeTable();

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

  /*! \brief
   */
  bool createUnitLinkView();

  /*! \brief
   */
  bool createUnitVehicleTypeView();

  /*! \brief
   */
  bool createLinkListView();

  Q_DISABLE_COPY(mdtClDatabaseSchema);

  mdtSqlDatabaseManager *pvDatabaseManager;
  QList<mdtSqlSchemaTable> pvTables;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CL_DATABASE_SCHEMA_H
