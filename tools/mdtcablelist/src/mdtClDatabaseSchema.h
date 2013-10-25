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
#include <QList>
#include <QString>

class mdtSqlDatabaseManager;

/*! \brief Build database schema
 */
class mdtClDatabaseSchema
{
 public:

  /*! \brief Constructor
   */
  mdtClDatabaseSchema();

  /*! \brief Destructor
   */
  ~mdtClDatabaseSchema();

  /*! \brief
   */
  bool createSchemaSqlite();

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

  /*! \brief
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
};

#endif // #ifndef MDT_CL_DATABASE_SCHEMA_H
