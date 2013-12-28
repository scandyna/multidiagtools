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
#include "mdtClDatabaseSchema.h"
#include "mdtSqlDatabaseManager.h"
#include "mdtDataTableManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlField>
#include <QFileInfo>
#include <QStringList>

#include <QDebug>

mdtClDatabaseSchema::mdtClDatabaseSchema(mdtSqlDatabaseManager *dbManager) 
{
  Q_ASSERT(dbManager != 0);

  ///pvDatabaseManager = new mdtSqlDatabaseManager;
  pvDatabaseManager = dbManager;
}

mdtClDatabaseSchema::~mdtClDatabaseSchema() 
{
  ///delete pvDatabaseManager;
}

bool mdtClDatabaseSchema::createSchemaSqlite(const QDir & startDirectory)
{
  if(!pvDatabaseManager->createDatabaseSqlite(startDirectory)){
    return false;
  }
  return createSchemaSqlite();
}

bool mdtClDatabaseSchema::createSchemaSqlite(const QFileInfo & dbFileInfo) 
{
  if(!pvDatabaseManager->createDatabaseSqlite(dbFileInfo, mdtSqlDatabaseManager::OverwriteExisting)){
    return false;
  }
  return createSchemaSqlite();
}

bool mdtClDatabaseSchema::createSchemaSqlite()
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

bool mdtClDatabaseSchema::importDatabase(const QDir & startDirectory)
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

bool mdtClDatabaseSchema::importDatabase(const QFileInfo sourceDbFileInfo)
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDatabaseSchema");
    pvLastError.commit();
    return false;
  }
  // Get the list of source tables and check if we have all in current database
  sourceTables = sourceDbManager.database().tables(QSql::Tables);
  destinationTables = pvDatabaseManager->database().tables(QSql::Tables);
  for(i = 0; i < sourceTables.size(); ++i){
    if(!destinationTables.contains(sourceTables.at(i))){
      pvLastError.setError("Table '" + sourceTables.at(i) + "' does not exists in current database - Will be ignored", mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtClDatabaseSchema");
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
  sourceTables.removeAll("LinkDirection_tbl");
  sourceTables.removeAll("LinkType_tbl");
  sourceTables.removeAll("TestNodeUnitType_tbl");
  // Copy tables
  if(!pvDatabaseManager->setForeignKeysEnabled(false)){
    pvLastError = pvDatabaseManager->lastError();
    return false;
  }
  for(i = 0; i < sourceTables.size(); ++i){
    tableManager.clearFieldMap();
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

bool mdtClDatabaseSchema::checkSchema()
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
      MDT_ERROR_SET_SRC(pvLastError, "mdtClDatabaseSchema");
      pvLastError.commit();
      return false;
    }
  }

  return true;
}

mdtError mdtClDatabaseSchema::lastError() const
{
  return pvLastError;
}

bool mdtClDatabaseSchema::setupTables() 
{
  // We must build tables list in correct order, regarding dependencies
  if(!setupVehicleTypeTable()){
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
  if(!setupLinkTable()){
    return false;
  }
  if(!setupVehicleTypeLinkTable()){
    return false;
  }
  /// \todo Move to test tool when created
  if(!setupTestNodeTable()){
    return false;
  }
  if(!setupTestNodeUnitTypeTable()){
    return false;
  }
  if(!setupTestNodeUnitTable()){
    return false;
  }
  if(!setupTestCableTable()){
    return false;
  }
  if(!setupTestLinkTable()){
    return false;
  }
  if(!setupTestTable()){
    return false;
  }
  if(!setupTestItemTable()){
    return false;
  }
  if(!setupTestNodeUnitSetupTable()){
    return false;
  }  
  return true;
}

bool mdtClDatabaseSchema::createTablesSqlite() 
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

bool mdtClDatabaseSchema::createViews() 
{
  if(!createArticleComponentView()){
    return false;
  }
  if(!createArticleComponentUsageView()){
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
  if(!createUnitConnectionView()){
    return false;
  }
  if(!createLinkListView()){
    return false;
  }
  if(!createVehicleTypeUnitView()){
    return false;
  }
  if(!createTestNodeUnitView()){
    return false;
  }
  if(!createTestLinkView()){
    return false;
  }
  if(!createTestItemLinkView()){
    return false;
  }
  if(!createTestItemNodeUnitSetupView()){
    return false;
  }
  if(!createTestItemNodeView()){
    return false;
  }
  if(!createTestItemNodeUnitView()){
    return false;
  }
  return true;
}

bool mdtClDatabaseSchema::populateTables()
{
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

bool mdtClDatabaseSchema::setupVehicleTypeTable() 
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

bool mdtClDatabaseSchema::setupVehicleTypeUnitTable() 
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

bool mdtClDatabaseSchema::setupVehicleTypeLinkTable() 
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

bool mdtClDatabaseSchema::setupConnectorTable()
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

bool mdtClDatabaseSchema::setupConnectorContactTable()
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
  // Indexes
  table.addIndex("Connector_Id_FK_idx", false);
  if(!table.addFieldToIndex("Connector_Id_FK_idx", "Connector_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("Connector_Id_FK_fk", "Connector_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Connector_Id_FK_fk", "Connector_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtClDatabaseSchema::setupArticleTable() 
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

bool mdtClDatabaseSchema::setupArticleComponentTable() 
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

bool mdtClDatabaseSchema::setupArticleConnectorTable()
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

bool mdtClDatabaseSchema::setupArticleConnectionTable() 
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
  // ArticleConnectorName
  /**
  field = QSqlField();
  field.setName("ArticleConnectorName");
  field.setType(QVariant::String);
  field.setLength(30);
  table.addField(field, false);
  */
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

  pvTables.append(table);

  return true;
}

bool mdtClDatabaseSchema::setupArticleLinkTable() 
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

bool mdtClDatabaseSchema::setupUnitTable() 
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

bool mdtClDatabaseSchema::setupUnitConnectorTable()
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

bool mdtClDatabaseSchema::setupUnitConnectionTable() 
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
  // UnitConnectorName
  /**
  field = QSqlField();
  field.setName("UnitConnectorName");
  field.setType(QVariant::String);
  field.setLength(30);
  table.addField(field, false);
  */
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

  pvTables.append(table);

  return true;
}

bool mdtClDatabaseSchema::setupLinkTable() 
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

  pvTables.append(table);

  return true;
}

bool mdtClDatabaseSchema::setupLinkDirectionTable() 
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

bool mdtClDatabaseSchema::setupLinkTypeTable() 
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

bool mdtClDatabaseSchema::setupTestNodeTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestNode_tbl", "UTF8");
  // VehicleType_Id_FK_PK
  field.setName("VehicleType_Id_FK_PK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  // NodeId
  field = QSqlField();
  field.setName("NodeId");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Indexes
  table.addIndex("VehicleType_Id_FK_PK_idx", false);
  if(!table.addFieldToIndex("VehicleType_Id_FK_PK_idx", "VehicleType_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys - None (cannot add FK to table of another database)

  pvTables.append(table);

  return true;
}

bool mdtClDatabaseSchema::setupTestNodeUnitTable()
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
  table.addField(field, false);
  // Type_Code_FK
  field = QSqlField();
  field.setName("Type_Code_FK");
  field.setType(QVariant::String);
  field.setLength(10);
  table.addField(field, false);
  // TestConnection_Id_FK
  field = QSqlField();
  field.setName("TestConnection_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // (Measure) Bus
  field = QSqlField();
  field.setName("Bus");
  field.setType(QVariant::String);
  field.setLength(30);
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
  table.addIndex("TestConnection_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestConnection_Id_FK_idx", "TestConnection_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys - No FK to Unit_tbl, because it will be in another database
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
  table.addForeignKey("TestConnection_Id_FK_fk", "UnitConnection_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestConnection_Id_FK_fk", "TestConnection_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtClDatabaseSchema::setupTestNodeUnitTypeTable()
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

bool mdtClDatabaseSchema::setupTestCableTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestCable_tbl", "UTF8");
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

bool mdtClDatabaseSchema::setupTestLinkTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestLink_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // TestConnection_Id_FK
  field = QSqlField();
  field.setName("TestConnection_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // DutConnection_Id_FK
  field = QSqlField();
  field.setName("DutConnection_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // TestCable_Id_FK
  field = QSqlField();
  field.setName("TestCable_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // Identification
  field = QSqlField();
  field.setName("Identification");
  field.setType(QVariant::String);
  field.setLength(50);
  table.addField(field, false);
  // Value
  field = QSqlField();
  field.setName("Value");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // Indexes
  /**
  table.addIndex("TestConnection_Id_FK_idx2", false);
  if(!table.addFieldToIndex("TestConnection_Id_FK_idx2", "TestConnection_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  */
  table.addIndex("TestCable_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestCable_Id_FK_idx", "TestCable_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys - No FK to UnitConnection_tbl, because it will be in another database
  /**
  table.addForeignKey("TestConnection_Id_FK_fk2", "TestNodeUnit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestConnection_Id_FK_fk2", "TestConnection_Id_FK", "TestConnection_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  */
  table.addForeignKey("TestCable_Id_FK_fk", "TestCable_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestCable_Id_FK_fk", "TestCable_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtClDatabaseSchema::setupTestTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("Test_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Designation EN
  field = QSqlField();
  field.setName("DesignationEN");
  field.setType(QVariant::String);
  field.setLength(150);
  table.addField(field, false);

  pvTables.append(table);

  return true;
}

bool mdtClDatabaseSchema::setupTestItemTable()
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
  // TestLinkBusA_Id_FK
  field = QSqlField();
  field.setName("TestLinkBusA_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // TestLinkBusB_Id_FK
  field = QSqlField();
  field.setName("TestLinkBusB_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // ExpectedValue
  field = QSqlField();
  field.setName("ExpectedValue");
  field.setType(QVariant::Double);
  table.addField(field, false);
  // Indexes
  table.addIndex("Test_Id_FK_idx", false);
  if(!table.addFieldToIndex("Test_Id_FK_idx", "Test_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
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
  // Foreign keys
  table.addForeignKey("Test_Id_FK_fk", "Test_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Test_Id_FK_fk", "Test_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
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

  pvTables.append(table);

  return true;
}

bool mdtClDatabaseSchema::setupTestNodeUnitSetupTable()
{
  mdtSqlSchemaTable table;
  QSqlField field;

  table.setTableName("TestNodeUnitSetup_tbl", "UTF8");
  // Id_PK
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // TestItem_Id_FK
  field = QSqlField();
  field.setName("TestItem_Id_FK");
  field.setType(QVariant::Int);
  table.addField(field, false);
  // TestNodeUnit_Id_FK
  field = QSqlField();
  field.setName("TestNodeUnit_Id_FK");
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
  // Indexes
  table.addIndex("TestItem_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestItem_Id_FK_idx", "TestItem_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addIndex("TestNodeUnit_Id_FK_idx", false);
  if(!table.addFieldToIndex("TestNodeUnit_Id_FK_idx", "TestNodeUnit_Id_FK")){
    pvLastError = table.lastError();
    return false;
  }
  // Foreign keys
  table.addForeignKey("TestItem_Id_FK_fk", "TestItem_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestItem_Id_FK_fk", "TestItem_Id_FK", "Id_PK")){
    pvLastError = table.lastError();
    return false;
  }
  table.addForeignKey("TestNodeUnit_Id_FK_fk", "TestNodeUnit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("TestNodeUnit_Id_FK_fk", "TestNodeUnit_Id_FK", "Unit_Id_FK_PK")){
    pvLastError = table.lastError();
    return false;
  }

  pvTables.append(table);

  return true;
}

bool mdtClDatabaseSchema::createView(const QString & viewName, const QString & sql) 
{
  QSqlQuery query(pvDatabaseManager->database());
  QSqlError sqlError;
  QString dropSql;

  qDebug() << "Creating view " << viewName << " ...";
  ///qDebug() << "SQL: " << sql;
  // Drop view
  dropSql = "DROP VIEW IF EXISTS '" + viewName +"'";
  if(!query.exec(dropSql)){
    sqlError = query.lastError();
    pvLastError.setError("Unable to drop view '" + viewName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDatabaseSchema");
    pvLastError.commit();
    return false;
  }
  // Create view
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Unable to create view '" + viewName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDatabaseSchema");
    pvLastError.commit();
    return false;
  }
  
  ///qDebug() << "View " << viewName << " created (DONE)";

  return true;
}

bool mdtClDatabaseSchema::createTestNodeUnitView()
{
  QString sql;

  sql = "CREATE VIEW TestNodeUnit_view AS\n"\
        "SELECT\n"\
        "TestNodeUnit_tbl.Unit_Id_FK_PK,\n"\
        "TestNodeUnit_tbl.TestNode_Id_FK,\n"\
        "TestNodeUnit_tbl.Type_Code_FK,\n"\
        "TestNodeUnit_tbl.TestConnection_Id_FK,\n"\
        "Unit_tbl.SchemaPosition,\n"\
        "TestNodeUnit_tbl.Bus,\n"\
        "UnitConnector_tbl.Name AS UnitConnectorName,\n"\
        "UnitConnection_tbl.UnitContactName,\n"\
        "UnitConnection_tbl.SignalName,\n"\
        "UnitConnection_tbl.FunctionEN,\n"\
        "UnitConnection_tbl.FunctionFR,\n"\
        "UnitConnection_tbl.FunctionDE,\n"\
        "UnitConnection_tbl.FunctionIT,\n"\
        "TestNodeUnitType_tbl.NameEN,\n"\
        "TestNodeUnitType_tbl.NameFR,\n"\
        "TestNodeUnitType_tbl.NameDE,\n"\
        "TestNodeUnitType_tbl.NameIT\n"\
        "FROM TestNodeUnit_tbl\n"\
        " JOIN Unit_tbl\n"\
        "  ON Unit_tbl.Id_PK = TestNodeUnit_tbl.Unit_Id_FK_PK\n"\
        " LEFT JOIN UnitConnection_tbl\n"\
        "  ON UnitConnection_tbl.Id_PK = TestNodeUnit_tbl.TestConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl\n"\
        "  ON UnitConnector_tbl.Id_PK = UnitConnection_tbl.UnitConnector_Id_FK\n"\
        " JOIN TestNodeUnitType_tbl\n"\
        "  ON TestNodeUnitType_tbl.Code_PK = TestNodeUnit_tbl.Type_Code_FK";

  return createView("TestNodeUnit_view", sql);
}

bool mdtClDatabaseSchema::createVehicleTypeUnitView() 
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
        "VehicleType_Unit_tbl.VehicleType_Id_FK ,\n"\
        "VehicleType_Unit_tbl.Unit_Id_FK\n"\
        "FROM VehicleType_Unit_tbl\n"\
        "JOIN VehicleType_tbl\n"\
        "  ON VehicleType_Unit_tbl.VehicleType_Id_FK = VehicleType_tbl.Id_PK\n"\
        "JOIN Unit_tbl\n"\
        "  ON VehicleType_Unit_tbl.Unit_Id_FK = Unit_tbl.Id_PK";

  return createView("VehicleType_Unit_view", sql);
}

bool mdtClDatabaseSchema::createArticleComponentUsageView() 
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

bool mdtClDatabaseSchema::createArticleComponentView() 
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

bool mdtClDatabaseSchema::createArticleConnectionView()
{
  QString sql;

  sql = "CREATE VIEW ArticleConnection_view AS\n"\
        "SELECT\n"\
        "ArticleConnector_tbl.Name AS ArticleConnectorName,\n"\
        "ArticleConnection_tbl.*\n"\
        "FROM ArticleConnection_tbl\n"\
        " LEFT JOIN ArticleConnector_tbl\n"\
        "  ON ArticleConnection_tbl.ArticleConnector_Id_FK = ArticleConnector_tbl.Id_PK";

  return createView("ArticleConnection_view", sql);
}

bool mdtClDatabaseSchema::createArticleLinkView() 
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

bool mdtClDatabaseSchema::createUnitView() 
{
  QString sql;

  sql = "CREATE VIEW Unit_view AS\n"\
        "SELECT\n"\
        " VehicleType_tbl.Type,\n"\
        " VehicleType_tbl.SubType,\n"\
        " VehicleType_tbl.SeriesNumber,\n"\
        " Unit_tbl.Id_PK AS Unit_Id_PK ,\n"\
        " Unit_tbl.Coordinate ,\n"\
        " Unit_tbl.Cabinet ,\n"\
        " Unit_tbl.SchemaPosition ,\n"\
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

bool mdtClDatabaseSchema::createUnitComponentView() 
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

bool mdtClDatabaseSchema::createUnitConnectionView() 
{
  QString sql;

  sql = "CREATE VIEW UnitConnection_view AS\n"\
        "SELECT\n"\
        " UnitConnection_tbl.Id_PK AS UnitConnection_Id_PK ,\n"\
        " UnitConnection_tbl.Unit_Id_FK ,\n"\
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

bool mdtClDatabaseSchema::createArticleLinkUnitConnectionView()
{
  QString sql;

  sql = "CREATE VIEW ArticleLink_UnitConnection_view AS\n"\
        "SELECT\n"\
        " UCS.Id_PK AS UnitConnectionStart_Id_FK,\n"\
        " UCE.Id_PK AS UnitConnectionEnd_Id_FK,\n"\
        " ArticleLink_tbl.ArticleConnectionStart_Id_FK,\n"\
        " ArticleLink_tbl.ArticleConnectionEnd_Id_FK,\n"\
        " ArticleLink_tbl.LinkType_Code_FK,\n"\
        " ArticleLink_tbl.LinkDirection_Code_FK,\n"\
        " ArticleLink_tbl.Identification,\n"\
        " ArticleLink_tbl.Value,\n"\
        " ArticleLink_tbl.SinceVersion,\n"\
        " ArticleLink_tbl.Modification\n"\
        "FROM ArticleLink_tbl\n"\
        " JOIN ArticleConnection_tbl ACS\n"\
        "  ON ACS.Id_PK = ArticleLink_tbl.ArticleConnectionStart_Id_FK\n"\
        " JOIN ArticleConnection_tbl ACE\n"\
        "  ON ACE.Id_PK = ArticleLink_tbl.ArticleConnectionEnd_Id_FK\n"\
        " JOIN UnitConnection_tbl UCS\n"\
        "  ON UCS.ArticleConnection_Id_FK = ArticleLink_tbl.ArticleConnectionStart_Id_FK\n"\
        " JOIN UnitConnection_tbl UCE\n"\
        "  ON UCE.ArticleConnection_Id_FK = ArticleLink_tbl.ArticleConnectionEnd_Id_FK\n";

  return createView("ArticleLink_UnitConnection_view", sql);
}

bool mdtClDatabaseSchema::createUnitLinkView() 
{
  QString sql, selectSql;

  selectSql = "SELECT\n"\
              " LNK.Identification ,\n"\
              " US.SchemaPosition AS StartSchemaPosition ,\n"\
              /**" UCNXS.UnitConnectorName AS StartUnitConnectorName ,\n"\*/
              " UCS.Name AS StartUnitConnectorName ,\n"\
              " UCNXS.UnitContactName AS StartUnitContactName ,\n"\
              " UE.SchemaPosition AS EndSchemaPosition ,\n"\
              /**" UCNXE.UnitConnectorName AS EndUnitConnectorName ,\n"\*/
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
              " UCNXS.Unit_Id_FK AS StartUnit_Id_FK ,\n"\
              " UCNXE.Unit_Id_FK AS EndUnit_Id_FK ,\n"\
              " LNK.LinkType_Code_FK ,\n"\
              " LNK.LinkDirection_Code_FK ,\n"\
              " LNK.ArticleConnectionStart_Id_FK ,\n"\
              " LNK.ArticleConnectionEnd_Id_FK\n";
  sql = "CREATE VIEW UnitLink_view AS\n";
  sql += selectSql;
  sql +=  "FROM Link_tbl LNK\n"\
          " JOIN UnitConnection_tbl UCNXS\n"\
          "  ON LNK.UnitConnectionStart_Id_FK = UCNXS.Id_PK\n"\
          " JOIN UnitConnection_tbl UCNXE\n"\
          "  ON LNK.UnitConnectionEnd_Id_FK = UCNXE.Id_PK\n"\
          " LEFT JOIN UnitConnector_tbl UCS\n"\
          "  ON UCNXS.UnitConnector_Id_FK = UCS.Id_PK\n"\
          " LEFT JOIN UnitConnector_tbl UCE\n"\
          "  ON UCNXE.UnitConnector_Id_FK = UCE.Id_PK\n"\
          " JOIN Unit_tbl US\n"\
          "  ON US.Id_PK = UCNXS.Unit_Id_FK\n"\
          " JOIN Unit_tbl UE\n"\
          "  ON UE.Id_PK = UCNXE.Unit_Id_FK\n"\
          " JOIN LinkType_tbl\n"\
          "  ON LinkType_tbl.Code_PK = LNK.LinkType_Code_FK\n"\
          " JOIN LinkDirection_tbl\n"\
          "  ON LinkDirection_tbl.Code_PK = LNK.LinkDirection_Code_FK\n"
          " LEFT JOIN ArticleLink_UnitConnection_view\n"\
          "  ON ArticleLink_UnitConnection_view.ArticleConnectionStart_Id_FK = LNK.ArticleConnectionStart_Id_FK\n"\
          "  AND ArticleLink_UnitConnection_view.ArticleConnectionEnd_Id_FK = LNK.ArticleConnectionEnd_Id_FK\n";
  sql += "UNION\n";
  sql += selectSql;
  sql +=  "FROM ArticleLink_UnitConnection_view LNK\n"\
          " LEFT JOIN Link_tbl\n"\
          "  ON Link_tbl.ArticleConnectionStart_Id_FK = LNK.ArticleConnectionStart_Id_FK\n"\
          "  AND Link_tbl.ArticleConnectionEnd_Id_FK = LNK.ArticleConnectionEnd_Id_FK\n"\
          " JOIN UnitConnection_tbl UCNXS\n"\
          "  ON LNK.UnitConnectionStart_Id_FK = UCNXS.Id_PK\n"\
          " JOIN UnitConnection_tbl UCNXE\n"\
          "  ON LNK.UnitConnectionEnd_Id_FK = UCNXE.Id_PK\n"\
          " LEFT JOIN UnitConnector_tbl UCS\n"\
          "  ON UCNXS.UnitConnector_Id_FK = UCS.Id_PK\n"\
          " LEFT JOIN UnitConnector_tbl UCE\n"\
          "  ON UCNXE.UnitConnector_Id_FK = UCE.Id_PK\n"\
          " JOIN Unit_tbl US\n"\
          "  ON US.Id_PK = UCNXS.Unit_Id_FK\n"\
          " JOIN Unit_tbl UE\n"\
          "  ON UE.Id_PK = UCNXE.Unit_Id_FK\n"\
          " JOIN LinkType_tbl\n"\
          "  ON LinkType_tbl.Code_PK = LNK.LinkType_Code_FK\n"\
          " JOIN LinkDirection_tbl\n"\
          "  ON LinkDirection_tbl.Code_PK = LNK.LinkDirection_Code_FK\n";

  return createView("UnitLink_view", sql);
}

bool mdtClDatabaseSchema::createUnitVehicleTypeView() 
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

bool mdtClDatabaseSchema::createLinkListView() 
{
  QString sql, selectSql;

  selectSql = "SELECT\n"\
              " LNK.UnitConnectionStart_Id_FK ,\n"\
              " LNK.UnitConnectionEnd_Id_FK ,\n"\
              " VS.Type AS StartVehicleType ,\n"\
              " VS.SubType AS StartVehicleSubType ,\n"\
              " VS.SeriesNumber AS StartVehicleSerie,\n"\
              " LNK.SinceVersion ,\n"\
              " LNK.Modification ,\n"\
              " LNK.Identification ,\n"\
              " LNK.LinkDirection_Code_FK ,\n"\
              " LNK.LinkType_Code_FK ,\n"\
              " US.Id_PK AS UnitStart_Id_PK ,\n"\
              " US.SchemaPosition AS StartSchemaPosition,\n"\
              " US.Cabinet AS StartCabinet,\n"\
              " US.Coordinate AS StartCoordinate ,\n"\
              /**" UCNXS.UnitConnectorName AS StartUnitConnectorName ,\n"\*/
              " UCS.Name AS StartUnitConnectorName ,\n"\
              " UCNXS.UnitContactName AS StartUnitContactName ,\n"\
              " VE.Type AS EndVehicleType ,\n"\
              " VE.SubType AS EndVehicleSubType ,\n"\
              " VE.SeriesNumber AS EndVehicleSerie,\n"\
              " UE.Id_PK AS UnitEnd_Id_PK ,\n"\
              " UE.SchemaPosition AS EndSchemaPosition,\n"\
              " UE.Cabinet AS EndCabinet,\n"\
              " UE.Coordinate AS EndCoordinate ,\n"\
              /**" UCNXE.UnitConnectorName AS EndUnitConnectorName ,\n"\*/
              " UCE.Name AS EndUnitConnectorName ,\n"\
              " UCNXE.UnitContactName AS EndUnitContactName\n";
  sql = "CREATE VIEW LinkList_view AS\n";
  sql += selectSql;
  sql +=  "FROM Link_tbl LNK\n"\
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
          " JOIN VehicleType_Link_tbl\n"\
          "  ON VehicleType_Link_tbl.UnitConnectionStart_Id_FK = LNK.UnitConnectionStart_Id_FK\n"\
          "  AND VehicleType_Link_tbl.UnitConnectionEnd_Id_FK = LNK.UnitConnectionEnd_Id_FK\n"\
          " JOIN VehicleType_tbl VS\n"\
          "  ON VS.Id_PK = VehicleType_Link_tbl.VehicleTypeStart_Id_FK\n"\
          " JOIN VehicleType_tbl VE\n"\
          "  ON VE.Id_PK = VehicleType_Link_tbl.VehicleTypeEnd_Id_FK\n"\
          " LEFT JOIN ArticleLink_UnitConnection_view\n"\
          "  ON ArticleLink_UnitConnection_view.ArticleConnectionStart_Id_FK = LNK.ArticleConnectionStart_Id_FK\n"\
          "  AND ArticleLink_UnitConnection_view.ArticleConnectionEnd_Id_FK = LNK.ArticleConnectionEnd_Id_FK\n";
  sql += "UNION\n";
  sql += selectSql;
  sql +=  "FROM ArticleLink_UnitConnection_view LNK\n"\
          " LEFT JOIN Link_tbl\n"\
          "  ON Link_tbl.ArticleConnectionStart_Id_FK = LNK.ArticleConnectionStart_Id_FK\n"\
          "  AND Link_tbl.ArticleConnectionEnd_Id_FK = LNK.ArticleConnectionEnd_Id_FK\n"\
          " JOIN UnitConnection_tbl UCNXS\n"\
          "  ON LNK.UnitConnectionStart_Id_FK = UCNXS.Id_PK\n"\
          " JOIN UnitConnection_tbl UCNXE\n"\
          "  ON LNK.UnitConnectionEnd_Id_FK = UCNXE.Id_PK\n"\
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
          "  ON VE.Id_PK = VehicleType_Link_tbl.VehicleTypeEnd_Id_FK";

  return createView("LinkList_view", sql);
}

bool mdtClDatabaseSchema::createTestLinkView()
{
  QString sql;

  sql = "CREATE VIEW TestLink_view AS\n"\
        "SELECT\n"\
        " LNK.Id_PK,\n"\
        " LNK.TestConnection_Id_FK,\n"\
        " LNK.DutConnection_Id_FK,\n"\
        " VTN.Type AS TestNodeType,\n"\
        " VTN.SubType AS TestNodeSubType,\n"\
        " VTN.SeriesNumber AS TestNodeSeriesNumber,\n"\
        " TN.NodeId,\n"\
        " TNU.Bus,\n"\
        " UTNU.SchemaPosition AS TestNodeUnitSchemaPosition,\n"\
        " UCT.Name AS TestConnectorName,\n"\
        " UCNXT.UnitContactName AS TestContactName,\n"\
        " LNK.Identification AS TestLinkIdentification,\n"\
        " LNK.Value AS TestLinkValue,\n"\
        " UD.SchemaPosition AS DutUnitSchemaPosition,\n"\
        " UCD.Name AS DutConnectorName,\n"\
        " UCNXD.UnitContactName AS DutContactName\n"\
        "FROM TestLink_tbl LNK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.TestConnection_Id_FK = LNK.TestConnection_Id_FK\n"\
        " JOIN Unit_tbl UTNU\n"\
        "  ON UTNU.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n"\
        " JOIN VehicleType_tbl VTN\n"\
        "  ON VTN.Id_PK = TN.VehicleType_Id_FK_PK\n"\
        " JOIN UnitConnection_tbl UCNXT\n"\
        "  ON UCNXT.Id_PK = LNK.TestConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCT\n"\
        "  ON UCT.Id_PK = UCNXT.UnitConnector_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXD\n"\
        "  ON UCNXD.Id_PK = LNK.DutConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCD\n"\
        "  ON UCD.Id_PK = UCNXD.UnitConnector_Id_FK\n"\
        " JOIN Unit_tbl UD\n"\
        "  ON UD.Id_PK = UCNXD.Unit_Id_FK\n";

  return createView("TestLink_view", sql);
}

bool mdtClDatabaseSchema::createTestItemLinkView()
{
  QString sql;

  sql = "CREATE VIEW TestItemLink_view AS\n"\
        "SELECT\n"\
        " TI.Id_PK AS TestItemId,\n"\
        " TI.Test_Id_FK,\n"\
        " TI.TestLinkBusA_Id_FK,\n"\
        " TI.TestLinkBusB_Id_FK,\n"\
        " TI.ExpectedValue,\n"\
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
        "FROM TestItem_tbl TI\n"\
        " JOIN TestLink_tbl LNKA\n"\
        "  ON LNKA.Id_PK = TI.TestLinkBusA_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXTA\n"\
        "  ON UCNXTA.Id_PK = LNKA.TestConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCTA\n"\
        "  ON UCTA.Id_PK = UCNXTA.UnitConnector_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXDA\n"\
        "  ON UCNXDA.Id_PK = LNKA.DutConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCDA\n"\
        "  ON UCDA.Id_PK = UCNXDA.UnitConnector_Id_FK\n"\
        " JOIN Unit_tbl UDA\n"\
        "  ON UDA.Id_PK = UCNXDA.Unit_Id_FK\n"\
        " JOIN TestLink_tbl LNKB\n"\
        "  ON LNKB.Id_PK = TI.TestLinkBusB_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXTB\n"\
        "  ON UCNXTB.Id_PK = LNKB.TestConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCTB\n"\
        "  ON UCTB.Id_PK = UCNXTB.UnitConnector_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXDB\n"\
        "  ON UCNXDB.Id_PK = LNKB.DutConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCDB\n"\
        "  ON UCDB.Id_PK = UCNXDB.UnitConnector_Id_FK\n"\
        " JOIN Unit_tbl UDB\n"\
        "  ON UDB.Id_PK = UCNXDB.Unit_Id_FK\n";

  return createView("TestItemLink_view", sql);
}

bool mdtClDatabaseSchema::createTestItemNodeUnitSetupView()
{
  QString sql;

  sql = "CREATE VIEW TestItemNodeUnitSetup_view AS\n"\
        "SELECT\n"\
        " TI.Test_Id_FK,\n"\
        " TNUS.Id_PK,\n"\
        " TNUS.TestItem_Id_FK,\n"\
        " TNUS.State,\n"\
        " TNUS.Value,\n"\
        " TNU.Bus,\n"\
        " TN.NodeId\n"\
        "FROM TestNodeUnitSetup_tbl TNUS\n"\
        " JOIN TestItem_tbl TI\n"\
        "  ON TI.Id_PK = TNUS.TestItem_Id_FK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUS.TestNodeUnit_Id_FK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n";

  return createView("TestItemNodeUnitSetup_view", sql);
}

bool mdtClDatabaseSchema::createTestItemNodeView()
{
  QString sql;

  sql = "CREATE VIEW TestItemNode_view AS\n"\
        "SELECT DISTINCT\n"\
        " TI.Test_Id_FK,\n"\
        " TN.VehicleType_Id_FK_PK AS TestNode_Id_FK,\n"\
        " TN.NodeId\n"\
        "FROM TestItem_tbl TI\n"\
        " JOIN TestLink_tbl LNKA\n"\
        "  ON LNKA.Id_PK = TI.TestLinkBusA_Id_FK\n"\
        " JOIN TestLink_tbl LNKB\n"\
        "  ON LNKB.Id_PK = TI.TestLinkBusB_Id_FK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.TestConnection_Id_FK = LNKA.TestConnection_Id_FK\n"\
        "  OR TNU.TestConnection_Id_FK = LNKB.TestConnection_Id_FK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK";

  return createView("TestItemNode_view", sql);
}

bool mdtClDatabaseSchema::createTestItemNodeUnitView()
{
  QString sql;

  sql = "CREATE VIEW TestItemNodeUnit_view AS\n"\
        "SELECT\n"\
        " TI.Id_PK,\n"\
        " TI.Test_Id_FK,\n"\
        " TNU.Unit_Id_FK_PK,\n"\
        " U.SchemaPosition,\n"\
        " TNU.Bus,\n"\
        " TNU.Type_Code_FK,\n"\
        " TN.VehicleType_Id_FK_PK AS TestNode_Id_FK,\n"\
        " TN.NodeId\n"\
        "FROM TestItem_tbl TI\n"\
        " JOIN TestLink_tbl LNKA\n"\
        "  ON LNKA.Id_PK = TI.TestLinkBusA_Id_FK\n"\
        " JOIN TestLink_tbl LNKB\n"\
        "  ON LNKB.Id_PK = TI.TestLinkBusB_Id_FK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.TestConnection_Id_FK = LNKA.TestConnection_Id_FK\n"\
        "  OR TNU.TestConnection_Id_FK = LNKB.TestConnection_Id_FK\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK";

  return createView("TestItemNodeUnit_view", sql);
}

bool mdtClDatabaseSchema::pkExistsInTable(const QString & tableName, const QString & pkField, const QVariant & pkData)
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDatabaseSchema");
    pvLastError.commit();
    return false;
  }
  if(query.next()){
    return true;
  }

  return false;
}

bool mdtClDatabaseSchema::insertDataIntoTable(const QString & tableName, const QStringList & fields, const QList<QVariant> & data)
{
  Q_ASSERT(fields.size() > 0);
  Q_ASSERT(data.size() == fields.size());

  QSqlQuery query(pvDatabaseManager->database());
  QSqlError sqlError;
  QString sql;
  int i;
  bool pkExists;

  /**
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
  */
  pkExists = pkExistsInTable(tableName, fields.at(0), data.at(0));
  ///pkExists = true;
  if(pkExists){
    sql = sqlForDataEdition(tableName, fields, data);
  }else{
    sql = sqlForDataInsertion(tableName, fields, data);
  }
  qDebug() << "INSERT SQL: " << sql;
  // Prepare query for insertion
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for insertion into table '" + tableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDatabaseSchema");
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
    qDebug() << "i: " << i << " , data: " << data.at(i);
    query.bindValue(i, data.at(i));
  }
  // Exec query
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for insertion into table '" + tableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDatabaseSchema");
    pvLastError.commit();
    return false;
  }

  return true;
}

QString mdtClDatabaseSchema::sqlForDataInsertion(const QString & tableName, const QStringList & fields, const QList<QVariant> & data)
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

QString mdtClDatabaseSchema::sqlForDataEdition(const QString & tableName, const QStringList & fields, const QList<QVariant> & data)
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

bool mdtClDatabaseSchema::populateLinkTypeTable()
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

bool mdtClDatabaseSchema::populateLinkDirectionTable()
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

bool mdtClDatabaseSchema::populateTestNodeUnitTypeTable()
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

  return true;
}
