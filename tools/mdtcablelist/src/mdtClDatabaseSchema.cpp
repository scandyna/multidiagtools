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
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlField>
#include <QFileInfo>

#include <QDebug>

mdtClDatabaseSchema::mdtClDatabaseSchema() 
{
  pvDatabaseManager = new mdtSqlDatabaseManager;
}

mdtClDatabaseSchema::~mdtClDatabaseSchema() 
{
  delete pvDatabaseManager;
}

bool mdtClDatabaseSchema::createSchemaSqlite() 
{
  QString sql;

  pvTables.clear();
  if(!pvDatabaseManager->createDatabaseSqlite(QFileInfo("/tmp/test01.sqlite"), mdtSqlDatabaseManager::OverwriteExisting)){
    return false;
  }
  // Enable foreing key support
  QSqlQuery query(pvDatabaseManager->database());
  sql = "PRAGMA foreign_keys = ON";
  if(!query.exec(sql)){
    return false;
  }

  setupTables();
  if(!createTablesSqlite()){
    return false;
  }
  
  qDebug() << "Tables: " << pvDatabaseManager->database().tables();
  
  if(!createViews()){
    return false;
  }

  

  return true;
}

void mdtClDatabaseSchema::setupTables() 
{
  // We must build tables list in correct order, regarding dependencies
  setupVehicleTypeTable();
  setupArticleTable();
  setupArticleComponentTable();
  setupArticleConnectionTable();
  setupUnitTable();
  setupUnitConnectionTable();
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
}

bool mdtClDatabaseSchema::createViews() 
{
}

void mdtClDatabaseSchema::setupVehicleTypeTable() 
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
}

void mdtClDatabaseSchema::setupVehicleTypeUnitTable() 
{
}

void mdtClDatabaseSchema::setupVehicleTypeLinkTable() 
{
}

void mdtClDatabaseSchema::setupArticleTable() 
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
  // Unit
  field = QSqlField();
  field.setName("Unit");
  field.setType(QVariant::String);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  table.addField(field, false);

  pvTables.append(table);
}

void mdtClDatabaseSchema::setupArticleComponentTable() 
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
    return;/// false
  }
  table.addForeignKey("Component_Id_FK_fk", "Article_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Component_Id_FK_fk", "Component_Id_FK", "Id_PK")){
    return;/// false
  }

  pvTables.append(table);
}

void mdtClDatabaseSchema::setupArticleConnectionTable() 
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
  table.addField(field, false);
  // ArticleConnectorName
  field = QSqlField();
  field.setName("ArticleConnectorName");
  field.setType(QVariant::String);
  field.setLength(30);
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
    return;/// false;
  }
  // Foreign keys
  table.addForeignKey("Article_Id_FK_fk2", "Article_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Article_Id_FK_fk2", "Article_Id_FK", "Id_PK")){
    return;/// false
  }

  pvTables.append(table);
}

void mdtClDatabaseSchema::setupArticleLinkTable() 
{
}

void mdtClDatabaseSchema::setupUnitTable() 
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
  // Indexes
  table.addIndex("Article_Id_FK_idx", false);
  if(!table.addFieldToIndex("Article_Id_FK_idx", "Article_Id_FK")){
    return;/// false;
  }
  table.addIndex("Composite_Id_FK_idx", false);
  if(!table.addFieldToIndex("Composite_Id_FK_idx", "Composite_Id_FK")){
    return;/// false;
  }
  // Foreign keys
  table.addForeignKey("Composite_Id_FK_fk", "Unit_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Composite_Id_FK_fk", "Composite_Id_FK", "Id_PK")){
    return;/// false
  }
  table.addForeignKey("Article_Id_FK_fk", "Article_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  if(!table.addFieldToForeignKey("Article_Id_FK_fk", "Article_Id_FK", "Id_PK")){
    return;/// false
  }

  pvTables.append(table);
}

void mdtClDatabaseSchema::setupUnitConnectionTable() 
{
  
}

void mdtClDatabaseSchema::setupLinkTable() 
{
}

void mdtClDatabaseSchema::setupLinkDirectionTable() 
{
}

void mdtClDatabaseSchema::setupLinkTypeTable() 
{
}

bool mdtClDatabaseSchema::createView(const QString & viewName, const QString & sql) 
{
}

bool mdtClDatabaseSchema::createVehicleTypeUnitView() 
{
}

bool mdtClDatabaseSchema::createArticleComponentUsageView() 
{
}

bool mdtClDatabaseSchema::createArticleComponentView() 
{
}

bool mdtClDatabaseSchema::createArticleLinkView() 
{
}

bool mdtClDatabaseSchema::createUnitView() 
{
}

bool mdtClDatabaseSchema::createUnitComponentView() 
{
}

bool mdtClDatabaseSchema::createUnitConnectionView() 
{
}

bool mdtClDatabaseSchema::createUnitLinkView() 
{
}

bool mdtClDatabaseSchema::createUnitVehicleTypeView() 
{
}

bool mdtClDatabaseSchema::createLinkListView() 
{
}
