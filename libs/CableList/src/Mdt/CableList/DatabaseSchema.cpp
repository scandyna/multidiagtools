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
 ** MERCHANTABILITY or FITNESS FOR A PARTMICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "DatabaseSchema.h"
#include "mdtSqlIndex.h"
#include "mdtSqlForeignKey.h"
#include "mdtSqlViewSchema.h"
#include "mdtSqlTriggerSchema.h"
#include "mdtSqlTablePopulationSchema.h"
#include <QObject>

#include <QDebug>

namespace Mdt{ namespace CableList{

DatabaseSchema::DatabaseSchema()
{
  buildSchema();
}

bool DatabaseSchema::createSchema(const QSqlDatabase & db)
{
  Q_ASSERT(db.isOpen());
  return pvSchema.createSchema(db);
}

bool DatabaseSchema::checkSchema(const QSqlDatabase & db)
{
  Q_ASSERT(db.isOpen());

  const auto dbTable = db.tables(QSql::AllTables);
  /*
   * Check that we have all needed tables in database
   */
  const auto tables = pvSchema.tableList();
  for(const auto & table : tables){
    if(!dbTable.contains(table.tableName())){
      pvLastError = mdtErrorNew(tr("Table '") + table.tableName() + tr("' is missing in database."), mdtError::Error, "Mdt::CableList::DatabaseSchema");
      pvLastError.commit();
      return false;
    }
  }
  /*
   * Check that we have all views in database
   */
  const auto views = pvSchema.viewList();
  for(const auto & view : views){
    if(!dbTable.contains(view.name())){
      pvLastError = mdtErrorNew(tr("View '") + view.name() + tr("' is missing in database."), mdtError::Error, "Mdt::CableList::DatabaseSchema");
      pvLastError.commit();
      return false;
    }
  }

  return true;
}

// bool mdtTtDatabaseSchema::checkSchema()
// {
//   Q_ASSERT(pvDatabaseManager->database().isOpen());
// 
//   QStringList dbTables;
//   int i;
// 
//   // We call setup methods to build the list of expected tables
//   pvTables.clear();
//   setupTables();
//   // Get database available tables and check
//   dbTables = pvDatabaseManager->database().tables();
//   for(i = 0; i < pvTables.size(); ++i){
//     if(!dbTables.contains(pvTables.at(i).tableName())){
//       pvLastError.setError("Table '" + pvTables.at(i).tableName() + "' is missing in database '" + pvDatabaseManager->database().databaseName() + "'.", mdtError::Error);
//       MDT_ERROR_SET_SRC(pvLastError, "mdtTtDatabaseSchema");
//       pvLastError.commit();
//       return false;
//     }
//   }
//   /// \todo Sould'nt we clear tables ??
// 
//   return true;
// }

void DatabaseSchema::buildSchema()
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
  setupUnitConnectorUsageView();
  setupUnitConnectionView();
  setupArticleLinkUnitConnectionView();
  setupUnitLinkView();
  setupUnitVehicleTypeView();
  setupLinkListView();
  setupLinkBeamUnitStartView();
  setupLinkBeamUnitEndView();
  
  // Setup table population
  setupModificationTablePopulation();
  setupConnectionTypeTablePopulation();
  setupLinkTypeTablePopulation();
  setupLinkDirectionTablePopulation();
  // Setup triggers
  setupOnLinkVersionAfterInsertTrigger();

}

void DatabaseSchema::setupVehicleTypeTable() 
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupVehicleTypeUnitTable() 
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupVehicleTypeLinkTable() 
{
  mdtSqlTableSchema table;
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
  VehicleTypeStart_Id_FK.setRequired(true);
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
  VehicleTypeEnd_Id_FK.setRequired(true);
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
  UnitConnectionStart_Id_FK.setRequired(true);
  table.addField(UnitConnectionStart_Id_FK, true);
  // UnitConnectionEnd_Id_FK
  UnitConnectionEnd_Id_FK.setName("UnitConnectionEnd_Id_FK");
  UnitConnectionEnd_Id_FK.setType(mdtSqlFieldType::Integer);
  UnitConnectionEnd_Id_FK.setRequired(true);
  table.addField(UnitConnectionEnd_Id_FK, true);
  // Link_Version_FK (refers to Link_tbl)
  Link_Version_FK.setName("Link_Version_FK");
  Link_Version_FK.setType(mdtSqlFieldType::Integer);
  Link_Version_FK.setRequired(true);
  table.addField(Link_Version_FK, true);
  // Link_Modification_Code_FK (refers to Link_tbl)
  Link_Modification_Code_FK.setName("Link_Modification_Code_FK");
  Link_Modification_Code_FK.setType(mdtSqlFieldType::Varchar);
  Link_Modification_Code_FK.setLength(10);
  Link_Modification_Code_FK.setRequired(true);
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

void DatabaseSchema::setupConnectionTypeTable()
{
  mdtSqlTableSchema table;
  mdtSqlField Code_PK;
  mdtSqlField NameEN;
  mdtSqlField NameFR;
  mdtSqlField NameDE;
  mdtSqlField NameIT;

  table.setTableName("ConnectionType_tbl", "UTF8");
  // Code_PK
  Code_PK.setName("Code_PK");
  Code_PK.setType(mdtSqlFieldType::Varchar);
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

void DatabaseSchema::setupConnectorTable()
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupConnectorContactTable()
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupArticleTable() 
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupArticleComponentTable() 
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupArticleConnectorTable()
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupArticleConnectionTable() 
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupArticleLinkTable() 
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupUnitTable() 
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupUnitConnectorTable()
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupUnitConnectionTable() 
{
  mdtSqlTableSchema table;
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
  fk_ConnectionType.addKeyFields("Code_PK", ConnectionType_Code_FK);
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

void DatabaseSchema::setupWireTable()
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupModificationTable()
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupLinkVersionTable()
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupOnLinkVersionAfterInsertTrigger()
{
  mdtSqlTriggerSchema trigger;
  QString script;

  trigger.setName("onLinkVersionAfterInsert");
  trigger.setEvent(mdtSqlTriggerSchema::AfterInsert);
  trigger.setTableName("LinkVersion_tbl");
  script = " INSERT INTO Link_tbl (UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK, Version_FK, Modification_Code_FK, "\
                                  "LinkType_Code_FK, LinkDirection_Code_FK, ArticleConnectionStart_Id_FK, ArticleConnectionEnd_Id_FK, "\
                                  "Wire_Id_FK, LinkBeam_Id_FK, Identification, Resistance, Length, Remarks)\n"\
           "  SELECT DISTINCT L.UnitConnectionStart_Id_FK, L.UnitConnectionEnd_Id_FK, NEW.Version_PK, 'EXISTS', "\
                                  "L.LinkType_Code_FK, L.LinkDirection_Code_FK, L.ArticleConnectionStart_Id_FK, L.ArticleConnectionEnd_Id_FK, "\
                                  "L.Wire_Id_FK, L.LinkBeam_Id_FK, L.Identification, L.Resistance, L.Length, L.Remarks\n"\
           "   FROM Link_tbl L\n"\
           "  WHERE L.Version_FK = (SELECT max(Version_FK) FROM Link_tbl)\n"\
           "  AND (L.Modification_Code_FK NOT IN ('REM') OR L.Modification_Code_FK IS NULL);\n";
  trigger.setScript(script);

  pvSchema.addTrigger(trigger);
}


void DatabaseSchema::setupLinkTable() 
{
  mdtSqlTableSchema table;
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
  UnitConnectionStart_Id_FK.setRequired(true);
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
  UnitConnectionEnd_Id_FK.setRequired(true);
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
  Version_FK.setRequired(true);
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
  Modification_Code_FK.setRequired(true);
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
  fk_LinkType.addKeyFields("Code_PK", LinkType_Code_FK);
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

void DatabaseSchema::setupLinkBeamTable()
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupLinkBeamUnitStartTable()
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupLinkBeamUnitEndTable()
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupLinkDirectionTable() 
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupLinkTypeTable() 
{
  mdtSqlTableSchema table;
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

void DatabaseSchema::setupVehicleTypeUnitView()
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

void DatabaseSchema::setupArticleComponentUsageView()
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

void DatabaseSchema::setupArticleComponentView()
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

void DatabaseSchema::setupArticleConnectorView()
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

void DatabaseSchema::setupArticleConnectionView()
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

void DatabaseSchema::setupArticleLinkView()
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

void DatabaseSchema::setupUnitView()
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

void DatabaseSchema::setupUnitComponentView()
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

void DatabaseSchema::setupUnitConnectorView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table UCNR("UnitConnector_tbl", "UCNR");
  Table CNR("Connector_tbl", "CNR");
  Table ACNR("ArticleConnector_tbl", "ACNR");
  JoinClause join;

  view.setName("UnitConnector_view");
  view.setTable(UCNR);
  view.addSelectField(UCNR, SelectField("Id_PK"));
  view.addSelectField(UCNR, SelectField("Unit_Id_FK"));
  view.addSelectField(UCNR, SelectField("Connector_Id_FK"));
  view.addSelectField(UCNR, SelectField("ArticleConnector_Id_FK"));
  view.addSelectField(UCNR, SelectField("Name", "ArticleConnectorName"));
  view.addSelectField(CNR, SelectField("Gender"));
  view.addSelectField(CNR, SelectField("Form"));
  view.addSelectField(CNR, SelectField("Manufacturer"));
  view.addSelectField(CNR, SelectField("Housing"));
  view.addSelectField(CNR, SelectField("Insert"));
  view.addSelectField(CNR, SelectField("InsertRotation"));
  view.addSelectField(CNR, SelectField("ManufacturerConfigCode"));
  view.addSelectField(CNR, SelectField("ManufacturerArticleCode"));
  join = pvSchema.joinClause(UCNR, CNR);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(UCNR, ACNR);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void DatabaseSchema::setupUnitConnectorUsageView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table UCNR("UnitConnector_tbl", "UCNR");
  Table U("Unit_tbl", "U");
  Table VTU("VehicleType_Unit_tbl", "VTU");
  Table VT("VehicleType_tbl", "VT");
  JoinClause join;

  view.setName("UnitConnectorUsage_view");
  view.setTable(UCNR);
  view.addSelectField(UCNR, SelectField("Id_PK"));
  view.addSelectField(UCNR, SelectField("Unit_Id_FK"));
  view.addSelectField(UCNR, SelectField("Connector_Id_FK"));
  view.addSelectField(UCNR, SelectField("ArticleConnector_Id_FK"));
  view.addSelectField(UCNR, SelectField("Name", "UnitConnectorName"));
  view.addSelectField(U, SelectField("SchemaPosition"));
  view.addSelectField(U, SelectField("Alias"));
  view.addSelectField(VT, SelectField("Type"));
  view.addSelectField(VT, SelectField("SubType"));
  view.addSelectField(VT, SelectField("SeriesNumber"));
  join = pvSchema.joinClause(UCNR, U);
  view.addJoinClause(join);
  join = pvSchema.joinClause(U, VTU);
  view.addJoinClause(join);
  join = pvSchema.joinClause(VTU, VT);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void DatabaseSchema::setupUnitConnectionView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table UCNX("UnitConnection_tbl", "UCNX");
  Table UCNR("UnitConnector_tbl", "UCNR");
  Table ACNR("ArticleConnector_tbl", "ACNR");
  Table ACNX("ArticleConnection_tbl", "ACNX");
  Table CT("ConnectionType_tbl", "CT");
  JoinClause join;

  view.setName("UnitConnection_view");
  view.setTable(UCNX);
  view.addSelectField(UCNR, SelectField("Connector_Id_FK", "UCNR_Connector_Id_FK"));
  view.addSelectField(UCNR, SelectField("Unit_Id_FK", "UCNR_Unit_Id_FK"));
  view.addSelectField(UCNR, SelectField("Name", "UnitConnectorName"));
  view.addSelectField(UCNX, SelectField("Id_PK"));
  view.addSelectField(UCNX, SelectField("Unit_Id_FK"));
  view.addSelectField(UCNX, SelectField("UnitConnector_Id_FK"));
  view.addSelectField(UCNX, SelectField("ConnectionType_Code_FK"));
  view.addSelectField(UCNX, SelectField("ArticleConnection_Id_FK"));
  view.addSelectField(UCNX, SelectField("UnitContactName"));
  view.addSelectField(CT, SelectField("NameEN", "ConnectionTypeEN"));
  view.addSelectField(CT, SelectField("NameFR", "ConnectionTypeFR"));
  view.addSelectField(CT, SelectField("NameDE", "ConnectionTypeDE"));
  view.addSelectField(CT, SelectField("NameIT", "ConnectionTypeIT"));
  view.addSelectField(UCNX, SelectField("Resistance", "UnitConnectionResistance"));
  view.addSelectField(UCNX, SelectField("SchemaPage"));
  view.addSelectField(UCNX, SelectField("FunctionEN", "UnitFunctionEN"));
  view.addSelectField(UCNX, SelectField("FunctionFR", "UnitFunctionFR"));
  view.addSelectField(UCNX, SelectField("FunctionDE", "UnitFunctionDE"));
  view.addSelectField(UCNX, SelectField("FunctionIT", "UnitFunctionIT"));
  view.addSelectField(UCNX, SelectField("SignalName"));
  view.addSelectField(UCNX, SelectField("SwAddress"));
  view.addSelectField(ACNR, SelectField("Connector_Id_FK", "ACNR_Connector_Id_FK"));
  view.addSelectField(ACNR, SelectField("Article_Id_FK", "ACNR_Article_Id_FK"));
  view.addSelectField(ACNR, SelectField("Name", "ArticleConnectorName"));
  view.addSelectField(ACNX, SelectField("ArticleConnector_Id_FK"));
  view.addSelectField(ACNX, SelectField("Article_Id_FK"));
  view.addSelectField(ACNX, SelectField("ConnectionType_Code_FK", "ACNX_ConnectionType_Code_FK"));
  view.addSelectField(ACNX, SelectField("ArticleContactName"));
  view.addSelectField(ACNX, SelectField("IoType"));
  view.addSelectField(ACNX, SelectField("FunctionEN", "ArticleFunctionEN"));
  view.addSelectField(ACNX, SelectField("FunctionFR", "ArticleFunctionFR"));
  view.addSelectField(ACNX, SelectField("FunctionDE", "ArticleFunctionDE"));
  view.addSelectField(ACNX, SelectField("FunctionIT", "ArticleFunctionIT"));
  join = pvSchema.joinClause(UCNX, UCNR);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(UCNR, ACNR);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(UCNX, ACNX);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(UCNX, CT);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void DatabaseSchema::setupArticleLinkUnitConnectionView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table AL("ArticleLink_tbl", "AL");
  Table UCNXS("UnitConnection_tbl", "UCNXS");
  Table UCNXE("UnitConnection_tbl", "UCNXE");
  JoinClause join;
  JoinKey key;

  view.setName("ArticleLink_UnitConnection_view");
  view.setTable(AL);
  view.addSelectField(UCNXS, SelectField("Id_PK", "UnitConnectionStart_Id_FK"));
  view.addSelectField(UCNXE, SelectField("Id_PK", "UnitConnectionEnd_Id_FK"));
  view.addSelectField(UCNXS, SelectField("Unit_Id_FK", "StartUnit_Id_FK"));
  view.addSelectField(UCNXE, SelectField("Unit_Id_FK", "EndUnit_Id_FK"));
  view.addSelectField(AL, SelectField("ArticleConnectionStart_Id_FK"));
  view.addSelectField(AL, SelectField("ArticleConnectionEnd_Id_FK"));
  view.addSelectField(AL, SelectField("LinkType_Code_FK"));
  view.addSelectField(AL, SelectField("LinkDirection_Code_FK"));
  view.addSelectField(AL, SelectField("Identification"));
  view.addSelectField(AL, SelectField("Resistance"));
  view.addSelectField(AL, SelectField("SinceVersion"));
  view.addSelectField(AL, SelectField("Modification"));
  // AL - UCNXS JOIN
  join.setMainTable(AL);
  join.setTableToJoin(UCNXS);
  key.clear();
  key.setMainTableField("ArticleConnectionStart_Id_FK");
  key.setTableToJoinField("ArticleConnection_Id_FK");
  join.addKey(key);
  view.addJoinClause(join);
  // AL - UCNXE JOIN
  join.clear();
  join.setMainTable(AL);
  join.setTableToJoin(UCNXE);
  key.clear();
  key.setMainTableField("ArticleConnectionEnd_Id_FK");
  key.setTableToJoinField("ArticleConnection_Id_FK");
  join.addKey(key);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void DatabaseSchema::setupUnitLinkView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table LNK("Link_tbl", "LNK");
  Table LV("LinkVersion_tbl", "LV");
  Table M("Modification_tbl", "M");
  Table W("Wire_tbl", "W");
  Table UCNXS("UnitConnection_tbl", "UCNXS");
  Table UCNXE("UnitConnection_tbl", "UCNXE");
  Table UCS("UnitConnector_tbl", "UCS");
  Table UCE("UnitConnector_tbl", "UCE");
  Table US("Unit_tbl", "US");
  Table UE("Unit_tbl", "UE");
  Table LT("LinkType_tbl", "LT");
  Table LD("LinkDirection_tbl", "LD");
  JoinClause join;
  JoinKey key;

  view.setName("UnitLink_view");
  view.setTable(LNK);
  view.addSelectField(LV, SelectField("Version"));
  view.addSelectField(M, SelectField("NameEN", "ModificationEN"));
  view.addSelectField(M, SelectField("NameFR", "ModificationFR"));
  view.addSelectField(M, SelectField("NameDE", "ModificationDE"));
  view.addSelectField(M, SelectField("NameIT", "ModificationIT"));
  view.addSelectField(M, SelectField("SortOrder", "ModificationSortOrder"));
  view.addSelectField(LNK, SelectField("Identification"));
  view.addSelectField(LNK, SelectField("LinkBeam_Id_FK"));
  view.addSelectField(US, SelectField("SchemaPosition", "StartSchemaPosition"));
  view.addSelectField(US, SelectField("Alias", "StartAlias"));
  view.addSelectField(UCS, SelectField("Name", "StartUnitConnectorName"));
  view.addSelectField(UCNXS, SelectField("UnitContactName", "StartUnitContactName"));
  view.addSelectField(UCNXS, SelectField("Resistance", "StartUnitConnectionResistance"));
  view.addSelectField(UE, SelectField("SchemaPosition", "EndSchemaPosition"));
  view.addSelectField(UE, SelectField("Alias", "EndAlias"));
  view.addSelectField(UCE, SelectField("Name", "EndUnitConnectorName"));
  view.addSelectField(UCNXE, SelectField("UnitContactName", "EndUnitContactName"));
  view.addSelectField(UCNXE, SelectField("Resistance", "EndUnitConnectionResistance"));
  view.addSelectField(LT, SelectField("NameEN", "LinkTypeNameEN"));
  view.addSelectField(LNK, SelectField("Length"));
  view.addSelectField(LNK, SelectField("Resistance"));
  view.addSelectField(W, SelectField("Model", "WireModel"));
  view.addSelectField(W, SelectField("Section"));
  view.addSelectField(W, SelectField("ColorEN"));
  view.addSelectField(W, SelectField("ArticleCode", "WireArticleCode"));
  view.addSelectField(LT, SelectField("ValueUnit"));
  view.addSelectField(LD, SelectField("PictureAscii", "LinkDirectionPictureAscii"));
  view.addSelectField(UCNXS, SelectField("SchemaPage", "StartSchemaPage"));
  view.addSelectField(UCNXS, SelectField("FunctionEN", "StartFunctionEN"));
  view.addSelectField(UCNXS, SelectField("SignalName", "StartSignalName"));
  view.addSelectField(UCNXS, SelectField("SwAddress", "StartSwAddress"));
  view.addSelectField(UCNXE, SelectField("SchemaPage", "EndSchemaPage"));
  view.addSelectField(UCNXE, SelectField("FunctionEN", "EndFunctionEN"));
  view.addSelectField(UCNXE, SelectField("SignalName", "EndSignalName"));
  view.addSelectField(UCNXE, SelectField("SwAddress", "EndSwAddress"));
  view.addSelectField(LNK, SelectField("UnitConnectionStart_Id_FK"));
  view.addSelectField(LNK, SelectField("UnitConnectionEnd_Id_FK"));
  view.addSelectField(UCNXS, SelectField("UnitConnector_Id_FK", "StartUnitConnector_Id_FK"));
  view.addSelectField(UCNXS, SelectField("Unit_Id_FK", "StartUnit_Id_FK"));
  view.addSelectField(UCNXE, SelectField("UnitConnector_Id_FK", "EndUnitConnector_Id_FK"));
  view.addSelectField(UCNXE, SelectField("Unit_Id_FK", "EndUnit_Id_FK"));
  view.addSelectField(LNK, SelectField("LinkType_Code_FK"));
  view.addSelectField(LNK, SelectField("LinkDirection_Code_FK"));
  view.addSelectField(LNK, SelectField("ArticleConnectionStart_Id_FK"));
  view.addSelectField(LNK, SelectField("ArticleConnectionEnd_Id_FK"));
  view.addSelectField(LNK, SelectField("Version_FK"));
  view.addSelectField(LNK, SelectField("Modification_Code_FK"));
  join = pvSchema.joinClause(LNK, LV);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(LNK, M);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(LNK, W);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  // LNK - UCNXS JOIN
  join.clear();
  join.setMainTable(LNK);
  join.setTableToJoin(UCNXS);
  key.clear();
  key.setMainTableField("UnitConnectionStart_Id_FK");
  key.setTableToJoinField("Id_PK");
  join.addKey(key);
  view.addJoinClause(join);
  // LNK - UCNXE JOIN
  join.clear();
  join.setMainTable(LNK);
  join.setTableToJoin(UCNXE);
  key.clear();
  key.setMainTableField("UnitConnectionEnd_Id_FK");
  key.setTableToJoinField("Id_PK");
  join.addKey(key);
  view.addJoinClause(join);
  // Other JOINs
  join = pvSchema.joinClause(UCNXS, UCS);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(UCNXE, UCE);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(UCNXS, US);
  view.addJoinClause(join);
  join = pvSchema.joinClause(UCNXE, UE);
  view.addJoinClause(join);
  join = pvSchema.joinClause(LNK, LT);
  view.addJoinClause(join);
  join = pvSchema.joinClause(LNK, LD);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void DatabaseSchema::setupUnitVehicleTypeView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table VT("VehicleType_tbl", "VT");
  Table VTU("VehicleType_Unit_tbl", "VTU");
  JoinClause join;

  view.setName("Unit_VehicleType_view");
  view.setTable(VT);
  view.addSelectField(VT, SelectField("Type"));
  view.addSelectField(VT, SelectField("SubType"));
  view.addSelectField(VT, SelectField("SeriesNumber"));
  view.addSelectField(VTU, SelectField("VehicleType_Id_FK"));
  view.addSelectField(VTU, SelectField("Unit_Id_FK"));
  join = pvSchema.joinClause(VT, VTU);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void DatabaseSchema::setupLinkListView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table LNK("Link_tbl", "LNK");
  ///Table LV("LinkVersion_tbl", "LV");
  ///Table M("Modification_tbl", "M");
  Table W("Wire_tbl", "W");
  Table UCNXS("UnitConnection_tbl", "UCNXS");
  Table UCNXE("UnitConnection_tbl", "UCNXE");
  Table UCS("UnitConnector_tbl", "UCS");
  Table UCE("UnitConnector_tbl", "UCE");
  Table US("Unit_tbl", "US");
  Table UE("Unit_tbl", "UE");
  Table VTL("VehicleType_Link_tbl", "VTL");
  Table VTS("VehicleType_tbl", "VTS");
  Table VTE("VehicleType_tbl", "VTE");
  Table LT("LinkType_tbl", "LT");
  Table LD("LinkDirection_tbl", "LD");
  JoinClause join;
  JoinKey key;

  view.setName("LinkList_view");
  view.setTable(LNK);
  view.addSelectField(VTS, SelectField("Id_PK", "StartVehicleType_Id_PK"));
  view.addSelectField(VTS, SelectField("Type", "StartVehicleType"));
  view.addSelectField(VTS, SelectField("SubType", "StartVehicleSubType"));
  view.addSelectField(VTS, SelectField("SeriesNumber", "StartVehicleSerie"));
  view.addSelectField(VTE, SelectField("Id_PK", "EndVehicleType_Id_PK"));
  view.addSelectField(VTE, SelectField("Type", "EndVehicleType"));
  view.addSelectField(VTE, SelectField("SubType", "EndVehicleSubType"));
  view.addSelectField(VTE, SelectField("SeriesNumber", "EndVehicleSerie"));
  ///view.addSelectField(LV, SelectField("Version"));
  ///view.addSelectField(M, SelectField("NameEN", "ModificationEN"));
  ///view.addSelectField(M, SelectField("NameFR", "ModificationFR"));
  ///view.addSelectField(M, SelectField("NameDE", "ModificationDE"));
  ///view.addSelectField(M, SelectField("NameIT", "ModificationIT"));
  ///view.addSelectField(M, SelectField("SortOrder", "ModificationSortOrder"));
  view.addSelectField(LNK, SelectField("Identification"));
  view.addSelectField(LNK, SelectField("LinkBeam_Id_FK"));
  view.addSelectField(US, SelectField("SchemaPosition", "StartSchemaPosition"));
  view.addSelectField(US, SelectField("Alias", "StartAlias"));
  view.addSelectField(UCS, SelectField("Name", "StartUnitConnectorName"));
  view.addSelectField(UCNXS, SelectField("UnitContactName", "StartUnitContactName"));
  view.addSelectField(UCNXS, SelectField("Resistance", "StartUnitConnectionResistance"));
  view.addSelectField(UE, SelectField("SchemaPosition", "EndSchemaPosition"));
  view.addSelectField(UE, SelectField("Alias", "EndAlias"));
  view.addSelectField(UCE, SelectField("Name", "EndUnitConnectorName"));
  view.addSelectField(UCNXE, SelectField("UnitContactName", "EndUnitContactName"));
  view.addSelectField(UCNXE, SelectField("Resistance", "EndUnitConnectionResistance"));
  view.addSelectField(LT, SelectField("NameEN", "LinkTypeNameEN"));
  view.addSelectField(LNK, SelectField("Length"));
  view.addSelectField(LNK, SelectField("Resistance"));
  view.addSelectField(W, SelectField("Model", "WireModel"));
  view.addSelectField(W, SelectField("Section"));
  view.addSelectField(W, SelectField("ColorEN"));
  view.addSelectField(W, SelectField("ArticleCode", "WireArticleCode"));
  view.addSelectField(LT, SelectField("ValueUnit"));
  view.addSelectField(LD, SelectField("PictureAscii", "LinkDirectionPictureAscii"));
  view.addSelectField(UCNXS, SelectField("SchemaPage", "StartSchemaPage"));
  view.addSelectField(UCNXS, SelectField("FunctionEN", "StartFunctionEN"));
  view.addSelectField(UCNXS, SelectField("SignalName", "StartSignalName"));
  view.addSelectField(UCNXS, SelectField("SwAddress", "StartSwAddress"));
  view.addSelectField(UCNXE, SelectField("SchemaPage", "EndSchemaPage"));
  view.addSelectField(UCNXE, SelectField("FunctionEN", "EndFunctionEN"));
  view.addSelectField(UCNXE, SelectField("SignalName", "EndSignalName"));
  view.addSelectField(UCNXE, SelectField("SwAddress", "EndSwAddress"));
  view.addSelectField(LNK, SelectField("UnitConnectionStart_Id_FK"));
  view.addSelectField(LNK, SelectField("UnitConnectionEnd_Id_FK"));
  view.addSelectField(UCNXS, SelectField("UnitConnector_Id_FK", "StartUnitConnector_Id_FK"));
  view.addSelectField(UCNXS, SelectField("Unit_Id_FK", "StartUnit_Id_FK"));
  view.addSelectField(UCNXE, SelectField("UnitConnector_Id_FK", "EndUnitConnector_Id_FK"));
  view.addSelectField(UCNXE, SelectField("Unit_Id_FK", "EndUnit_Id_FK"));
  view.addSelectField(LNK, SelectField("LinkType_Code_FK"));
  view.addSelectField(LNK, SelectField("LinkDirection_Code_FK"));
  view.addSelectField(LNK, SelectField("ArticleConnectionStart_Id_FK"));
  view.addSelectField(LNK, SelectField("ArticleConnectionEnd_Id_FK"));
  view.addSelectField(LNK, SelectField("Version_FK"));
  view.addSelectField(LNK, SelectField("Modification_Code_FK"));
  /** \todo Add versionning
  join = pvSchema.joinClause(LNK, LV);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(LNK, M);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  */
  // LNK - UCNXS JOIN
  join.clear();
  join.setMainTable(LNK);
  join.setTableToJoin(UCNXS);
  key.clear();
  key.setMainTableField("UnitConnectionStart_Id_FK");
  key.setTableToJoinField("Id_PK");
  join.addKey(key);
  view.addJoinClause(join);
  // LNK - UCNXE JOIN
  join.clear();
  join.setMainTable(LNK);
  join.setTableToJoin(UCNXE);
  key.clear();
  key.setMainTableField("UnitConnectionEnd_Id_FK");
  key.setTableToJoinField("Id_PK");
  join.addKey(key);
  view.addJoinClause(join);
  // Other JOINs
  join = pvSchema.joinClause(UCNXS, UCS);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(UCNXE, UCE);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  join = pvSchema.joinClause(UCNXS, US);
  view.addJoinClause(join);
  join = pvSchema.joinClause(UCNXE, UE);
  view.addJoinClause(join);
  join = pvSchema.joinClause(LNK, VTL);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);
  // VTL - VTS JOIN
  join.clear();
  join.setOperator(JoinClause::LeftJoin);
  join.setMainTable(VTL);
  join.setTableToJoin(VTS);
  key.clear();
  key.setMainTableField("VehicleTypeStart_Id_FK");
  key.setTableToJoinField("Id_PK");
  join.addKey(key);
  view.addJoinClause(join);
  // VTL - VTE JOIN
  join.clear();
  join.setOperator(JoinClause::LeftJoin);
  join.setMainTable(VTL);
  join.setTableToJoin(VTE);
  key.clear();
  key.setMainTableField("VehicleTypeEnd_Id_FK");
  key.setTableToJoinField("Id_PK");
  join.addKey(key);
  view.addJoinClause(join);
  // Other JOINs
  join = pvSchema.joinClause(LNK, LT);
  view.addJoinClause(join);
  join = pvSchema.joinClause(LNK, LD);
  view.addJoinClause(join);
  join = pvSchema.joinClause(LNK, W);
  join.setOperator(JoinClause::LeftJoin);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void DatabaseSchema::setupLinkBeamUnitStartView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table LBUS("LinkBeam_UnitStart_tbl", "LBUS");
  Table U("Unit_tbl", "U");
  JoinClause join;

  view.setName("LinkBeam_UnitStart_view");
  view.setTable(LBUS);
  view.addSelectField(LBUS, SelectField("Unit_Id_FK"));
  view.addSelectField(LBUS, SelectField("LinkBeam_Id_FK"));
  view.addSelectField(U, SelectField("*"));
  join = pvSchema.joinClause(LBUS, U);
  view.addJoinClause(join);

  pvSchema.addView(view);
}

void DatabaseSchema::setupLinkBeamUnitEndView()
{
  using namespace mdtSqlViewSchema;

  Schema view;
  Table LBUE("LinkBeam_UnitEnd_tbl", "LBUE");
  Table U("Unit_tbl", "U");
  JoinClause join;

  view.setName("LinkBeam_UnitEnd_view");
  view.setTable(LBUE);
  view.addSelectField(LBUE, SelectField("Unit_Id_FK"));
  view.addSelectField(LBUE, SelectField("LinkBeam_Id_FK"));
  view.addSelectField(U, SelectField("*"));
  join = pvSchema.joinClause(LBUE, U);
  view.addJoinClause(join);

  pvSchema.addView(view);

}

void DatabaseSchema::setupModificationTablePopulation()
{
  mdtSqlTablePopulationSchema tp;

  tp.setName("Modification_tbl data");
  tp.setTableName("Modification_tbl");
  tp.addFieldName("Code_PK");
  tp.addFieldName("SortOrder");
  tp.addFieldName("NameEN");
  tp.addFieldName("NameDE");
  tp.addFieldName("NameFR");
  tp.addFieldName("NameIT");
  // NEW
  tp.currentRowData() << "NEW" << 3 << "New" << "Neu" << "Nouveau" << "Nuovo";
  tp.commitCurrentRowData();
  // REM
  tp.currentRowData() << "REM" << 3 << "Remove" << "Entfernen" << "Supprimer" << "Rimuovere";
  tp.commitCurrentRowData();
  // MOD
  tp.currentRowData() << "MOD" << 3 << "Modifiy" << "Ändern" << "Modifier" << "Modificare";
  tp.commitCurrentRowData();
  // MODNEW
  tp.currentRowData() << "MODNEW" << 2 << "Modifiy (+)" << "Ändern (+)" << "Modifier (+)" << "Modificare (+)";
  tp.commitCurrentRowData();
  // MODREM
  tp.currentRowData() << "MODREM" << 1 << "Modifiy (-)" << "Ändern (-)" << "Modifier (-)" << "Modificare (-)";
  tp.commitCurrentRowData();
  // EXISTS
  tp.currentRowData() << "EXISTS" << 3 << "Existing" << "Bestehend" << "Existant" << "Esistente";
  tp.commitCurrentRowData();

  pvSchema.addTablePopulation(tp);
}

void DatabaseSchema::setupConnectionTypeTablePopulation()
{
  mdtSqlTablePopulationSchema tp;

  tp.setName("ConnectionType_tbl data");
  tp.setTableName("ConnectionType_tbl");
  tp.addFieldName("Code_PK");
  tp.addFieldName("NameEN");
  tp.addFieldName("NameDE");
  tp.addFieldName("NameFR");
  tp.addFieldName("NameIT");
  // Terminal type
  tp.currentRowData() << "T" << "Terminal" << "Klemme" << "Borne" << "Terminale";
  tp.commitCurrentRowData();
  // Pin type
  tp.currentRowData() << "P" << "Pin (male)" << "Stift (männlich)" << "Contact (mâle)" << "perno (maschio)";
  tp.commitCurrentRowData();
  // Socket type
  tp.currentRowData() << "S" << "Socket (female)" << "Buchse (weiblich)" << "Douille (femelle)" << "presa (femminile)";
  tp.commitCurrentRowData();

  pvSchema.addTablePopulation(tp);
}

void DatabaseSchema::setupLinkTypeTablePopulation()
{
  mdtSqlTablePopulationSchema tp;

  tp.setName("LinkType_tbl data");
  tp.setTableName("LinkType_tbl");
  tp.addFieldName("Code_PK");
  tp.addFieldName("NameEN");
  tp.addFieldName("NameDE");
  tp.addFieldName("NameFR");
  tp.addFieldName("NameIT");
  tp.addFieldName("ValueUnit");
  // Cable link type
  tp.currentRowData() << "CABLELINK" << "Cable link" << "Kabel Verbindung" << "Liaison cablée" << "Collegamento via cavo" << "Ohm";
  tp.commitCurrentRowData();
  // Internal link
  tp.currentRowData() << "INTERNLINK" << "Internal link" << "Interne Verbindung" << "Liaison interne" << "Collegamento interno" << "Ohm";
  tp.commitCurrentRowData();
  // Connection type
  tp.currentRowData() << "CONNECTION" << "Connection" << "Anschluss" << "Raccordement" << "Collegamento" << "Ohm";
  tp.commitCurrentRowData();
  // Test link type
  tp.currentRowData() << "TESTLINK" << "Test link" << "Test Verbindung" << "Liaison de test" << "Collegamento test" << "Ohm";
  tp.commitCurrentRowData();

  pvSchema.addTablePopulation(tp);
}

void DatabaseSchema::setupLinkDirectionTablePopulation()
{
  mdtSqlTablePopulationSchema tp;

  tp.setName("LinkDirection_tbl data");
  tp.setTableName("LinkDirection_tbl");
  tp.addFieldName("Code_PK");
  tp.addFieldName("PictureAscii");
  tp.addFieldName("NameEN");
  tp.addFieldName("NameDE");
  tp.addFieldName("NameFR");
  tp.addFieldName("NameIT");
  // Bidirectional
  tp.currentRowData() << "BID" << "<-->" << "Bidirectional" << "Bidirektional" << "Bidirectionnel" << "Bidirezionale";
  tp.commitCurrentRowData();
  // Start to end
  tp.currentRowData() << "STE" << "-->" << "Start to end" << "Start zum Ende" << "Départ vers arrivée" << "Dall'inizio alla fine";
  tp.commitCurrentRowData();
  // End to start
  tp.currentRowData() << "ETS" << "<--" << "End to Start" << "Ende zum Start" << "Arrivée vers départ" << "Dall'fine alla inizio";
  tp.commitCurrentRowData();

  pvSchema.addTablePopulation(tp);
}

QString DatabaseSchema::tr(const char *sourceText)
{
  return QObject::tr(sourceText);
}

}} // namespace Mdt{ namespace CableList{
