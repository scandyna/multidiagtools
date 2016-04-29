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
#ifndef MDT_CABLE_LIST_DATABASE_SCHEMA_H
#define MDT_CABLE_LIST_DATABASE_SCHEMA_H

#include "mdtSqlDatabaseSchema.h"
#include "mdtError.h"
#include <QList>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QSqlDatabase>

namespace Mdt{ namespace CableList{

/*! \brief Build database schema
 */
class DatabaseSchema
{
 public:

  /*! \brief Constructor
   */
  DatabaseSchema();

  // Copy disabled
  DatabaseSchema(const DatabaseSchema &) = delete;
  DatabaseSchema & operator=(const DatabaseSchema &) = delete;

  /*! \brief Get schema
   */
  mdtSqlDatabaseSchema databaseSchema() const
  {
    return pvSchema;
  }

  /*! \brief Create schema in database
   *
   * This is a helper function,
   *  mainly used for unit tests.
   *  It is only possible to check if creation failed or not,
   *  lastError() will not return any informations.
   *
   * \note When creating GUI applications, consider using
   *        mdtSqlDatabaseSchemaDialog.
   * \pre db must be valid (its driver must be loaded) and open
   * \sa mdtSqlDatabaseSchemaDialog , mdtSqlDatabaseSchemaThread , mdtSqlDatabaseSchemaModel .
   */
  bool createSchema(const QSqlDatabase & db);

  /*! \brief Check validity of a schema
   *
   * \todo Current version only checks that all expected tables exists .
   *
   * \pre db must allready be open .
   */
  bool checkSchema(const QSqlDatabase & db);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Build schema
   */
  void buildSchema();

  /*! \brief Setup VehicleType_tbl
   */
  void setupVehicleTypeTable();

  /*! \brief VehicleType_Unit_tbl
   */
  void setupVehicleTypeUnitTable();

  /*! \brief VehicleType_Link_tbl
   */
  void setupVehicleTypeLinkTable();

  /*! \brief Setup connection type table
   */
  void setupConnectionTypeTable();

  /*! \brief Setup connector table
   */
  void setupConnectorTable();

  /*! \brief Setup Connector contact table
   */
  void setupConnectorContactTable();

  /*! \brief Article_tbl
   */
  void setupArticleTable();

  /*! \brief ArticleComponent_tbl
   */
  void setupArticleComponentTable();

  /*! \brief ArticleConnector_tbl
   */
  void setupArticleConnectorTable();

  /*! \brief ArticleConnection_tbl
   */
  void setupArticleConnectionTable();

  /*! \brief ArticleLink_tbl
   */
  void setupArticleLinkTable();

  /*! \brief Unit_tbl
   */
  void setupUnitTable();

  /*! \brief UnitConnector_tbl
   */
  void setupUnitConnectorTable();

  /*! \brief UnitConnection_tbl
   */
  void setupUnitConnectionTable();

  /*! \brief Wire_tbl
   *
   * Wire_tbl is a sort of wires database
   */
  void setupWireTable();

  /*! \brief Setup Modification_tbl
   */
  void setupModificationTable();

    /*! \brief Setup LinkModification_tbl
     */
//   bool setupLinkModificationTable();

  /*! \brief Setup LinkVersion_tbl
   */
  void setupLinkVersionTable();

  /*! \brief Setup onLinkVersionAfterInsert trigger
   *
   * When creating a new link version in LinkVersion_tbl
   *  a entry in LinkModification_tbl will be added for each link
   *  that is allready versionned
   */
  void setupOnLinkVersionAfterInsertTrigger();

  /*! \brief Setup Link_tbl
   */
  void setupLinkTable();

  /*! \brief Setup LinkBeam_tbl
   */
  void setupLinkBeamTable();

  /*! \brief Setup LinkBeam_Unit_tbl
   */
  void setupLinkBeamUnitStartTable();

  /*! \brief Setup LinkBeam_Unit_tbl
   */
  void setupLinkBeamUnitEndTable();

  /*! \brief LinkDirection_tbl
   */
  void setupLinkDirectionTable();

  /*! \brief LinkType_tbl
   */
  void setupLinkTypeTable();

  /*! \brief VehicleType_Unit_view
   */
  void setupVehicleTypeUnitView();

  /*! \brief ArticleComponentUsage_view
   */
  void setupArticleComponentUsageView();

  /*! \brief ArticleComponent_view
   */
  void setupArticleComponentView();

  /*! \brief ArticleConnector_view
   */
  void setupArticleConnectorView();

  /*! \brief ArticleConnection_view
   */
  void setupArticleConnectionView();

  /*! \brief ArticleLink_view
   */
  void setupArticleLinkView();

  /*! \brief Unit_view
   */
  void setupUnitView();

  /*! \brief UnitComponent_view
   */
  void setupUnitComponentView();

  /*! \brief UnitConnector_view
   */
  void setupUnitConnectorView();

  /*! \brief UnitConnectorUsage_view
   *
   * Display wich units uses a given connector
   */
  void setupUnitConnectorUsageView();

  /*! \brief UnitConnection_view
   */
  void setupUnitConnectionView();

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
  void setupArticleLinkUnitConnectionView();

  /*! \brief UnitLink_view
   */
  void setupUnitLinkView();

  /*! \brief Unit_VehicleType_view
   */
  void setupUnitVehicleTypeView();

  /*! \brief LinkList_view
   */
  void setupLinkListView();

  /*! \brief LinkBeam_UnitStart_view
   */
  void setupLinkBeamUnitStartView();

  /*! \brief Create LinkBeam_UnitEnd_view
   */
  void setupLinkBeamUnitEndView();

  /*! \brief Populate Modification_tbl
   */
  void setupModificationTablePopulation();

  /*! \brief Populate Connection type table
   */
  void setupConnectionTypeTablePopulation();

  /*! \brief Populate LinkType table
   */
  void setupLinkTypeTablePopulation();

  /*! \brief Populate LinkType table
   */
  void setupLinkDirectionTablePopulation();

  /*! \brief Calls QObject::tr()
   */
  static QString tr(const char *sourceText);

  mdtSqlDatabaseSchema pvSchema;
  mdtError pvLastError;
};

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_DATABASE_SCHEMA_H
