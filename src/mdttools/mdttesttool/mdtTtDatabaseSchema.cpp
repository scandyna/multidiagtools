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
 ** MERCHANTABILITY or FITNESS FOR A PARTMICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtSqlDatabaseManager.h"
#include "mdtDataTableManager.h"
#include "mdtSqlForeignKeySetting.h"

#include "mdtTtDatabaseSchema.h"
#include "mdtSqlIndex.h"
#include "mdtSqlForeignKey.h"
#include "mdtSqlViewSchema.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlField>
#include <QFileInfo>
#include <QStringList>

#include <QDebug>

mdtTtDatabaseSchema::mdtTtDatabaseSchema()
{
}

mdtTtDatabaseSchema::mdtTtDatabaseSchema(mdtSqlDatabaseManager *dbManager) 
{
  Q_ASSERT(dbManager != 0);

  pvDatabaseManager = dbManager;
}

mdtTtDatabaseSchema::~mdtTtDatabaseSchema() 
{
}

bool mdtTtDatabaseSchema::buildSchema()
{
  // Setup tables
  setupModificationTable();
  setupVehicleTypeTable();
  setupVehicleTypeUnitTable();
  setupVehicleTypeLinkTable();
  setupConnectionTypeTable();
  setupConnectorTable();
  setupConnectorContactTable();
  setupArticleTable();
  setupArticleComponentTable();
  setupArticleConnectorTable();
  setupArticleConnectionTable();
  setupArticleLinkTable();
  setupUnitTable();
  setupUnitConnectorTable();
  setupUnitConnectionTable();
  setupLinkDirectionTable();
  setupLinkTypeTable();
  setupLinkVersionTable();
  setupWireTable();
  setupLinkBeamTable();
  setupLinkBeamUnitStartTable();
  setupLinkBeamUnitEndTable();
  setupLinkTable();
  setupTestSystemTable();
  setupTestSystemComponentTypeTable();
  setupTestSystemComponentTable();
  setupTestSystem_TestSystemComponentTable();
  setupTestSystemUnitTable();
  setupTestSystemUnitTypeTable();
  // Setup views
  setupVehicleTypeUnitView();
  setupArticleComponentView();
  setupArticleLinkView();
  setupArticleComponentUsageView();
  setupArticleConnectorView();
  setupArticleConnectionView();
  setupUnitView();
  setupUnitComponentView();
  setupUnitConnectorView();

  return true;
}

bool mdtTtDatabaseSchema::checkSchema(const QSqlDatabase & db)
{
  qDebug() << "mdtTtDatabaseSchema::checkSchema() NOT IMPLEMENTED yet !!!";
  return true;  /// \todo Provisoire !!
}

bool mdtTtDatabaseSchema::checkSchema()
{
  Q_ASSERT(pvDatabaseManager->database().isOpen());

  QStringList dbTables;
  int i;

  // We call setup methods to build the list of expected tables
  pvTables.clear();
  setupTables();
  // Get database available tables and check
  dbTables = pvDatabaseManager->database().tables();
  for(i = 0; i < pvTables.size(); ++i){
    if(!dbTables.contains(pvTables.at(i).tableName())){
      pvLastError.setError("Table '" + pvTables.at(i).tableName() + "' is missing in database '" + pvDatabaseManager->database().databaseName() + "'.", mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtDatabaseSchema");
      pvLastError.commit();
      return false;
    }
  }
  /// \todo Sould'nt we clear tables ??

  return true;
}

bool mdtTtDatabaseSchema::createSchemaSqlite(const QDir & startDirectory)
{
  if(!pvDatabaseManager->createDatabaseSqlite(startDirectory)){
    return false;
  }
  return createSchemaSqlite();
}

bool mdtTtDatabaseSchema::createSchemaSqlite(const QFileInfo & dbFileInfo) 
{
  if(!pvDatabaseManager->createDatabaseSqlite(dbFileInfo, mdtSqlDatabaseManager::OverwriteExisting)){
    return false;
  }
  return createSchemaSqlite();
}

bool mdtTtDatabaseSchema::createSchemaSqlite()
{
  Q_ASSERT(pvDatabaseManager->database().isOpen());

  pvTables.clear();
  if(!setupTables()){
    return false;
  }
  if(!createTablesSqlite()){
    return false;
  }
  if(!createTriggers()){
    return false;
  }

  qDebug() << "Table count: " << pvDatabaseManager->database().tables(QSql::Tables).size();
  qDebug() << "Tables: " << pvDatabaseManager->database().tables(QSql::Tables);

  if(!createViews()){
    return false;
  }

  qDebug() << "View count: " << pvDatabaseManager->database().tables(QSql::Views).size();
  qDebug() << "Views: " << pvDatabaseManager->database().tables(QSql::Views);

  if(!checkSchema()){
    return false;
  }

  return populateTables();
}

bool mdtTtDatabaseSchema::importDatabase(const QDir & startDirectory)
{
  Q_ASSERT(pvDatabaseManager->database().isOpen());

  QFileInfo dbFileInfo;

  // Let the user choose source database
  dbFileInfo = pvDatabaseManager->chooseDatabaseSqlite(startDirectory);
  if(dbFileInfo.fileName().isEmpty()){
    return false;
  }

  return importDatabase(dbFileInfo);
}

bool mdtTtDatabaseSchema::importDatabase(const QFileInfo sourceDbFileInfo)
{
  Q_ASSERT(pvDatabaseManager->database().isOpen());

  mdtSqlForeignKeySetting fkSetting(pvDatabaseManager->database(), mdtSqlForeignKeySetting::Temporary);
  mdtSqlDatabaseManager sourceDbManager;
  mdtDataTableManager tableManager;
  QStringList sourceTables, destinationTables, ignoredTables;
  int i;

  // Open source database
  if(!sourceDbManager.openDatabaseSqlite(sourceDbFileInfo, "import_db_connection")){
    pvLastError = sourceDbManager.lastError();
    return false;
  }
  // Check that it's not the current database
  if(sourceDbManager.database().databaseName() == pvDatabaseManager->database().databaseName()){
    pvLastError.setError("Selected source database is the same as current (destination) database.", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtDatabaseSchema");
    pvLastError.commit();
    return false;
  }
  // Get the list of source tables and check if we have all in current database
  sourceTables = sourceDbManager.database().tables(QSql::Tables);
  destinationTables = pvDatabaseManager->database().tables(QSql::Tables);
  for(i = 0; i < sourceTables.size(); ++i){
    if(!destinationTables.contains(sourceTables.at(i))){
      pvLastError.setError("Table '" + sourceTables.at(i) + "' does not exists in current database - Will be ignored", mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtDatabaseSchema");
      pvLastError.commit();
      ignoredTables.append(sourceTables.at(i));
    }
  }
  // Remove ignored tables
  for(i = 0; i < ignoredTables.size(); ++i){
    sourceTables.removeAll(ignoredTables.at(i));
  }
  // We also remove the sqlite_sequence table
  sourceTables.removeAll("sqlite_sequence");
  // We also ignore some type tables
  sourceTables.removeAll("Modification_tbl");
  sourceTables.removeAll("ConnectionType_tbl");
  sourceTables.removeAll("LinkDirection_tbl");
  sourceTables.removeAll("LinkType_tbl");
  
  sourceTables.removeAll("TestSystemComponentType_tbl");
  sourceTables.removeAll("TestSystemUnitType_tbl");
  
  sourceTables.removeAll("TestNodeUnitType_tbl");
  
  /// \todo Provisoire !!
  ///sourceTables.removeAll("TestNodeUnitConnection_tbl");
  ///sourceTables.removeAll("TestNodeUnitSetup_tbl");
  sourceTables.removeAll("LinkModification_tbl");
  
  // Copy tables
  tableManager.enableProgressDialog(true);
  if(!fkSetting.disable()){
//   if(!pvDatabaseManager->setForeignKeysEnabled(false)){
    pvLastError = pvDatabaseManager->lastError();
    return false;
  }
  for(i = 0; i < sourceTables.size(); ++i){
    tableManager.clearFieldMap();
    /// \todo Provisoire !!
    /**
    if(sourceTables.at(i) == "TestNode_tbl"){
      tableManager.addFieldMapping("NodeIdentification", "Alias", "", QVariant::String);
      tableManager.addFieldMapping("DeviceIdentification", "Alias", "", QVariant::String);
    }
    */
    /**
    if(sourceTables.at(i) == "Link_tbl"){
      tableManager.addFieldMapping("Value", "Resistance", "", QVariant::String);
    }
    if(sourceTables.at(i) == "ArticleLink_tbl"){
      tableManager.addFieldMapping("Value", "Resistance", "", QVariant::String);
    }
    */
    if(!tableManager.copyTable(sourceTables.at(i), sourceTables.at(i), mdtSqlDatabaseManager::KeepExisting, sourceDbManager.database(), pvDatabaseManager->database())){
      pvLastError = tableManager.lastError();
      return false;
    }
  }
  if(!fkSetting.enable()){
//   if(!pvDatabaseManager->setForeignKeysEnabled(true)){
    pvLastError = pvDatabaseManager->lastError();
    return false;
  }

  return true;
}

mdtError mdtTtDatabaseSchema::lastError() const
{
  return pvLastError;
}

bool mdtTtDatabaseSchema::setupTables() 
{
  if(!setupTestNodeTable()){
    return false;
  }
  if(!setupTestNodeUnitTypeTable()){
    return false;
  }
  if(!setupTestNodeUnitTable()){
    return false;
  }
//   if(!setupTestNodeBusTable()){
//     return false;
//   }
  if(!setupTestNodeUnitConnectionTable()){
    return false;
  }
  if(!setupTestNodeRouteTable()){
    return false;
  }
  if(!setupTestNodeRouteUnitTable()){
    return false;
  }
  if(!setupTestCableTable()){
    return false;
  }
  if(!setupLogicalTestCableTable()){
    return false;
  }
  if(!setupTestCableTestNodeUnitTable()){
    return false;
  }
  if(!setupTestCableDutUnitTable()){
    return false;
  }
  if(!setupTestLinkTable()){
    return false;
  }
  if(!setupTestModelTable()){
    return false;
  }
  if(!setupTestModelTestNodeTable()){
    return false;
  }
  if(!setupTestModelItemTable()){
    return false;
  }
  if(!setupTestModelItemTestLinkTable()){
    return false;
  }
  if(!setupTestNodeUnitSetupTable()){
    return false;
  }
//   if(!setupTestModelItemRouteTable()){
//     return false;
//   }
  if(!setupTestTable()){
    return false;
  }
  if(!setupTestItemTable()){
    return false;
  }
  return true;
}

bool mdtTtDatabaseSchema::createTablesSqlite() 
{
  int i;
  QSqlQuery query(pvDatabaseManager->database());

  for(i = 0; i < pvTables.size(); ++i){
    if(!pvDatabaseManager->createTable(pvTables[i], mdtSqlDatabaseManager::OverwriteExisting)){
      pvTables.clear();
      return false;
    }
  }

  return true;
}

bool mdtTtDatabaseSchema::createTriggers()
{
  if(!createOnLinkVersionAfterInsertTrigger()){
    return false;
  }
  return true;
}

bool mdtTtDatabaseSchema::createViews() 
{
//   if(!createArticleComponentUsageView()){
//     return false;
//   }
//   if(!createArticleConnectorView()){
//     return false;
//   }
//   if(!createArticleConnectionView()){
//     return false;
//   }
//   if(!createArticleLinkUnitConnectionView()){
//     return false;
//   }
//   if(!createUnitLinkView()){
//     return false;
//   }
//   if(!createUnitVehicleTypeView()){
//     return false;
//   }
//   if(!createUnitView()){
//     return false;
//   }
//   if(!createUnitComponentView()){
//     return false;
//   }
//   if(!createUnitConnectorView()){
//     return false;
//   }
//   if(!createUnitConnectorUsageView()){
//     return false;
//   }
//   if(!createUnitConnectionView()){
//     return false;
//   }
//   if(!createLinkListView()){
//     return false;
//   }
//   if(!createLinkBeamUnitStartView()){
//     return false;
//   }
//   if(!createTestNodeView()){
//     return false;
//   }
//   if(!createLinkBeamUnitEndView()){
//     return false;
//   }
//   if(!createTestSystemComponentView()){
//     return false;
//   }
//   if(!createTestSytemOfComponentView()){
//     return false;
//   }
//   if(!createTestSystemUnitView()){
//     return false;
//   }
// 
//   if(!createTestNodeUnitView()){
//     return false;
//   }
//   if(!createUnitTestNodeView()){
//     return false;
//   }
//   if(!createTestNodeUnitConnectionView()){
//     return false;
//   }
//   if(!createTestLinkView()){
//     return false;
//   }
//   if(!createTestCableTestNodeUnitView()){
//     return false;
//   }
//   if(!createTestCableDutUnitView()){
//     return false;
//   }
//   if(!createTestNodeTestCableView()){
//     return false;
//   }
//   if(!createTestModelTestNodeView()){
//     return false;
//   }
//   if(!createTestModelItemTestLinkView()){
//     return false;
//   }
//   if(!createTestNodeUnitSetupView()){
//     return false;
//   }
//   if(!createTestView()){
//     return false;
//   }
//   if(!createTestItemView()){
//     return false;
//   }
//   if(!createTestItemNodeUnitSetupView()){
//     return false;
//   }
//   if(!createTestNodeRouteView()){
//     return false;
//   }
//   if(!createTestModelItemRouteView()){
//     return false;
//   }
  /**
  if(!createTestModelItemNodeUnitSetupView()){
    return false;
  }
  */
  /**
  if(!createTestModelItemView()){
    return false;
  }
  */
  /**
  if(!createTestModelItemNodeView()){
    return false;
  }
  if(!createTestModelItemNodeUnitView()){
    return false;
  }
  */
  return true;
}

bool mdtTtDatabaseSchema::populateTables()
{
  if(!populateModificationTable()){
    return false;
  }
  if(!populateConnectionTypeTable()){
    return false;
  }
  if(!populateLinkTypeTable()){
    return false;
  }
  if(!populateLinkDirectionTable()){
    return false;
  }
  if(!populateTestSystemComponentTypeTable()){
    return false;
  }
  if(!populateTestSystemUnitTypeTable()){
    return false;
  }
  
  if(!populateTestNodeUnitTypeTable()){
    return false;
  }
  /**
  if(!populateTestNodeUnitTypeTable()){
    return false;
  }
  */
  return true;
}

void mdtTtDatabaseSchema::setupVehicleTypeTable() 
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField Type;
  mdtSqlField SubType;
  mdtSqlField SeriesNumber;

  table.setTableName("VehicleType_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // Type
  Type.setName("Type");
  Type.setType(mdtSqlFieldType::Varchar);
  Type.setLength(50);
  table.addField(Type, false);
  // SubType
  SubType.setName("SubType");
  SubType.setType(mdtSqlFieldType::Varchar);
  SubType.setLength(50);
  table.addField(SubType, false);
  // SeriesNumber
  SeriesNumber.setName("SeriesNumber");
  SeriesNumber.setType(mdtSqlFieldType::Varchar);
  SeriesNumber.setLength(50);
  table.addField(SeriesNumber, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupVehicleTypeUnitTable() 
{
  mdtSqlSchemaTable table;
  mdtSqlField VehicleType_Id_FK;
  mdtSqlField Unit_Id_FK;
  mdtSqlForeignKey fk_VehicleType_Id_FK;
  mdtSqlForeignKey fk_Unit_Id_FK;

  table.setTableName("VehicleType_Unit_tbl", "UTF8");
  // VehicleType_Id_FK
  VehicleType_Id_FK.setName("VehicleType_Id_FK");
  VehicleType_Id_FK.setType(mdtSqlFieldType::Integer);
  VehicleType_Id_FK.setRequired(true);
  table.addField(VehicleType_Id_FK, true);
  fk_VehicleType_Id_FK.setParentTableName("VehicleType_tbl");
  fk_VehicleType_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_VehicleType_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_VehicleType_Id_FK.setCreateChildIndex(true);
  fk_VehicleType_Id_FK.addKeyFields("Id_PK", VehicleType_Id_FK);
  table.addForeignKey(fk_VehicleType_Id_FK);
  // Unit_Id_FK
  Unit_Id_FK.setName("Unit_Id_FK");
  Unit_Id_FK.setType(mdtSqlFieldType::Integer);
  Unit_Id_FK.setRequired(true);
  table.addField(Unit_Id_FK, true);
  fk_Unit_Id_FK.setParentTableName("Unit_tbl");
  fk_Unit_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Unit_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Unit_Id_FK.setCreateChildIndex(true);
  fk_Unit_Id_FK.addKeyFields("Id_PK", Unit_Id_FK);
  table.addForeignKey(fk_Unit_Id_FK);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupVehicleTypeLinkTable() 
{
  mdtSqlSchemaTable table;
  mdtSqlField VehicleTypeStart_Id_FK;
  mdtSqlForeignKey fk_VehicleTypeStart_Id_FK;
  mdtSqlField VehicleTypeEnd_Id_FK;
  mdtSqlForeignKey fk_VehicleTypeEnd_Id_FK;
  mdtSqlField UnitConnectionStart_Id_FK;
  mdtSqlField UnitConnectionEnd_Id_FK;
  mdtSqlField Link_Version_FK;
  mdtSqlField Link_Modification_Code_FK;
  mdtSqlForeignKey fk_Link_tbl;

  table.setTableName("VehicleType_Link_tbl", "UTF8");
  // VehicleTypeStart_Id_FK
  VehicleTypeStart_Id_FK.setName("VehicleTypeStart_Id_FK");
  VehicleTypeStart_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(VehicleTypeStart_Id_FK, true);
  fk_VehicleTypeStart_Id_FK.setParentTableName("VehicleType_tbl");
  fk_VehicleTypeStart_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_VehicleTypeStart_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_VehicleTypeStart_Id_FK.setCreateChildIndex(true);
  fk_VehicleTypeStart_Id_FK.addKeyFields("Id_PK", VehicleTypeStart_Id_FK);
  table.addForeignKey(fk_VehicleTypeStart_Id_FK);
  // VehicleTypeEnd_Id_FK
  VehicleTypeEnd_Id_FK.setName("VehicleTypeEnd_Id_FK");
  VehicleTypeEnd_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(VehicleTypeEnd_Id_FK, true);
  fk_VehicleTypeEnd_Id_FK.setParentTableName("VehicleType_tbl");
  fk_VehicleTypeEnd_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_VehicleTypeEnd_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_VehicleTypeEnd_Id_FK.setCreateChildIndex(true);
  fk_VehicleTypeEnd_Id_FK.addKeyFields("Id_PK", VehicleTypeEnd_Id_FK);
  table.addForeignKey(fk_VehicleTypeEnd_Id_FK);
  // UnitConnectionStart_Id_FK
  UnitConnectionStart_Id_FK.setName("UnitConnectionStart_Id_FK");
  UnitConnectionStart_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(UnitConnectionStart_Id_FK, true);
  // UnitConnectionEnd_Id_FK
  UnitConnectionEnd_Id_FK.setName("UnitConnectionEnd_Id_FK");
  UnitConnectionEnd_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(UnitConnectionEnd_Id_FK, true);
  // Link_Version_FK (refers to Link_tbl)
  Link_Version_FK.setName("Link_Version_FK");
  Link_Version_FK.setType(mdtSqlFieldType::Integer);
  table.addField(Link_Version_FK, true);
  // Link_Modification_Code_FK (refers to Link_tbl)
  Link_Modification_Code_FK.setName("Link_Modification_Code_FK");
  Link_Modification_Code_FK.setType(mdtSqlFieldType::Varchar);
  Link_Modification_Code_FK.setLength(10);
  table.addField(Link_Modification_Code_FK, true);
  // Foreign key to Link_tbl
  fk_Link_tbl.setParentTableName("Link_tbl");
  fk_Link_tbl.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Link_tbl.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Link_tbl.setCreateChildIndex(true);
  fk_Link_tbl.addKeyFields("UnitConnectionStart_Id_FK", UnitConnectionStart_Id_FK);
  fk_Link_tbl.addKeyFields("UnitConnectionEnd_Id_FK", UnitConnectionEnd_Id_FK);
  fk_Link_tbl.addKeyFields("Version_FK", Link_Modification_Code_FK);
  fk_Link_tbl.addKeyFields("Modification_Code_FK", Link_Version_FK);
  table.addForeignKey(fk_Link_tbl);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupConnectionTypeTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Code_PK;
  mdtSqlField NameEN;
  mdtSqlField NameFR;
  mdtSqlField NameDE;
  mdtSqlField NameIT;

  table.setTableName("ConnectionType_tbl", "UTF8");
  // Code_PK
  Code_PK.setName("Code_PK");
  Code_PK.setType(mdtSqlFieldType::Integer);
  Code_PK.setLength(1);
  table.addField(Code_PK, true);
  // NameEN
  NameEN.setName("NameEN");
  NameEN.setType(mdtSqlFieldType::Varchar);
  NameEN.setLength(50);
  table.addField(NameEN, false);
  // NameFR
  NameFR.setName("NameFR");
  NameFR.setType(mdtSqlFieldType::Varchar);
  NameFR.setLength(50);
  table.addField(NameFR, false);
  // NameDE
  NameDE.setName("NameDE");
  NameDE.setType(mdtSqlFieldType::Varchar);
  NameDE.setLength(50);
  table.addField(NameDE, false);
  // NameIT
  NameIT.setName("NameIT");
  NameIT.setType(mdtSqlFieldType::Varchar);
  NameIT.setLength(50);
  table.addField(NameIT, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupConnectorTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField Gender;
  mdtSqlField ContactQty;
  mdtSqlField Form;
  mdtSqlField Manufacturer;
  mdtSqlField Series;
  mdtSqlField Housing;
  mdtSqlField Insert;
  mdtSqlField InsertRotation;
  mdtSqlField ManufacturerConfigCode;
  mdtSqlField ManufacturerArticleCode;

  table.setTableName("Connector_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // Gender
  Gender.setName("Gender");
  Gender.setType(mdtSqlFieldType::Varchar);
  Gender.setLength(50);
  table.addField(Gender, false);
  // ContactQty
  ContactQty.setName("ContactQty");
  ContactQty.setType(mdtSqlFieldType::Integer);
  table.addField(ContactQty, false);
  // Form
  Form.setName("Form");
  Form.setType(mdtSqlFieldType::Varchar);
  Form.setLength(50);
  table.addField(Form, false);
  // Manufacturer
  Manufacturer.setName("Manufacturer");
  Manufacturer.setType(mdtSqlFieldType::Varchar);
  Manufacturer.setLength(50);
  table.addField(Manufacturer, false);
  // Series
  Series.setName("Series");
  Series.setType(mdtSqlFieldType::Varchar);
  Series.setLength(50);
  table.addField(Series, false);
  // Housing (could also be shell, hood, ...)
  Housing.setName("Housing");
  Housing.setType(mdtSqlFieldType::Varchar);
  Housing.setLength(50);
  table.addField(Housing, false);
  // Insert
  Insert.setName("Insert");
  Insert.setType(mdtSqlFieldType::Varchar);
  Insert.setLength(50);
  table.addField(Insert, false);
  // InsertRotation
  InsertRotation.setName("InsertRotation");
  InsertRotation.setType(mdtSqlFieldType::Varchar);
  InsertRotation.setLength(50);
  table.addField(InsertRotation, false);
  // ManufacturerConfigCode
  ManufacturerConfigCode.setName("ManufacturerConfigCode");
  ManufacturerConfigCode.setType(mdtSqlFieldType::Varchar);
  ManufacturerConfigCode.setLength(50);
  table.addField(ManufacturerConfigCode, false);
  // ManufacturerArticleCode
  ManufacturerArticleCode.setName("ManufacturerArticleCode");
  ManufacturerArticleCode.setType(mdtSqlFieldType::Varchar);
  ManufacturerArticleCode.setLength(50);
  table.addField(ManufacturerArticleCode, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupConnectorContactTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField Connector_Id_FK;
  mdtSqlField Name;
  mdtSqlField ConnectionType_Code_FK;
  mdtSqlForeignKey fk_Connector_Id_FK;
  mdtSqlForeignKey fk_ConnectionType_Code_FK;

  table.setTableName("ConnectorContact_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // Connector_Id_FK
  Connector_Id_FK.setName("Connector_Id_FK");
  Connector_Id_FK.setType(mdtSqlFieldType::Integer);
  Connector_Id_FK.setRequired(true);
  table.addField(Connector_Id_FK, false);
  fk_Connector_Id_FK.setParentTableName("Connector_tbl");
  fk_Connector_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Connector_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Connector_Id_FK.setCreateChildIndex(true);
  fk_Connector_Id_FK.addKeyFields("Id_PK", Connector_Id_FK);
  table.addForeignKey(fk_Connector_Id_FK);
  // Name
  Name.setName("Name");
  Name.setType(mdtSqlFieldType::Varchar);
  Name.setLength(50);
  table.addField(Name, false);
  // ConnectionType_Code_FK [P/S/T] (Pin/Socket/Terminal)
  ConnectionType_Code_FK.setName("ConnectionType_Code_FK");
  ConnectionType_Code_FK.setType(mdtSqlFieldType::Varchar);
  ConnectionType_Code_FK.setLength(1);
  ///ConnectionType_Code_FK.setDefaultValue("T");
  table.addField(ConnectionType_Code_FK, false);
  fk_ConnectionType_Code_FK.setParentTableName("ConnectionType_tbl");
  fk_ConnectionType_Code_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_ConnectionType_Code_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_ConnectionType_Code_FK.setCreateChildIndex(true);
  fk_ConnectionType_Code_FK.addKeyFields("Code_PK", ConnectionType_Code_FK);
  table.addForeignKey(fk_ConnectionType_Code_FK);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupArticleTable() 
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField ArticleCode;
  mdtSqlField DesignationEN;
  mdtSqlField DesignationFR;
  mdtSqlField DesignationDE;
  mdtSqlField DesignationIT;
  mdtSqlField IdentificationDocument;
  mdtSqlField Manufacturer;
  mdtSqlField ManufacturerType;
  mdtSqlField ManufacturerCode;
  mdtSqlField ManufacturerIdentificationDocument;
  mdtSqlField Unit;

  table.setTableName("Article_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // ArticleCode
  ArticleCode.setName("ArticleCode");
  ArticleCode.setType(mdtSqlFieldType::Varchar);
  ArticleCode.setLength(50);
  table.addField(ArticleCode, false);
  // Designation EN
  DesignationEN.setName("DesignationEN");
  DesignationEN.setType(mdtSqlFieldType::Varchar);
  DesignationEN.setLength(150);
  table.addField(DesignationEN, false);
  // Designation FR
  DesignationFR.setName("DesignationFR");
  DesignationFR.setType(mdtSqlFieldType::Varchar);
  DesignationFR.setLength(150);
  table.addField(DesignationFR, false);
  // Designation DE
  DesignationDE.setName("DesignationDE");
  DesignationDE.setType(mdtSqlFieldType::Varchar);
  DesignationDE.setLength(150);
  table.addField(DesignationDE, false);
  // Designation IT
  DesignationIT.setName("DesignationIT");
  DesignationIT.setType(mdtSqlFieldType::Varchar);
  DesignationIT.setLength(150);
  table.addField(DesignationIT, false);
  // IdentificationDocument
  IdentificationDocument.setName("IdentificationDocument");
  IdentificationDocument.setType(mdtSqlFieldType::Varchar);
  IdentificationDocument.setLength(100);
  table.addField(IdentificationDocument, false);
  // Manufacturer
  Manufacturer.setName("Manufacturer");
  Manufacturer.setType(mdtSqlFieldType::Varchar);
  Manufacturer.setLength(100);
  table.addField(Manufacturer, false);
  // Manufacturer type
  ManufacturerType.setName("ManufacturerType");
  ManufacturerType.setType(mdtSqlFieldType::Varchar);
  ManufacturerType.setLength(100);
  table.addField(ManufacturerType, false);
  // Manufacturer type
  ManufacturerCode.setName("ManufacturerCode");
  ManufacturerCode.setType(mdtSqlFieldType::Varchar);
  ManufacturerCode.setLength(100);
  table.addField(ManufacturerCode, false);
  // ManufacturerIdentificationDocument
  ManufacturerIdentificationDocument.setName("ManufacturerIdentificationDocument");
  ManufacturerIdentificationDocument.setType(mdtSqlFieldType::Varchar);
  ManufacturerIdentificationDocument.setLength(100);
  table.addField(ManufacturerIdentificationDocument, false);
  // Unit
  Unit.setName("Unit");
  Unit.setType(mdtSqlFieldType::Varchar);
  Unit.setLength(10);
  Unit.setRequired(true);
  Unit.setDefaultValue("pce");
  table.addField(Unit, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupArticleComponentTable() 
{
  mdtSqlSchemaTable table;
  mdtSqlField Composite_Id_FK;
  mdtSqlField Component_Id_FK;
  mdtSqlField ComponentQty;
  mdtSqlField ComponentQtyUnit;
  mdtSqlForeignKey fk_Composite_Id_FK;
  mdtSqlForeignKey fk_Component_Id_FK;

  table.setTableName("ArticleComponent_tbl", "UTF8");
  // Composite_Id_FK
  Composite_Id_FK.setName("Composite_Id_FK");
  Composite_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(Composite_Id_FK, true);
  fk_Composite_Id_FK.setParentTableName("Article_tbl");
  fk_Composite_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Composite_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Composite_Id_FK.setCreateChildIndex(true);
  fk_Composite_Id_FK.addKeyFields("Id_PK", Composite_Id_FK);
  table.addForeignKey(fk_Composite_Id_FK);
  // Component_Id_FK
  Component_Id_FK.setName("Component_Id_FK");
  Component_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(Component_Id_FK, true);
  fk_Component_Id_FK.setParentTableName("Article_tbl");
  fk_Component_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Component_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Component_Id_FK.setCreateChildIndex(true);
  fk_Component_Id_FK.addKeyFields("Id_PK", Component_Id_FK);
  table.addForeignKey(fk_Component_Id_FK);
  // ComponentQty
  ComponentQty.setName("ComponentQty");
  ComponentQty.setType(mdtSqlFieldType::Double);
  ComponentQty.setRequired(true);
  table.addField(ComponentQty, false);
  // ComponentQtyUnit
  ComponentQtyUnit.setName("ComponentQtyUnit");
  ComponentQtyUnit.setType(mdtSqlFieldType::Varchar);
  ComponentQtyUnit.setLength(10);
  ComponentQtyUnit.setRequired(true);
  table.addField(ComponentQtyUnit, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupArticleConnectorTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField Article_Id_FK;
  mdtSqlField Connector_Id_FK;
  mdtSqlField Name;
  mdtSqlForeignKey fk_Article_Id_FK;
  mdtSqlForeignKey fk_Connector_Id_FK;

  table.setTableName("ArticleConnector_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // Article_Id_FK
  Article_Id_FK.setName("Article_Id_FK");
  Article_Id_FK.setType(mdtSqlFieldType::Integer);
  Article_Id_FK.setRequired(true);
  table.addField(Article_Id_FK, false);
  fk_Article_Id_FK.setParentTableName("Article_tbl");
  fk_Article_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Article_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Article_Id_FK.setCreateChildIndex(true);
  fk_Article_Id_FK.addKeyFields("Id_PK", Article_Id_FK);
  table.addForeignKey(fk_Article_Id_FK);
  // Connector_Id_FK
  Connector_Id_FK.setName("Connector_Id_FK");
  Connector_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(Connector_Id_FK, false);
  fk_Connector_Id_FK.setParentTableName("Connector_tbl");
  fk_Connector_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Connector_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Connector_Id_FK.setCreateChildIndex(true);
  fk_Connector_Id_FK.addKeyFields("Id_PK", Connector_Id_FK);
  table.addForeignKey(fk_Connector_Id_FK);
  // Connector Name
  Name.setName("Name");
  Name.setType(mdtSqlFieldType::Varchar);
  Name.setLength(30);
  table.addField(Name, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupArticleConnectionTable() 
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField Article_Id_FK;
  mdtSqlForeignKey fk_Article_Id_FK;
  mdtSqlField ArticleConnector_Id_FK;
  mdtSqlForeignKey fk_ArticleConnector_Id_FK;
  mdtSqlField ConnectionType_Code_FK;
  mdtSqlForeignKey fk_ConnectionType_Code_FK;
  mdtSqlField ArticleContactName;
  mdtSqlField Resistance;
  mdtSqlField IoType;
  mdtSqlField FunctionEN;
  mdtSqlField FunctionFR;
  mdtSqlField FunctionDE;
  mdtSqlField FunctionIT;

  table.setTableName("ArticleConnection_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // Article_Id_FK
  Article_Id_FK.setName("Article_Id_FK");
  Article_Id_FK.setType(mdtSqlFieldType::Integer);
  Article_Id_FK.setRequired(true);
  table.addField(Article_Id_FK, false);
  fk_Article_Id_FK.setParentTableName("Article_tbl");
  fk_Article_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Article_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Article_Id_FK.setCreateChildIndex(true);
  fk_Article_Id_FK.addKeyFields("Id_PK", Article_Id_FK);
  table.addForeignKey(fk_Article_Id_FK);
  // ArticleConnector_Id_FK
  ArticleConnector_Id_FK.setName("ArticleConnector_Id_FK");
  ArticleConnector_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(ArticleConnector_Id_FK, false);
  fk_ArticleConnector_Id_FK.setParentTableName("ArticleConnector_tbl");
  fk_ArticleConnector_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_ArticleConnector_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_ArticleConnector_Id_FK.setCreateChildIndex(true);
  fk_ArticleConnector_Id_FK.addKeyFields("Id_PK", ArticleConnector_Id_FK);
  table.addForeignKey(fk_ArticleConnector_Id_FK);
  // ConnectionType_Code_FK [P/S/T] (Pin/Socket/Terminal)
  ConnectionType_Code_FK.setName("ConnectionType_Code_FK");
  ConnectionType_Code_FK.setType(mdtSqlFieldType::Varchar);
  ConnectionType_Code_FK.setLength(1);
  ConnectionType_Code_FK.setRequired(true);
  table.addField(ConnectionType_Code_FK, false);
  fk_ConnectionType_Code_FK.setParentTableName("ConnectionType_tbl");
  fk_ConnectionType_Code_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_ConnectionType_Code_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_ConnectionType_Code_FK.setCreateChildIndex(true);
  fk_ConnectionType_Code_FK.addKeyFields("Code_PK", ConnectionType_Code_FK);
  table.addForeignKey(fk_ConnectionType_Code_FK);
  // ArticleContactName
  ArticleContactName.setName("ArticleContactName");
  ArticleContactName.setType(mdtSqlFieldType::Varchar);
  ArticleContactName.setLength(30);
  ArticleContactName.setRequired(true);
  table.addField(ArticleContactName, false);
  // Resistance
  Resistance.setName("Resistance");
  Resistance.setType(mdtSqlFieldType::Double);
  Resistance.setDefaultValue(0.0);
  table.addField(Resistance, false);
  // IoType
  IoType.setName("IoType");
  IoType.setType(mdtSqlFieldType::Varchar);
  IoType.setLength(30);
  table.addField(IoType, false);
  // FunctionEN
  FunctionEN.setName("FunctionEN");
  FunctionEN.setType(mdtSqlFieldType::Varchar);
  FunctionEN.setLength(100);
  table.addField(FunctionEN, false);
  // FunctionFR
  FunctionFR.setName("FunctionFR");
  FunctionFR.setType(mdtSqlFieldType::Varchar);
  FunctionFR.setLength(100);
  table.addField(FunctionFR, false);
  // FunctionDE
  FunctionDE.setName("FunctionDE");
  FunctionDE.setType(mdtSqlFieldType::Varchar);
  FunctionDE.setLength(100);
  table.addField(FunctionDE, false);
  // FunctionIT
  FunctionIT.setName("FunctionIT");
  FunctionIT.setType(mdtSqlFieldType::Varchar);
  FunctionIT.setLength(100);
  table.addField(FunctionIT, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupArticleLinkTable() 
{
  mdtSqlSchemaTable table;
  mdtSqlField ArticleConnectionStart_Id_FK;
  mdtSqlForeignKey fk_ArticleConnectionStart;
  mdtSqlField ArticleConnectionEnd_Id_FK;
  mdtSqlForeignKey fk_ArticleConnectionEnd;
  mdtSqlField SinceVersion;
  mdtSqlField Modification;
  mdtSqlField Identification;
  mdtSqlField LinkDirection_Code_FK;
  mdtSqlForeignKey fk_LinkDirection;
  mdtSqlField LinkType_Code_FK;
  mdtSqlForeignKey fk_LinkType;
  mdtSqlField Resistance;

  table.setTableName("ArticleLink_tbl", "UTF8");
  // ArticleConnectionStart_Id_FK
  ArticleConnectionStart_Id_FK.setName("ArticleConnectionStart_Id_FK");
  ArticleConnectionStart_Id_FK.setType(mdtSqlFieldType::Integer);
  ArticleConnectionStart_Id_FK.setRequired(true);
  table.addField(ArticleConnectionStart_Id_FK, true);
  fk_ArticleConnectionStart.setParentTableName("ArticleConnection_tbl");
  fk_ArticleConnectionStart.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_ArticleConnectionStart.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_ArticleConnectionStart.setCreateChildIndex(true);
  fk_ArticleConnectionStart.addKeyFields("Id_PK", ArticleConnectionStart_Id_FK);
  table.addForeignKey(fk_ArticleConnectionStart);
  // ArticleConnectionEnd_Id_FK
  ArticleConnectionEnd_Id_FK.setName("ArticleConnectionEnd_Id_FK");
  ArticleConnectionEnd_Id_FK.setType(mdtSqlFieldType::Integer);
  ArticleConnectionEnd_Id_FK.setRequired(true);
  table.addField(ArticleConnectionEnd_Id_FK, true);
  fk_ArticleConnectionEnd.setParentTableName("ArticleConnection_tbl");
  fk_ArticleConnectionEnd.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_ArticleConnectionEnd.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_ArticleConnectionEnd.setCreateChildIndex(true);
  fk_ArticleConnectionEnd.addKeyFields("Id_PK", ArticleConnectionEnd_Id_FK);
  table.addForeignKey(fk_ArticleConnectionEnd);
  // SinceVersion
  SinceVersion.setName("SinceVersion");
  SinceVersion.setType(mdtSqlFieldType::Double);
  table.addField(SinceVersion, false);
  // Modification
  Modification.setName("Modification");
  Modification.setType(mdtSqlFieldType::Varchar);
  Modification.setLength(20);
  table.addField(Modification, false);
  // Identification
  Identification.setName("Identification");
  Identification.setType(mdtSqlFieldType::Varchar);
  Identification.setLength(50);
  table.addField(Identification, false);
  // LinkDirection_Code_FK
  LinkDirection_Code_FK.setName("LinkDirection_Code_FK");
  LinkDirection_Code_FK.setType(mdtSqlFieldType::Varchar);
  LinkDirection_Code_FK.setLength(10);
  LinkDirection_Code_FK.setRequired(true);
  table.addField(LinkDirection_Code_FK, false);
  fk_LinkDirection.setParentTableName("LinkDirection_tbl");
  fk_LinkDirection.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_LinkDirection.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_LinkDirection.setCreateChildIndex(true);
  fk_LinkDirection.addKeyFields("Code_PK", LinkDirection_Code_FK);
  table.addForeignKey(fk_LinkDirection);
  // LinkType_Code_FK
  LinkType_Code_FK.setName("LinkType_Code_FK");
  LinkType_Code_FK.setType(mdtSqlFieldType::Varchar);
  LinkType_Code_FK.setLength(10);
  LinkType_Code_FK.setRequired(true);
  table.addField(LinkType_Code_FK, false);
  fk_LinkType.setParentTableName("LinkType_tbl");
  fk_LinkType.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_LinkType.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_LinkType.setCreateChildIndex(true);
  fk_LinkType.addKeyFields("Code_PK", LinkType_Code_FK);
  table.addForeignKey(fk_LinkType);
  // Resistance
  Resistance.setName("Resistance");
  Resistance.setType(mdtSqlFieldType::Double);
  table.addField(Resistance, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupUnitTable() 
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField Composite_Id_FK;
  mdtSqlForeignKey fk_Composite_Id_FK;
  mdtSqlField Article_Id_FK;
  mdtSqlForeignKey fk_Article_Id_FK;
  mdtSqlField Coordinate;
  mdtSqlField Cabinet;
  mdtSqlField SchemaPosition;
  mdtSqlField Alias;

  table.setTableName("Unit_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // Composite_Id_FK
  Composite_Id_FK.setName("Composite_Id_FK");
  Composite_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(Composite_Id_FK, false);
  fk_Composite_Id_FK.setParentTableName("Unit_tbl");
  fk_Composite_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Composite_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Composite_Id_FK.setCreateChildIndex(true);
  fk_Composite_Id_FK.addKeyFields("Id_PK", Composite_Id_FK);
  table.addForeignKey(fk_Composite_Id_FK);
  // Article_Id_FK
  Article_Id_FK.setName("Article_Id_FK");
  Article_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(Article_Id_FK, false);
  fk_Article_Id_FK.setParentTableName("Article_tbl");
  fk_Article_Id_FK.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Article_Id_FK.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Article_Id_FK.setCreateChildIndex(true);
  fk_Article_Id_FK.addKeyFields("Id_PK", Article_Id_FK);
  table.addForeignKey(fk_Article_Id_FK);
  // Coordinate
  Coordinate.setName("Coordinate");
  Coordinate.setType(mdtSqlFieldType::Varchar);
  Coordinate.setLength(10);
  table.addField(Coordinate, false);
  // Cabinet
  Cabinet.setName("Cabinet");
  Cabinet.setType(mdtSqlFieldType::Varchar);
  Cabinet.setLength(30);
  table.addField(Cabinet, false);
  // SchemaPosition
  SchemaPosition.setName("SchemaPosition");
  SchemaPosition.setType(mdtSqlFieldType::Varchar);
  SchemaPosition.setLength(50);
  table.addField(SchemaPosition, false);
  // Alias
  Alias.setName("Alias");
  Alias.setType(mdtSqlFieldType::Varchar);
  Alias.setLength(50);
  table.addField(Alias, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupUnitConnectorTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField Unit_Id_FK;
  mdtSqlForeignKey fk_Unit;
  mdtSqlField Connector_Id_FK;
  mdtSqlForeignKey fk_Connector;
  mdtSqlField ArticleConnector_Id_FK;
  mdtSqlForeignKey fk_ArticleConnector;
  mdtSqlField Name;

  table.setTableName("UnitConnector_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // Unit_Id_FK
  Unit_Id_FK.setName("Unit_Id_FK");
  Unit_Id_FK.setType(mdtSqlFieldType::Integer);
  Unit_Id_FK.setRequired(true);
  table.addField(Unit_Id_FK, false);
  fk_Unit.setParentTableName("Unit_tbl");
  fk_Unit.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Unit.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Unit.setCreateChildIndex(true);
  fk_Unit.addKeyFields("Id_PK", Unit_Id_FK);
  table.addForeignKey(fk_Unit);
  // Connector_Id_FK
  Connector_Id_FK.setName("Connector_Id_FK");
  Connector_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(Connector_Id_FK, false);
  fk_Connector.setParentTableName("Connector_tbl");
  fk_Connector.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Connector.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Connector.setCreateChildIndex(true);
  fk_Connector.addKeyFields("Id_PK", Connector_Id_FK);
  table.addForeignKey(fk_Connector);
  // ArticleConnector_Id_FK
  ArticleConnector_Id_FK.setName("ArticleConnector_Id_FK");
  ArticleConnector_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(ArticleConnector_Id_FK, false);
  fk_ArticleConnector.setParentTableName("ArticleConnector_tbl");
  fk_ArticleConnector.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_ArticleConnector.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_ArticleConnector.setCreateChildIndex(true);
  fk_ArticleConnector.addKeyFields("Id_PK", ArticleConnector_Id_FK);
  table.addForeignKey(fk_ArticleConnector);
  // Connector Name
  Name.setName("Name");
  Name.setType(mdtSqlFieldType::Varchar);
  Name.setLength(30);
  table.addField(Name, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupUnitConnectionTable() 
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField Unit_Id_FK;
  mdtSqlForeignKey fk_Unit;
  mdtSqlField UnitConnector_Id_FK;
  mdtSqlForeignKey fk_UnitConnector;
  mdtSqlField ArticleConnection_Id_FK;
  mdtSqlForeignKey fk_ArticleConnection;
  mdtSqlField ConnectionType_Code_FK;
  mdtSqlForeignKey fk_ConnectionType;
  mdtSqlField UnitContactName;
  mdtSqlField Resistance;
  mdtSqlField IsATestPoint;
  mdtSqlField SchemaPage;
  mdtSqlField FunctionEN;
  mdtSqlField FunctionFR;
  mdtSqlField FunctionDE;
  mdtSqlField FunctionIT;
  mdtSqlField SignalName;
  mdtSqlField SwAddress;

  table.setTableName("UnitConnection_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // Unit_Id_FK
  Unit_Id_FK.setName("Unit_Id_FK");
  Unit_Id_FK.setType(mdtSqlFieldType::Integer);
  Unit_Id_FK.setRequired(true);
  table.addField(Unit_Id_FK, false);
  fk_Unit.setParentTableName("Unit_tbl");
  fk_Unit.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Unit.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Unit.setCreateChildIndex(true);
  fk_Unit.addKeyFields("Id_PK", Unit_Id_FK);
  table.addForeignKey(fk_Unit);
  // UnitConnector_Id_FK
  UnitConnector_Id_FK.setName("UnitConnector_Id_FK");
  UnitConnector_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(UnitConnector_Id_FK, false);
  fk_UnitConnector.setParentTableName("UnitConnector_tbl");
  fk_UnitConnector.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_UnitConnector.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_UnitConnector.setCreateChildIndex(true);
  fk_UnitConnector.addKeyFields("Id_PK", UnitConnector_Id_FK);
  table.addForeignKey(fk_UnitConnector);
  // ArticleConnection_Id_FK
  ArticleConnection_Id_FK.setName("ArticleConnection_Id_FK");
  ArticleConnection_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(ArticleConnection_Id_FK, false);
  fk_ArticleConnection.setParentTableName("ArticleConnection_tbl");
  fk_ArticleConnection.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_ArticleConnection.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_ArticleConnection.setCreateChildIndex(true);
  fk_ArticleConnection.addKeyFields("Id_PK", ArticleConnection_Id_FK);
  table.addForeignKey(fk_ArticleConnection);
  // ConnectionType_Code_FK [P/S/T] (Pin/Socket/Terminal)
  ConnectionType_Code_FK.setName("ConnectionType_Code_FK");
  ConnectionType_Code_FK.setType(mdtSqlFieldType::Varchar);
  ConnectionType_Code_FK.setLength(1);
  ConnectionType_Code_FK.setRequired(true);
  table.addField(ConnectionType_Code_FK, false);
  fk_ConnectionType.setParentTableName("ConnectionType_tbl");
  fk_ConnectionType.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_ConnectionType.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_ConnectionType.setCreateChildIndex(true);
  fk_ConnectionType.addKeyFields("Id_PK", ConnectionType_Code_FK);
  table.addForeignKey(fk_ConnectionType);
  // UnitContactName
  UnitContactName.setName("UnitContactName");
  UnitContactName.setType(mdtSqlFieldType::Varchar);
  UnitContactName.setLength(30);
  table.addField(UnitContactName, false);
  // Resistance
  Resistance.setName("Resistance");
  Resistance.setType(mdtSqlFieldType::Double);
  Resistance.setDefaultValue(0.0);
  table.addField(Resistance, false);
  // IsATestPoint
  IsATestPoint.setName("IsATestPoint");
  IsATestPoint.setType(mdtSqlFieldType::Boolean);
  table.addField(IsATestPoint, false);
  // SchemaPage
  SchemaPage.setName("SchemaPage");
  SchemaPage.setType(mdtSqlFieldType::Varchar);
  SchemaPage.setLength(20);
  table.addField(SchemaPage, false);
  // FunctionEN
  FunctionEN.setName("FunctionEN");
  FunctionEN.setType(mdtSqlFieldType::Varchar);
  FunctionEN.setLength(100);
  table.addField(FunctionEN, false);
  // FunctionFR
  FunctionFR.setName("FunctionFR");
  FunctionFR.setType(mdtSqlFieldType::Varchar);
  FunctionFR.setLength(100);
  table.addField(FunctionFR, false);
  // FunctionDE
  FunctionDE.setName("FunctionDE");
  FunctionDE.setType(mdtSqlFieldType::Varchar);
  FunctionDE.setLength(100);
  table.addField(FunctionDE, false);
  // FunctionIT
  FunctionIT.setName("FunctionIT");
  FunctionIT.setType(mdtSqlFieldType::Varchar);
  FunctionIT.setLength(100);
  table.addField(FunctionIT, false);
  // SignalName
  SignalName.setName("SignalName");
  SignalName.setType(mdtSqlFieldType::Varchar);
  SignalName.setLength(50);
  table.addField(SignalName, false);
  // SwAddress
  SwAddress.setName("SwAddress");
  SwAddress.setType(mdtSqlFieldType::Integer);
  table.addField(SwAddress, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupWireTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField ArticleCode;
  mdtSqlField Model;
  mdtSqlField Manufacturer;
  mdtSqlField ManufacturerArticleCode;
  mdtSqlField Section;
  mdtSqlField ExternalDiameter;
  mdtSqlField LineicResistance;
  mdtSqlField ColorEN;
  mdtSqlField ColorFR;
  mdtSqlField ColorDE;
  mdtSqlField ColorIT;

  table.setTableName("Wire_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // ArticleCode
  ArticleCode.setName("ArticleCode");
  ArticleCode.setType(mdtSqlFieldType::Varchar);
  ArticleCode.setLength(50);
  table.addField(ArticleCode, false);
  // Manufacturer
  Manufacturer.setName("Manufacturer");
  Manufacturer.setType(mdtSqlFieldType::Varchar);
  Manufacturer.setLength(100);
  table.addField(Manufacturer, false);
  // Model
  Model.setName("Model");
  Model.setType(mdtSqlFieldType::Varchar);
  Model.setLength(100);
  table.addField(Model, false);
  // ManufacturerArticleCode
  ManufacturerArticleCode.setName("ManufacturerArticleCode");
  ManufacturerArticleCode.setType(mdtSqlFieldType::Varchar);
  ManufacturerArticleCode.setLength(50);
  table.addField(ManufacturerArticleCode, false);
  // Section
  Section.setName("Section");
  Section.setType(mdtSqlFieldType::Double);
  table.addField(Section, false);
  // ExternalDiameter
  ExternalDiameter.setName("ExternalDiameter");
  ExternalDiameter.setType(mdtSqlFieldType::Double);
  table.addField(ExternalDiameter, false);
  // LineicResistance
  LineicResistance.setName("LineicResistance");
  LineicResistance.setType(mdtSqlFieldType::Double);
  table.addField(LineicResistance, false);
  // ColorEN
  ColorEN.setName("ColorEN");
  ColorEN.setType(mdtSqlFieldType::Varchar);
  ColorEN.setLength(50);
  table.addField(ColorEN, false);
  // ColorFR
  ColorFR.setName("ColorFR");
  ColorFR.setType(mdtSqlFieldType::Varchar);
  ColorFR.setLength(50);
  table.addField(ColorFR, false);
  // ColorDE
  ColorDE.setName("ColorDE");
  ColorDE.setType(mdtSqlFieldType::Varchar);
  ColorDE.setLength(50);
  table.addField(ColorDE, false);
  // ColorIT
  ColorIT.setName("ColorIT");
  ColorIT.setType(mdtSqlFieldType::Varchar);
  ColorIT.setLength(50);
  table.addField(ColorIT, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupModificationTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Code_PK;
  mdtSqlField SortOrder;
  mdtSqlField NameEN;
  mdtSqlField NameFR;
  mdtSqlField NameDE;
  mdtSqlField NameIT;

  table.setTableName("Modification_tbl", "UTF8");
  // Code_PK
  Code_PK.setName("Code_PK");
  Code_PK.setType(mdtSqlFieldType::Varchar);
  Code_PK.setLength(10);
  table.addField(Code_PK, true);
  // SortOrder
  SortOrder.setName("SortOrder");
  SortOrder.setType(mdtSqlFieldType::Integer);
  table.addField(SortOrder, false);
  // NameEN
  NameEN.setName("NameEN");
  NameEN.setType(mdtSqlFieldType::Varchar);
  NameEN.setLength(50);
  table.addField(NameEN, false);
  // NameFR
  NameFR.setName("NameFR");
  NameFR.setType(mdtSqlFieldType::Varchar);
  NameFR.setLength(50);
  table.addField(NameFR, false);
  // NameDE
  NameDE.setName("NameDE");
  NameDE.setType(mdtSqlFieldType::Varchar);
  NameDE.setLength(50);
  table.addField(NameDE, false);
  // NameIT
  NameIT.setName("NameIT");
  NameIT.setType(mdtSqlFieldType::Varchar);
  NameIT.setLength(50);
  table.addField(NameIT, false);

  pvSchema.addTable(table);
}

// bool mdtTtDatabaseSchema::setupLinkModificationTable()
// {
//   mdtSqlSchemaTable table;
//   QSqlField field;
// 
//   table.setTableName("LinkModification_tbl", "UTF8");
//   // UnitConnectionStart_Id_FK
//   field.setName("UnitConnectionStart_Id_FK");
//   field.setType(QVariant::Int);
//   table.addField(field, true);
//   // UnitConnectionEnd_Id_FK
//   field = QSqlField();
//   field.setName("UnitConnectionEnd_Id_FK");
//   field.setType(QVariant::Int);
//   table.addField(field, true);
//   // Version_FK
//   field = QSqlField();
//   field.setName("Version_FK");
//   field.setType(QVariant::Int);
//   table.addField(field, true);
//   // Modification_Code_FK
//   field = QSqlField();
//   field.setName("Modification_Code_FK");
//   field.setType(QVariant::String);
//   field.setLength(10);
//   table.addField(field, true);
//   // TextEN
//   field = QSqlField();
//   field.setName("TextEN");
//   field.setType(QVariant::String);
//   field.setLength(300);
//   table.addField(field, false);
//   // TextFR
//   field = QSqlField();
//   field.setName("TextFR");
//   field.setType(QVariant::String);
//   field.setLength(300);
//   table.addField(field, false);
//   // TextDE
//   field = QSqlField();
//   field.setName("TextDE");
//   field.setType(QVariant::String);
//   field.setLength(300);
//   table.addField(field, false);
//   // TextIT
//   field = QSqlField();
//   field.setName("TextIT");
//   field.setType(QVariant::String);
//   field.setLength(300);
//   table.addField(field, false);
//   // Foreign keys
//   table.addForeignKey("LinkModification_Link_fk", "Link_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
//   if(!table.addFieldToForeignKey("LinkModification_Link_fk", "UnitConnectionStart_Id_FK", "UnitConnectionStart_Id_FK")){
//     pvLastError = table.lastError();
//     return false;
//   }
//   if(!table.addFieldToForeignKey("LinkModification_Link_fk", "UnitConnectionEnd_Id_FK", "UnitConnectionEnd_Id_FK")){
//     pvLastError = table.lastError();
//     return false;
//   }
//   table.addForeignKey("LinkModification_LinkVersion_fk", "LinkVersion_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
//   if(!table.addFieldToForeignKey("LinkModification_LinkVersion_fk", "Version_FK", "Version_PK")){
//     pvLastError = table.lastError();
//     return false;
//   }
//   table.addForeignKey("LinkModification_Modification_fk", "Modification_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
//   if(!table.addFieldToForeignKey("LinkModification_Modification_fk", "Modification_Code_FK", "Code_PK")){
//     pvLastError = table.lastError();
//     return false;
//   }
// 
//   pvTables.append(table);
// 
//   return true;
// }

void mdtTtDatabaseSchema::setupLinkVersionTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Version_PK;
  mdtSqlField Version;

  table.setTableName("LinkVersion_tbl", "UTF8");
  // Version_PK
  Version_PK.setName("Version_PK");
  Version_PK.setType(mdtSqlFieldType::Integer);
  table.addField(Version_PK, true);
  // Version
  Version.setName("Version");
  Version.setType(mdtSqlFieldType::Double);
  table.addField(Version, false);

  pvSchema.addTable(table);
}

// bool mdtTtDatabaseSchema::createOnLinkVersionAfterInsertTrigger()
// {
//   QString sql;
// 
//   /*
//    * When creating a new link version in LinkVersion_tbl
//    * we must add a entry in LinkModification_tbl for each link
//    * that is allready versionned
//    */
//   sql = "CREATE TRIGGER onLinkVersionAfterInsert AFTER INSERT ON LinkVersion_tbl\n"\
//         "FOR EACH ROW\n"\
//         "BEGIN\n"\
//         " INSERT INTO LinkModification_tbl (UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK, Version_FK, Modification_Code_FK)\n"\
//         "  SELECT DISTINCT L.UnitConnectionStart_Id_FK, L.UnitConnectionEnd_Id_FK, NEW.Version_PK, 'EXISTS'\n"\
//         "   FROM Link_tbl L\n"\
//         "   LEFT JOIN LinkModification_tbl LM\n"\
//         "    ON LM.UnitConnectionStart_Id_FK = L.UnitConnectionStart_Id_FK\n"\
//         "    AND LM.UnitConnectionEnd_Id_FK = L.UnitConnectionEnd_Id_FK\n"\
//         "  WHERE LM.Version_FK = (SELECT max(Version_FK) FROM LinkModification_tbl)\n"\
//         "  AND (LM.Modification_Code_FK NOT IN ('REM') OR Modification_Code_FK IS NULL);\n"\
//         "END";
// 
//   return createTrigger("onLinkVersionAfterInsert", sql);
// }

bool mdtTtDatabaseSchema::createOnLinkVersionAfterInsertTrigger()
{
  QString sql;

  /*
   * When creating a new link version in LinkVersion_tbl
   * we must add a entry in LinkModification_tbl for each link
   * that is allready versionned
   */
  sql = "CREATE TRIGGER onLinkVersionAfterInsert AFTER INSERT ON LinkVersion_tbl\n"\
        "FOR EACH ROW\n"\
        "BEGIN\n"\
        " INSERT INTO Link_tbl (UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK, Version_FK, Modification_Code_FK, "\
                               "LinkType_Code_FK, LinkDirection_Code_FK, ArticleConnectionStart_Id_FK, ArticleConnectionEnd_Id_FK, "\
                               "Wire_Id_FK, LinkBeam_Id_FK, Identification, Resistance, Length, Remarks)\n"\
        "  SELECT DISTINCT L.UnitConnectionStart_Id_FK, L.UnitConnectionEnd_Id_FK, NEW.Version_PK, 'EXISTS', "\
                               "L.LinkType_Code_FK, L.LinkDirection_Code_FK, L.ArticleConnectionStart_Id_FK, L.ArticleConnectionEnd_Id_FK, "\
                               "L.Wire_Id_FK, L.LinkBeam_Id_FK, L.Identification, L.Resistance, L.Length, L.Remarks\n"\
        "   FROM Link_tbl L\n"\
        "  WHERE L.Version_FK = (SELECT max(Version_FK) FROM Link_tbl)\n"\
        "  AND (L.Modification_Code_FK NOT IN ('REM') OR L.Modification_Code_FK IS NULL);\n"\
        "END";

  return createTrigger("onLinkVersionAfterInsert", sql);
}

void mdtTtDatabaseSchema::setupLinkTable() 
{
  mdtSqlSchemaTable table;
  mdtSqlField UnitConnectionStart_Id_FK;
  mdtSqlForeignKey fk_UnitConnectionStart;
  mdtSqlField UnitConnectionEnd_Id_FK;
  mdtSqlForeignKey fk_UnitConnectionEnd;
  mdtSqlField Version_FK;
  mdtSqlForeignKey fk_Version;
  mdtSqlField Modification_Code_FK;
  mdtSqlForeignKey fk_Modification;
  mdtSqlField ArticleConnectionStart_Id_FK;
  mdtSqlField ArticleConnectionEnd_Id_FK;
  mdtSqlForeignKey fk_ArticleLink;
  mdtSqlField Wire_Id_FK;
  mdtSqlForeignKey fk_Wire;
  mdtSqlField LinkBeam_Id_FK;
  mdtSqlForeignKey fk_LinkBeam;
  mdtSqlField Identification;
  mdtSqlField LinkDirection_Code_FK;
  mdtSqlForeignKey fk_LinkDirection;
  mdtSqlField LinkType_Code_FK;
  mdtSqlForeignKey fk_LinkType;
  mdtSqlField Resistance;
  mdtSqlField Length;
  mdtSqlField Remarks;

  table.setTableName("Link_tbl", "UTF8");
  // UnitConnectionStart_Id_FK
  UnitConnectionStart_Id_FK.setName("UnitConnectionStart_Id_FK");
  UnitConnectionStart_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(UnitConnectionStart_Id_FK, true);
  fk_UnitConnectionStart.setParentTableName("UnitConnection_tbl");
  fk_UnitConnectionStart.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_UnitConnectionStart.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_UnitConnectionStart.setCreateChildIndex(true);
  fk_UnitConnectionStart.addKeyFields("Id_PK", UnitConnectionStart_Id_FK);
  table.addForeignKey(fk_UnitConnectionStart);
  // UnitConnectionEnd_Id_FK
  UnitConnectionEnd_Id_FK.setName("UnitConnectionEnd_Id_FK");
  UnitConnectionEnd_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(UnitConnectionEnd_Id_FK, true);
  fk_UnitConnectionEnd.setParentTableName("UnitConnection_tbl");
  fk_UnitConnectionEnd.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_UnitConnectionEnd.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_UnitConnectionEnd.setCreateChildIndex(true);
  fk_UnitConnectionEnd.addKeyFields("Id_PK", UnitConnectionEnd_Id_FK);
  table.addForeignKey(fk_UnitConnectionEnd);
  // Version_FK
  Version_FK.setName("Version_FK");
  Version_FK.setType(mdtSqlFieldType::Integer);
  table.addField(Version_FK, true);
  fk_Version.setParentTableName("LinkVersion_tbl");
  fk_Version.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Version.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Version.setCreateChildIndex(true);
  fk_Version.addKeyFields("Version_PK", Version_FK);
  table.addForeignKey(fk_Version);
  // Modification_Code_FK
  Modification_Code_FK.setName("Modification_Code_FK");
  Modification_Code_FK.setType(mdtSqlFieldType::Varchar);
  Modification_Code_FK.setLength(10);
  table.addField(Modification_Code_FK, true);
  fk_Modification.setParentTableName("Modification_tbl");
  fk_Modification.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Modification.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Modification.setCreateChildIndex(true);
  fk_Modification.addKeyFields("Code_PK", Modification_Code_FK);
  table.addForeignKey(fk_Modification);
  // ArticleConnectionStart_Id_FK
  ArticleConnectionStart_Id_FK.setName("ArticleConnectionStart_Id_FK");
  ArticleConnectionStart_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(ArticleConnectionStart_Id_FK, false);
  // ArticleConnectionEnd_Id_FK
  ArticleConnectionEnd_Id_FK.setName("ArticleConnectionEnd_Id_FK");
  ArticleConnectionEnd_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(ArticleConnectionEnd_Id_FK, false);
  fk_ArticleLink.setParentTableName("ArticleLink_tbl");
  fk_ArticleLink.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_ArticleLink.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_ArticleLink.setCreateChildIndex(true);
  fk_ArticleLink.addKeyFields("ArticleConnectionStart_Id_FK", ArticleConnectionStart_Id_FK);
  fk_ArticleLink.addKeyFields("ArticleConnectionEnd_Id_FK", ArticleConnectionEnd_Id_FK);
  table.addForeignKey(fk_ArticleLink);
  // Wire_Id_FK
  Wire_Id_FK.setName("Wire_Id_FK");
  Wire_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(Wire_Id_FK, false);
  fk_Wire.setParentTableName("Wire_tbl");
  fk_Wire.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Wire.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Wire.setCreateChildIndex(true);
  fk_Wire.addKeyFields("Id_PK", Wire_Id_FK);
  table.addForeignKey(fk_Wire);
  // LinkBeam_Id_FK
  LinkBeam_Id_FK.setName("LinkBeam_Id_FK");
  LinkBeam_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(LinkBeam_Id_FK, false);
  fk_LinkBeam.setParentTableName("LinkBeam_tbl");
  fk_LinkBeam.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_LinkBeam.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_LinkBeam.setCreateChildIndex(true);
  fk_LinkBeam.addKeyFields("Id_PK", LinkBeam_Id_FK);
  table.addForeignKey(fk_LinkBeam);
  // Identification
  Identification.setName("Identification");
  Identification.setType(mdtSqlFieldType::Varchar);
  Identification.setLength(50);
  table.addField(Identification, false);
  // LinkDirection_Code_FK
  LinkDirection_Code_FK.setName("LinkDirection_Code_FK");
  LinkDirection_Code_FK.setType(mdtSqlFieldType::Varchar);
  LinkDirection_Code_FK.setLength(10);
  LinkDirection_Code_FK.setRequired(true);
  table.addField(LinkDirection_Code_FK, false);
  fk_LinkDirection.setParentTableName("LinkDirection_tbl");
  fk_LinkDirection.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_LinkDirection.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_LinkDirection.setCreateChildIndex(true);
  fk_LinkDirection.addKeyFields("Code_PK", LinkDirection_Code_FK);
  table.addForeignKey(fk_LinkDirection);
  // LinkType_Code_FK
  LinkType_Code_FK.setName("LinkType_Code_FK");
  LinkType_Code_FK.setType(mdtSqlFieldType::Varchar);
  LinkType_Code_FK.setLength(10);
  LinkType_Code_FK.setRequired(true);
  table.addField(LinkType_Code_FK, false);
  fk_LinkType.setParentTableName("LinkType_tbl");
  fk_LinkType.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_LinkType.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_LinkType.setCreateChildIndex(true);
  fk_LinkType.addKeyFields("Id_PK", LinkType_Code_FK);
  table.addForeignKey(fk_LinkType);
  // Resistance
  Resistance.setName("Resistance");
  Resistance.setType(mdtSqlFieldType::Double);
  table.addField(Resistance, false);
  // Length
  Length.setName("Length");
  Length.setType(mdtSqlFieldType::Double);
  table.addField(Length, false);
  // Remarks
  Remarks.setName("Remarks");
  Remarks.setType(mdtSqlFieldType::Varchar);
  Remarks.setLength(200);
  table.addField(Remarks, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupLinkBeamTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField Identification;

  table.setTableName("LinkBeam_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // Identification
  Identification.setName("Identification");
  Identification.setType(mdtSqlFieldType::Varchar);
  Identification.setLength(50);
  table.addField(Identification, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupLinkBeamUnitStartTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Unit_Id_FK;
  mdtSqlForeignKey fk_Unit;
  mdtSqlField LinkBeam_Id_FK;
  mdtSqlForeignKey fk_LinkBeam;

  table.setTableName("LinkBeam_UnitStart_tbl", "UTF8");

  // Unit_Id_FK
  Unit_Id_FK.setName("Unit_Id_FK");
  Unit_Id_FK.setType(mdtSqlFieldType::Integer);
  Unit_Id_FK.setAutoValue(true);
  table.addField(Unit_Id_FK, true);
  fk_Unit.setParentTableName("Unit_tbl");
  fk_Unit.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Unit.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Unit.setCreateChildIndex(true);
  fk_Unit.addKeyFields("Id_PK", Unit_Id_FK);
  table.addForeignKey(fk_Unit);
  // LinkBeam_Id_FK
  LinkBeam_Id_FK.setName("LinkBeam_Id_FK");
  LinkBeam_Id_FK.setType(mdtSqlFieldType::Integer);
  LinkBeam_Id_FK.setAutoValue(true);
  table.addField(LinkBeam_Id_FK, true);
  fk_LinkBeam.setParentTableName("LinkBeam_tbl");
  fk_LinkBeam.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_LinkBeam.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_LinkBeam.setCreateChildIndex(true);
  fk_LinkBeam.addKeyFields("Id_PK", LinkBeam_Id_FK);
  table.addForeignKey(fk_LinkBeam);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupLinkBeamUnitEndTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Unit_Id_FK;
  mdtSqlForeignKey fk_Unit;
  mdtSqlField LinkBeam_Id_FK;
  mdtSqlForeignKey fk_LinkBeam;

  table.setTableName("LinkBeam_UnitEnd_tbl", "UTF8");

  // Unit_Id_FK
  Unit_Id_FK.setName("Unit_Id_FK");
  Unit_Id_FK.setType(mdtSqlFieldType::Integer);
  Unit_Id_FK.setAutoValue(true);
  table.addField(Unit_Id_FK, true);
  fk_Unit.setParentTableName("Unit_tbl");
  fk_Unit.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Unit.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Unit.setCreateChildIndex(true);
  fk_Unit.addKeyFields("Id_PK", Unit_Id_FK);
  table.addForeignKey(fk_Unit);
  // LinkBeam_Id_FK
  LinkBeam_Id_FK.setName("LinkBeam_Id_FK");
  LinkBeam_Id_FK.setType(mdtSqlFieldType::Integer);
  LinkBeam_Id_FK.setAutoValue(true);
  table.addField(LinkBeam_Id_FK, true);
  fk_LinkBeam.setParentTableName("LinkBeam_tbl");
  fk_LinkBeam.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_LinkBeam.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_LinkBeam.setCreateChildIndex(true);
  fk_LinkBeam.addKeyFields("Id_PK", LinkBeam_Id_FK);
  table.addForeignKey(fk_LinkBeam);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupLinkDirectionTable() 
{
  mdtSqlSchemaTable table;
  mdtSqlField Code_PK;
  mdtSqlField PictureAscii;
  mdtSqlField NameEN;
  mdtSqlField NameFR;
  mdtSqlField NameDE;
  mdtSqlField NameIT;

  table.setTableName("LinkDirection_tbl", "UTF8");
  // Code_PK
  Code_PK.setName("Code_PK");
  Code_PK.setType(mdtSqlFieldType::Varchar);
  Code_PK.setLength(10);
  table.addField(Code_PK, true);
  // PictureAscii
  PictureAscii.setName("PictureAscii");
  PictureAscii.setType(mdtSqlFieldType::Varchar);
  PictureAscii.setLength(50);
  table.addField(PictureAscii, false);
  // NameEN
  NameEN.setName("NameEN");
  NameEN.setType(mdtSqlFieldType::Varchar);
  NameEN.setLength(50);
  table.addField(NameEN, false);
  // NameFR
  NameFR.setName("NameFR");
  NameFR.setType(mdtSqlFieldType::Varchar);
  NameFR.setLength(50);
  table.addField(NameFR, false);
  // NameDE
  NameDE.setName("NameDE");
  NameDE.setType(mdtSqlFieldType::Varchar);
  NameDE.setLength(50);
  table.addField(NameDE, false);
  // NameIT
  NameIT.setName("NameIT");
  NameIT.setType(mdtSqlFieldType::Varchar);
  NameIT.setLength(50);
  table.addField(NameIT, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupLinkTypeTable() 
{
  mdtSqlSchemaTable table;
  mdtSqlField Code_PK;
  mdtSqlField NameEN;
  mdtSqlField NameFR;
  mdtSqlField NameDE;
  mdtSqlField NameIT;
  mdtSqlField ValueUnit;

  table.setTableName("LinkType_tbl", "UTF8");
  // Code_PK
  Code_PK.setName("Code_PK");
  Code_PK.setType(mdtSqlFieldType::Varchar);
  Code_PK.setLength(10);
  table.addField(Code_PK, true);
  // NameEN
  NameEN.setName("NameEN");
  NameEN.setType(mdtSqlFieldType::Varchar);
  NameEN.setLength(50);
  table.addField(NameEN, false);
  // NameFR
  NameFR.setName("NameFR");
  NameFR.setType(mdtSqlFieldType::Varchar);
  NameFR.setLength(50);
  table.addField(NameFR, false);
  // NameDE
  NameDE.setName("NameDE");
  NameDE.setType(mdtSqlFieldType::Varchar);
  NameDE.setLength(50);
  table.addField(NameDE, false);
  // NameIT
  NameIT.setName("NameIT");
  NameIT.setType(mdtSqlFieldType::Varchar);
  NameIT.setLength(50);
  table.addField(NameIT, false);
  // ValueUnit
  ValueUnit.setName("ValueUnit");
  ValueUnit.setType(mdtSqlFieldType::Varchar);
  ValueUnit.setLength(10);
  table.addField(ValueUnit, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupTestSystemTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField NameEN;
  mdtSqlField NameFR;
  mdtSqlField NameDE;
  mdtSqlField NameIT;

  table.setTableName("TestSystem_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // NameEN
  NameEN.setName("NameEN");
  NameEN.setType(mdtSqlFieldType::Varchar);
  NameEN.setLength(200);
  table.addField(NameEN, false);
  // NameFR
  NameFR.setName("NameFR");
  NameFR.setType(mdtSqlFieldType::Varchar);
  NameFR.setLength(200);
  table.addField(NameFR, false);
  // NameDE
  NameDE.setName("NameDE");
  NameDE.setType(mdtSqlFieldType::Varchar);
  NameDE.setLength(200);
  table.addField(NameDE, false);
  // NameIT
  NameIT.setName("NameIT");
  NameIT.setType(mdtSqlFieldType::Varchar);
  NameIT.setLength(200);
  table.addField(NameIT, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupTestSystemComponentTypeTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Code_PK;
  mdtSqlField NameEN;
  mdtSqlField NameFR;
  mdtSqlField NameDE;
  mdtSqlField NameIT;

  table.setTableName("TestSystemComponentType_tbl", "UTF8");
  // Code_PK
  Code_PK.setName("Code_PK");
  Code_PK.setType(mdtSqlFieldType::Varchar);
  Code_PK.setLength(20);
  table.addField(Code_PK, true);
  // NameEN
  NameEN.setName("NameEN");
  NameEN.setType(mdtSqlFieldType::Varchar);
  NameEN.setLength(200);
  table.addField(NameEN, false);
  // NameFR
  NameFR.setName("NameFR");
  NameFR.setType(mdtSqlFieldType::Varchar);
  NameFR.setLength(200);
  table.addField(NameFR, false);
  // NameDE
  NameDE.setName("NameDE");
  NameDE.setType(mdtSqlFieldType::Varchar);
  NameDE.setLength(200);
  table.addField(NameDE, false);
  // NameIT
  NameIT.setName("NameIT");
  NameIT.setType(mdtSqlFieldType::Varchar);
  NameIT.setLength(200);
  table.addField(NameIT, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupTestSystemComponentTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Id_PK;
  mdtSqlField Type_Code_FK;
  mdtSqlForeignKey fk_Type;
  mdtSqlField Alias;
  mdtSqlField NameEN;
  mdtSqlField NameFR;
  mdtSqlField NameDE;
  mdtSqlField NameIT;
  mdtSqlField DescriptionEN;
  mdtSqlField DescriptionFR;
  mdtSqlField DescriptionDE;
  mdtSqlField DescriptionIT;

  table.setTableName("TestSystemComponent_tbl", "UTF8");
  // Id_PK
  Id_PK.setName("Id_PK");
  Id_PK.setType(mdtSqlFieldType::Integer);
  Id_PK.setAutoValue(true);
  table.addField(Id_PK, true);
  // Type_Code_FK
  Type_Code_FK.setName("Type_Code_FK");
  Type_Code_FK.setType(mdtSqlFieldType::Varchar);
  Type_Code_FK.setLength(20);
  Type_Code_FK.setRequired(true);
  table.addField(Type_Code_FK, false);
  fk_Type.setParentTableName("TestSystemComponentType_tbl");
  fk_Type.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Type.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Type.setCreateChildIndex(true);
  fk_Type.addKeyFields("Code_PK", Type_Code_FK);
  table.addForeignKey(fk_Type);
  // Alias
  Alias.setName("Alias");
  Alias.setRequired(true);
  Alias.setType(mdtSqlFieldType::Varchar);
  Alias.setLength(50);
  table.addField(Alias, false);
  // NameEN
  NameEN.setName("NameEN");
  NameEN.setType(mdtSqlFieldType::Varchar);
  NameEN.setLength(200);
  table.addField(NameEN, false);
  // NameFR
  NameFR.setName("NameFR");
  NameFR.setType(mdtSqlFieldType::Varchar);
  NameFR.setLength(200);
  table.addField(NameFR, false);
  // NameDE
  NameDE.setName("NameDE");
  NameDE.setType(mdtSqlFieldType::Varchar);
  NameDE.setLength(200);
  table.addField(NameDE, false);
  // NameIT
  NameIT.setName("NameIT");
  NameIT.setType(mdtSqlFieldType::Varchar);
  NameIT.setLength(200);
  table.addField(NameIT, false);
  // DescriptionEN
  DescriptionEN.setName("DescriptionEN");
  DescriptionEN.setType(mdtSqlFieldType::Varchar);
  DescriptionEN.setLength(500);
  table.addField(DescriptionEN, false);
  // DescriptionFR
  DescriptionFR.setName("DescriptionFR");
  DescriptionFR.setType(mdtSqlFieldType::Varchar);
  DescriptionFR.setLength(500);
  table.addField(DescriptionFR, false);
  // DescriptionDE
  DescriptionDE.setName("DescriptionDE");
  DescriptionDE.setType(mdtSqlFieldType::Varchar);
  DescriptionDE.setLength(500);
  table.addField(DescriptionDE, false);
  // DescriptionIT
  DescriptionIT.setName("DescriptionIT");
  DescriptionIT.setType(mdtSqlFieldType::Varchar);
  DescriptionIT.setLength(500);
  table.addField(DescriptionIT, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupTestSystem_TestSystemComponentTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField TestSystem_Id_FK;
  mdtSqlForeignKey fk_TestSystem;
  mdtSqlField TestSystemComponent_Id_FK;
  mdtSqlForeignKey fk_TestSystemComponent;

  table.setTableName("TestSystem_TestSystemComponent_tbl", "UTF8");
  // TestSystem_Id_FK
  TestSystem_Id_FK.setName("TestSystem_Id_FK");
  TestSystem_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(TestSystem_Id_FK, true);
  fk_TestSystem.setParentTableName("TestSystem_tbl");
  fk_TestSystem.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_TestSystem.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_TestSystem.setCreateChildIndex(true);
  fk_TestSystem.addKeyFields("Id_PK", TestSystem_Id_FK);
  table.addForeignKey(fk_TestSystem);
  // TestSystemComponent_Id_FK
  TestSystemComponent_Id_FK.setName("TestSystemComponent_Id_FK");
  TestSystemComponent_Id_FK.setType(mdtSqlFieldType::Integer);
  table.addField(TestSystemComponent_Id_FK, true);
  fk_TestSystemComponent.setParentTableName("TestSystemComponent_tbl");
  fk_TestSystemComponent.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_TestSystemComponent.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_TestSystemComponent.setCreateChildIndex(true);
  fk_TestSystemComponent.addKeyFields("Id_PK", TestSystemComponent_Id_FK);
  table.addForeignKey(fk_TestSystemComponent);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupTestSystemUnitTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Code_PK;
  mdtSqlField NameEN;
  mdtSqlField NameFR;
  mdtSqlField NameDE;
  mdtSqlField NameIT;


  table.setTableName("TestSystemUnitType_tbl", "UTF8");
  // Code_PK
  Code_PK.setName("Code_PK");
  Code_PK.setType(mdtSqlFieldType::Varchar);
  Code_PK.setLength(20);
  table.addField(Code_PK, true);
  // NameEN
  NameEN.setName("NameEN");
  NameEN.setType(mdtSqlFieldType::Varchar);
  NameEN.setLength(200);
  table.addField(NameEN, false);
  // NameFR
  NameFR.setName("NameFR");
  NameFR.setType(mdtSqlFieldType::Varchar);
  NameFR.setLength(200);
  table.addField(NameFR, false);
  // NameDE
  NameDE.setName("NameDE");
  NameDE.setType(mdtSqlFieldType::Varchar);
  NameDE.setLength(200);
  table.addField(NameDE, false);
  // NameIT
  NameIT.setName("NameIT");
  NameIT.setType(mdtSqlFieldType::Varchar);
  NameIT.setLength(200);
  table.addField(NameIT, false);

  pvSchema.addTable(table);
}

void mdtTtDatabaseSchema::setupTestSystemUnitTypeTable()
{
  mdtSqlSchemaTable table;
  mdtSqlField Unit_Id_FK_PK;
  mdtSqlForeignKey fk_Unit;
  mdtSqlField TestSystemComponent_Id_FK;
  mdtSqlForeignKey fk_TestSystemComponent;
  mdtSqlField Type_Code_FK;
  mdtSqlForeignKey fk_Type;
  mdtSqlField IoPosition;
  mdtSqlField CalibrationDate;
  

  table.setTableName("TestSystemUnit_tbl", "UTF8");
  // Unit_Id_FK_PK
  Unit_Id_FK_PK.setName("Unit_Id_FK_PK");
  Unit_Id_FK_PK.setType(mdtSqlFieldType::Integer);
  table.addField(Unit_Id_FK_PK, true);
  fk_Unit.setParentTableName("Unit_tbl");
  fk_Unit.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Unit.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Unit.setCreateChildIndex(true);
  fk_Unit.addKeyFields("Id_PK", Unit_Id_FK_PK);
  table.addForeignKey(fk_Unit);
  // TestSystemComponent_Id_FK
  TestSystemComponent_Id_FK.setName("TestSystemComponent_Id_FK");
  TestSystemComponent_Id_FK.setType(mdtSqlFieldType::Integer);
  TestSystemComponent_Id_FK.setRequired(true);
  table.addField(TestSystemComponent_Id_FK, false);
  fk_TestSystemComponent.setParentTableName("TestSystemComponent_tbl");
  fk_TestSystemComponent.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_TestSystemComponent.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_TestSystemComponent.setCreateChildIndex(true);
  fk_TestSystemComponent.addKeyFields("Id_PK", TestSystemComponent_Id_FK);
  table.addForeignKey(fk_TestSystemComponent);
  // Type_Code_FK
  Type_Code_FK.setName("Type_Code_FK");
  Type_Code_FK.setType(mdtSqlFieldType::Varchar);
  Type_Code_FK.setLength(10);
  Type_Code_FK.setRequired(true);
  table.addField(Type_Code_FK, false);
  fk_Type.setParentTableName("TestSystemUnitType_tbl");
  fk_Type.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk_Type.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk_Type.setCreateChildIndex(true);
  fk_Type.addKeyFields("Code_PK", Type_Code_FK);
  table.addForeignKey(fk_Type);
  // IoPosition
  IoPosition.setName("IoPosition");
  IoPosition.setType(mdtSqlFieldType::Integer);
  table.addField(IoPosition, false);
  // CalibrationDate
  CalibrationDate.setName("CalibrationDate");
  CalibrationDate.setType(mdtSqlFieldType::DateTime);
  table.addField(CalibrationDate, false);

  pvSchema.addTable(table);
}




bool mdtTtDatabaseSchema::setupTestNodeTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestNode_tbl", "UTF8");
  // VehicleType_Id_FK_PK
  field.setName("VehicleType_Id_FK_PK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // AddressString
  field = QSqlField();
  field.setName("AddressString");
  field.setType(QVariant::String);
  field.setLength(200);
  table.addField(field, false);
  // Alias
  field = QSqlField();
  field.setName("Alias");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
//   // NodeIdentification
//   field = QSqlField();
//   field.setName("NodeIdentification");
//   field.setType(QVariant::String);
//   field.setLength(50);
//   ///field.setRequired(true);
//   table.addField(field, false);
//   // DeviceIdentification
//   field = QSqlField();
//   field.setName("DeviceIdentification");
//   field.setType(QVariant::String);
//   field.setLength(50);
//   table.addField(field, false);
  // Indexes
  /**
  table.addIndex("VehicleType_Id_FK_PK_idx", false);
  if(!table.addFieldToIndex("VehicleType_Id_FK_PK_idx", "VehicleType_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }
  */
  table.addIndex("AddressString_idx", true);
  if(!table.addFieldToIndex("AddressString_idx", "AddressString")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("Alias_idx", true);
  if(!table.addFieldToIndex("Alias_idx", "Alias")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("VehicleType_Id_FK_PK_fk", "VehicleType_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("VehicleType_Id_FK_PK_fk", "VehicleType_Id_FK_PK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestNodeUnitTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestNodeUnit_tbl", "UTF8");
  // Unit_Id_FK_PK
  field.setName("Unit_Id_FK_PK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // TestNode_Id_FK
  field = QSqlField();
  field.setName("TestNode_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // Type_Code_FK
  field = QSqlField();
  field.setName("Type_Code_FK");
  field.setType(QVariant::String);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // CalibrationOffset
  field = QSqlField();
  field.setName("CalibrationOffset");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // CalibrationDate
  field = QSqlField();
  field.setName("CalibrationDate");
  field.setType(QVariant::DateTime);
  table.addField(field, false);
  // IoPosition
  field = QSqlField();
  field.setName("IoPosition");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Indexes
  table.addIndex("Unit_Id_FK_PK_idx", false);
  if(!table.addFieldToIndex("Unit_Id_FK_PK_idx", "Unit_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("TestNode_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestNode_Id_FK_idx", "TestNode_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("Type_Code_FK_idx", false);
  if(!table.addFieldToIndex("Type_Code_FK_idx", "Type_Code_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("Unit_Id_FK_PK_fk", "Unit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Unit_Id_FK_PK_fk", "Unit_Id_FK_PK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("TestNode_Id_FK_fk", "TestNode_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNode_Id_FK_fk", "TestNode_Id_FK", "VehicleType_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("Type_Code_FK_fk", "TestNodeUnitType_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Type_Code_FK_fk", "Type_Code_FK", "Code_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestNodeUnitConnectionTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestNodeUnitConnection_tbl", "UTF8");
  // UnitConnection_Id_FK_PK
  field.setName("UnitConnection_Id_FK_PK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // TestNodeUnit_Id_FK
  field = QSqlField();
  field.setName("TestNodeUnit_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // TestNodeBus_Id_FK
//   field = QSqlField();
//   field.setName("TestNodeBus_Id_FK");
//   field.setType(QVariant::Int);
//   table.addField(field, false);
  // Indexes
  table.addIndex("TestNodeUnit_Id_FK_idx2", false);
  if(!table.addFieldToIndex("TestNodeUnit_Id_FK_idx2", "TestNodeUnit_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
//   table.addIndex("TestNodeBus_Id_FK_idx", false);
//   if(!table.addFieldToIndex("TestNodeBus_Id_FK_idx", "TestNodeBus_Id_FK")){
//     pvLastError = table.lastError();
//     return false;
//   }
  // Foreign keys
  table.addForeignKey("UnitConnection_Id_FK_PK_fk", "UnitConnection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("UnitConnection_Id_FK_PK_fk", "UnitConnection_Id_FK_PK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("TestNodeUnit_Id_FK_fk2", "TestNodeUnit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNodeUnit_Id_FK_fk2", "TestNodeUnit_Id_FK", "Unit_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }
//   table.addForeignKey("TestNodeBus_Id_FK_fk", "TestNodeBus_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
//   if(!table.addFieldToForeignKey("TestNodeBus_Id_FK_fk", "TestNodeBus_Id_FK", "Id_PK")){
//     pvLastError = table.lastError();
//     return false;
//   }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestNodeRouteTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestNodeRoute_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // TestNode_Id_FK
  field = QSqlField();
  field.setName("TestNode_Id_FK");
  field.setType(QVariant::Int);
  field.setRequired(true);
  table.addField(field, false);
  // TestNodeUnitConnectionA_Id_FK
  field = QSqlField();
  field.setName("TestNodeUnitConnectionA_Id_FK");
  field.setType(QVariant::Int);
  field.setRequired(true);
  table.addField(field, false);
  // TestNodeUnitConnectionB_Id_FK
  field = QSqlField();
  field.setName("TestNodeUnitConnectionB_Id_FK");
  field.setType(QVariant::Int);
  field.setRequired(true);
  table.addField(field, false);
  // Name
  field = QSqlField();
  field.setName("Name");
  field.setType(QVariant::String);
  table.addField(field, false);
  // Resistance
  field = QSqlField();
  field.setName("Resistance");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // CalibrationDate
  field = QSqlField();
  field.setName("CalibrationDate");
  field.setType(QVariant::DateTime);
  table.addField(field, false);
  // Indexes
  table.addIndex("TestNodeRoute_idx", false);
  if(!table.addFieldToIndex("TestNodeRoute_idx", "TestNode_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  if(!table.addFieldToIndex("TestNodeRoute_idx", "TestNodeUnitConnectionA_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  if(!table.addFieldToIndex("TestNodeRoute_idx", "TestNodeUnitConnectionB_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("TestNodeRouteName_idx", false);
  if(!table.addFieldToIndex("TestNodeRouteName_idx", "Name")){
    pvLastError = table.lastError();
    return false;
  }
  /**
  table.addIndex("TestNode_Id_FK_idx4", false);
  if(!table.addFieldToIndex("TestNode_Id_FK_idx4", "TestNode_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("TestNodeUnitConnectionA_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestNodeUnitConnectionA_Id_FK_idx", "TestNodeUnitConnectionA_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("TestNodeUnitConnectionB_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestNodeUnitConnectionB_Id_FK_idx", "TestNodeUnitConnectionB_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  */
  // Foreign keys
  table.addForeignKey("TestNode_Id_FK_fk4", "TestNode_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNode_Id_FK_fk4", "TestNode_Id_FK", "VehicleType_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("TestNodeUnitConnectionA_Id_FK_fk", "TestNodeUnitConnection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNodeUnitConnectionA_Id_FK_fk", "TestNodeUnitConnectionA_Id_FK", "UnitConnection_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("TestNodeUnitConnectionB_Id_FK_fk", "TestNodeUnitConnection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNodeUnitConnectionB_Id_FK_fk", "TestNodeUnitConnectionB_Id_FK", "UnitConnection_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestNodeRouteUnitTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestNodeRouteUnit_tbl", "UTF8");
  // TestNodeRoute_Id_FK
  field.setName("TestNodeRoute_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // TestNodeUnit_Id_FK
  field = QSqlField();
  field.setName("TestNodeUnit_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // Foreign keys
  table.addForeignKey("TestNodeRoute_Id_FK_fk4", "TestNodeRoute_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNodeRoute_Id_FK_fk4", "TestNodeRoute_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("TestNodeUnit_Id_FK_kf4", "TestNodeUnit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNodeUnit_Id_FK_kf4", "TestNodeUnit_Id_FK", "Unit_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestNodeUnitTypeTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestNodeUnitType_tbl", "UTF8");
  // Code_PK
  field.setName("Code_PK");
  field.setType(QVariant::String);
  field.setLength(10);
  table.addField(field, true);
  // NameEN
  field = QSqlField();
  field.setName("NameEN");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // NameFR
  field = QSqlField();
  field.setName("NameFR");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // NameDE
  field = QSqlField();
  field.setName("NameDE");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // NameIT
  field = QSqlField();
  field.setName("NameIT");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestCableTestNodeUnitTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("LogicalTestCable_TestNodeUnit_tbl", "UTF8");
  // TestNodeUnit_Id_FK
  field.setName("TestNodeUnit_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // TestCable_Id_FK
  field = QSqlField();
  field.setName("LogicalTestCable_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // Foreign keys
  table.addForeignKey("TestNodeUnit_Id_FK_fk3", "TestNodeUnit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNodeUnit_Id_FK_fk3", "TestNodeUnit_Id_FK", "Unit_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("LogicalTestCable_Id_FK_fk2", "LogicalTestCable_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("LogicalTestCable_Id_FK_fk2", "LogicalTestCable_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestCableDutUnitTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("LogicalTestCable_DutUnit_tbl", "UTF8");
  // TestNodeUnit_Id_FK
  field.setName("DutUnit_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // LogicalTestCable_Id_FK
  field = QSqlField();
  field.setName("LogicalTestCable_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // Foreign keys
  table.addForeignKey("DutUnit_Id_FK_fk", "Unit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("DutUnit_Id_FK_fk", "DutUnit_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("LogicalTestCable_Id_FK_fk3", "LogicalTestCable_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("LogicalTestCable_Id_FK_fk3", "LogicalTestCable_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestCableTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestCable_tbl", "UTF8");
  // Unit_Id_FK_PK
  field.setName("Unit_Id_FK_PK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // OffsetResetTestModel_Id_FK
  field = QSqlField();
  field.setName("OffsetResetTestModel_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Identification
  field = QSqlField();
  field.setName("Identification");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // DescriptionEN
  field = QSqlField();
  field.setName("DescriptionEN");
  field.setType(QVariant::String);
  field.setLength(150);
  table.addField(field, false);
  // OffsetResetDate
  field = QSqlField();
  field.setName("OffsetResetDate");
  field.setType(QVariant::DateTime);
  table.addField(field, false);
  // Foreign keys
  table.addForeignKey("Unit_Id_FK_PK_fk2", "Unit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Unit_Id_FK_PK_fk2", "Unit_Id_FK_PK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("OffsetResetTestModel_Id_FK_fk", "TestModel_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("OffsetResetTestModel_Id_FK_fk", "OffsetResetTestModel_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupLogicalTestCableTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("LogicalTestCable_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // TestCable_Id_FK
  field = QSqlField();
  field.setName("TestCable_Id_FK");
  field.setType(QVariant::Int);
  field.setRequired(true);
  table.addField(field, false);
  // Key
  field = QSqlField();
  field.setName("Key");
  field.setType(QVariant::String);
  field.setLength(70);
  table.addField(field, false);
  // Indexes
  table.addIndex("Key_idx2", true);
  if(!table.addFieldToIndex("Key_idx2", "Key")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("TestCable_Id_FK_fk", "TestCable_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestCable_Id_FK_fk", "TestCable_Id_FK", "Unit_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestLinkTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestLink_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // TestCableUnitConnectionStart_Id_FK
  field = QSqlField();
  field.setName("TestCableUnitConnectionStart_Id_FK");
  field.setType(QVariant::Int);
  field.setRequired(true);
  table.addField(field, false);
  // TestCableUnitConnectionEnd_Id_FK
  field = QSqlField();
  field.setName("TestCableUnitConnectionEnd_Id_FK");
  field.setType(QVariant::Int);
  field.setRequired(true);
  table.addField(field, false);
  // TestConnection_Id_FK
  field = QSqlField();
  field.setName("TestConnection_Id_FK");
  field.setType(QVariant::Int);
  field.setRequired(true);
  table.addField(field, false);
  // DutConnection_Id_FK
  field = QSqlField();
  field.setName("DutConnection_Id_FK");
  field.setType(QVariant::Int);
  field.setRequired(true);
  table.addField(field, false);
  // TestCable_Id_FK
  field = QSqlField();
  field.setName("LogicalTestCable_Id_FK");
  field.setType(QVariant::Int);
  field.setRequired(true);
  table.addField(field, false);
  // Identification
  field = QSqlField();
  field.setName("Identification");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // Resistance
  /**
  field = QSqlField();
  field.setName("Resistance");
  field.setType(QVariant::Double);
  table.addField(field, false);
  */
  // Indexes
  table.addIndex("Link_Id_FK_idx", false);
  if(!table.addFieldToIndex("Link_Id_FK_idx", "TestCableUnitConnectionStart_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  if(!table.addFieldToIndex("Link_Id_FK_idx", "TestCableUnitConnectionEnd_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("TestConnection_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestConnection_Id_FK_idx", "TestConnection_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("DutConnection_Id_FK_idx", false);
  if(!table.addFieldToIndex("DutConnection_Id_FK_idx", "DutConnection_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("LogicalTestCable_Id_FK_idx", false);
  if(!table.addFieldToIndex("LogicalTestCable_Id_FK_idx", "LogicalTestCable_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  /**
   * \todo Update once all is defined !!
  table.addForeignKey("Link_Id_FK_fk", "Link_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Link_Id_FK_fk", "TestCableUnitConnectionStart_Id_FK", "UnitConnectionStart_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  if(!table.addFieldToForeignKey("Link_Id_FK_fk", "TestCableUnitConnectionEnd_Id_FK", "UnitConnectionEnd_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  */
  table.addForeignKey("TestConnection_Id_FK_fk2", "TestNodeUnitConnection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestConnection_Id_FK_fk2", "TestConnection_Id_FK", "UnitConnection_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("DutConnection_Id_FK_fk", "UnitConnection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("DutConnection_Id_FK_fk", "DutConnection_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("LogicalTestCable_Id_FK_fk", "LogicalTestCable_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("LogicalTestCable_Id_FK_fk", "LogicalTestCable_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestModelTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestModel_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Key
  field = QSqlField();
  field.setName("Key");
  field.setType(QVariant::String);
  field.setLength(70);
  table.addField(field, false);
  // Designation EN
  field = QSqlField();
  field.setName("DesignationEN");
  field.setType(QVariant::String);
  field.setLength(150);
  table.addField(field, false);
  // Indexes
  table.addIndex("Key_idx", true);
  if(!table.addFieldToIndex("Key_idx", "Key")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestModelTestNodeTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestModel_TestNode_tbl", "UTF8");
  // TestModel_Id_FK
  field.setName("TestModel_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // TestNode_Id_FK
  field = QSqlField();
  field.setName("TestNode_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // Foreign keys
  table.addForeignKey("TestModel_Id_FK_fk3", "TestModel_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestModel_Id_FK_fk3", "TestModel_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("TestNode_Id_FK_fk3", "TestNode_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNode_Id_FK_fk3", "TestNode_Id_FK", "VehicleType_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestModelItemTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestModelItem_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // TestModel_Id_FK
  field = QSqlField();
  field.setName("TestModel_Id_FK");
  field.setRequiredStatus(QSqlField::Required);
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Key
  field = QSqlField();
  field.setName("Key");
  field.setType(QVariant::String);
  field.setLength(70);
  table.addField(field, false);
  // TestLinkBusA_Id_FK
  field = QSqlField();
  // SequenceNumber
  field = QSqlField();
  field.setName("SequenceNumber");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // DesignationEN
  field = QSqlField();
  field.setName("DesignationEN");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // ExpectedValue
  field = QSqlField();
  field.setName("ExpectedValue");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // LeftBottomLimit
  field = QSqlField();
  field.setName("LeftBottomLimit");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // LeftTopLimit
  field = QSqlField();
  field.setName("LeftTopLimit");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // RightBottomLimit
  field = QSqlField();
  field.setName("RightBottomLimit");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // RightTopLimit
  field = QSqlField();
  field.setName("RightTopLimit");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // ValueUnit
  field = QSqlField();
  field.setName("ValueUnit");
  field.setType(QVariant::String);
  field.setLength(10);
  table.addField(field, false);
  // Indexes
  table.addIndex("TestModel_Id_FK_and_Key_idx", true);
  if(!table.addFieldToIndex("TestModel_Id_FK_and_Key_idx", "TestModel_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  if(!table.addFieldToIndex("TestModel_Id_FK_and_Key_idx", "Key")){
    pvLastError = table.lastError();
    return false;
  }
  /**
  table.addIndex("TestLinkBusA_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestLinkBusA_Id_FK_idx", "TestLinkBusA_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("TestLinkBusB_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestLinkBusB_Id_FK_idx", "TestLinkBusB_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  */
  // Foreign keys
  table.addForeignKey("TestModel_Id_FK_fk", "TestModel_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestModel_Id_FK_fk", "TestModel_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  /**
  table.addForeignKey("TestLinkBusA_Id_FK_fk", "TestLink_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestLinkBusA_Id_FK_fk", "TestLinkBusA_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("TestLinkBusB_Id_FK_fk", "TestLink_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestLinkBusB_Id_FK_fk", "TestLinkBusB_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  */

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestModelItemTestLinkTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestModelItem_TestLink_tbl", "UTF8");
  // TestModelItem_Id_FK
  field = QSqlField();
  field.setName("TestModelItem_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // TestLink_Id_FK
  field = QSqlField();
  field.setName("TestLink_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // Foreign keys
  table.addForeignKey("TestModelItem_Id_FK_fk3", "TestModelItem_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestModelItem_Id_FK_fk3", "TestModelItem_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("TestLink_Id_FK_fk", "TestLink_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestLink_Id_FK_fk", "TestLink_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestNodeUnitSetupTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestNodeUnitSetup_tbl", "UTF8");
  // Id_PK
  /**
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  */
  // TestModelItem_Id_FK
  field = QSqlField();
  field.setName("TestModelItem_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  ///field.setRequiredStatus(QSqlField::Required);
  // TestNodeUnit_Id_FK
  field = QSqlField();
  field.setName("TestNodeUnit_Id_FK");
  field.setType(QVariant::Int);
  ///field.setRequiredStatus(QSqlField::Required);
  table.addField(field, true);
  // TestModelItemRoute_Id_FK
//   field = QSqlField();
//   field.setName("TestModelItemRoute_Id_FK");
//   field.setType(QVariant::Int);
//   table.addField(field, false);
  /**
  // TestNodeRoute_Id_FK
  field = QSqlField();
  field.setName("TestNodeRoute_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  */
  // StepNumber
  field = QSqlField();
  field.setName("StepNumber");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // State (ON/OFF)
  field = QSqlField();
  field.setName("State");
  field.setType(QVariant::Bool);
  table.addField(field, false);
  // Value (for analog I/O, f.ex.)
  field = QSqlField();
  field.setName("Value");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // Command (for example, SCPI)
  field = QSqlField();
  field.setName("Command");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // Indexes
//   table.addIndex("TestModelItemRoute_Id_FK_idx", false);
//   if(!table.addFieldToIndex("TestModelItemRoute_Id_FK_idx", "TestModelItemRoute_Id_FK")){
//     pvLastError = table.lastError();
//     return false;
//   }
  /**
  table.addIndex("TestNodeRoute_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestNodeRoute_Id_FK_idx", "TestNodeRoute_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  */
  /**
  table.addIndex("TestModelItem_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestModelItem_Id_FK_idx", "TestModelItem_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("TestNodeUnit_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestNodeUnit_Id_FK_idx", "TestNodeUnit_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  */
  // Foreign keys
  table.addForeignKey("TestModelItem_Id_FK_fk", "TestModelItem_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestModelItem_Id_FK_fk", "TestModelItem_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("TestNodeUnit_Id_FK_fk", "TestNodeUnit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNodeUnit_Id_FK_fk", "TestNodeUnit_Id_FK", "Unit_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }
//   table.addForeignKey("TestModelItemRoute_Id_FK_fk", "TestModelItemRoute_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
//   if(!table.addFieldToForeignKey("TestModelItemRoute_Id_FK_fk", "TestModelItemRoute_Id_FK", "Id_PK")){
//     pvLastError = table.lastError();
//     return false;
//   }
/**
  table.addForeignKey("TestNodeRoute_Id_FK_fk", "TestNodeRoute_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNodeRoute_Id_FK_fk", "TestNodeRoute_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
*/

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("Test_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // TestModel_Id_FK
  field = QSqlField();
  field.setName("TestModel_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Date
  field = QSqlField();
  field.setName("Date");
  field.setType(QVariant::Date);
  table.addField(field, false);
  // DutName
  field = QSqlField();
  field.setName("DutName");
  field.setType(QVariant::String);
  field.setLength(150);
  table.addField(field, false);
  // DutSerialNumber
  field = QSqlField();
  field.setName("DutSerialNumber");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // Indexes
  table.addIndex("TestModel_Id_FK_idx2", false);
  if(!table.addFieldToIndex("TestModel_Id_FK_idx2", "TestModel_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("TestModel_Id_FK_fk2", "TestModel_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestModel_Id_FK_fk2", "TestModel_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestItemTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestItem_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Test_Id_FK
  field = QSqlField();
  field.setName("Test_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // TestModelItem_Id_FK
  field = QSqlField();
  field.setName("TestModelItem_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Date
  field = QSqlField();
  field.setName("Date");
  field.setType(QVariant::DateTime);
  table.addField(field, false);
  // MeasuredValue
  field = QSqlField();
  field.setName("MeasuredValue");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // InstrumentRangeMin
  field = QSqlField();
  field.setName("InstrumentRangeMin");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // InstrumentRangeMax
  field = QSqlField();
  field.setName("InstrumentRangeMax");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // ResultValue
  field = QSqlField();
  field.setName("ResultValue");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // Result
  field = QSqlField();
  field.setName("Result");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // Remark
  field = QSqlField();
  field.setName("Remark");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // Indexes
  table.addIndex("Test_Id_FK_idx", false);
  if(!table.addFieldToIndex("Test_Id_FK_idx", "Test_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("TestModelItem_Id_FK_idx2", false);
  if(!table.addFieldToIndex("TestModelItem_Id_FK_idx2", "TestModelItem_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("Test_Id_FK_fk", "Test_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Test_Id_FK_fk", "Test_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("TestModelItem_Id_FK_fk2", "TestModelItem_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestModelItem_Id_FK_fk2", "TestModelItem_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::createTrigger(const QString & triggerName, const QString & sql)
{
  QSqlQuery query(pvDatabaseManager->database());
  QSqlError sqlError;
  QString dropSql;

  qDebug() << "Creating trigger " << triggerName << " ...";
  // Drop trigger
  dropSql = "DROP TRIGGER IF EXISTS '" + triggerName +"'";
  if(!query.exec(dropSql)){
    sqlError = query.lastError();
    pvLastError.setError("Unable to drop trigger '" + triggerName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtDatabaseSchema");
    pvLastError.commit();
    return false;
  }
  // Create trigger
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Unable to create trigger '" + triggerName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtDatabaseSchema");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtDatabaseSchema::createView(const QString & viewName, const QString & sql) 
{
  QSqlQuery query(pvDatabaseManager->database());
  QSqlError sqlError;
  QString dropSql;

  qDebug() << "Creating view " << viewName << " ...";
  // Drop view
  dropSql = "DROP VIEW IF EXISTS '" + viewName +"'";
  if(!query.exec(dropSql)){
    sqlError = query.lastError();
    pvLastError.setError("Unable to drop view '" + viewName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtDatabaseSchema");
    pvLastError.commit();
    return false;
  }
  // Create view
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Unable to create view '" + viewName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtDatabaseSchema");
    pvLastError.commit();
    return false;
  }

  return true;
}

void mdtTtDatabaseSchema::setupVehicleTypeUnitView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table VTU("VehicleType_Unit_tbl", "VTU");
  Table VT("VehicleType_tbl", "VT");
  Table U("Unit_tbl", "U");
  Table A("Article_tbl", "A");
  JoinClause join;

  view.setName("VehicleType_Unit_view");
  view.setTable(VTU);
  view.addSelectField(VT, SelectField("Type"));
  view.addSelectField(VT, SelectField("SubType"));
  view.addSelectField(VT, SelectField("SeriesNumber"));
  view.addSelectField(U, SelectField("Coordinate"));
  view.addSelectField(U, SelectField("Cabinet"));
  view.addSelectField(U, SelectField("SchemaPosition"));
  view.addSelectField(U, SelectField("Alias"));
  view.addSelectField(A, SelectField("ArticleCode"));
  view.addSelectField(A, SelectField("Manufacturer"));
  view.addSelectField(A, SelectField("ManufacturerCode"));
  view.addSelectField(A, SelectField("DesignationEN"));
  view.addSelectField(A, SelectField("DesignationFR"));
  view.addSelectField(A, SelectField("DesignationDE"));
  view.addSelectField(A, SelectField("DesignationIT"));
  view.addSelectField(VTU, SelectField("VehicleType_Id_FK"));
  view.addSelectField(VTU, SelectField("Unit_Id_FK"));
  join = pvSchema.joinClause(VTU, VT);
  view.addJoinClause(join);
  join = pvSchema.joinClause(VTU, U);
  view.addJoinClause(join);
  join = pvSchema.joinClause(U, A);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void mdtTtDatabaseSchema::setupArticleComponentUsageView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table AC("ArticleComponent_tbl", "AC");
  Table A("Article_tbl", "A");
  JoinClause join;
  JoinKey key;

  view.setName("ArticleComponentUsage_view");
  view.setTable(AC);
  view.addSelectField(AC, SelectField("Component_Id_FK", "Component_Id_PK"));
  view.addSelectField(AC, SelectField("Composite_Id_FK", "Article_Id_PK"));
  view.addSelectField(A, SelectField("ArticleCode"));
  view.addSelectField(A, SelectField("DesignationEN"));
  view.addSelectField(A, SelectField("DesignationFR"));
  view.addSelectField(A, SelectField("DesignationDE"));
  view.addSelectField(A, SelectField("DesignationIT"));
  join.setMainTable(AC);
  join.setTableToJoin(A);
  key.setMainTableField("Composite_Id_FK");
  key.setTableToJoinField("Id_PK");
  join.addKey(key);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void mdtTtDatabaseSchema::setupArticleComponentView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table AC("ArticleComponent_tbl", "AC");
  Table A("Article_tbl", "A");
  JoinClause join;
  JoinKey key;

  view.setName("ArticleComponent_view");
  view.setTable(AC);
  view.addSelectField(AC, SelectField("Composite_Id_FK", "Article_Id_PK"));
  view.addSelectField(AC, SelectField("Component_Id_FK", "Component_Id_PK"));
  view.addSelectField(AC, SelectField("ComponentQty"));
  view.addSelectField(AC, SelectField("ComponentQtyUnit"));
  view.addSelectField(A, SelectField("ArticleCode"));
  view.addSelectField(A, SelectField("DesignationEN"));
  view.addSelectField(A, SelectField("DesignationFR"));
  view.addSelectField(A, SelectField("DesignationDE"));
  view.addSelectField(A, SelectField("DesignationIT"));
  // This is a special JOIN (reflexive), do it by hand
  join.setMainTable(AC);
  join.setTableToJoin(A);
  key.setMainTableField("Component_Id_FK");
  key.setTableToJoinField("Id_PK");
  join.addKey(key);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void mdtTtDatabaseSchema::setupArticleConnectorView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table ACNR("ArticleConnector_tbl", "ACNR");
  Table CNR("Connector_tbl", "CNR");
  JoinClause join;

  view.setName("ArticleConnector_view");
  view.setTable(ACNR);
  view.addSelectField(ACNR, SelectField("Id_PK"));
  view.addSelectField(ACNR, SelectField("Article_Id_FK"));
  view.addSelectField(ACNR, SelectField("Connector_Id_FK"));
  view.addSelectField(ACNR, SelectField("Name", "ArticleConnectorName"));
  view.addSelectField(CNR, SelectField("Gender"));
  view.addSelectField(CNR, SelectField("Form"));
  view.addSelectField(CNR, SelectField("Manufacturer"));
  view.addSelectField(CNR, SelectField("ManufacturerConfigCode"));
  view.addSelectField(CNR, SelectField("ManufacturerArticleCode"));
  join = pvSchema.joinClause(ACNR, CNR);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void mdtTtDatabaseSchema::setupArticleConnectionView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table ACNX("ArticleConnection_tbl", "ACNX");
  Table ACNR("ArticleConnector_tbl", "ACNR");
  Table CT("ConnectionType_tbl", "CT");
  JoinClause join;

  view.setName("ArticleConnection_view");
  view.setTable(ACNX);
  view.addSelectField(ACNR, SelectField("Connector_Id_FK"));
  view.addSelectField(ACNR, SelectField("Name", "ArticleConnectorName"));
  view.addSelectField(ACNX, SelectField("Id_PK"));
  view.addSelectField(ACNX, SelectField("Article_Id_FK"));
  view.addSelectField(ACNX, SelectField("ArticleConnector_Id_FK"));
  view.addSelectField(ACNX, SelectField("ConnectionType_Code_FK"));
  view.addSelectField(ACNX, SelectField("ArticleContactName"));
  view.addSelectField(CT, SelectField("NameEN", "ConnectionTypeEN"));
  view.addSelectField(CT, SelectField("NameFR", "ConnectionTypeFR"));
  view.addSelectField(CT, SelectField("NameDE", "ConnectionTypeDE"));
  view.addSelectField(CT, SelectField("NameIT", "ConnectionTypeIT"));
  view.addSelectField(ACNX, SelectField("Resistance"));
  view.addSelectField(ACNX, SelectField("IoType"));
  view.addSelectField(ACNX, SelectField("FunctionEN"));
  view.addSelectField(ACNX, SelectField("FunctionFR"));
  view.addSelectField(ACNX, SelectField("FunctionDE"));
  view.addSelectField(ACNX, SelectField("FunctionIT"));
  join = pvSchema.joinClause(ACNX, ACNR);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(ACNX, CT);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void mdtTtDatabaseSchema::setupArticleLinkView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table AL("ArticleLink_tbl", "AL");
  Table ACNXS("ArticleConnection_tbl", "ACNXS");
  Table ACNXE("ArticleConnection_tbl", "ACNXE");
  Table ACS("ArticleConnector_tbl", "ACS");
  Table ACE("ArticleConnector_tbl", "ACE");
  Table LT("LinkType_tbl", "LT");
  Table LD("LinkDirection_tbl", "LD");
  JoinClause join;
  JoinKey key;

  view.setName("ArticleLink_view");
  view.setTable(AL);
  view.addSelectField(AL, SelectField("LinkType_Code_FK"));
  view.addSelectField(AL, SelectField("LinkDirection_Code_FK"));
  view.addSelectField(AL, SelectField("ArticleConnectionStart_Id_FK"));
  view.addSelectField(AL, SelectField("ArticleConnectionEnd_Id_FK"));
  view.addSelectField(AL, SelectField("Identification"));
  view.addSelectField(LT, SelectField("NameEN", "LinkTypeNameEN"));
  view.addSelectField(LT, SelectField("NameFR", "LinkTypeNameFR"));
  view.addSelectField(LT, SelectField("NameDE", "LinkTypeNameDE"));
  view.addSelectField(LT, SelectField("NameIT", "LinkTypeNameIT"));
  view.addSelectField(AL, SelectField("Resistance"));
  view.addSelectField(LT, SelectField("ValueUnit"));
  view.addSelectField(ACNXS, SelectField("Article_Id_FK", "StartArticle_Id_FK"));
  view.addSelectField(ACS, SelectField("Name", "StartArticleConnectorName"));
  view.addSelectField(ACNXS, SelectField("ArticleContactName", "StartArticleContactName"));
  view.addSelectField(ACNXS, SelectField("IoType", "StartIoType"));
  view.addSelectField(ACNXS, SelectField("FunctionEN", "StartFunctionEN"));
  view.addSelectField(ACNXS, SelectField("FunctionFR", "StartFunctionFR"));
  view.addSelectField(ACNXS, SelectField("FunctionDE", "StartFunctionDE"));
  view.addSelectField(ACNXS, SelectField("FunctionIT", "StartFunctionIT"));
  view.addSelectField(LD, SelectField("PictureAscii", "LinkDirectionPictureAscii"));
  view.addSelectField(ACNXE, SelectField("Article_Id_FK", "EndArticle_Id_FK"));
  view.addSelectField(ACE, SelectField("Name", "EndArticleConnectorName"));
  view.addSelectField(ACNXE, SelectField("ArticleContactName", "EndArticleContactName"));
  view.addSelectField(ACNXE, SelectField("IoType", "EndIoType"));
  view.addSelectField(ACNXE, SelectField("FunctionEN", "EndFunctionEN"));
  view.addSelectField(ACNXE, SelectField("FunctionFR", "EndFunctionFR"));
  view.addSelectField(ACNXE, SelectField("FunctionDE", "EndFunctionDE"));
  view.addSelectField(ACNXE, SelectField("FunctionIT", "EndFunctionIT"));
  // Setup ArticleLink_tbl.ArticleConnectionStart_Id_FK -> ArticleConnection_tbl JOIN
  join.setMainTable(AL);
  join.setTableToJoin(ACNXS);
  key.setMainTableField("ArticleConnectionStart_Id_FK");
  key.setTableToJoinField("Id_PK");
  join.addKey(key);
  view.addJoinClause(join);
  // Setup ArticleLink_tbl.ArticleConnectionEnd_Id_FK -> ArticleConnection_tbl JOIN
  join.clear();
  join.setMainTable(AL);
  join.setTableToJoin(ACNXE);
  key.setMainTableField("ArticleConnectionEnd_Id_FK");
  key.setTableToJoinField("Id_PK");
  join.addKey(key);
  view.addJoinClause(join);
  // Other JOINs
  join = pvSchema.joinClause(ACNXS, ACS);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(ACNXE, ACE);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(AL, LT);
  view.addJoinClause(join);
  join = pvSchema.joinClause(AL, LD);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void mdtTtDatabaseSchema::setupUnitView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table U("Unit_tbl", "U");
  Table A("Article_tbl", "A");
  Table VTU("VehicleType_Unit_tbl", "VTU");
  Table VT("VehicleType_tbl", "VT");
  JoinClause join;

  view.setName("Unit_view");
  view.setTable(U);
  view.addSelectField(U, SelectField("Id_PK", "Unit_Id_PK"));
  view.addSelectField(U, SelectField("SchemaPosition"));
  view.addSelectField(U, SelectField("Alias"));
  view.addSelectField(U, SelectField("Coordinate"));
  view.addSelectField(U, SelectField("Cabinet"));
  view.addSelectField(VT, SelectField("Id_PK", "VehicleType_Id_PK"));
  view.addSelectField(VT, SelectField("Type"));
  view.addSelectField(VT, SelectField("SubType"));
  view.addSelectField(VT, SelectField("SeriesNumber"));
  view.addSelectField(A, SelectField("Id_PK", "Article_Id_PK"));
  view.addSelectField(A, SelectField("ArticleCode"));
  view.addSelectField(A, SelectField("DesignationEN"));
  view.addSelectField(A, SelectField("DesignationFR"));
  view.addSelectField(A, SelectField("DesignationDE"));
  view.addSelectField(A, SelectField("DesignationIT"));
  join = pvSchema.joinClause(U, A);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(U, VTU);
  view.addJoinClause(join);
  join = pvSchema.joinClause(VTU ,VT);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

// bool mdtTtDatabaseSchema::createUnitComponentView() 
// {
//   QString sql;
// 
//   sql = "CREATE VIEW UnitComponent_view AS\n"\
//         "SELECT\n"\
//         " UnitComposite.Id_PK AS Unit_Id_PK ,\n"\
//         " Unit_tbl.Id_PK AS UnitComponent_Id_PK ,\n"\
//         " Unit_tbl.Article_Id_FK ,\n"\
//         " Unit_tbl.Coordinate ,\n"\
//         " Unit_tbl.Cabinet ,\n"\
//         " Unit_tbl.SchemaPosition ,\n"\
//         " Unit_tbl.Alias ,\n"\
//         " Article_tbl.Id_PK AS Article_Id_PK ,\n"\
//         " Article_tbl.ArticleCode ,\n"\
//         " Article_tbl.DesignationEN,\n"\
//         " Article_tbl.DesignationFR,\n"\
//         " Article_tbl.DesignationDE,\n"\
//         " Article_tbl.DesignationIT\n"\
//         "FROM Unit_tbl AS UnitComposite\n"\
//         " JOIN Unit_tbl\n"\
//         "  ON Unit_tbl.Composite_Id_FK = UnitComposite.Id_PK\n"\
//         " LEFT JOIN Article_tbl\n"\
//         "  ON Unit_tbl.Article_Id_FK = Article_tbl.Id_PK";
// 
//   return createView("UnitComponent_view", sql);
// }

void mdtTtDatabaseSchema::setupUnitComponentView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table UnitComposite("Unit_tbl", "UnitComposite");
  Table U("Unit_tbl", "U");
  Table A("Article_tbl", "A");
  JoinClause join;
  JoinKey key;

  view.setName("UnitComponent_view");
  view.setTable(UnitComposite);
  view.addSelectField(UnitComposite, SelectField("Id_PK", "Unit_Id_PK"));
  view.addSelectField(U, SelectField("Id_PK", "UnitComponent_Id_PK"));
  view.addSelectField(U, SelectField("Article_Id_FK"));
  view.addSelectField(U, SelectField("Coordinate"));
  view.addSelectField(U, SelectField("Cabinet"));
  view.addSelectField(U, SelectField("SchemaPosition"));
  view.addSelectField(U, SelectField("Alias"));
  view.addSelectField(A, SelectField("Id_PK", "Article_Id_PK"));
  view.addSelectField(A, SelectField("ArticleCode"));
  view.addSelectField(A, SelectField("DesignationEN"));
  view.addSelectField(A, SelectField("DesignationFR"));
  view.addSelectField(A, SelectField("DesignationDE"));
  view.addSelectField(A, SelectField("DesignationIT"));
  // Unit composite - component JOIN
  join.setMainTable(UnitComposite);
  join.setTableToJoin(U);
  key.setMainTableField("Id_PK");
  key.setTableToJoinField("Composite_Id_FK");
  join.addKey(key);
  view.addJoinClause(join);
  // Other JOINs
  join = pvSchema.joinClause(U, A);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

bool mdtTtDatabaseSchema::createUnitConnectorView()
{
  QString sql;

  sql = "CREATE VIEW UnitConnector_view AS\n"\
        "SELECT\n"\
        " UnitConnector_tbl.Id_PK,\n"\
        " UnitConnector_tbl.Unit_Id_FK,\n"\
        " UnitConnector_tbl.Connector_Id_FK,\n"\
        " UnitConnector_tbl.ArticleConnector_Id_FK,\n"\
        " UnitConnector_tbl.Name AS UnitConnectorName,\n"\
        " ArticleConnector_tbl.Name AS ArticleConnectorName,\n"\
        " Connector_tbl.Gender,\n"\
        " Connector_tbl.Form,\n"\
        " Connector_tbl.Manufacturer,\n"\
        " Connector_tbl.Housing,\n"\
        " Connector_tbl.`Insert`,\n"\
        " Connector_tbl.InsertRotation,\n"\
        " Connector_tbl.ManufacturerConfigCode,\n"\
        " Connector_tbl.ManufacturerArticleCode\n"\
        "FROM UnitConnector_tbl\n"\
        " LEFT JOIN Connector_tbl\n"\
        "  ON Connector_tbl.Id_PK = UnitConnector_tbl.Connector_Id_FK\n"\
        " LEFT JOIN ArticleConnector_tbl\n"\
        "  ON ArticleConnector_tbl.Id_PK = UnitConnector_tbl.ArticleConnector_Id_FK";

  return createView("UnitConnector_view", sql);
}

void mdtTtDatabaseSchema::setupUnitConnectorView()
{

}

bool mdtTtDatabaseSchema::createUnitConnectorUsageView()
{
  QString sql;

  sql = "CREATE VIEW UnitConnectorUsage_view AS\n"\
        "SELECT\n"\
        " UC.Id_PK,\n"\
        " UC.Unit_Id_FK,\n"\
        " UC.Connector_Id_FK,\n"\
        " UC.ArticleConnector_Id_FK,\n"\
        " UC.Name AS UnitConnectorName,\n"\
        " U.SchemaPosition,\n"\
        " U.Alias,\n"\
        " VT.Type,\n"\
        " VT.SubType,\n"\
        " VT.SeriesNumber\n"\
        "FROM UnitConnector_tbl UC\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = UC.Unit_Id_FK\n"\
        " JOIN VehicleType_Unit_tbl VTU\n"\
        "  ON VTU.Unit_Id_FK = U.Id_PK\n"\
        " JOIN VehicleType_tbl VT\n"\
        "  ON VT.Id_PK = VTU.VehicleType_Id_FK";

  return createView("UnitConnectorUsage_view", sql);
}


bool mdtTtDatabaseSchema::createUnitConnectionView() 
{
  QString sql;

  sql = "CREATE VIEW UnitConnection_view AS\n"\
        "SELECT\n"\
        " UCNR.Connector_Id_FK AS UCNR_Connector_Id_FK ,\n"\
        " UCNR.Unit_Id_FK AS UCNR_Unit_Id_FK ,\n"\
        " UCNR.Name AS UnitConnectorName ,\n"\
        " UCNX.Id_PK,\n"\
        " UCNX.Unit_Id_FK ,\n"\
        " UCNX.UnitConnector_Id_FK ,\n"\
        " UCNX.ConnectionType_Code_FK ,\n"\
        " UCNX.ArticleConnection_Id_FK,\n"\
        " UCNX.UnitContactName ,\n"\
        " CT.NameEN AS ConnectionTypeEN,\n"\
        " CT.NameFR AS ConnectionTypeFR,\n"\
        " CT.NameDE AS ConnectionTypeDE,\n"\
        " CT.NameIT AS ConnectionTypeIT,\n"\
        " UCNX.Resistance AS UnitConnectionResistance,\n"\
        " UCNX.SchemaPage ,\n"\
        " UCNX.FunctionEN AS UnitFunctionEN,\n"\
        " UCNX.FunctionFR AS UnitFunctionFR,\n"\
        " UCNX.FunctionDE AS UnitFunctionDE,\n"\
        " UCNX.FunctionIT AS UnitFunctionIT,\n"\
        " UCNX.SignalName ,\n"\
        " UCNX.SwAddress ,\n"\
        " ACNR.Connector_Id_FK AS ACNR_Connector_Id_FK ,\n"\
        " ACNR.Article_Id_FK AS ACNR_Article_Id_FK ,\n"\
        " ACNR.Name AS ArticleConnectorName ,\n"\
        " ACNX.ArticleConnector_Id_FK ,\n"\
        " ACNX.Article_Id_FK ,\n"\
        " ACNX.ConnectionType_Code_FK AS ACNX_ConnectionType_Code_FK,\n"\
        " ACNX.ArticleContactName ,\n"\
        " ACNX.IoType ,\n"\
        " ACNX.FunctionEN AS ArticleFunctionEN,\n"\
        " ACNX.FunctionFR AS ArticleFunctionFR,\n"\
        " ACNX.FunctionDE AS ArticleFunctionDE,\n"\
        " ACNX.FunctionIT AS ArticleFunctionIT\n"\
        "FROM UnitConnection_tbl UCNX\n"\
        " LEFT JOIN UnitConnector_tbl UCNR\n"\
        "  ON UCNR.Id_PK = UCNX.UnitConnector_Id_FK\n"\
        " LEFT JOIN ArticleConnector_tbl ACNR\n"\
        "  ON UCNR.ArticleConnector_Id_FK = ACNR.Id_PK\n"\
        " LEFT JOIN ArticleConnection_tbl ACNX\n"\
        "  ON UCNX.ArticleConnection_Id_FK = ACNX.Id_PK\n"\
        " JOIN ConnectionType_tbl CT\n"\
        "  ON CT.Code_PK = UCNX.ConnectionType_Code_FK";

  return createView("UnitConnection_view", sql);
}

bool mdtTtDatabaseSchema::createArticleLinkUnitConnectionView()
{
  QString sql;

  sql = "CREATE VIEW ArticleLink_UnitConnection_view AS\n"\
        "SELECT\n"\
        " UCNXS.Id_PK AS UnitConnectionStart_Id_FK,\n"\
        " UCNXE.Id_PK AS UnitConnectionEnd_Id_FK,\n"\
        " UCNXS.Unit_Id_FK AS StartUnit_Id_FK,\n"\
        " UCNXE.Unit_Id_FK AS EndUnit_Id_FK,\n"\
        " AL.ArticleConnectionStart_Id_FK,\n"\
        " AL.ArticleConnectionEnd_Id_FK,\n"\
        " AL.LinkType_Code_FK,\n"\
        " AL.LinkDirection_Code_FK,\n"\
        " AL.Identification,\n"\
        " AL.Resistance,\n"\
        " AL.SinceVersion,\n"\
        " AL.Modification\n"\
        "FROM ArticleLink_tbl AL\n"\
        " JOIN UnitConnection_tbl UCNXS\n"\
        "  ON UCNXS.ArticleConnection_Id_FK = AL.ArticleConnectionStart_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXE\n"\
        "  ON UCNXE.ArticleConnection_Id_FK = AL.ArticleConnectionEnd_Id_FK\n";

  return createView("ArticleLink_UnitConnection_view", sql);
}

bool mdtTtDatabaseSchema::createUnitLinkView()
{
  QString sql, selectSql;

  selectSql = "SELECT\n"\
              " LV.Version,\n"\
              " M.NameEN AS ModificationEN,\n"\
              " M.NameFR AS ModificationFR,\n"\
              " M.NameDE AS ModificationDE,\n"\
              " M.NameIT AS ModificationIT,\n"\
              " M.SortOrder AS ModificationSortOrder,\n"\
              " LNK.Identification ,\n"\
              " LNK.LinkBeam_Id_FK ,\n"\
              " US.SchemaPosition AS StartSchemaPosition ,\n"\
              " US.Alias AS StartAlias,\n"\
              " UCS.Name AS StartUnitConnectorName ,\n"\
              " UCNXS.UnitContactName AS StartUnitContactName ,\n"\
              " UCNXS.Resistance AS StartUnitConnectionResistance ,\n"\
              " UE.SchemaPosition AS EndSchemaPosition ,\n"\
              " UE.Alias AS EndAlias,\n"\
              " UCE.Name AS EndUnitConnectorName ,\n"\
              " UCNXE.UnitContactName AS EndUnitContactName ,\n"\
              " UCNXE.Resistance AS EndUnitConnectionResistance ,\n"\
              " LinkType_tbl.NameEN AS LinkTypeNameEN ,\n"\
              " LNK.Length ,\n"\
              " LNK.Resistance ,\n"\
              " W.Model AS WireModel,\n"\
              " W.Section,\n"\
              " W.ColorEN,\n"\
              " W.ArticleCode AS WireArticleCode,\n"\
              " LinkType_tbl.ValueUnit ,\n"\
              " LinkDirection_tbl.PictureAscii AS LinkDirectionPictureAscii ,\n"\
              " UCNXS.SchemaPage AS StartSchemaPage ,\n"\
              " UCNXS.FunctionEN AS StartFunctionEN ,\n"\
              " UCNXS.SignalName AS StartSignalName ,\n"\
              " UCNXS.SwAddress AS StartSwAddress ,\n"\
              " UCNXE.SchemaPage AS EndSchemaPage ,\n"\
              " UCNXE.FunctionEN AS EndFunctionEN ,\n"\
              " UCNXE.SignalName AS EndSignalName ,\n"\
              " UCNXE.SwAddress AS EndSwAddress ,\n"\
              " LNK.UnitConnectionStart_Id_FK ,\n"\
              " LNK.UnitConnectionEnd_Id_FK ,\n"\
              " UCNXS.UnitConnector_Id_FK AS StartUnitConnector_Id_FK ,\n"\
              " UCNXS.Unit_Id_FK AS StartUnit_Id_FK ,\n"\
              " UCNXE.UnitConnector_Id_FK AS EndUnitConnector_Id_FK ,\n"\
              " UCNXE.Unit_Id_FK AS EndUnit_Id_FK ,\n"\
              " LNK.LinkType_Code_FK ,\n"\
              " LNK.LinkDirection_Code_FK ,\n"\
              " LNK.ArticleConnectionStart_Id_FK ,\n"\
              " LNK.ArticleConnectionEnd_Id_FK,\n"\
              " LNK.Version_FK,\n"\
              " LNK.Modification_Code_FK\n";
  sql = "CREATE VIEW UnitLink_view AS\n";
  sql += selectSql;
  sql += "FROM Link_tbl LNK\n"\
         " LEFT JOIN LinkVersion_tbl LV\n"\
         "  ON LV.Version_PK = LNK.Version_FK\n"\
         " LEFT JOIN Modification_tbl M\n"\
         "  ON M.Code_PK = LNK.Modification_Code_FK\n"\
         " LEFT JOIN Wire_tbl W\n"\
         "  ON W.Id_PK = LNK.Wire_Id_FK\n"\
         " JOIN UnitConnection_tbl UCNXS\n"\
         "  ON UCNXS.Id_PK = LNK.UnitConnectionStart_Id_FK\n"\
         " JOIN UnitConnection_tbl UCNXE\n"\
         "  ON UCNXE.Id_PK = LNK.UnitConnectionEnd_Id_FK\n"\
          " LEFT JOIN UnitConnector_tbl UCS\n"\
          "  ON UCNXS.UnitConnector_Id_FK = UCS.Id_PK\n"\
          " LEFT JOIN UnitConnector_tbl UCE\n"\
          "  ON UCNXE.UnitConnector_Id_FK = UCE.Id_PK\n"\
          " JOIN Unit_tbl US\n"\
          "  ON US.Id_PK = UCNXS.Unit_Id_FK\n"\
          " JOIN Unit_tbl UE\n"\
          "  ON UE.Id_PK = UCNXE.Unit_Id_FK\n"\
          /**
          " LEFT JOIN VehicleType_Link_tbl\n"\
          "  ON LNK.UnitConnectionStart_Id_FK = VehicleType_Link_tbl.UnitConnectionStart_Id_FK\n"\
          "  AND LNK.UnitConnectionEnd_Id_FK = VehicleType_Link_tbl.UnitConnectionEnd_Id_FK\n"\
          " LEFT JOIN VehicleType_tbl VS\n"\
          "  ON VS.Id_PK = VehicleType_Link_tbl.VehicleTypeStart_Id_FK\n"\
          " LEFT JOIN VehicleType_tbl VE\n"\
          "  ON VE.Id_PK = VehicleType_Link_tbl.VehicleTypeEnd_Id_FK"\
          */
          " JOIN LinkType_tbl\n"\
          "  ON LinkType_tbl.Code_PK = LNK.LinkType_Code_FK\n"\
          " JOIN LinkDirection_tbl\n"\
          "  ON LinkDirection_tbl.Code_PK = LNK.LinkDirection_Code_FK\n";

  return createView("UnitLink_view", sql);
}

bool mdtTtDatabaseSchema::createUnitVehicleTypeView() 
{
  QString sql;

  sql = "CREATE VIEW Unit_VehicleType_view AS\n"\
        "SELECT\n"\
        " VehicleType_tbl.Type ,\n"\
        " VehicleType_tbl.SubType ,\n"\
        " VehicleType_tbl.SeriesNumber ,\n"\
        " VehicleType_Unit_tbl.VehicleType_Id_FK ,\n"\
        " VehicleType_Unit_tbl.Unit_Id_FK\n"\
        "FROM VehicleType_tbl\n"\
        " JOIN VehicleType_Unit_tbl\n"\
        "  ON VehicleType_tbl.Id_PK = VehicleType_Unit_tbl.VehicleType_Id_FK";

  return createView("Unit_VehicleType_view", sql);
}

bool mdtTtDatabaseSchema::createLinkListView()
{
  QString sql, selectSql;

  selectSql = "SELECT\n"\
              " LNK.UnitConnectionStart_Id_FK ,\n"\
              " LNK.UnitConnectionEnd_Id_FK ,\n"\
              " LNK.LinkBeam_Id_FK ,\n"\
              " VS.Id_PK AS StartVehicleType_Id_PK,\n"\
              " VS.Type AS StartVehicleType ,\n"\
              " VS.SubType AS StartVehicleSubType ,\n"\
              " VS.SeriesNumber AS StartVehicleSerie,\n"\
              " LNK.Identification ,\n"\
              " LNK.LinkDirection_Code_FK ,\n"\
              " LNK.LinkType_Code_FK ,\n"\
              " US.Id_PK AS StartUnit_Id_PK ,\n"\
              " US.SchemaPosition AS StartSchemaPosition,\n"\
              " US.Alias AS StartAlias,\n"\
              " US.Cabinet AS StartCabinet,\n"\
              " US.Coordinate AS StartCoordinate ,\n"\
              " UCS.Id_PK AS UnitConnectorStart_Id_FK,\n"\
              " UCS.Name AS StartUnitConnectorName ,\n"\
              " UCNXS.UnitContactName AS StartUnitContactName ,\n"\
              " UCNXS.ConnectionType_Code_FK AS StartConnectionType_Code_FK,\n"\
              " UCNXS.FunctionEN AS StartFunctionEN,\n"\
              " UCNXS.FunctionFR AS StartFunctionFR,\n"\
              " UCNXS.FunctionDE AS StartFunctionDE,\n"\
              " UCNXS.FunctionIT AS StartFunctionIT,\n"\
              " VE.Id_PK AS EndVehicleType_Id_PK,\n"\
              " VE.Type AS EndVehicleType ,\n"\
              " VE.SubType AS EndVehicleSubType ,\n"\
              " VE.SeriesNumber AS EndVehicleSerie,\n"\
              " UE.Id_PK AS EndUnit_Id_PK ,\n"\
              " UE.SchemaPosition AS EndSchemaPosition,\n"\
              " UE.Alias AS EndAlias,\n"\
              " UE.Cabinet AS EndCabinet,\n"\
              " UE.Coordinate AS EndCoordinate ,\n"\
              " UCE.Id_PK AS UnitConnectorEnd_Id_FK,\n"\
              " UCE.Name AS EndUnitConnectorName ,\n"\
              " UCNXE.UnitContactName AS EndUnitContactName,\n"\
              " UCNXE.ConnectionType_Code_FK AS EndConnectionType_Code_FK,\n"\
              " W.Model AS WireModel,\n"\
              " LNK.Length,\n"\
              " W.Section,\n"\
              " LNK.Resistance,\n"\
              " W.ColorEN AS WireColorEN,\n"\
              " UCNXE.FunctionEN AS EndFunctionEN,\n"\
              " UCNXE.FunctionFR AS EndFunctionFR,\n"\
              " UCNXE.FunctionDE AS EndFunctionDE,\n"\
              " UCNXE.FunctionIT AS EndFunctionIT\n";
  sql = "CREATE VIEW LinkList_view AS\n";
  sql += selectSql;
  sql += "FROM Link_tbl LNK\n"\
         " JOIN UnitConnection_tbl UCNXS\n"\
         "  ON UCNXS.Id_PK = LNK.UnitConnectionStart_Id_FK\n"\
         " JOIN UnitConnection_tbl UCNXE\n"\
         "  ON UCNXE.Id_PK = LNK.UnitConnectionEnd_Id_FK\n"\
          " LEFT JOIN UnitConnector_tbl UCS\n"\
          "  ON UCNXS.UnitConnector_Id_FK = UCS.Id_PK\n"\
          " LEFT JOIN UnitConnector_tbl UCE\n"\
          "  ON UCNXE.UnitConnector_Id_FK = UCE.Id_PK\n"\
          " JOIN Unit_tbl US\n"\
          "  ON US.Id_PK = UCNXS.Unit_Id_FK\n"\
          " JOIN Unit_tbl UE\n"\
          "  ON UE.Id_PK = UCNXE.Unit_Id_FK\n"\
          " LEFT JOIN VehicleType_Link_tbl\n"\
          "  ON LNK.UnitConnectionStart_Id_FK = VehicleType_Link_tbl.UnitConnectionStart_Id_FK\n"\
          "  AND LNK.UnitConnectionEnd_Id_FK = VehicleType_Link_tbl.UnitConnectionEnd_Id_FK\n"\
          " LEFT JOIN VehicleType_tbl VS\n"\
          "  ON VS.Id_PK = VehicleType_Link_tbl.VehicleTypeStart_Id_FK\n"\
          " LEFT JOIN VehicleType_tbl VE\n"\
          "  ON VE.Id_PK = VehicleType_Link_tbl.VehicleTypeEnd_Id_FK"\
          " JOIN LinkType_tbl\n"\
          "  ON LinkType_tbl.Code_PK = LNK.LinkType_Code_FK\n"\
          " JOIN LinkDirection_tbl\n"\
          "  ON LinkDirection_tbl.Code_PK = LNK.LinkDirection_Code_FK\n"\
          " LEFT JOIN Wire_tbl W\n"\
          "  ON W.Id_PK = LNK.Wire_Id_FK";

  return createView("LinkList_view", sql);
}

bool mdtTtDatabaseSchema::createLinkBeamUnitStartView()
{
  QString sql, selectSql;

  selectSql = "SELECT\n"\
              " LB.Unit_Id_FK,\n"\
              " LB.LinkBeam_Id_FK,\n"\
              " U.*";
  sql = "CREATE VIEW LinkBeam_UnitStart_view AS\n";
  sql += selectSql;
  sql += "FROM LinkBeam_UnitStart_tbl LB\n"\
         " JOIN Unit_tbl U\n"\
         "  ON U.Id_PK = LB.Unit_Id_FK";

  return createView("LinkBeam_UnitStart_view", sql);
}

bool mdtTtDatabaseSchema::createLinkBeamUnitEndView()
{
  QString sql, selectSql;

  selectSql = "SELECT\n"\
              " LB.Unit_Id_FK,\n"\
              " LB.LinkBeam_Id_FK,\n"\
              " U.*";
  sql = "CREATE VIEW LinkBeam_UnitEnd_view AS\n";
  sql += selectSql;
  sql += "FROM LinkBeam_UnitEnd_tbl LB\n"\
         " JOIN Unit_tbl U\n"\
         "  ON U.Id_PK = LB.Unit_Id_FK";

  return createView("LinkBeam_UnitEnd_view", sql);
}


bool mdtTtDatabaseSchema::createTestSystemComponentView()
{
  QString sql;

  sql = "CREATE VIEW TestSystemComponent_view AS\n"\
        "SELECT\n"\
        " TS_TSC.*,\n"\
        " TSC.*\n"\
        "FROM TestSystem_TestSystemComponent_tbl TS_TSC\n"\
        " JOIN TestSystemComponent_tbl TSC\n"\
        "  ON TSC.Id_PK = TS_TSC.TestSystemComponent_Id_FK\n";

  return createView("TestSystemComponent_view", sql);
}

bool mdtTtDatabaseSchema::createTestSytemOfComponentView()
{
  QString sql;

  sql = "CREATE VIEW TestSystemOfComponent_view AS\n"\
        "SELECT\n"\
        " TS_TSC.*,\n"\
        " TS.*\n"\
        "FROM TestSystem_TestSystemComponent_tbl TS_TSC\n"\
        " JOIN TestSystem_tbl TS\n"\
        "  ON TS.Id_PK = TS_TSC.TestSystem_Id_FK\n";

  return createView("TestSystemOfComponent_view", sql);
}

bool mdtTtDatabaseSchema::createTestSystemUnitView()
{
  QString sql;

  sql = "CREATE VIEW TestSystemUnit_view AS\n"\
        "SELECT\n"\
        " TSU.Unit_Id_FK_PK,\n"\
        " TSU.TestSystemComponent_Id_FK,\n"\
        " TSU.Type_Code_FK,\n"\
        " U.Article_Id_FK,\n"\
        " U.SchemaPosition,\n"\
        " TSU.IoPosition,\n"\
        " TSU.CalibrationDate,\n"\
        " TSUT.NameEN AS TsuTypeEN,\n"\
        " TSUT.NameFR AS TsuTypeFR,\n"\
        " TSUT.NameDE AS TsuTypeDE,\n"\
        " TSUT.NameIT AS TsuTypeIT\n"\
        "FROM TestSystemUnit_tbl TSU\n"\
        " JOIN TestSystemUnitType_tbl TSUT\n"\
        "  ON TSUT.Code_PK = TSU.Type_Code_FK\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TSU.Unit_Id_FK_PK";

  return createView("TestSystemUnit_view", sql);
}



bool mdtTtDatabaseSchema::createTestNodeView()
{
  QString sql;

  sql = "CREATE VIEW TestNode_view AS\n"\
        "SELECT\n"\
        " TN.*,\n"\
        " VT.Type,\n"\
        " VT.SubType,\n"\
        " VT.SeriesNumber\n"\
        "FROM TestNode_tbl TN\n"\
        " JOIN VehicleType_tbl VT\n"\
        "  ON VT.Id_PK = TN.VehicleType_Id_FK_PK\n";

  return createView("TestNode_view", sql);
}

bool mdtTtDatabaseSchema::createUnitTestNodeView()
{
  QString sql;

  sql = "CREATE VIEW Unit_TestNode_view AS\n"\
        "SELECT\n"\
        " TN.*,\n"\
        " VT.Type ,\n"\
        " VT.SubType ,\n"\
        " VT.SeriesNumber ,\n"\
        " VTU.Unit_Id_FK\n"\
        "FROM TestNode_tbl TN\n"\
        " JOIN VehicleType_tbl VT\n"\
        "  ON VT.Id_PK = TN.VehicleType_Id_FK_PK\n"\
        " JOIN VehicleType_Unit_tbl VTU\n"\
        "  ON VTU.VehicleType_Id_FK = VT.Id_PK";

  return createView("Unit_TestNode_view", sql);

}

bool mdtTtDatabaseSchema::createTestNodeUnitView()
{
  QString sql;

  sql = "CREATE VIEW TestNodeUnit_view AS\n"\
        "SELECT\n"\
        "TNU.Unit_Id_FK_PK,\n"\
        "TNU.TestNode_Id_FK,\n"\
        "TNU.Type_Code_FK,\n"\
        "U.SchemaPosition,\n"\
        "TNU.IoPosition,\n"\
        "TNU.CalibrationOffset,\n"\
        "TNU.CalibrationDate,\n"\
        "TNUT.NameEN,\n"\
        "TNUT.NameFR,\n"\
        "TNUT.NameDE,\n"\
        "TNUT.NameIT,\n"\
        "A.Manufacturer,\n"\
        "A.ManufacturerCode,\n"\
        "A.ArticleCode,\n"\
        "A.DesignationEN,\n"\
        "A.DesignationFR,\n"\
        "A.DesignationDE,\n"\
        "A.DesignationIT\n"\
        "FROM TestNodeUnit_tbl TNU\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNodeUnitType_tbl TNUT\n"\
        "  ON TNUT.Code_PK = TNU.Type_Code_FK\n"\
        " LEFT JOIN Article_tbl A\n"\
        "  ON A.Id_PK = U.Article_Id_FK";

  return createView("TestNodeUnit_view", sql);
}

bool mdtTtDatabaseSchema::createTestNodeUnitConnectionView()
{
  QString sql;

  sql = "CREATE VIEW TestNodeUnitConnection_view AS\n"\
        "SELECT\n"\
        "U.SchemaPosition,\n"\
        "U.Alias,\n"\
        "TNU.Unit_Id_FK_PK,\n"\
        "TNU.TestNode_Id_FK,\n"\
        "TNU.IoPosition,\n"\
        "UCNR.Name AS UnitConnectorName,\n"\
        "TNUCNX.UnitConnection_Id_FK_PK,\n"\
        "TNUCNX.TestNodeUnit_Id_FK,\n"\
        /* "TNUCNX.TestNodeBus_Id_FK,\n"\ */
        "UCNX.UnitContactName,\n"\
        "UCNX.Resistance AS UnitConnectionResistance\n"\
        /* "TNB.NameEN AS Bus\n"\ */
        "FROM TestNodeUnitConnection_tbl TNUCNX\n"\
        " JOIN UnitConnection_tbl UCNX\n"\
        "  ON UCNX.Id_PK = TNUCNX.UnitConnection_Id_FK_PK\n"\
        " LEFT JOIN UnitConnector_tbl UCNR\n"\
        "  ON UCNR.Id_PK = UCNX.UnitConnector_Id_FK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUCNX.TestNodeUnit_Id_FK\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n";
        /* " LEFT JOIN TestNodeBus_tbl TNB\n"\
        "  ON TNB.Id_PK = TNUCNX.TestNodeBus_Id_FK\n"; */

  return createView("TestNodeUnitConnection_view", sql);
}

/**
bool mdtTtDatabaseSchema::createTestLinkView()
{
  QString sql;

  sql = "CREATE VIEW TestLink_view AS\n"\
        "SELECT\n"\
        " LNK.Id_PK,\n"\
        " LNK.LogicalTestCable_Id_FK,\n"\
        " LNK.TestConnection_Id_FK,\n"\
        " LNK.DutConnection_Id_FK,\n"\
        " LNK.Identification AS TestLinkIdentification,\n"\
        " TNU.Unit_Id_FK_PK,\n"\
        " TNU.TestNode_Id_FK,\n"\
        " UD.Id_PK AS DutUnitId,\n"\
        " UD.SchemaPosition AS DutUnitSchemaPosition,\n"\
        " UD.Alias AS DutUnitAlias,\n"\
        " UCD.Name AS DutConnectorName,\n"\
        " UCNXD.UnitContactName AS DutContactName,\n"\
        " UTNU.SchemaPosition AS TestNodeUnitSchemaPosition,\n"\
        " UCT.Name AS TestConnectorName,\n"\
        " UCNXT.UnitContactName AS TestContactName,\n"\
        " TNB.NameEN AS Bus,\n"\
        " TNU.IoPosition,\n"\
        " VTN.Type AS TestNodeType,\n"\
        " VTN.SubType AS TestNodeSubType,\n"\
        " VTN.SeriesNumber AS TestNodeSeriesNumber,\n"\
        " TN.VehicleType_Id_FK_PK,\n"\
        " TN.NodeIdentification,\n"\
        " UVT.Type,\n"\
        " UVT.SubType,\n"\
        " UVT.SeriesNumber\n"\
        "FROM TestLink_tbl LNK\n"\
        " JOIN TestNodeUnitConnection_tbl TNUCNX\n"\
        "  ON TNUCNX.UnitConnection_Id_FK_PK = LNK.TestConnection_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXT\n"\
        "  ON UCNXT.Id_PK = LNK.TestConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCT\n"\
        "  ON UCT.Id_PK = UCNXT.UnitConnector_Id_FK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUCNX.TestNodeUnit_Id_FK\n"\
        " JOIN Unit_tbl UTNU\n"\
        "  ON UTNU.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n"\
        " JOIN VehicleType_tbl VTN\n"\
        "  ON VTN.Id_PK = TN.VehicleType_Id_FK_PK\n"\
        " LEFT JOIN TestNodeBus_tbl TNB\n"\
        "  ON TNB.Id_PK = TNUCNX.TestNodeBus_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXD\n"\
        "  ON UCNXD.Id_PK = LNK.DutConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCD\n"\
        "  ON UCD.Id_PK = UCNXD.UnitConnector_Id_FK\n"\
        " JOIN Unit_tbl UD\n"\
        "  ON UD.Id_PK = UCNXD.Unit_Id_FK\n"\
        " JOIN VehicleType_Unit_tbl UVTU\n"\
        "  ON UVTU.Unit_Id_FK = UD.Id_PK\n"\
        " JOIN VehicleType_tbl UVT\n"\
        "  ON UVT.Id_PK = UVTU.VehicleType_Id_FK\n";

  return createView("TestLink_view", sql);
}
*/

bool mdtTtDatabaseSchema::createTestLinkView()
{
  QString sql;

  sql = "CREATE VIEW TestLink_view AS\n"\
        "SELECT\n"\
        " TL.*,\n"\
        " LTC.Key AS LogicalTestCableKey,\n"\
        " DUTU.SchemaPosition AS DutSchemaPosition,\n"\
        " DUTU.Alias AS DutAlias,\n"\
        " DUTUCNR.Name AS DutConnectorName,\n"\
        " DUTUCNX.UnitConnector_Id_FK AS DutUnitConnector_Id_FK,\n"\
        " DUTUCNX.Unit_Id_FK AS DutUnit_Id_FK,\n"\
        " DUTUCNX.UnitContactName AS DutContactName,\n"\
        " DUTUCNX.Resistance AS DutConnectionResistance,\n"\
        " TCDUCNR.Name AS TestCableDutSideConnectorName,\n"\
        " TCDUCNX.UnitContactName AS TestCableDutSideContactName,\n"\
        " TCDUCNX.Resistance AS TestCableDutSideConnectionResistance,\n"\
        " TCL.Identification AS TestCableLinkIdentification,\n"\
        " TCL.Resistance AS TestCableLinkResistance,\n"\
        " TCTUCNR.Name AS TestCableTestSideConnectorName,\n"\
        " TCTUCNX.UnitContactName AS TestCableTestSideContactName,\n"\
        " TCTUCNX.Resistance AS TestCableTestSideConnectionResistance,\n"\
        " TNUCNR.Name AS TestNodeConnectorName,\n"\
        " TNUCNX.UnitConnector_Id_FK AS TestNodeUnitConnector_Id_FK,\n"\
        " TNUCNX.UnitContactName AS TestNodeContactName,\n"\
        " TNUCNX.Resistance AS TestNodeConnectionResistance\n"\
        /**
        " LNK.Id_PK,\n"\
        " LNK.LogicalTestCable_Id_FK,\n"\
        " LNK.TestConnection_Id_FK,\n"\
        " LNK.DutConnection_Id_FK,\n"\
        " LNK.Identification AS TestLinkIdentification,\n"\
        " TNU.Unit_Id_FK_PK,\n"\
        " TNU.TestNode_Id_FK,\n"\
        " UD.Id_PK AS DutUnitId,\n"\
        " UD.SchemaPosition AS DutUnitSchemaPosition,\n"\
        " UD.Alias AS DutUnitAlias,\n"\
        " UCD.Name AS DutConnectorName,\n"\
        " UCNXD.UnitContactName AS DutContactName,\n"\
        " UTNU.SchemaPosition AS TestNodeUnitSchemaPosition,\n"\
        " UCT.Name AS TestConnectorName,\n"\
        " UCNXT.UnitContactName AS TestContactName,\n"\
        " TNB.NameEN AS Bus,\n"\
        " TNU.IoPosition,\n"\
        " VTN.Type AS TestNodeType,\n"\
        " VTN.SubType AS TestNodeSubType,\n"\
        " VTN.SeriesNumber AS TestNodeSeriesNumber,\n"\
        " TN.VehicleType_Id_FK_PK,\n"\
        " TN.NodeIdentification,\n"\
        " UVT.Type,\n"\
        " UVT.SubType,\n"\
        " UVT.SeriesNumber\n"\
        */
        "FROM TestLink_tbl TL\n"\
        " JOIN Link_tbl TCL\n"\
        "  ON TCL.UnitConnectionStart_Id_FK = TL.TestCableUnitConnectionStart_Id_FK\n"\
        "  AND TCL.UnitConnectionEnd_Id_FK = TL.TestCableUnitConnectionEnd_Id_FK\n"\
        " JOIN UnitConnection_tbl TCDUCNX\n"\
        "  ON TCDUCNX.Id_PK = TCL.UnitConnectionStart_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl TCDUCNR\n"\
        "  ON TCDUCNR.Id_PK = TCDUCNX.UnitConnector_Id_FK\n"\
        " JOIN UnitConnection_tbl TCTUCNX\n"\
        "  ON TCTUCNX.Id_PK = TCL.UnitConnectionEnd_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl TCTUCNR\n"\
        "  ON TCTUCNR.Id_PK = TCTUCNX.UnitConnector_Id_FK\n"\
        /**
        " JOIN 
        " JOIN Unit_tbl TCU\n"\
        "  ON TCU.Id_PK = TCDUCNX.Unit_Id_FK\n"\
        "  AND TCU.Id_PK = TCTUCNX.Unit_Id_FK\n"\
        */
        " JOIN UnitConnection_tbl DUTUCNX\n"\
        "  ON DUTUCNX.Id_PK = TL.DutConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl DUTUCNR\n"\
        "  ON DUTUCNR.Id_PK = DUTUCNX.UnitConnector_Id_FK\n"\
        " JOIN Unit_tbl DUTU\n"\
        "  ON DUTU.Id_PK = DUTUCNX.Unit_Id_FK\n"\
        " JOIN TestNodeUnitConnection_tbl\n"\
        "  ON TestNodeUnitConnection_tbl.UnitConnection_Id_FK_PK = TL.TestConnection_Id_FK\n"\
        " JOIN UnitConnection_tbl TNUCNX\n"\
        "  ON TNUCNX.Id_PK = TestNodeUnitConnection_tbl.UnitConnection_Id_FK_PK\n"\
        " LEFT JOIN UnitConnector_tbl TNUCNR\n"\
        "  ON TNUCNR.Id_PK = TNUCNX.UnitConnector_Id_FK\n"\
        /**
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUCNX.TestNodeUnit_Id_FK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n"\
        " JOIN VehicleType_tbl VTN\n"\
        "  ON VTN.Id_PK = TN.VehicleType_Id_FK_PK\n"\
        */
        " JOIN LogicalTestCable_tbl LTC\n"\
        "  ON LTC.Id_PK = TL.LogicalTestCable_Id_FK\n";
        /**
        " JOIN TestNodeUnitConnection_tbl TNUCNX\n"\
        "  ON TNUCNX.UnitConnection_Id_FK_PK = LNK.TestConnection_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXT\n"\
        "  ON UCNXT.Id_PK = LNK.TestConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCT\n"\
        "  ON UCT.Id_PK = UCNXT.UnitConnector_Id_FK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUCNX.TestNodeUnit_Id_FK\n"\
        " JOIN Unit_tbl UTNU\n"\
        "  ON UTNU.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n"\
        " JOIN VehicleType_tbl VTN\n"\
        "  ON VTN.Id_PK = TN.VehicleType_Id_FK_PK\n"\
        " LEFT JOIN TestNodeBus_tbl TNB\n"\
        "  ON TNB.Id_PK = TNUCNX.TestNodeBus_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXD\n"\
        "  ON UCNXD.Id_PK = LNK.DutConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCD\n"\
        "  ON UCD.Id_PK = UCNXD.UnitConnector_Id_FK\n"\
        " JOIN Unit_tbl UD\n"\
        "  ON UD.Id_PK = UCNXD.Unit_Id_FK\n"\
        " JOIN VehicleType_Unit_tbl UVTU\n"\
        "  ON UVTU.Unit_Id_FK = UD.Id_PK\n"\
        " JOIN VehicleType_tbl UVT\n"\
        "  ON UVT.Id_PK = UVTU.VehicleType_Id_FK\n";
	*/

  return createView("TestLink_view", sql);
}

bool mdtTtDatabaseSchema::createTestCableTestNodeUnitView()
{
  QString sql, selectSql;

  selectSql = "SELECT\n"\
              " LTCTNU.*,\n"\
              " U.SchemaPosition,\n"\
              " U.Alias,\n"\
              " TNU.*,\n"\
              " TNUT.NameEN AS TestNodeUnitTypeEN,\n"\
              " TNUT.NameEN AS TestNodeUnitTypeFR,\n"\
              " TNUT.NameEN AS TestNodeUnitTypeDE,\n"\
              " TNUT.NameEN AS TestNodeUnitTypeIT,\n"\
              " VT.Type,\n"\
              " VT.SubType,\n"\
              " VT.SeriesNumber\n";
  sql = "CREATE VIEW LogicalTestCable_TestNodeUnit_view AS\n";
  sql += selectSql;
  sql += "FROM LogicalTestCable_TestNodeUnit_tbl LTCTNU\n"\
         " JOIN TestNodeUnit_tbl TNU\n"\
         "  ON TNU.Unit_Id_FK_PK = LTCTNU.TestNodeUnit_Id_FK\n"\
         " JOIN Unit_tbl U\n"\
         "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
         " JOIN TestNodeUnitType_tbl TNUT\n"\
         "  ON TNUT.Code_PK = TNU.Type_Code_FK\n"\
         " JOIN TestNode_tbl TN\n"\
         "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n"\
         " JOIN VehicleType_tbl VT\n"\
         "  ON VT.Id_PK = TN.VehicleType_Id_FK_PK\n";

  return createView("LogicalTestCable_TestNodeUnit_view", sql);
}

bool mdtTtDatabaseSchema::createTestNodeTestCableView()
{
  QString sql;

  sql = "CREATE VIEW TestNode_LogicalTestCable_view AS\n"\
        "SELECT DISTINCT\n"\
        " TNU.TestNode_Id_FK,\n"\
        " TMTN.TestModel_Id_FK,\n"\
        " LTCTNU.LogicalTestCable_Id_FK,\n"\
        " TC.Identification,\n"\
        " LTC.Key,\n"\
        " TC.OffsetResetDate,\n"\
        " TC.DescriptionEN\n"\
        "FROM TestNode_tbl TN\n"\
        " JOIN TestModel_TestNode_tbl TMTN\n"\
        "  ON TMTN.TestNode_Id_FK = TN.VehicleType_Id_FK_PK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.TestNode_Id_FK = TN.VehicleType_Id_FK_PK\n"\
        " JOIN LogicalTestCable_TestNodeUnit_tbl LTCTNU\n"\
        "  ON LTCTNU.TestNodeUnit_Id_FK = TNU.Unit_Id_FK_PK\n"\
        " JOIN LogicalTestCable_tbl LTC\n"\
        "  ON LTC.Id_PK = LTCTNU.LogicalTestCable_Id_FK\n"\
        " JOIN TestCable_tbl TC\n"\
        "  ON TC.Unit_Id_FK_PK = LTC.TestCable_Id_FK\n";

  return createView("TestNode_LogicalTestCable_view", sql);
}

bool mdtTtDatabaseSchema::createTestCableDutUnitView()
{
  QString sql, selectSql;

  selectSql = "SELECT\n"\
              " LTCDU.*,\n"\
              " U.SchemaPosition,\n"\
              " U.Alias,\n"\
              " U.Cabinet,\n"\
              " U.Coordinate,\n"\
              " A.ArticleCode,\n"\
              " A.DesignationEN,\n"\
              " A.DesignationFR,\n"\
              " A.DesignationDE,\n"\
              " A.DesignationIT,\n"\
              " VT.Type,\n"\
              " VT.SubType,\n"\
              " VT.SeriesNumber\n";
  sql = "CREATE VIEW LogicalTestCable_DutUnit_view AS\n";
  sql += selectSql;
  sql += "FROM LogicalTestCable_DutUnit_tbl LTCDU\n"\
         " JOIN Unit_tbl U\n"\
         "  ON U.Id_PK = LTCDU.DutUnit_Id_FK"\
         " LEFT JOIN Article_tbl A\n"\
         "  ON A.Id_PK = U.Article_Id_FK\n"\
         " JOIN VehicleType_Unit_tbl VTU\n"\
         "  ON VTU.Unit_Id_FK = U.Id_PK\n"\
         " JOIN VehicleType_tbl VT\n"\
         "  ON VT.Id_PK = VTU.VehicleType_Id_FK";

  return createView("LogicalTestCable_DutUnit_view", sql);
}

bool mdtTtDatabaseSchema::createTestModelTestNodeView()
{
  QString sql;

  sql = "CREATE VIEW TestModel_TestNode_view AS\n"\
        "SELECT\n"\
        " TMTN.*,\n"\
        " TN.AddressString,\n"\
        " TN.Alias,\n"\
        " VT.Type,\n"\
        " VT.SubType,\n"\
        " VT.SeriesNumber\n"\
        "FROM TestModel_TestNode_tbl TMTN\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TMTN.TestNode_Id_FK\n"\
        " JOIN VehicleType_tbl VT\n"\
        "  ON VT.Id_PK = TN.VehicleType_Id_FK_PK";

  return createView("TestModel_TestNode_view", sql);
}

bool mdtTtDatabaseSchema::createTestModelItemTestLinkView()
{
  QString sql;

  sql = "CREATE VIEW TestModelItem_TestLink_view AS\n"\
        "SELECT\n"\
        " TMITL.*,\n"\
        " UVT.Type,\n"\
        " UVT.SubType,\n"\
        " UVT.SeriesNumber,\n"\
        " UD.Id_PK AS DutUnitId,\n"\
        " UD.SchemaPosition AS DutUnitSchemaPosition,\n"\
        " UD.Alias AS DutUnitAlias,\n"\
        " UCD.Name AS DutConnectorName,\n"\
        " UCNXD.UnitContactName AS DutContactName,\n"\
        " TL.TestConnection_Id_FK,\n"\
        " TL.DutConnection_Id_FK,\n"\
        " TL.Identification AS TestLinkIdentification,\n"\
        /** " TL.Resistance,\n"\ */
        " UCNXT.UnitContactName AS TestContactName,\n"\
        " UCT.Name AS TestConnectorName,\n"\
        " TNUCNX.TestNodeUnit_Id_FK,\n"\
        " UTNU.SchemaPosition AS TestNodeUnitSchemaPosition,\n"\
        /** " TNU.Unit_Id_FK_PK,\n"\ */
        /** " TNB.NameEN AS Bus,\n"\ */
        " TNU.IoPosition,\n"\
        " TN.Alias,\n"\
        " VTN.Type AS TestNodeType,\n"\
        " VTN.SubType AS TestNodeSubType,\n"\
        " VTN.SeriesNumber AS TestNodeSeriesNumber,\n"\
        " TN.VehicleType_Id_FK_PK,\n"\
        " TN.AddressString\n"\
        "FROM TestModelItem_TestLink_tbl TMITL\n"\
        " JOIN TestLink_tbl TL\n"\
        "  ON TL.Id_PK = TMITL.TestLink_Id_FK\n"\
        " JOIN TestNodeUnitConnection_tbl TNUCNX\n"\
        "  ON TNUCNX.UnitConnection_Id_FK_PK = TL.TestConnection_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXT\n"\
        "  ON UCNXT.Id_PK = TL.TestConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCT\n"\
        "  ON UCT.Id_PK = UCNXT.UnitConnector_Id_FK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUCNX.TestNodeUnit_Id_FK\n"\
        " JOIN Unit_tbl UTNU\n"\
        "  ON UTNU.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n"\
        " JOIN VehicleType_tbl VTN\n"\
        "  ON VTN.Id_PK = TN.VehicleType_Id_FK_PK\n"\
        /**
        " LEFT JOIN TestNodeBus_tbl TNB\n"\
        "  ON TNB.Id_PK = TNUCNX.TestNodeBus_Id_FK\n"\
        */
        " JOIN UnitConnection_tbl UCNXD\n"\
        "  ON UCNXD.Id_PK = TL.DutConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCD\n"\
        "  ON UCD.Id_PK = UCNXD.UnitConnector_Id_FK\n"\
        " JOIN Unit_tbl UD\n"\
        "  ON UD.Id_PK = UCNXD.Unit_Id_FK\n"\
        " JOIN VehicleType_Unit_tbl UVTU\n"\
        "  ON UVTU.Unit_Id_FK = UD.Id_PK\n"\
        " JOIN VehicleType_tbl UVT\n"\
        "  ON UVT.Id_PK = UVTU.VehicleType_Id_FK\n";

  return createView("TestModelItem_TestLink_view", sql);
}

bool mdtTtDatabaseSchema::createTestNodeUnitSetupView()
{
  QString sql;

  sql = "CREATE VIEW TestNodeUnitSetup_view AS\n"\
        "SELECT\n"\
        " TNUS.TestModelItem_Id_FK,\n"\
        /** " TNUS.TestNodeRoute_Id_FK,\n"\ */
        " TNUS.TestNodeUnit_Id_FK,\n"\
        " TNUS.StepNumber,\n"\
        " U.SchemaPosition,\n"\
        " U.Alias,\n"\
        " TNUT.NameEN,\n"\
        " TNUT.NameFR,\n"\
        " TNUT.NameDE,\n"\
        " TNUT.NameIT,\n"\
        " TNU.TestNode_Id_FK,\n"\
        " TNU.Type_Code_FK,\n"\
        " TNU.IoPosition,\n"\
        " TNUS.State,\n"\
        " TNUS.Value,\n"\
        " TNU.CalibrationOffset,\n"\
        " TNU.CalibrationDate\n"\
        "FROM TestNodeUnitSetup_tbl TNUS\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUS.TestNodeUnit_Id_FK\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNodeUnitType_tbl TNUT\n"\
        "  ON TNUT.Code_PK = TNU.Type_Code_FK";

  return createView("TestNodeUnitSetup_view", sql);
}

bool mdtTtDatabaseSchema::createTestNodeRouteView()
{
  QString sql;

  sql = "CREATE VIEW TestNodeRoute_view AS\n"\
        "SELECT\n"\
        " TNR.*,\n"\
        // Test node connection A part
        " UA.SchemaPosition AS TestNodeUnitConnectionASchemaPosition,\n"\
        " UCNXA.UnitContactName AS TestNodeUnitConnectionAContactName,\n"\
        // Test node connection B part
        " UB.SchemaPosition AS TestNodeUnitConnectionBSchemaPosition,\n"\
        " UCNXB.UnitContactName AS TestNodeUnitConnectionBContactName,\n"\
        // Node units that are used to create the route
        " GROUP_CONCAT(UR.SchemaPosition, ', ') AS RouteTestNodeUnits\n"\
        "FROM TestNodeRoute_tbl TNR\n"\
        // Test node connection A part
        " JOIN UnitConnection_tbl UCNXA\n"\
        "  ON UCNXA.Id_PK = TNR.TestNodeUnitConnectionA_Id_FK\n"\
        " JOIN Unit_tbl UA\n"\
        "  ON UA.Id_PK = UCNXA.Unit_Id_FK\n"\
        // Test node connection B part
        " JOIN UnitConnection_tbl UCNXB\n"\
        "  ON UCNXB.Id_PK = TNR.TestNodeUnitConnectionB_Id_FK\n"\
        " JOIN Unit_tbl UB\n"\
        "  ON UB.Id_PK = UCNXB.Unit_Id_FK\n"\
        // Node units that are used to create the route
        " JOIN TestNodeRouteUnit_tbl TNRU\n"\
        "  ON TNRU.TestNodeRoute_Id_FK = TNR.Id_PK\n"\
        " JOIN Unit_tbl UR\n"\
        "  ON UR.Id_PK = TNRU.TestNodeUnit_Id_FK\n"\
        " GROUP BY TNR.Id_PK\n";

  return createView("TestNodeRoute_view", sql);
}

bool mdtTtDatabaseSchema::createTestModelItemRouteView()
{
  QString sql;

  sql = "CREATE VIEW TestModelItemRoute_view AS\n"\
        "SELECT\n"\
        " TMIR.*,\n"\
        " UM.SchemaPosition AS MeasureSchemaPosition,\n"\
        " UM.Alias AS MeasureAlias,\n"\
        " UCNXM.UnitContactName AS MeasureContactName,\n"\
        " UCNXM.FunctionEN AS MeasureContactFunctionEN,\n"\
        " UCNXM.FunctionFR AS MeasureContactFunctionFR,\n"\
        " UCNXM.FunctionDE AS MeasureContactFunctionDE,\n"\
        " UCNXM.FunctionIT AS MeasureContactFunctionIT,\n"\
        " TL.TestConnection_Id_FK,\n"\
        " TL.DutConnection_Id_FK,\n"\
        " TL.Identification,\n"\
        " UT.SchemaPosition AS TestSchemaPosition,\n"\
        " UT.Alias AS TestAlias,\n"\
        " UCNXT.UnitContactName AS TestContactName,\n"\
        " UCNXT.FunctionEN AS TestContactFunctionEN,\n"\
        " UCNXT.FunctionFR AS TestContactFunctionFR,\n"\
        " UCNXT.FunctionDE AS TestContactFunctionDE,\n"\
        " UCNXT.FunctionIT AS TestContactFunctionIT,\n"\
        " GROUP_CONCAT(U.SchemaPosition, ', ') AS RouteTestNodeUnits,\n"\
        " SUM(TNU.CalibrationOffset) AS RouteResistance,\n"\
        " TNU.TestNode_Id_FK\n";
  sql += "FROM TestModelItemRoute_tbl TMIR\n"\
         " JOIN TestLink_tbl TL\n"\
         "  ON TL.Id_PK = TMIR.TestLink_Id_FK\n";
         // Test unit and connection part
  sql += " JOIN TestNodeUnitConnection_tbl TNUCNXT\n"\
         "  ON TNUCNXT.UnitConnection_Id_FK_PK = TL.TestConnection_Id_FK\n"\
         " JOIN UnitConnection_tbl UCNXT\n"\
         "  ON UCNXT.Id_PK = TNUCNXT.UnitConnection_Id_FK_PK\n"\
         " JOIN TestNodeUnit_tbl TNUT\n"\
         "  ON TNUT.Unit_Id_FK_PK = TNUCNXT.TestNodeUnit_Id_FK\n"\
         " JOIN Unit_tbl UT\n"\
         "  ON UT.Id_PK = TNUT.Unit_Id_FK_PK\n";
         // Measure unit and connection part
  sql += " JOIN TestNodeUnitConnection_tbl TNUCNXM\n"\
         "  ON TNUCNXM.UnitConnection_Id_FK_PK = TMIR.MeasureTestNodeUnitConnection_Id_FK\n"\
         " JOIN UnitConnection_tbl UCNXM\n"\
         "  ON UCNXM.Id_PK = TNUCNXM.UnitConnection_Id_FK_PK\n"\
         " JOIN TestNodeUnit_tbl TNUM\n"\
         "  ON TNUM.Unit_Id_FK_PK = TNUCNXM.TestNodeUnit_Id_FK\n"\
         " JOIN Unit_tbl UM\n"\
         "  ON UM.Id_PK = TNUM.Unit_Id_FK_PK\n";
         // Node units that are used to create the route
  sql += " LEFT JOIN TestNodeUnitSetup_tbl TNUS\n"\
         "  ON TNUS.TestModelItemRoute_Id_FK = TMIR.Id_PK\n"\
         " LEFT JOIN TestNodeUnit_tbl TNU\n"\
         "  ON TNU.Unit_Id_FK_PK = TNUS.TestNodeUnit_Id_FK\n"\
         " LEFT JOIN Unit_tbl U\n"\
         "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n";
  sql += "GROUP BY TMIR.Id_PK\n";

  return createView("TestModelItemRoute_view", sql);
}


/**
bool mdtTtDatabaseSchema::createTestModelItemView()
{
  QString sql;

  sql = "CREATE VIEW TestModelItem_view AS\n"\
        "SELECT\n"\
        " TMI.Id_PK,\n"\
        " TMI.TestModel_Id_FK,\n"\
        " TMI.TestLinkBusA_Id_FK,\n"\
        " TMI.TestLinkBusB_Id_FK,\n"\
        " TMI.SequenceNumber,\n"\
        " TMI.ExpectedValue,\n"\
        " UCTA.Name AS TestConnectorNameBusA,\n"\
        " UCNXTA.UnitContactName AS TestContactNameBusA,\n"\
        " UDA.SchemaPosition AS DutUnitSchemaPositionBusA,\n"\
        " UCDA.Name AS DutConnectorNameBusA,\n"\
        " UCNXDA.UnitContactName AS DutContactNameBusA,\n"\
        " UCTB.Name AS TestConnectorNameBusB,\n"\
        " UCNXTB.UnitContactName AS TestContactNameBusB,\n"\
        " UDB.SchemaPosition AS DutUnitSchemaPositionBusB,\n"\
        " UCDB.Name AS DutConnectorNameBusB,\n"\
        " UCNXDB.UnitContactName AS DutContactNameBusB\n"\
        "FROM TestModelItem_tbl TMI\n"\
        " LEFT JOIN TestLink_tbl LNKA\n"\
        "  ON LNKA.Id_PK = TMI.TestLinkBusA_Id_FK\n"\
        " LEFT JOIN UnitConnection_tbl UCNXTA\n"\
        "  ON UCNXTA.Id_PK = LNKA.TestConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCTA\n"\
        "  ON UCTA.Id_PK = UCNXTA.UnitConnector_Id_FK\n"\
        " LEFT JOIN UnitConnection_tbl UCNXDA\n"\
        "  ON UCNXDA.Id_PK = LNKA.DutConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCDA\n"\
        "  ON UCDA.Id_PK = UCNXDA.UnitConnector_Id_FK\n"\
        " LEFT JOIN Unit_tbl UDA\n"\
        "  ON UDA.Id_PK = UCNXDA.Unit_Id_FK\n"\
        " LEFT JOIN TestLink_tbl LNKB\n"\
        "  ON LNKB.Id_PK = TMI.TestLinkBusB_Id_FK\n"\
        " LEFT JOIN UnitConnection_tbl UCNXTB\n"\
        "  ON UCNXTB.Id_PK = LNKB.TestConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCTB\n"\
        "  ON UCTB.Id_PK = UCNXTB.UnitConnector_Id_FK\n"\
        " LEFT JOIN UnitConnection_tbl UCNXDB\n"\
        "  ON UCNXDB.Id_PK = LNKB.DutConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCDB\n"\
        "  ON UCDB.Id_PK = UCNXDB.UnitConnector_Id_FK\n"\
        " LEFT JOIN Unit_tbl UDB\n"\
        "  ON UDB.Id_PK = UCNXDB.Unit_Id_FK\n"\
        "ORDER BY SequenceNumber ASC";

  return createView("TestModelItem_view", sql);
}
*/

/**
bool mdtTtDatabaseSchema::createTestModelItemNodeUnitSetupView()
{
  QString sql;

  sql = "CREATE VIEW TestModelItemNodeUnitSetup_view AS\n"\
        "SELECT\n"\
        " TMI.TestModel_Id_FK,\n"\
        " TNUS.Id_PK,\n"\
        " TNUS.TestModelItem_Id_FK,\n"\
        " TMI.SequenceNumber,\n"\
        " U.SchemaPosition,\n"\
        " TNUS.State,\n"\
        " TNUS.Value,\n"\
        " TNU.Bus,\n"\
        " TNU.IoPosition,\n"\
        " TN.NodeIdentification\n"\
        "FROM TestNodeUnitSetup_tbl TNUS\n"\
        " JOIN TestModelItem_tbl TMI\n"\
        "  ON TMI.Id_PK = TNUS.TestModelItem_Id_FK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUS.TestNodeUnit_Id_FK\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n"\
        "ORDER BY SequenceNumber ASC";

  return createView("TestModelItemNodeUnitSetup_view", sql);
}
*/

/**
bool mdtTtDatabaseSchema::createTestModelItemNodeView()
{
  QString sql;

  sql = "CREATE VIEW TestModelItemNode_view AS\n"\
        "SELECT DISTINCT\n"\
        " TMI.TestModel_Id_FK,\n"\
        " TN.VehicleType_Id_FK_PK AS TestNode_Id_FK,\n"\
        " TN.NodeId\n"\
        "FROM TestModelItem_tbl TMI\n"\
        " JOIN TestLink_tbl LNKA\n"\
        "  ON LNKA.Id_PK = TMI.TestLinkBusA_Id_FK\n"\
        " JOIN TestLink_tbl LNKB\n"\
        "  ON LNKB.Id_PK = TMI.TestLinkBusB_Id_FK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.TestConnection_Id_FK = LNKA.TestConnection_Id_FK\n"\
        "  OR TNU.TestConnection_Id_FK = LNKB.TestConnection_Id_FK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK";

  return createView("TestModelItemNode_view", sql);
}
*/
/**
bool mdtTtDatabaseSchema::createTestModelItemNodeUnitView()
{
  QString sql;

  sql = "CREATE VIEW TestModelItemNodeUnit_view AS\n"\
        "SELECT\n"\
        " TMI.Id_PK,\n"\
        " TMI.TestModel_Id_FK,\n"\
        " TNU.Unit_Id_FK_PK,\n"\
        " U.SchemaPosition,\n"\
        " TNU.Bus,\n"\
        " TNU.IoPosition,\n"\
        " TNU.Type_Code_FK,\n"\
        " TN.VehicleType_Id_FK_PK AS TestNode_Id_FK,\n"\
        " TN.NodeIdentification\n"\
        "FROM TestModelItem_tbl TMI\n"\
        " JOIN TestLink_tbl LNKA\n"\
        "  ON LNKA.Id_PK = TMI.TestLinkBusA_Id_FK\n"\
        " JOIN TestLink_tbl LNKB\n"\
        "  ON LNKB.Id_PK = TMI.TestLinkBusB_Id_FK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.TestConnection_Id_FK = LNKA.TestConnection_Id_FK\n"\
        "  OR TNU.TestConnection_Id_FK = LNKB.TestConnection_Id_FK\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK";

  return createView("TestModelItemNodeUnit_view", sql);
}
*/

bool mdtTtDatabaseSchema::createTestView()
{
  QString sql;

  sql = "CREATE VIEW Test_view AS\n"\
        "SELECT\n"\
        " T.Id_PK,\n"\
        /**" T.Id_PK,\n"\ */
        " TM.Key,\n"\
        " TM.DesignationEN,\n"\
        " T.Date,\n"\
        " T.DutName,\n"\
        " T.DutSerialNumber\n"\
        "FROM Test_tbl T\n"\
        " JOIN TestModel_tbl TM\n"\
        "  ON TM.Id_PK = T.TestModel_Id_FK\n";

  return createView("Test_view", sql);
}

bool mdtTtDatabaseSchema::createTestItemView()
{
  QString sql;

  sql = "CREATE VIEW TestItem_view AS\n"\
        "SELECT\n"\
        " TI.Id_PK,\n"\
        " TI.Test_Id_FK,\n"\
        " TI.TestModelItem_Id_FK,\n"\
        " TI.Date,\n"\
        " TMI.Key,\n"\
        " TMI.SequenceNumber,\n"\
        " TMI.DesignationEN,\n"\
        " TI.MeasuredValue,\n"\
        " TI.InstrumentRangeMin,\n"\
        " TI.InstrumentRangeMax,\n"\
        " TI.ResultValue,\n"\
        " TMI.ExpectedValue,\n"\
        " TMI.LeftBottomLimit,\n"\
        " TMI.LeftTopLimit,\n"\
        " TMI.RightBottomLimit,\n"\
        " TMI.RightTopLimit,\n"\
        " TI.Result,\n"\
        " TI.Remark\n"\
        "FROM TestModelItem_tbl TMI\n"\
        " JOIN TestItem_tbl TI\n"\
        "  ON TMI.Id_PK = TI.TestModelItem_Id_FK\n"\
        "ORDER BY SequenceNumber ASC";

  return createView("TestItem_view", sql);
}

/**
bool mdtTtDatabaseSchema::createTestItemNodeUnitSetupView()
{
  QString sql;

  sql = "CREATE VIEW TestItemNodeUnitSetup_view AS\n"\
        "SELECT\n"\
        " TI.Id_PK,\n"\
        " TI.Test_Id_FK,\n"\
        " TI.TestModelItem_Id_FK,\n"\
        " TMI.TestModel_Id_FK,\n"\
        " TMI.Key,\n"\
        " TMI.SequenceNumber,\n"\
        " TNUS.StepNumber,\n"\
        " U.SchemaPosition,\n"\
        " TNUS.TestNodeUnit_Id_FK,\n"\
        " TNUS.State,\n"\
        " TNUS.Value,\n"\
        " TNU.TestNode_Id_FK,\n"\
        " TNU.Type_Code_FK,\n"\
        " TNU.IoPosition,\n"\
        " TN.NodeIdentification,\n"\
        " TN.DeviceIdentification\n"\
        "FROM TestItem_tbl TI\n"\
        " JOIN TestModelItem_tbl TMI\n"\
        "  ON TMI.Id_PK = TI.TestModelItem_Id_FK\n"\
        " JOIN TestNodeUnitSetup_tbl TNUS\n"\
        "  ON TNUS.TestModelItem_Id_FK = TMI.Id_PK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUS.TestNodeUnit_Id_FK\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n"\
        "ORDER BY SequenceNumber ASC, TN.VehicleType_Id_FK_PK ASC, StepNumber ASC";

  return createView("TestItemNodeUnitSetup_view", sql);
}
*/

/// \todo Obselete ?
bool mdtTtDatabaseSchema::createTestItemNodeUnitSetupView()
{
  QString sql;

  sql = "CREATE VIEW TestItemNodeUnitSetup_view AS\n"\
        "SELECT\n"\
        " TMI.Id_PK AS TestModelItem_Id_FK,\n"\
        " TMI.TestModel_Id_FK,\n"\
        " TMI.Key,\n"\
        " TMI.SequenceNumber,\n"\
        " TNUS.StepNumber,\n"\
        " U.SchemaPosition,\n"\
        " TNUS.TestNodeUnit_Id_FK,\n"\
        " TNUS.State,\n"\
        " TNUS.Value,\n"\
        " TNU.TestNode_Id_FK,\n"\
        " TNU.Type_Code_FK,\n"\
        " TNU.IoPosition,\n"\
        " TNU.CalibrationOffset,\n"\
        " TNU.CalibrationDate,\n"\
        " TN.AddressString,\n"\
        " TN.Alias AS TestNodeAlias\n"\
        "FROM TestModelItem_tbl TMI\n"\
        " JOIN TestNodeUnitSetup_tbl TNUS\n"\
        "  ON TNUS.TestModelItem_Id_FK = TMI.Id_PK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUS.TestNodeUnit_Id_FK\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n"\
        "ORDER BY SequenceNumber ASC, TN.VehicleType_Id_FK_PK ASC, StepNumber ASC";

  return createView("TestItemNodeUnitSetup_view", sql);
}

bool mdtTtDatabaseSchema::pkExistsInTable(const QString & tableName, const QString & pkField, const QVariant & pkData)
{
  QSqlQuery query(pvDatabaseManager->database());
  QSqlError sqlError;
  QString sql;
  QString dp;

  if(pkData.type() == QVariant::String){
    dp = "'";
  }
  sql = "SELECT " + pkField + " FROM '" + tableName + "' WHERE " + pkField + " = " + dp + pkData.toString() + dp;
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to check if data exists in table '" + tableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtDatabaseSchema");
    pvLastError.commit();
    return false;
  }
  if(query.next()){
    return true;
  }

  return false;
}

bool mdtTtDatabaseSchema::insertDataIntoTable(const QString & tableName, const QStringList & fields, const QList<QVariant> & data)
{
  Q_ASSERT(fields.size() > 0);
  Q_ASSERT(data.size() == fields.size());

  QSqlQuery query(pvDatabaseManager->database());
  QSqlError sqlError;
  QString sql;
  int i;
  bool pkExists;

  pkExists = pkExistsInTable(tableName, fields.at(0), data.at(0));
  if(pkExists){
    sql = sqlForDataEdition(tableName, fields, data);
  }else{
    sql = sqlForDataInsertion(tableName, fields, data);
  }
  // Prepare query for insertion
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for insertion into table '" + tableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtDatabaseSchema");
    pvLastError.commit();
    return false;
  }
  // Bind values
  if(pkExists){
    i = 1;
  }else{
    i = 0;
  }
  for(i = i; i < data.size(); ++i){
    query.bindValue(i, data.at(i));
  }
  // Exec query
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for insertion into table '" + tableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtDatabaseSchema");
    pvLastError.commit();
    return false;
  }

  return true;
}

QString mdtTtDatabaseSchema::sqlForDataInsertion(const QString & tableName, const QStringList & fields, const QList<QVariant> & data)
{
  QString sql;
  int i;

  // Generate SQL statement
  sql = "INSERT INTO '" + tableName + "' (";
  for(i = 0; i < fields.size(); ++i){
    sql += fields.at(i);
    if(i < (fields.size() - 1)){
      sql += ",";
    }
  }
  sql += ") VALUES(";
  for(i = 0; i < fields.size(); ++i){
    sql += "?";
    if(i < (fields.size() - 1)){
      sql += ",";
    }
  }
  sql += ")";

  return sql;
}

QString mdtTtDatabaseSchema::sqlForDataEdition(const QString & tableName, const QStringList & fields, const QList<QVariant> & data)
{
  Q_ASSERT(fields.size() > 0);
  Q_ASSERT(data.size() == fields.size());

  QString sql;
  QVariant pkData;
  QString dp;
  int i;

  pkData = data.at(0);
  if(pkData.type() == QVariant::String){
    dp = "'";
  }
  // Generate SQL statement
  sql = "UPDATE '" + tableName + "' SET\n";
  for(i = 1; i < fields.size(); ++i){
    sql += fields.at(i) + " = :" + fields.at(i);
    if(i < (fields.size() - 1)){
      sql += ",";
    }
    sql += "\n";
  }
  sql += " WHERE " + fields.at(0) + " = " + dp + pkData.toString() + dp;

  return sql;
}

bool mdtTtDatabaseSchema::populateModificationTable()
{
  QStringList fields;
  QList<QVariant> data;

  fields << "Code_PK" << "SortOrder" << "NameEN" << "NameDE" << "NameFR" << "NameIT";
  // NEW
  data << "NEW" << 3 << "New" << "Neu" << "Nouveau" << "Nuovo";
  if(!insertDataIntoTable("Modification_tbl", fields, data)){
    return false;
  }
  // REM
  data.clear();
  data << "REM" << 3 << "Remove" << "Entfernen" << "Supprimer" << "Rimuovere";
  if(!insertDataIntoTable("Modification_tbl", fields, data)){
    return false;
  }
  // MOD
  data.clear();
  data << "MOD" << 3 << "Modifiy" << "Ändern" << "Modifier" << "Modificare";
  if(!insertDataIntoTable("Modification_tbl", fields, data)){
    return false;
  }
  // MODNEW
  data.clear();
  data << "MODNEW" << 2 << "Modifiy (+)" << "Ändern (+)" << "Modifier (+)" << "Modificare (+)";
  if(!insertDataIntoTable("Modification_tbl", fields, data)){
    return false;
  }
  // MODREM
  data.clear();
  data << "MODREM" << 1 << "Modifiy (-)" << "Ändern (-)" << "Modifier (-)" << "Modificare (-)";
  if(!insertDataIntoTable("Modification_tbl", fields, data)){
    return false;
  }
  // EXISTS
  data.clear();
  data << "EXISTS" << 3 << "Existing" << "Bestehend" << "Existant" << "Esistente";
  if(!insertDataIntoTable("Modification_tbl", fields, data)){
    return false;
  }

  return true;
}

bool mdtTtDatabaseSchema::populateConnectionTypeTable()
{
  QStringList fields;
  QList<QVariant> data;

  fields << "Code_PK" << "NameEN" << "NameDE" << "NameFR" << "NameIT";

  // Terminal type
  data << "T" << "Terminal" << "Klemme" << "Borne" << "Terminale";
  if(!insertDataIntoTable("ConnectionType_tbl", fields, data)){
    return false;
  }
  // Pin type
  data.clear();
  data << "P" << "Pin (male)" << "Stift (männlich)" << "Contact (mâle)" << "perno (maschio)";
  if(!insertDataIntoTable("ConnectionType_tbl", fields, data)){
    return false;
  }
  // Socket type
  data.clear();
  data << "S" << "Socket (female)" << "Buchse (weiblich)" << "Douille (femelle)" << "presa (femminile)";
  if(!insertDataIntoTable("ConnectionType_tbl", fields, data)){
    return false;
  }

  return true;
}

bool mdtTtDatabaseSchema::populateLinkTypeTable()
{
  QStringList fields;
  QList<QVariant> data;

  fields << "Code_PK" << "NameEN" << "NameDE" << "NameFR" << "NameIT" << "ValueUnit";

  // Cable link type
  data << "CABLELINK" << "Cable link" << "Kabel Verbindung" << "Liaison cablée" << "Collegamento via cavo" << "Ohm";
  if(!insertDataIntoTable("LinkType_tbl", fields, data)){
    return false;
  }
  // Internal link
  data.clear();
  data << "INTERNLINK" << "Internal link" << "Interne Verbindung" << "Liaison interne" << "Collegamento interno" << "Ohm";
  if(!insertDataIntoTable("LinkType_tbl", fields, data)){
    return false;
  }
  // Connection type
  data.clear();
  data << "CONNECTION" << "Connection" << "Anschluss" << "Raccordement" << "Collegamento" << "Ohm";
  if(!insertDataIntoTable("LinkType_tbl", fields, data)){
    return false;
  }
  // Test link type
  data.clear();
  data << "TESTLINK" << "Test link" << "Test Verbindung" << "Liaison de test" << "Collegamento test" << "Ohm";
  if(!insertDataIntoTable("LinkType_tbl", fields, data)){
    return false;
  }

  return true;
}

bool mdtTtDatabaseSchema::populateLinkDirectionTable()
{
  QStringList fields;
  QList<QVariant> data;

  fields << "Code_PK" << "PictureAscii" << "NameEN" << "NameDE" << "NameFR" << "NameIT";

  // Bidirectional
  data << "BID" << "<-->" << "Bidirectional" << "Bidirektional" << "Bidirectionnel" << "Bidirezionale";
  if(!insertDataIntoTable("LinkDirection_tbl", fields, data)){
    return false;
  }
  // Start to end
  data.clear();
  data << "STE" << "-->" << "Start to end" << "Start zum Ende" << "Départ vers arrivée" << "Dall'inizio alla fine";
  if(!insertDataIntoTable("LinkDirection_tbl", fields, data)){
    return false;
  }
  // End to start
  data.clear();
  data << "ETS" << "<--" << "End to Start" << "Ende zum Start" << "Arrivée vers départ" << "Dall'fine alla inizio";
  if(!insertDataIntoTable("LinkDirection_tbl", fields, data)){
    return false;
  }

  return true;
}


bool mdtTtDatabaseSchema::populateTestSystemComponentTypeTable()
{
  QStringList fields;
  QList<QVariant> data;

  fields << "Code_PK" << "NameEN" << "NameFR" << "NameDE" << "NameIT";

  // Test node
  data << "TESTNODE" << "Test node" << "Noeud de test" << "Testknoten" << "Nodo di prova";
  if(!insertDataIntoTable("TestSystemComponentType_tbl", fields, data)){
    return false;
  }
  // Test cable
  data.clear();
  data << "TESTCABLE" << "Test cable" << "Câble de test" << "Test Kabel" << "Cavo di prova";
  if(!insertDataIntoTable("TestSystemComponentType_tbl", fields, data)){
    return false;
  }
  // Test plug
  data.clear();
  data << "TESTPLUG" << "Test plug" << "Fiche de test" << "Test Stecker" << "Spina di prova";
  if(!insertDataIntoTable("TestSystemComponentType_tbl", fields, data)){
    return false;
  }

  return true;
}

bool mdtTtDatabaseSchema::populateTestSystemUnitTypeTable()
{
  QStringList fields;
  QList<QVariant> data;

  fields << "Code_PK" << "NameEN" << "NameFR" << "NameDE" << "NameIT";

  // Bus coupling relay
  data << "BUSCPLREL" << "Bus coupling relay" << "Relai de couplage de bus" << "Bus Koppelrelais" << "Relè di accoppiamento bus";
  if(!insertDataIntoTable("TestSystemUnitType_tbl", fields, data)){
    return false;
  }
  // Channel relay
  data.clear();
  data << "CHANELREL" << "Channel relay" << "Relai de canal" << "Kanal-Relais" << "Relè della Manica";
  if(!insertDataIntoTable("TestSystemUnitType_tbl", fields, data)){
    return false;
  }
  // Test connector
  data.clear();
  data << "TESTCONNECTOR" << "Test connector" << "Connecteur de test" << "Test Stecker" << "Connettore di test";
  if(!insertDataIntoTable("TestSystemUnitType_tbl", fields, data)){
    return false;
  }
  // Measure connector
  data.clear();
  data << "MEASCONNECTOR" << "Measure connector" << "Connecteur de mesure" << "Mess Stecker" << "Connettore di misura";
  if(!insertDataIntoTable("TestSystemUnitType_tbl", fields, data)){
    return false;
  }
  // Power supply
  data.clear();
  data << "PWR" << "Power supply" << "Alimentation" << "Speisung" << "Alimentazione";
  if(!insertDataIntoTable("TestSystemUnitType_tbl", fields, data)){
    return false;
  }
  // Analog output
  data.clear();
  data << "AO" << "Analog output" << "Sortie analogique" << "Analog Ausgabe" << "Uscita analogica";
  if(!insertDataIntoTable("TestSystemUnitType_tbl", fields, data)){
    return false;
  }
  // Analog input
  data.clear();
  data << "AI" << "Analog input" << "Entrée analogique" << "Analog Eingabe" << "Ingresso analogica";
  if(!insertDataIntoTable("TestSystemUnitType_tbl", fields, data)){
    return false;
  }

  return true;
}



bool mdtTtDatabaseSchema::populateTestNodeUnitTypeTable()
{
  QStringList fields;
  QList<QVariant> data;

  fields << "Code_PK" << "NameEN" << "NameFR" << "NameDE" << "NameIT";

  // Bus coupling relay
  data << "BUSCPLREL" << "Bus coupling relay" << "Relai de couplage de bus" << "Bus Koppelrelais" << "Relè di accoppiamento bus";
  if(!insertDataIntoTable("TestNodeUnitType_tbl", fields, data)){
    return false;
  }
  // Channel relay
  data.clear();
  data << "CHANELREL" << "Channel relay" << "Relai de canal" << "Kanal-Relais" << "Relè della Manica";
  if(!insertDataIntoTable("TestNodeUnitType_tbl", fields, data)){
    return false;
  }
  // Test connector
  data.clear();
  data << "TESTCONNECTOR" << "Test connector" << "Connecteur de test" << "Test Stecker" << "Connettore di test";
  if(!insertDataIntoTable("TestNodeUnitType_tbl", fields, data)){
    return false;
  }
  // Measure connector
  data.clear();
  data << "MEASCONNECTOR" << "Measure connector" << "Connecteur de mesure" << "Mess Stecker" << "Connettore di misura";
  if(!insertDataIntoTable("TestNodeUnitType_tbl", fields, data)){
    return false;
  }
  // Power supply
  data.clear();
  data << "PWR" << "Power supply" << "Alimentation" << "Speisung" << "Alimentazione";
  if(!insertDataIntoTable("TestNodeUnitType_tbl", fields, data)){
    return false;
  }
  // Analog output
  data.clear();
  data << "AO" << "Analog output" << "Sortie analogique" << "Analog Ausgabe" << "Uscita analogica";
  if(!insertDataIntoTable("TestNodeUnitType_tbl", fields, data)){
    return false;
  }
  // Analog input
  data.clear();
  data << "AI" << "Analog input" << "Entrée analogique" << "Analog Eingabe" << "Ingresso analogica";
  if(!insertDataIntoTable("TestNodeUnitType_tbl", fields, data)){
    return false;
  }

  return true;
}
