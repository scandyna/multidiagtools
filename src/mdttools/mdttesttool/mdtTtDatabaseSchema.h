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
#ifndef MDT_TT_DATABASE_SCHEMA_H
#define MDT_TT_DATABASE_SCHEMA_H

#include "mdtSqlTableSchema.h"

#include "mdtSqlDatabaseSchema.h"
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
class mdtTtDatabaseSchema
{
 public:

  /*! \brief Constructor
   */
  mdtTtDatabaseSchema();

  /*! \brief Constructor
   */
  [[deprecated]]
  mdtTtDatabaseSchema(mdtSqlDatabaseManager *dbManager);

  /*! \brief Destructor
   */
  ~mdtTtDatabaseSchema();

  /*! \brief Build schema
   */
  bool buildSchema();

  /*! \brief Get schema
   */
  mdtSqlDatabaseSchema databaseSchema() const
  {
    return pvSchema;
  }

  /*! \brief Check validity of a schema
   *
   * \todo Current version only checks that all expected tables exists .
   *
   * \pre db must allready be open .
   */
  bool checkSchema(const QSqlDatabase & db);

  /*! \brief Check validity of a schema
   *
   * \todo Current version only checks that all expected tables exists .
   *
   * \pre A database must allready be open .
   */
  bool checkSchema();

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

  /*! \brief Create triggers
   */
  bool createTriggers();

  /*! \brief
   */
  bool createViews();

  /*! \brief Populate database with base data
   */
//   bool populateTables();

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

  /*! \brief
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
   * Wire_tbl is a sort of wires databse
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

  /*! \brief Create onLinkVersionAfterInsert trigger
   */
  bool createOnLinkVersionAfterInsertTrigger();

  /*! \brief Setup onLinkVersionAfterInsert trigger
   */
  void setupOnLinkVersionAfterInsertTrigger();

  /*! \brief Create Link_tbl
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

  /*! \brief Setup TestSystem_tbl
   */
  void setupTestSystemTable();

  /*! \brief Setup TestSystemComponentType_tbl
   */
  void setupTestSystemComponentTypeTable();

  /*! \brief Setup TestSystemComponent_tbl
   */
  void setupTestSystemComponentTable();

  /*! \brief Setup TestSystem_TestSystemComponent_tbl
   */
  void setupTestSystem_TestSystemComponentTable();

  /*! \brief Setup TestSystemUnit_tbl
   */
  void setupTestSystemUnitTable();

  /*! \brief Setup TestSystemUnitType_tbl
   */
  void setupTestSystemUnitTypeTable();



  /*! \brief Setup TestNode_tbl
   *
   * \deprecated Will be replaced by NOTE
   */
  bool setupTestNodeTable();

  /*! \brief Setup TestNodeUnit_tbl
   */
  bool setupTestNodeUnitTable();

  /*! \brief Setup TestNodeUnitConnection_tbl
   */
  bool setupTestNodeUnitConnectionTable();

  /*! \brief Setup TestNodeRoute_tbl
   */
  bool setupTestNodeRouteTable();

  /*! \brief Setup TestNodeRouteUnit_tbl
   */
  bool setupTestNodeRouteUnitTable();

  /*! \brief Setup TestNodeUnitType_tbl
   */
  bool setupTestNodeUnitTypeTable();

  /*! \brief Setup TestCable_TestNodeUnit_tbl
   */
  bool setupTestCableTestNodeUnitTable();

  /*! \brief Setup TestCable_DutUnit_tbl
   */
  bool setupTestCableDutUnitTable();

  /*! \brief Setup TestCable_tbl
   */
  bool setupTestCableTable();

  /*! \brief Setup LogicalTestCable_tbl
   */
  bool setupLogicalTestCableTable();

  /*! \brief Setup TestLink_tbl
   */
  bool setupTestLinkTable();

  /*! \brief Setup TestModel_tbl
   */
  bool setupTestModelTable();

  /*! \brief Setup TestModel_TestNode_tbl
   */
  bool setupTestModelTestNodeTable();

  /*! \brief Setup test item table
   *
   * \todo Move to test tool when created
   */
  bool setupTestModelItemTable();

  /*! \brief Setup TestModelItem_TestLink_tbl
   */
  bool setupTestModelItemTestLinkTable();

  /*! \brief Setup test node unit setup table
   *
   * \todo Move to test tool when created
   */
  bool setupTestNodeUnitSetupTable();

  /*! \brief Setup test result table
   *
   * \todo Move to test tool when created
   */
  bool setupTestTable();

  /*! \brief Setup test result item table
   */
  bool setupTestItemTable();

  /*! \brief Create a trigger
   *
   * Will also drop trigger first if extits
   */
  bool createTrigger(const QString & triggerName, const QString & sql);

  /*! \brief Create a view
   *
   * Will also drop it first if extits
   */
  bool createView(const QString & viewName, const QString & sql);

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

  
  /*! \brief Create TestSystemComponent_view
   *
   * Lists components of a test system.
   */
  bool createTestSystemComponentView();

  /*! \brief Create TestSystemOfComponent_view
   *
   * Lists test systems assigned to a component
   */
  bool createTestSytemOfComponentView();

  /*! \brief Create TestSystemUnit_view
   */
  bool createTestSystemUnitView();


  /*! \brief Create TestNode_view
   */
  bool createTestNodeView();

  /*! \brief Create Unit_TestNode_view
   *
   * Display test nodes that are related to given unit
   */
  bool createUnitTestNodeView();

  /*! \brief Create TestNodeUnitConnection_view
   */
  bool createTestNodeUnitView();

  /*! \brief Create TestNodeUnitConnection_view
   */
  bool createTestNodeUnitConnectionView();

  /*! \brief Create test link view
   */
  bool createTestLinkView();

  /*! \brief Create TestCable_TestNodeUnit_view
   */
  bool createTestCableTestNodeUnitView();

  /*! \brief Create TestNode_TestCable_view
   */
  bool createTestNodeTestCableView();

  /*! \brief Create TestCable_DutUnit_view
   */
  bool createTestCableDutUnitView();

  /*! \brief Create TestModel_TestNode_view
   */
  bool createTestModelTestNodeView();

  /*! \brief Create TestModelItem_TestLink_view
   */
  bool createTestModelItemTestLinkView();

  /*! \brief Create TestNodeUnitSetup_view
   */
  bool createTestNodeUnitSetupView();

  /*! \brief Create TestNodeRoute_view
   */
  bool createTestNodeRouteView();

  /*! \brief Create TestModelItemRoute_view
   */
  bool createTestModelItemRouteView();

  /*! \brief Create test item link view
   *
   * \todo Move to test tool when created
   */
  ///bool createTestModelItemView();

  /*! \brief Create test model item node unit view
   *
   * \todo Move to test tool when created
   */
  ///bool createTestModelItemNodeUnitSetupView();

  /*! \brief Create view to get nodes used by a test item
   *
   * \todo Move to test tool when created
   */
  ///bool createTestModelItemNodeView();

  /*! \brief Create Test_view
   */
  bool createTestView();

  /*! \brief Create view to get test result item
   */
  bool createTestItemView();

  /*! \brief Create test item node unit view
   *
   * \todo Move to test tool when created
   */
  bool createTestItemNodeUnitSetupView();

  /*! \brief Create view to get node units used by a test item
   *
   * \todo Move to test tool when created
   */
  ///bool createTestModelItemNodeUnitView();

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

  /*! \brief Populate TestSystemComponentType_tbl
   */
  void setupTestSystemComponentTypeTablePopulation();

  /*! \brief Populate TestSystemUnitType_tbl
   */
  void setupTestSystemUnitTypeTablePopulation();

  Q_DISABLE_COPY(mdtTtDatabaseSchema);

  mdtSqlDatabaseSchema pvSchema;
  
  mdtSqlDatabaseManager *pvDatabaseManager;
  QList<mdtSqlTableSchema> pvTables;
  mdtError pvLastError;
};

#endif // #ifndef MDT_TT_DATABASE_SCHEMA_H
