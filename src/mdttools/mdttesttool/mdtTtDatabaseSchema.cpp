/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlDatabaseManager.h"
#include "mdtDataTableManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlField>
#include <QFileInfo>
#include <QStringList>

#include <QDebug>

mdtTtDatabaseSchema::mdtTtDatabaseSchema(mdtSqlDatabaseManager *dbManager) 
{
  Q_ASSERT(dbManager != 0);

  pvDatabaseManager = dbManager;
}

mdtTtDatabaseSchema::~mdtTtDatabaseSchema() 
{
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
  sourceTables.removeAll("ConnectionType_tbl");
  sourceTables.removeAll("LinkDirection_tbl");
  sourceTables.removeAll("LinkType_tbl");
  sourceTables.removeAll("TestNodeUnitType_tbl");
  // Copy tables
  tableManager.enableProgressDialog(true);
  if(!pvDatabaseManager->setForeignKeysEnabled(false)){
    pvLastError = pvDatabaseManager->lastError();
    return false;
  }
  for(i = 0; i < sourceTables.size(); ++i){
    tableManager.clearFieldMap();
    /**
    if(sourceTables.at(i) == "TestNode_tbl"){
      tableManager.addFieldMapping("NodeId", "NodeIdentification", "", QVariant::String);
    }
    */
    if(!tableManager.copyTable(sourceTables.at(i), sourceTables.at(i), mdtSqlDatabaseManager::KeepExisting, sourceDbManager.database(), pvDatabaseManager->database())){
      pvLastError = tableManager.lastError();
      return false;
    }
  }
  if(!pvDatabaseManager->setForeignKeysEnabled(true)){
    pvLastError = pvDatabaseManager->lastError();
    return false;
  }

  return true;
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

  return true;
}

mdtError mdtTtDatabaseSchema::lastError() const
{
  return pvLastError;
}

bool mdtTtDatabaseSchema::setupTables() 
{
  // We must build tables list in correct order, regarding dependencies
  if(!setupVehicleTypeTable()){
    return false;
  }
  if(!setupConnectionTypeTable()){
    return false;
  }
  if(!setupConnectorTable()){
    return false;
  }
  if(!setupConnectorContactTable()){
    return false;
  }
  if(!setupArticleTable()){
    return false;
  }
  if(!setupArticleComponentTable()){
    return false;
  }
  if(!setupArticleConnectorTable()){
    return false;
  }
  if(!setupArticleConnectionTable()){
    return false;
  }
  if(!setupUnitTable()){
    return false;
  }
  if(!setupUnitConnectorTable()){
    return false;
  }
  if(!setupUnitConnectionTable()){
    return false;
  }
  if(!setupVehicleTypeUnitTable()){
    return false;
  }
  if(!setupLinkDirectionTable()){
    return false;
  }
  if(!setupLinkTypeTable()){
    return false;
  }
  if(!setupArticleLinkTable()){
    return false;
  }
  if(!setupLinkBeamTable()){
    return false;
  }
  if(!setupLinkBeamUnitStartTable()){
    return false;
  }
  if(!setupLinkBeamUnitEndTable()){
    return false;
  }
  if(!setupLinkTable()){
    return false;
  }
  if(!setupVehicleTypeLinkTable()){
    return false;
  }
  // Test tool part
  if(!setupTestNodeTable()){
    return false;
  }
  if(!setupTestNodeUnitTypeTable()){
    return false;
  }
  if(!setupTestNodeUnitTable()){
    return false;
  }
  if(!setupTestNodeBusTable()){
    return false;
  }
  if(!setupTestNodeUnitConnectionTable()){
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
  if(!setupTestModelItemRouteTable()){
    return false;
  }
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

bool mdtTtDatabaseSchema::createViews() 
{
  if(!createArticleComponentView()){
    return false;
  }
  if(!createArticleComponentUsageView()){
    return false;
  }
  if(!createArticleConnectorView()){
    return false;
  }
  if(!createArticleConnectionView()){
    return false;
  }
  if(!createArticleLinkView()){
    return false;
  }
  if(!createArticleLinkUnitConnectionView()){
    return false;
  }
  if(!createUnitLinkView()){
    return false;
  }
  if(!createUnitVehicleTypeView()){
    return false;
  }
  if(!createUnitView()){
    return false;
  }
  if(!createUnitComponentView()){
    return false;
  }
  if(!createUnitConnectorView()){
    return false;
  }
  if(!createUnitConnectorUsageView()){
    return false;
  }
  if(!createUnitConnectionView()){
    return false;
  }
  if(!createLinkListView()){
    return false;
  }
  if(!createLinkBeamUnitStartView()){
    return false;
  }
  if(!createTestNodeView()){
    return false;
  }
  if(!createLinkBeamUnitEndView()){
    return false;
  }
  if(!createVehicleTypeUnitView()){
    return false;
  }
  if(!createTestNodeUnitView()){
    return false;
  }
  if(!createUnitTestNodeView()){
    return false;
  }
  if(!createTestNodeUnitConnectionView()){
    return false;
  }
  if(!createTestLinkView()){
    return false;
  }
  if(!createTestCableTestNodeUnitView()){
    return false;
  }
  if(!createTestCableDutUnitView()){
    return false;
  }
  if(!createTestNodeTestCableView()){
    return false;
  }
  if(!createTestModelTestNodeView()){
    return false;
  }
  if(!createTestModelItemTestLinkView()){
    return false;
  }
  if(!createTestNodeUnitSetupView()){
    return false;
  }
  if(!createTestView()){
    return false;
  }
  if(!createTestItemView()){
    return false;
  }
  if(!createTestItemNodeUnitSetupView()){
    return false;
  }
  if(!createTestModelItemRouteView()){
    return false;
  }
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
  if(!populateConnectionTypeTable()){
    return false;
  }
  if(!populateLinkTypeTable()){
    return false;
  }
  if(!populateLinkDirectionTable()){
    return false;
  }
  if(!populateTestNodeUnitTypeTable()){
    return false;
  }
  return true;
}

bool mdtTtDatabaseSchema::setupVehicleTypeTable() 
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("VehicleType_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Type
  field = QSqlField();
  field.setName("Type");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // SubType
  field = QSqlField();
  field.setName("SubType");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // SubType
  field = QSqlField();
  field.setName("SeriesNumber");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  field = QSqlField();

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupVehicleTypeUnitTable() 
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("VehicleType_Unit_tbl", "UTF8");
  // VehicleType_Id_FK
  field = QSqlField();
  field.setName("VehicleType_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, true);
  // Unit_Id_FK
  field = QSqlField();
  field.setName("Unit_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, true);
  // Foreign keys
  table.addForeignKey("VehicleType_Id_FK_fk", "VehicleType_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("VehicleType_Id_FK_fk", "VehicleType_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("Unit_Id_FK_fk2", "Unit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Unit_Id_FK_fk2", "Unit_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupVehicleTypeLinkTable() 
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("VehicleType_Link_tbl", "UTF8");
  // VehicleTypeStart_Id_FK
  field.setName("VehicleTypeStart_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // VehicleTypeEnd_Id_FK
  field.setName("VehicleTypeEnd_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // UnitConnectionStart_Id_FK
  field.setName("UnitConnectionStart_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // UnitConnectionEnd_Id_FK
  field.setName("UnitConnectionEnd_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // Indexes
  table.addIndex("UnitConnectionIndex", false);
  if(!table.addFieldToIndex("UnitConnectionIndex", "UnitConnectionStart_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  if(!table.addFieldToIndex("UnitConnectionIndex", "UnitConnectionEnd_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("VehicleTypeEnd_Id_FK_idx", false);
  if(!table.addFieldToIndex("VehicleTypeEnd_Id_FK_idx", "VehicleTypeEnd_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("UnitConnection_fk", "Link_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("UnitConnection_fk", "UnitConnectionStart_Id_FK", "UnitConnectionStart_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  if(!table.addFieldToForeignKey("UnitConnection_fk", "UnitConnectionEnd_Id_FK", "UnitConnectionEnd_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("VehicleTypeStart_Id_FK_fk", "VehicleType_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("VehicleTypeStart_Id_FK_fk", "VehicleTypeStart_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("VehicleTypeEnd_Id_FK_fk", "VehicleType_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("VehicleTypeEnd_Id_FK_fk", "VehicleTypeEnd_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupConnectionTypeTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("ConnectionType_tbl", "UTF8");
  // Id_PK
  field.setName("Code_PK");
  field.setType(QVariant::String);
  field.setLength(1);
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

bool mdtTtDatabaseSchema::setupConnectorTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("Connector_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Gender
  field = QSqlField();
  field.setName("Gender");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // ContactQty
  field = QSqlField();
  field.setName("ContactQty");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Form
  field = QSqlField();
  field.setName("Form");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // Manufacturer
  field = QSqlField();
  field.setName("Manufacturer");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // Series
  field = QSqlField();
  field.setName("Series");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // Housing (could also be shell, hood, ...)
  field = QSqlField();
  field.setName("Housing");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // Insert
  field = QSqlField();
  field.setName("Insert");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // InsertRotation
  field = QSqlField();
  field.setName("InsertRotation");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // ManufacturerConfigCode
  field = QSqlField();
  field.setName("ManufacturerConfigCode");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // ManufacturerArticleCode
  field = QSqlField();
  field.setName("ManufacturerArticleCode");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupConnectorContactTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("ConnectorContact_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Connector_Id_FK
  field = QSqlField();
  field.setName("Connector_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // Name
  field = QSqlField();
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // ConnectionType_Code_FK [P/S/T] (Pin/Socket/Terminal)
  field = QSqlField();
  field.setName("ConnectionType_Code_FK");
  field.setType(QVariant::String);
  field.setLength(1);
  ///field.setDefaultValue("T");
  table.addField(field, false);
  // Indexes
  table.addIndex("Connector_Id_FK_idx", false);
  if(!table.addFieldToIndex("Connector_Id_FK_idx", "Connector_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("ConnectionType_Code_FK_idx", false);
  if(!table.addFieldToIndex("ConnectionType_Code_FK_idx", "ConnectionType_Code_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("Connector_Id_FK_fk", "Connector_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Connector_Id_FK_fk", "Connector_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("ConnectionType_Code_FK_fk", "ConnectionType_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("ConnectionType_Code_FK_fk", "ConnectionType_Code_FK", "Code_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupArticleTable() 
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("Article_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Coordinate
  field = QSqlField();
  field.setName("ArticleCode");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // Designation EN
  field = QSqlField();
  field.setName("DesignationEN");
  field.setType(QVariant::String);
  field.setLength(150);
  table.addField(field, false);
  // Designation FR
  field = QSqlField();
  field.setName("DesignationFR");
  field.setType(QVariant::String);
  field.setLength(150);
  table.addField(field, false);
  // Designation DE
  field = QSqlField();
  field.setName("DesignationDE");
  field.setType(QVariant::String);
  field.setLength(150);
  table.addField(field, false);
  // Designation IT
  field = QSqlField();
  field.setName("DesignationIT");
  field.setType(QVariant::String);
  field.setLength(150);
  table.addField(field, false);
  // IdentificationDocument
  field = QSqlField();
  field.setName("IdentificationDocument");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // Manufacturer
  field = QSqlField();
  field.setName("Manufacturer");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // Manufacturer type
  field = QSqlField();
  field.setName("ManufacturerType");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // Manufacturer type
  field = QSqlField();
  field.setName("ManufacturerCode");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // ManufacturerIdentificationDocument
  field = QSqlField();
  field.setName("ManufacturerIdentificationDocument");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // Unit
  field = QSqlField();
  field.setName("Unit");
  field.setType(QVariant::String);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  field.setDefaultValue("pce");
  table.addField(field, false);

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupArticleComponentTable() 
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("ArticleComponent_tbl", "UTF8");
  // Composite_Id_FK
  field.setName("Composite_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // Component_Id_FK
  field = QSqlField();
  field.setName("Component_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // ComponentQty
  field.setName("ComponentQty");
  field.setType(QVariant::Double);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // ComponentQtyUnit
  field = QSqlField();
  field.setName("ComponentQtyUnit");
  field.setType(QVariant::String);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // Foreign keys
  table.addForeignKey("Composite_Id_FK_fk2", "Article_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Composite_Id_FK_fk2", "Composite_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("Component_Id_FK_fk", "Article_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Component_Id_FK_fk", "Component_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupArticleConnectorTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("ArticleConnector_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Article_Id_FK
  field = QSqlField();
  field.setName("Article_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // Connector_Id_FK
  field = QSqlField();
  field.setName("Connector_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Connector Name
  field = QSqlField();
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(30);
  table.addField(field, false);
  // Indexes
  table.addIndex("Article_Id_FK_idx3", false);
  if(!table.addFieldToIndex("Article_Id_FK_idx3", "Article_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("Connector_Id_FK_idx2", false);
  if(!table.addFieldToIndex("Connector_Id_FK_idx2", "Connector_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("Article_Id_FK_fk3", "Article_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Article_Id_FK_fk3", "Article_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("Connector_Id_FK_fk2", "Connector_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Connector_Id_FK_fk2", "Connector_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupArticleConnectionTable() 
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("ArticleConnection_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Article_Id_FK
  field = QSqlField();
  field.setName("Article_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // ArticleConnector_Id_FK
  field = QSqlField();
  field.setName("ArticleConnector_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // ConnectionType_Code_FK [P/S/T] (Pin/Socket/Terminal)
  field = QSqlField();
  field.setName("ConnectionType_Code_FK");
  field.setType(QVariant::String);
  field.setLength(1);
  /// \todo Remove default value
  field.setDefaultValue("T");
  table.addField(field, false);
  // ArticleContactName
  field = QSqlField();
  field.setName("ArticleContactName");
  field.setType(QVariant::String);
  field.setLength(30);
  table.addField(field, false);
  // IoType
  field = QSqlField();
  field.setName("IoType");
  field.setType(QVariant::String);
  field.setLength(30);
  table.addField(field, false);
  // FunctionEN
  field = QSqlField();
  field.setName("FunctionEN");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // FunctionFR
  field = QSqlField();
  field.setName("FunctionFR");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // FunctionDE
  field = QSqlField();
  field.setName("FunctionDE");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // FunctionIT
  field = QSqlField();
  field.setName("FunctionIT");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // Indexes
  table.addIndex("Article_Id_FK_idx2", false);
  if(!table.addFieldToIndex("Article_Id_FK_idx2", "Article_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("ArticleConnector_Id_FK_idx", false);
  if(!table.addFieldToIndex("ArticleConnector_Id_FK_idx", "ArticleConnector_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("ConnectionType_Code_FK_idx2", false);
  if(!table.addFieldToIndex("ConnectionType_Code_FK_idx2", "ConnectionType_Code_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("Article_Id_FK_fk2", "Article_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Article_Id_FK_fk2", "Article_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("ArticleConnector_Id_FK_fk", "ArticleConnector_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("ArticleConnector_Id_FK_fk", "ArticleConnector_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("ConnectionType_Code_FK_fk2", "ConnectionType_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("ConnectionType_Code_FK_fk2", "ConnectionType_Code_FK", "Code_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupArticleLinkTable() 
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("ArticleLink_tbl", "UTF8");
  // ArticleConnectionStart_Id_FK
  field = QSqlField();
  field.setName("ArticleConnectionStart_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, true);
  // ArticleConnectionEnd_Id_FK
  field = QSqlField();
  field.setName("ArticleConnectionEnd_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, true);
  // SinceVersion
  field = QSqlField();
  field.setName("SinceVersion");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // Modification
  field = QSqlField();
  field.setName("Modification");
  field.setType(QVariant::String);
  field.setLength(20);
  table.addField(field, false);
  // Identification
  field = QSqlField();
  field.setName("Identification");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // LinkDirection_Code_FK
  field = QSqlField();
  field.setName("LinkDirection_Code_FK");
  field.setType(QVariant::String);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // LinkType_Code_FK
  field = QSqlField();
  field.setName("LinkType_Code_FK");
  field.setType(QVariant::String);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // Value
  field = QSqlField();
  field.setName("Value");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // Indexes
  table.addIndex("LinkType_Code_FK_idx", false);
  if(!table.addFieldToIndex("LinkType_Code_FK_idx", "LinkType_Code_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("LinkDirection_Code_FK_idx", false);
  if(!table.addFieldToIndex("LinkDirection_Code_FK_idx", "LinkDirection_Code_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("ArticleConnectionStart_Id_FK_fk", "ArticleConnection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("ArticleConnectionStart_Id_FK_fk", "ArticleConnectionStart_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("ArticleConnectionEnd_Id_FK_fk", "ArticleConnection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("ArticleConnectionEnd_Id_FK_fk", "ArticleConnectionEnd_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("LinkDirection_Code_FK_fk", "LinkDirection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("LinkDirection_Code_FK_fk", "LinkDirection_Code_FK", "Code_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("LinkType_Code_FK_fk", "LinkType_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("LinkType_Code_FK_fk", "LinkType_Code_FK", "Code_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupUnitTable() 
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("Unit_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Composite_Id_FK
  field = QSqlField();
  field.setName("Composite_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Article_Id_FK
  field = QSqlField();
  field.setName("Article_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Coordinate
  field = QSqlField();
  field.setName("Coordinate");
  field.setType(QVariant::String);
  field.setLength(10);
  table.addField(field, false);
  // Cabinet
  field = QSqlField();
  field.setName("Cabinet");
  field.setType(QVariant::String);
  field.setLength(30);
  table.addField(field, false);
  // SchemaPosition
  field = QSqlField();
  field.setName("SchemaPosition");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // Alias
  field = QSqlField();
  field.setName("Alias");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // Indexes
  table.addIndex("Article_Id_FK_idx", false);
  if(!table.addFieldToIndex("Article_Id_FK_idx", "Article_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("Composite_Id_FK_idx", false);
  if(!table.addFieldToIndex("Composite_Id_FK_idx", "Composite_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("Composite_Id_FK_fk", "Unit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Composite_Id_FK_fk", "Composite_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("Article_Id_FK_fk", "Article_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Article_Id_FK_fk", "Article_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupUnitConnectorTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("UnitConnector_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Unit_Id_FK
  field = QSqlField();
  field.setName("Unit_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // Connector_Id_FK
  field = QSqlField();
  field.setName("Connector_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // ArticleConnector_Id_FK
  field = QSqlField();
  field.setName("ArticleConnector_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Connector Name
  field = QSqlField();
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(30);
  table.addField(field, false);
  // Indexes
  table.addIndex("Unit_Id_FK_idx3", false);
  if(!table.addFieldToIndex("Unit_Id_FK_idx3", "Unit_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("Connector_Id_FK_idx3", false);
  if(!table.addFieldToIndex("Connector_Id_FK_idx3", "Connector_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("ArticleConnector_Unit_idx", true);
  if(!table.addFieldToIndex("ArticleConnector_Unit_idx", "ArticleConnector_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  if(!table.addFieldToIndex("ArticleConnector_Unit_idx", "Unit_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("Unit_Id_FK_fk3", "Unit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Unit_Id_FK_fk3", "Unit_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("Connector_Id_FK_fk3", "Connector_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Connector_Id_FK_fk3", "Connector_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("ArticleConnector_Id_FK_fk2", "ArticleConnector_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("ArticleConnector_Id_FK_fk2", "ArticleConnector_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupUnitConnectionTable() 
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("UnitConnection_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Unit_Id_FK
  field = QSqlField();
  field.setName("Unit_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // UnitConnector_Id_FK
  field = QSqlField();
  field.setName("UnitConnector_Id_FK");
  field.setType(QVariant::Int);
  ///field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // ArticleConnection_Id_FK
  field = QSqlField();
  field.setName("ArticleConnection_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // ConnectionType_Code_FK [P/S/T] (Pin/Socket/Terminal)
  field = QSqlField();
  field.setName("ConnectionType_Code_FK");
  field.setType(QVariant::String);
  field.setLength(1);
  /// \todo Remove default value
  /// \todo Set required
  field.setDefaultValue("T");
  table.addField(field, false);
  // IsATestPoint
  field = QSqlField();
  field.setName("IsATestPoint");
  field.setType(QVariant::Bool);
  table.addField(field, false);
  // SchemaPage
  field = QSqlField();
  field.setName("SchemaPage");
  field.setType(QVariant::String);
  field.setLength(20);
  table.addField(field, false);
  // FunctionEN
  field = QSqlField();
  field.setName("FunctionEN");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // FunctionFR
  field = QSqlField();
  field.setName("FunctionFR");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // FunctionDE
  field = QSqlField();
  field.setName("FunctionDE");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // FunctionIT
  field = QSqlField();
  field.setName("FunctionIT");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  // SignalName
  field = QSqlField();
  field.setName("SignalName");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // SwAddress
  field = QSqlField();
  field.setName("SwAddress");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // UnitContactName
  field = QSqlField();
  field.setName("UnitContactName");
  field.setType(QVariant::String);
  field.setLength(30);
  table.addField(field, false);
  // Indexes
  table.addIndex("Unit_Id_FK_ArticleConnection_Id_FK_idx", true);
  if(!table.addFieldToIndex("Unit_Id_FK_ArticleConnection_Id_FK_idx", "Unit_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  if(!table.addFieldToIndex("Unit_Id_FK_ArticleConnection_Id_FK_idx", "ArticleConnection_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("ArticleConnection_Id_FK_idx", false);
  if(!table.addFieldToIndex("ArticleConnection_Id_FK_idx", "ArticleConnection_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("UnitConnector_Id_FK_idx", false);
  if(!table.addFieldToIndex("UnitConnector_Id_FK_idx", "UnitConnector_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("ConnectionType_Code_FK_idx3", false);
  if(!table.addFieldToIndex("ConnectionType_Code_FK_idx3", "ConnectionType_Code_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("Unit_Id_FK_fk", "Unit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Unit_Id_FK_fk", "Unit_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("ArticleConnection_Id_FK_idx2", "ArticleConnection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("ArticleConnection_Id_FK_idx2", "ArticleConnection_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("UnitConnector_Id_FK_fk", "UnitConnector_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("UnitConnector_Id_FK_fk", "UnitConnector_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("ConnectionType_Code_FK_fk3", "ConnectionType_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("ConnectionType_Code_FK_fk3", "ConnectionType_Code_FK", "Code_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupLinkTable() 
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("Link_tbl", "UTF8");
  // UnitConnectionStart_Id_FK
  field.setName("UnitConnectionStart_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // UnitConnectionEnd_Id_FK
  field = QSqlField();
  field.setName("UnitConnectionEnd_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // ArticleConnectionStart_Id_FK
  field = QSqlField();
  field.setName("ArticleConnectionStart_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // ArticleConnectionEnd_Id_FK
  field = QSqlField();
  field.setName("ArticleConnectionEnd_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // SinceVersion
  field = QSqlField();
  field.setName("SinceVersion");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // Modification
  field = QSqlField();
  field.setName("Modification");
  field.setType(QVariant::String);
  field.setLength(20);
  table.addField(field, false);
  // Identification
  field = QSqlField();
  field.setName("Identification");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // LinkDirection_Code_FK
  field = QSqlField();
  field.setName("LinkDirection_Code_FK");
  field.setType(QVariant::String);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // LinkType_Code_FK
  field = QSqlField();
  field.setName("LinkType_Code_FK");
  field.setType(QVariant::String);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);
  // Value
  field = QSqlField();
  field.setName("Value");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // LinkBeam_Id_FK
  field = QSqlField();
  field.setName("LinkBeam_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Indexes
  table.addIndex("Direction_Id_FK_idx", false);
  if(!table.addFieldToIndex("Direction_Id_FK_idx", "LinkDirection_Code_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("Type_Id_FK_idx", false);
  if(!table.addFieldToIndex("Type_Id_FK_idx", "LinkType_Code_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("Link_ArticleLink_idx", false);
  if(!table.addFieldToIndex("Link_ArticleLink_idx", "ArticleConnectionStart_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  if(!table.addFieldToIndex("Link_ArticleLink_idx", "ArticleConnectionEnd_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("LinkBeam_Id_FK_idx", false);
  if(!table.addFieldToIndex("LinkBeam_Id_FK_idx", "LinkBeam_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("UnitConnectionStart_Id_FK_fk", "UnitConnection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("UnitConnectionStart_Id_FK_fk", "UnitConnectionStart_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("UnitConnectionEnd_Id_FK_fk", "UnitConnection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("UnitConnectionEnd_Id_FK_fk", "UnitConnectionEnd_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("Link_ArticleLink_fk", "ArticleLink_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Link_ArticleLink_fk", "ArticleConnectionStart_Id_FK", "ArticleConnectionStart_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  if(!table.addFieldToForeignKey("Link_ArticleLink_fk", "ArticleConnectionEnd_Id_FK", "ArticleConnectionEnd_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("LinkDirection_Code_FK_fk2", "LinkDirection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("LinkDirection_Code_FK_fk2", "LinkDirection_Code_FK", "Code_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("LinkType_Code_FK_fk2", "LinkType_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("LinkType_Code_FK_fk2", "LinkType_Code_FK", "Code_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("LinkBeam_Id_FK_fk", "LinkBeam_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("LinkBeam_Id_FK_fk", "LinkBeam_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupLinkBeamTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("LinkBeam_tbl", "UTF8");

  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Identification
  field = QSqlField();
  field.setName("Identification");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupLinkBeamUnitStartTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("LinkBeam_UnitStart_tbl", "UTF8");

  // UnitStart_Id_FK
  field.setName("Unit_Id_FK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // LinkBeam_Id_FK
  field = QSqlField();
  field.setName("LinkBeam_Id_FK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Indexes
  table.addIndex("Unit_Id_FK_idx4", false);
  if(!table.addFieldToIndex("Unit_Id_FK_idx4", "Unit_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("LinkBeam_Id_FK_idx2", false);
  if(!table.addFieldToIndex("LinkBeam_Id_FK_idx2", "LinkBeam_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("Unit_Id_FK_fk4", "Unit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Unit_Id_FK_fk4", "Unit_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("LinkBeam_Id_FK_fk2", "LinkBeam_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("LinkBeam_Id_FK_fk2", "LinkBeam_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupLinkBeamUnitEndTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("LinkBeam_UnitEnd_tbl", "UTF8");

  // UnitStart_Id_FK
  field.setName("Unit_Id_FK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // LinkBeam_Id_FK
  field = QSqlField();
  field.setName("LinkBeam_Id_FK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Indexes
  table.addIndex("Unit_Id_FK_idx5", false);
  if(!table.addFieldToIndex("Unit_Id_FK_idx5", "Unit_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("LinkBeam_Id_FK_idx3", false);
  if(!table.addFieldToIndex("LinkBeam_Id_FK_idx3", "LinkBeam_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("Unit_Id_FK_fk5", "Unit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Unit_Id_FK_fk5", "Unit_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("LinkBeam_Id_FK_fk3", "LinkBeam_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("LinkBeam_Id_FK_fk3", "LinkBeam_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupLinkDirectionTable() 
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("LinkDirection_tbl", "UTF8");
  // Code_PK
  field.setName("Code_PK");
  field.setType(QVariant::String);
  field.setLength(10);
  table.addField(field, true);
  // PictureAscii
  field = QSqlField();
  field.setName("PictureAscii");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
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

bool mdtTtDatabaseSchema::setupLinkTypeTable() 
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("LinkType_tbl", "UTF8");
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
  // ValueUnit
  field = QSqlField();
  field.setName("ValueUnit");
  field.setType(QVariant::String);
  field.setLength(10);
  table.addField(field, false);

  pvTables.append(table);

  return true;
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
  // NodeIdentification
  field = QSqlField();
  field.setName("NodeIdentification");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // DeviceIdentification
  field = QSqlField();
  field.setName("DeviceIdentification");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // Indexes
  table.addIndex("VehicleType_Id_FK_PK_idx", false);
  if(!table.addFieldToIndex("VehicleType_Id_FK_PK_idx", "VehicleType_Id_FK_PK")){
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
  field = QSqlField();
  field.setName("TestNodeBus_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Indexes
  table.addIndex("TestNodeUnit_Id_FK_idx2", false);
  if(!table.addFieldToIndex("TestNodeUnit_Id_FK_idx2", "TestNodeUnit_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("TestNodeBus_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestNodeBus_Id_FK_idx", "TestNodeBus_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
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
  table.addForeignKey("TestNodeBus_Id_FK_fk", "TestNodeBus_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNodeBus_Id_FK_fk", "TestNodeBus_Id_FK", "Id_PK")){
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

bool mdtTtDatabaseSchema::setupTestNodeBusTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestNodeBus_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // TestNode_Id_FK
  field = QSqlField();
  field.setName("TestNode_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // NameEN
  field = QSqlField();
  field.setName("NameEN");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // Indexes
  table.addIndex("TestNode_Id_FK_idx2", false);
  if(!table.addFieldToIndex("TestNode_Id_FK_idx2", "TestNode_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("TestNode_Id_FK_fk2", "TestNode_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNode_Id_FK_fk2", "TestNode_Id_FK", "VehicleType_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }

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
  table.addForeignKey("Link_Id_FK_fk", "Link_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Link_Id_FK_fk", "TestCableUnitConnectionStart_Id_FK", "UnitConnectionStart_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  if(!table.addFieldToForeignKey("Link_Id_FK_fk", "TestCableUnitConnectionEnd_Id_FK", "UnitConnectionEnd_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
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
  /**
  field.setName("TestLinkBusA_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // TestLinkBusB_Id_FK
  field = QSqlField();
  field.setName("TestLinkBusB_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  */
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
  // LimitValueMin
  field = QSqlField();
  field.setName("LimitValueMin");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // LimitValueMax
  field = QSqlField();
  field.setName("LimitValueMax");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // FailValueMin
  field = QSqlField();
  field.setName("FailValueMin");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // FailValueMax
  field = QSqlField();
  field.setName("FailValueMax");
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
  field = QSqlField();
  field.setName("TestModelItemRoute_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
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
  table.addIndex("TestModelItemRoute_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestModelItemRoute_Id_FK_idx", "TestModelItemRoute_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
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
  table.addForeignKey("TestModelItemRoute_Id_FK_fk", "TestModelItemRoute_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestModelItemRoute_Id_FK_fk", "TestModelItemRoute_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtTtDatabaseSchema::setupTestModelItemRouteTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestModelItemRoute_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // TestModelItem_Id_FK
  field = QSqlField();
  field.setName("TestModelItem_Id_FK");
  field.setType(QVariant::Int);
  field.setRequired(true);
  table.addField(field, false);
  // TestLink_Id_FK
  field = QSqlField();
  field.setName("TestLink_Id_FK");
  field.setType(QVariant::Int);
  field.setRequired(true);
  table.addField(field, false);
  // MeasureTestNodeUnitConnection_Id_FK
  field = QSqlField();
  field.setName("MeasureTestNodeUnitConnection_Id_FK");
  field.setType(QVariant::Int);
  field.setRequired(true);
  table.addField(field, false);
  // Indexes
  table.addIndex("TestModelItem_Id_FK_idx3", false);
  if(!table.addFieldToIndex("TestModelItem_Id_FK_idx3", "TestModelItem_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("TestLink_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestLink_Id_FK_idx", "TestLink_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("MeasureTestNodeUnitConnection_Id_FK_idx", false);
  if(!table.addFieldToIndex("MeasureTestNodeUnitConnection_Id_FK_idx", "MeasureTestNodeUnitConnection_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("TestModelItem_Id_FK_fk4", "TestModelItem_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestModelItem_Id_FK_fk4", "TestModelItem_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("TestLink_Id_FK_fk2", "TestLink_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestLink_Id_FK_fk2", "TestLink_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("MeasureTestNodeUnitConnection_Id_FK_fk", "TestNodeUnitConnection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("MeasureTestNodeUnitConnection_Id_FK_fk", "MeasureTestNodeUnitConnection_Id_FK", "UnitConnection_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }

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

bool mdtTtDatabaseSchema::createVehicleTypeUnitView() 
{
  QString sql;

  sql = "CREATE VIEW VehicleType_Unit_view AS\n"\
        "SELECT\n"\
        "VehicleType_tbl.Type ,\n"\
        "VehicleType_tbl.SubType ,\n"\
        "VehicleType_tbl.SeriesNumber ,\n"\
        "Unit_tbl.Coordinate ,\n"\
        "Unit_tbl.Cabinet ,\n"\
        "Unit_tbl.SchemaPosition ,\n"\
        "Unit_tbl.Alias ,\n"\
        "VehicleType_Unit_tbl.VehicleType_Id_FK ,\n"\
        "VehicleType_Unit_tbl.Unit_Id_FK\n"\
        "FROM VehicleType_Unit_tbl\n"\
        "JOIN VehicleType_tbl\n"\
        "  ON VehicleType_Unit_tbl.VehicleType_Id_FK = VehicleType_tbl.Id_PK\n"\
        "JOIN Unit_tbl\n"\
        "  ON VehicleType_Unit_tbl.Unit_Id_FK = Unit_tbl.Id_PK";

  return createView("VehicleType_Unit_view", sql);
}

bool mdtTtDatabaseSchema::createArticleComponentUsageView() 
{
  QString sql;

  sql = "CREATE VIEW ArticleComponentUsage_view AS\n"\
        "SELECT\n"\
        " ArticleComponent_tbl.Component_Id_FK AS Component_Id_PK ,\n"\
        " ArticleComponent_tbl.Composite_Id_FK AS Article_Id_PK ,\n"\
        " Article_tbl.ArticleCode ,\n"\
        " Article_tbl.DesignationEN,\n"\
        " Article_tbl.DesignationFR,\n"\
        " Article_tbl.DesignationDE,\n"\
        " Article_tbl.DesignationIT\n"\
        "FROM ArticleComponent_tbl\n"\
        " JOIN Article_tbl\n"\
        "  ON Article_tbl.Id_PK = ArticleComponent_tbl.Composite_Id_FK";

  return createView("ArticleComponentUsage_view", sql);
}

bool mdtTtDatabaseSchema::createArticleComponentView() 
{
  QString sql;

  sql = "CREATE VIEW ArticleComponent_view AS\n"\
        "SELECT\n"\
        " ArticleComponent_tbl.Composite_Id_FK AS Article_Id_PK ,\n"\
        " ArticleComponent_tbl.Component_Id_FK AS Component_Id_PK ,\n"\
        " ArticleComponent_tbl.ComponentQty ,\n"\
        " ArticleComponent_tbl.ComponentQtyUnit ,\n"\
        " Article_tbl.ArticleCode ,\n"\
        " Article_tbl.DesignationEN,\n"\
        " Article_tbl.DesignationFR,\n"\
        " Article_tbl.DesignationDE,\n"\
        " Article_tbl.DesignationIT\n"\
        "FROM ArticleComponent_tbl\n"\
        " JOIN Article_tbl\n"\
        "  ON Article_tbl.Id_PK = ArticleComponent_tbl.Component_Id_FK";

  return createView("ArticleComponent_view", sql);
}

bool mdtTtDatabaseSchema::createArticleConnectorView()
{
  QString sql;

  sql = "CREATE VIEW ArticleConnector_view AS\n"\
        "SELECT\n"\
        " ArticleConnector_tbl.Id_PK,\n"\
        " ArticleConnector_tbl.Article_Id_FK,\n"\
        " ArticleConnector_tbl.Connector_Id_FK,\n"\
        " ArticleConnector_tbl.Name AS ArticleConnectorName,\n"\
        " Connector_tbl.Gender,\n"\
        " Connector_tbl.Form,\n"\
        " Connector_tbl.Manufacturer,\n"\
        " Connector_tbl.ManufacturerConfigCode,\n"\
        " Connector_tbl.ManufacturerArticleCode\n"\
        "FROM ArticleConnector_tbl\n"\
        " LEFT JOIN Connector_tbl\n"\
        "  ON Connector_tbl.Id_PK = ArticleConnector_tbl.Connector_Id_FK";

  return createView("ArticleConnector_view", sql);
}

bool mdtTtDatabaseSchema::createArticleConnectionView()
{
  QString sql;

  sql = "CREATE VIEW ArticleConnection_view AS\n"\
        "SELECT\n"\
        " ArticleConnector_tbl.Connector_Id_FK,\n"\
        " ArticleConnector_tbl.Name AS ArticleConnectorName,\n"\
        " ArticleConnection_tbl.*\n"\
        "FROM ArticleConnection_tbl\n"\
        " LEFT JOIN ArticleConnector_tbl\n"\
        "  ON ArticleConnection_tbl.ArticleConnector_Id_FK = ArticleConnector_tbl.Id_PK";

  return createView("ArticleConnection_view", sql);
}

bool mdtTtDatabaseSchema::createArticleLinkView() 
{
  QString sql;

  sql = "CREATE VIEW ArticleLink_view AS\n"\
        "SELECT\n"\
        " ArticleLink_tbl.LinkType_Code_FK ,\n"\
        " ArticleLink_tbl.LinkDirection_Code_FK ,\n"\
        " ArticleLink_tbl.ArticleConnectionStart_Id_FK ,\n"\
        " ArticleLink_tbl.ArticleConnectionEnd_Id_FK ,\n"\
        " ArticleLink_tbl.Identification ,\n"\
        " LinkType_tbl.NameEN AS LinkTypeNameEN ,\n"\
        " LinkType_tbl.NameFR AS LinkTypeNameFR ,\n"\
        " LinkType_tbl.NameDE AS LinkTypeNameDE ,\n"\
        " LinkType_tbl.NameIT AS LinkTypeNameIT ,\n"\
        " ArticleLink_tbl.Value ,\n"\
        " LinkType_tbl.ValueUnit ,\n"\
        " ACNXS.Article_Id_FK AS StartArticle_Id_FK ,\n"\
        " ACS.Name AS StartArticleConnectorName ,\n"\
        " ACNXS.ArticleContactName AS StartArticleContactName ,\n"\
        " ACNXS.IoType AS StartIoType ,\n"\
        " ACNXS.FunctionEN AS StartFunctionEN ,\n"\
        " ACNXS.FunctionFR AS StartFunctionFR ,\n"\
        " ACNXS.FunctionDE AS StartFunctionDE ,\n"\
        " ACNXS.FunctionIT AS StartFunctionIT ,\n"\
        " LinkDirection_tbl.PictureAscii AS LinkDirectionPictureAscii ,\n"\
        " ACNXE.Article_Id_FK AS EndArticle_Id_FK ,\n"\
        " ACE.Name AS EndArticleConnectorName ,\n"\
        " ACNXE.ArticleContactName AS EndArticleContactName ,\n"\
        " ACNXE.IoType AS EndIoType ,\n"\
        " ACNXE.FunctionEN AS EndFunctionEN,\n"\
        " ACNXE.FunctionFR AS EndFunctionFR,\n"\
        " ACNXE.FunctionDE AS EndFunctionDE,\n"\
        " ACNXE.FunctionIT AS EndFunctionIT\n"\
        "FROM ArticleLink_tbl\n"\
        " JOIN ArticleConnection_tbl ACNXS\n"\
        "  ON ArticleLink_tbl.ArticleConnectionStart_Id_FK = ACNXS.Id_PK\n"\
        " JOIN ArticleConnection_tbl ACNXE\n"\
        "  ON ArticleLink_tbl.ArticleConnectionEnd_Id_FK = ACNXE.Id_PK\n"\
        " LEFT JOIN ArticleConnector_tbl ACS\n"\
        "  ON ACNXS.ArticleConnector_Id_FK = ACS.Id_PK\n"\
        " LEFT JOIN ArticleConnector_tbl ACE\n"\
        "  ON ACNXE.ArticleConnector_Id_FK = ACE.Id_PK\n"\
        " JOIN LinkType_tbl\n"\
        "  ON LinkType_tbl.Code_PK = ArticleLink_tbl.LinkType_Code_FK\n"\
        " JOIN LinkDirection_tbl\n"\
        "  ON LinkDirection_tbl.Code_PK = ArticleLink_tbl.LinkDirection_Code_FK";

  return createView("ArticleLink_view", sql);
}

bool mdtTtDatabaseSchema::createUnitView() 
{
  QString sql;

  sql = "CREATE VIEW Unit_view AS\n"\
        "SELECT\n"\
        " Unit_tbl.Id_PK AS Unit_Id_PK ,\n"\
        " Unit_tbl.SchemaPosition,\n"\
        " Unit_tbl.Alias ,\n"\
        " Unit_tbl.Coordinate ,\n"\
        " Unit_tbl.Cabinet ,\n"\
        " VehicleType_tbl.Id_PK AS VehicleType_Id_PK,\n"\
        " VehicleType_tbl.Type,\n"\
        " VehicleType_tbl.SubType,\n"\
        " VehicleType_tbl.SeriesNumber,\n"\
        " Article_tbl.Id_PK AS Article_Id_PK ,\n"\
        " Article_tbl.ArticleCode ,\n"\
        " Article_tbl.DesignationEN,\n"\
        " Article_tbl.DesignationFR,\n"\
        " Article_tbl.DesignationDE,\n"\
        " Article_tbl.DesignationIT\n"\
        "FROM Unit_tbl\n"\
        " LEFT JOIN Article_tbl\n"\
        "  ON Unit_tbl.Article_Id_FK = Article_tbl.Id_PK\n"\
        " JOIN VehicleType_Unit_tbl\n"\
        "  ON VehicleType_Unit_tbl.Unit_Id_FK = Unit_tbl.Id_PK\n"\
        " JOIN VehicleType_tbl\n"\
        "  ON VehicleType_tbl.Id_PK = VehicleType_Unit_tbl.VehicleType_Id_FK";

  return createView("Unit_view", sql);
}

bool mdtTtDatabaseSchema::createUnitComponentView() 
{
  QString sql;

  sql = "CREATE VIEW UnitComponent_view AS\n"\
        "SELECT\n"\
        " UnitComposite.Id_PK AS Unit_Id_PK ,\n"\
        " Unit_tbl.Id_PK AS UnitComponent_Id_PK ,\n"\
        " Unit_tbl.Article_Id_FK ,\n"\
        " Unit_tbl.Coordinate ,\n"\
        " Unit_tbl.Cabinet ,\n"\
        " Unit_tbl.SchemaPosition ,\n"\
        " Unit_tbl.Alias ,\n"\
        " Article_tbl.Id_PK AS Article_Id_PK ,\n"\
        " Article_tbl.ArticleCode ,\n"\
        " Article_tbl.DesignationEN,\n"\
        " Article_tbl.DesignationFR,\n"\
        " Article_tbl.DesignationDE,\n"\
        " Article_tbl.DesignationIT\n"\
        "FROM Unit_tbl AS UnitComposite\n"\
        " JOIN Unit_tbl\n"\
        "  ON Unit_tbl.Composite_Id_FK = UnitComposite.Id_PK\n"\
        " LEFT JOIN Article_tbl\n"\
        "  ON Unit_tbl.Article_Id_FK = Article_tbl.Id_PK";

  return createView("UnitComponent_view", sql);
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
        " Connector_tbl.ManufacturerConfigCode,\n"\
        " Connector_tbl.ManufacturerArticleCode\n"\
        "FROM UnitConnector_tbl\n"\
        " LEFT JOIN Connector_tbl\n"\
        "  ON Connector_tbl.Id_PK = UnitConnector_tbl.Connector_Id_FK\n"\
        " LEFT JOIN ArticleConnector_tbl\n"\
        "  ON ArticleConnector_tbl.Id_PK = UnitConnector_tbl.ArticleConnector_Id_FK";

  return createView("UnitConnector_view", sql);
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
        " UnitConnection_tbl.Id_PK AS UnitConnection_Id_PK ,\n"\
        " UnitConnection_tbl.Unit_Id_FK ,\n"\
        " UnitConnection_tbl.UnitConnector_Id_FK ,\n"\
        " UnitConnection_tbl.ConnectionType_Code_FK ,\n"\
        " UnitConnection_tbl.ArticleConnection_Id_FK,\n"\
        " UnitConnector_tbl.Name AS UnitConnectorName ,\n"\
        " UnitConnection_tbl.UnitContactName ,\n"\
        " UnitConnection_tbl.SchemaPage ,\n"\
        " UnitConnection_tbl.FunctionEN AS UnitFunctionEN,\n"\
        " UnitConnection_tbl.FunctionFR AS UnitFunctionFR,\n"\
        " UnitConnection_tbl.FunctionDE AS UnitFunctionDE,\n"\
        " UnitConnection_tbl.FunctionIT AS UnitFunctionIT,\n"\
        " UnitConnection_tbl.SignalName ,\n"\
        " UnitConnection_tbl.SwAddress ,\n"\
        " ArticleConnector_tbl.Name AS ArticleConnectorName ,\n"\
        " ArticleConnection_tbl.ArticleContactName ,\n"\
        " ArticleConnection_tbl.IoType ,\n"\
        " ArticleConnection_tbl.FunctionEN AS ArticleFunctionEN,\n"\
        " ArticleConnection_tbl.FunctionFR AS ArticleFunctionFR,\n"\
        " ArticleConnection_tbl.FunctionDE AS ArticleFunctionDE,\n"\
        " ArticleConnection_tbl.FunctionIT AS ArticleFunctionIT\n"\
        "FROM UnitConnection_tbl\n"\
        " LEFT JOIN UnitConnector_tbl\n"\
        "  ON UnitConnector_tbl.Id_PK = UnitConnection_tbl.UnitConnector_Id_FK\n"\
        " LEFT JOIN ArticleConnector_tbl\n"\
        "  ON UnitConnector_tbl.ArticleConnector_Id_FK = ArticleConnector_tbl.Id_PK\n"\
        " LEFT JOIN ArticleConnection_tbl\n"\
        "  ON UnitConnection_tbl.ArticleConnection_Id_FK = ArticleConnection_tbl.Id_PK\n";

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
        " AL.Value,\n"\
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
              " LNK.Identification ,\n"\
              " LNK.LinkBeam_Id_FK ,\n"\
              " US.SchemaPosition AS StartSchemaPosition ,\n"\
              " US.Alias AS StartAlias,\n"\
              " UCS.Name AS StartUnitConnectorName ,\n"\
              " UCNXS.UnitContactName AS StartUnitContactName ,\n"\
              " UE.SchemaPosition AS EndSchemaPosition ,\n"\
              " UE.Alias AS EndAlias,\n"\
              " UCE.Name AS EndUnitConnectorName ,\n"\
              " UCNXE.UnitContactName AS EndUnitContactName ,\n"\
              " LNK.SinceVersion ,\n"\
              " LNK.Modification ,\n"\
              " LinkType_tbl.NameEN AS LinkTypeNameEN ,\n"\
              " LNK.Value ,\n"\
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
              " LNK.ArticleConnectionEnd_Id_FK\n";
  sql = "CREATE VIEW UnitLink_view AS\n";
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
              " LNK.SinceVersion ,\n"\
              " LNK.Modification ,\n"\
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
          "  ON LinkDirection_tbl.Code_PK = LNK.LinkDirection_Code_FK\n";

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
        "TNUT.NameIT\n"\
        "FROM TestNodeUnit_tbl TNU\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNodeUnitType_tbl TNUT\n"\
        "  ON TNUT.Code_PK = TNU.Type_Code_FK";

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
        "TNUCNX.TestNodeBus_Id_FK,\n"\
        "UCNX.UnitContactName,\n"\
        "TNB.NameEN AS Bus\n"\
        "FROM TestNodeUnitConnection_tbl TNUCNX\n"\
        " JOIN UnitConnection_tbl UCNX\n"\
        "  ON UCNX.Id_PK = TNUCNX.UnitConnection_Id_FK_PK\n"\
        " LEFT JOIN UnitConnector_tbl UCNR\n"\
        "  ON UCNR.Id_PK = UCNX.UnitConnector_Id_FK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUCNX.TestNodeUnit_Id_FK\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " LEFT JOIN TestNodeBus_tbl TNB\n"\
        "  ON TNB.Id_PK = TNUCNX.TestNodeBus_Id_FK\n";

  return createView("TestNodeUnitConnection_view", sql);
}

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
        /**" LNK.Resistance AS TestLinkResistance,\n"\ */
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
        " TN.NodeIdentification,\n"\
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
        " TL.TestConnection_Id_FK,\n"\
        " TL.DutConnection_Id_FK,\n"\
        " TL.Identification,\n"\
        /**" TL.Resistance,\n"\ */
        " TNU.Unit_Id_FK_PK,\n"\
        " TNB.NameEN AS Bus,\n"\
        " TNU.IoPosition,\n"\
        " UTNU.SchemaPosition AS TestNodeUnitSchemaPosition,\n"\
        " UCT.Name AS TestConnectorName,\n"\
        " UCNXT.UnitContactName AS TestContactName,\n"\
        " UD.Id_PK AS DutUnitId,\n"\
        " UD.SchemaPosition AS DutUnitSchemaPosition,\n"\
        " UD.Alias AS DutUnitAlias,\n"\
        " UCD.Name AS DutConnectorName,\n"\
        " UCNXD.UnitContactName AS DutContactName,\n"\
        " VTN.Type AS TestNodeType,\n"\
        " VTN.SubType AS TestNodeSubType,\n"\
        " VTN.SeriesNumber AS TestNodeSeriesNumber,\n"\
        " TN.VehicleType_Id_FK_PK,\n"\
        " TN.NodeIdentification,\n"\
        " UVT.Type,\n"\
        " UVT.SubType,\n"\
        " UVT.SeriesNumber\n"\
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
        " LEFT JOIN TestNodeBus_tbl TNB\n"\
        "  ON TNB.Id_PK = TNUCNX.TestNodeBus_Id_FK\n"\
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
        " TNU.TestNode_Id_FK,\n"\
        " TNU.Type_Code_FK,\n"\
        " TNU.IoPosition,\n"\
        " U.SchemaPosition,\n"\
        " U.Alias,\n"\
        " TNUS.*,\n"\
        " TNUT.NameEN,\n"\
        " TNUT.NameFR,\n"\
        " TNUT.NameDE,\n"\
        " TNUT.NameIT\n"\
        "FROM TestNodeUnitSetup_tbl TNUS\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUS.TestNodeUnit_Id_FK\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNodeUnitType_tbl TNUT\n"\
        "  ON TNUT.Code_PK = TNU.Type_Code_FK";

  return createView("TestNodeUnitSetup_view", sql);
}

bool mdtTtDatabaseSchema::createTestModelItemRouteView()
{
  QString sql;

  sql = "CREATE VIEW TestModelItemRoute_view AS\n"\
        "SELECT\n"\
        " TMIR.*,\n"\
        " TL.Identification,\n"\
        " UT.SchemaPosition AS TestSchemaPosition,\n"\
        " UT.Alias AS TestAlias,\n"\
        " UCNXT.UnitContactName AS TestContactName,\n"\
        " UCNXT.FunctionEN AS TestContactFunctionEN,\n"\
        " UCNXT.FunctionFR AS TestContactFunctionFR,\n"\
        " UCNXT.FunctionDE AS TestContactFunctionDE,\n"\
        " UCNXT.FunctionIT AS TestContactFunctionIT,\n"\
        " GROUP_CONCAT(U.SchemaPosition, ', ') AS RouteTestNodeUnits,\n"\
        " UM.SchemaPosition AS MeasureSchemaPosition,\n"\
        " UM.Alias AS MeasureAlias,\n"\
        " UCNXM.UnitContactName AS MeasureContactName,\n"\
        " UCNXM.FunctionEN AS MeasureContactFunctionEN,\n"\
        " UCNXM.FunctionFR AS MeasureContactFunctionFR,\n"\
        " UCNXM.FunctionDE AS MeasureContactFunctionDE,\n"\
        " UCNXM.FunctionIT AS MeasureContactFunctionIT\n";
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
        " TMI.LimitValueMin,\n"\
        " TMI.LimitValueMax,\n"\
        " TMI.FailValueMin,\n"\
        " TMI.FailValueMax,\n"\
        " TI.Result,\n"\
        " TI.Remark\n"\
        "FROM TestModelItem_tbl TMI\n"\
        " JOIN TestItem_tbl TI\n"\
        "  ON TMI.Id_PK = TI.TestModelItem_Id_FK\n"\
        "ORDER BY SequenceNumber ASC";

  return createView("TestItem_view", sql);
}

bool mdtTtDatabaseSchema::createTestItemNodeUnitSetupView()
{
  QString sql;

  sql = "CREATE VIEW TestItemNodeUnitSetup_view AS\n"\
        "SELECT\n"\
        " TI.Id_PK,\n"\
        " TI.Test_Id_FK,\n"\
        " TI.TestModelItem_Id_FK,\n"\
        " TMI.TestModel_Id_FK,\n"\
        /*" TNUS.Id_PK,\n"\*/
        /*" TNUS.TestModelItem_Id_FK,\n"\*/
        " TMI.Key,\n"\
        " TMI.SequenceNumber,\n"\
        " TNUS.StepNumber,\n"\
        " U.SchemaPosition,\n"\
        " TNUS.TestNodeUnit_Id_FK,\n"\
        " TNUS.State,\n"\
        " TNUS.Value,\n"\
        /*" TNU.Bus,\n"\*/
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
  // Cable link type
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
  data << "ETS" << "-->" << "End to Start" << "Ende zum Start" << "Arrivée vers départ" << "Dall'fine alla inizio";
  if(!insertDataIntoTable("LinkDirection_tbl", fields, data)){
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
