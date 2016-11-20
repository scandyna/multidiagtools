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
#include "SchemaTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Schema/FieldTypeList.h"
#include "Mdt/Sql/Schema/FieldTypeListModel.h"
#include "Mdt/Sql/Schema/FieldTypeName.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/Schema/Charset.h"
#include "Mdt/Sql/Schema/Field.h"
#include "Mdt/Sql/Schema/FieldList.h"
#include "Mdt/Sql/Schema/AutoIncrementPrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKeyContainer.h"
#include "Mdt/Sql/Schema/Index.h"
#include "Mdt/Sql/Schema/IndexList.h"
#include "Mdt/Sql/Schema/IndexListModel.h"
#include "Mdt/Sql/Schema/ForeignKey.h"
#include "Mdt/Sql/Schema/ForeignKeyList.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Mdt/Sql/Schema/TableModel.h"
#include "Mdt/Sql/Schema/TableList.h"
#include "Mdt/Sql/Schema/TablePopulation.h"
#include "Mdt/Sql/Schema/Schema.h"
#include "Schema/Client_tbl.h"
#include "Schema/Address_tbl.h"
#include "Schema/ClientAddressView.h"
#include <QSqlDatabase>
#include <QComboBox>
#include <QTableView>
#include <QTreeView>

namespace Sql = Mdt::Sql;

void SchemaTest::initTestCase()
{
  QSqlDatabase::addDatabase("QSQLITE", "SQLITE_1");
}

void SchemaTest::cleanupTestCase()
{
  QSqlDatabase::removeDatabase("SQLITE_1");
}

/*
 * Tests
 */

void SchemaTest::charsetTest()
{
  using Mdt::Sql::Schema::Charset;

  /*
   * Initial state
   */
  Charset cs;
  QVERIFY(cs.isNull());
  /*
   * Set/get
   */
  cs.setCharsetName("utf8");
  QVERIFY(!cs.isNull());
  QCOMPARE(cs.charsetName(), QString("utf8"));
  /*
   * Clear
   */
  cs.clear();
  QVERIFY(cs.charsetName().isEmpty());
  QVERIFY(cs.isNull());
}

void SchemaTest::localeTest()
{
  using Mdt::Sql::Schema::Locale;

  Locale locale;

  /*
   * Initial state
   */
  QVERIFY(locale.country() == QLocale::AnyCountry);
  QVERIFY(locale.language() == QLocale::AnyLanguage);
  QVERIFY(locale.isNull());
  /*
   * Set/get
   */
  locale.setCountry(QLocale::Switzerland);
  QVERIFY(!locale.isNull());
  locale.setLanguage(QLocale::French);
  QVERIFY(locale.country() == QLocale::Switzerland);
  QVERIFY(locale.language() == QLocale::French);
  QVERIFY(!locale.isNull());
  /*
   * Clear
   */
  locale.clear();
  QVERIFY(locale.country() == QLocale::AnyCountry);
  QVERIFY(locale.language() == QLocale::AnyLanguage);
  QVERIFY(locale.isNull());
}

void SchemaTest::collationTest()
{
  using Mdt::Sql::Schema::Collation;
  using Mdt::Sql::Schema::CaseSensitivity;

  Collation collation;

  /*
   * Initial state
   */
  QVERIFY(collation.caseSensitivity() == CaseSensitivity::NotDefined);
  QVERIFY(collation.locale().isNull());
  QVERIFY(collation.isNull());
  /*
   * Simple set/get
   */
  collation.setCaseSensitive(false);
  collation.setCountry(QLocale::Switzerland);
  collation.setLanguage(QLocale::French);
//   collation.setCharset("utf8");
  QVERIFY(collation.caseSensitivity() == CaseSensitivity::CaseInsensitive);
  QVERIFY(!collation.isCaseSensitive());
  QVERIFY(collation.locale().country() == QLocale::Switzerland);
  QVERIFY(collation.locale().language() == QLocale::French);
//   QCOMPARE(collation.charset(), QString("utf8"));
  QVERIFY(!collation.isNull());
  /*
   * Clear
   */
  collation.clear();
  QVERIFY(collation.caseSensitivity() == CaseSensitivity::NotDefined);
  QVERIFY(collation.locale().isNull());
//   QVERIFY(collation.charset().isEmpty());
  QVERIFY(collation.isNull());
  /*
   * Check null flag
   */
  collation.setCaseSensitive(true);
  QVERIFY(!collation.isNull());
  collation.clear();
  QVERIFY(collation.isNull());
  collation.setCountry(QLocale::Switzerland);
  collation.setLanguage(QLocale::French);
  QVERIFY(!collation.isNull());
  collation.clear();
  QVERIFY(collation.isNull());
}

void SchemaTest::indexTest()
{
  using Mdt::Sql::Schema::Index;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Table;

  /*
   * Setup a field
   */
  Field Id_A;
  Id_A.setType(FieldType::Integer);
  Id_A.setName("Id_A");
  /*
   * Setup a table
   */
  Table Client_tbl;
  Client_tbl.setTableName("Client_tbl");
  Client_tbl.addField(Id_A);
  /*
   * Initial state
   */
  Index index;
  QVERIFY(!index.isUnique());
  QVERIFY(index.isNull());
  /*
   * Simple set/get
   */
  index.setTable(Client_tbl);
  QVERIFY(index.isNull());
  index.addField(Id_A);
  QVERIFY(index.isNull());
  index.generateName();
  QVERIFY(!index.isNull());
  index.setUnique(true);
  // Check
  QCOMPARE(index.name(), QString("Client_tbl_Id_A_index"));
  QCOMPARE(index.tableName(), QString("Client_tbl"));
  QCOMPARE(index.fieldCount(), 1);
  QCOMPARE(index.fieldName(0), QString("Id_A"));
  QCOMPARE(index.fieldNameList().size(), 1);
  QCOMPARE(index.fieldNameList().at(0), QString("Id_A"));
  QVERIFY(index.containsFieldName("Id_A"));
  QVERIFY(index.containsFieldName("id_a"));
  QVERIFY(!index.containsFieldName("Id_B"));
  QVERIFY(index.isUnique());
  /*
   * Clear
   */
  index.clear();
  QVERIFY(index.name().isEmpty());
  QVERIFY(index.tableName().isEmpty());
  QCOMPARE(index.fieldCount(), 0);
  QVERIFY(!index.isUnique());
  QVERIFY(index.isNull());
  /*
   * Check setting table name from user defined table
   */
  index.setTable(Schema::Address_tbl());
  QCOMPARE(index.tableName(), QString("Address_tbl"));
  /*
   * Clear
   */
  index.clear();
  QVERIFY(index.name().isEmpty());
  QVERIFY(index.tableName().isEmpty());
  QCOMPARE(index.fieldCount(), 0);
  QVERIFY(!index.isUnique());
  QVERIFY(index.isNull());
}

void SchemaTest::indexListTest()
{
  using Mdt::Sql::Schema::Index;
  using Mdt::Sql::Schema::IndexList;
  using Mdt::Sql::FieldName;
  using Mdt::Sql::TableName;

  /*
   * Setup index
   */
  Index index;
  index.setTableName("A_tbl");
  index.setUnique(true);
  index.addFieldName("Name");
  index.generateName();
  QVERIFY(!index.isNull());
  /*
   * Initial state
   */
  IndexList list;
  QCOMPARE(list.size(), 0);
  /*
   * Add 1 element
   */
  list.append(index);
  QCOMPARE(list.size(), 1);
  QCOMPARE(list.at(0).tableName(), QString("A_tbl"));
  QVERIFY(list.at(0).isUnique());
  for(const auto & idx : list){
    QCOMPARE(idx.tableName(), QString("A_tbl"));
  }
  /*
   * Check updating table name
   */
  list.updateTableName("B_tbl");
  QCOMPARE(list.size(), 1);
  QCOMPARE(list.at(0).tableName(), QString("B_tbl"));
  QVERIFY(list.at(0).isUnique());
  /*
   * Check find index
   */
  QVERIFY(list.findIndex(TableName("None_tbl"), FieldName("None")).isNull());
  QVERIFY(list.findIndex(TableName("B_tbl"), FieldName("None")).isNull());
  QVERIFY(list.findIndex(TableName("None_tbl"), FieldName("Name")).isNull());
  QVERIFY(!list.findIndex(TableName("B_tbl"), FieldName("Name")).isNull());
  QVERIFY(!list.findIndex(TableName("b_tbl"), FieldName("name")).isNull());
  QCOMPARE(list.findIndex(TableName("B_tbl"), FieldName("Name")).tableName(), QString("B_tbl"));
  QCOMPARE(list.findIndex(TableName("B_tbl"), FieldName("Name")).fieldCount(), 1);
  QCOMPARE(list.findIndex(TableName("B_tbl"), FieldName("Name")).fieldNameList().at(0), QString("Name"));
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.size(), 0);
}

void SchemaTest::indexListModelTest()
{
  using Mdt::Sql::Schema::Index;
  using Mdt::Sql::Schema::IndexList;
  using Mdt::Sql::Schema::IndexListModel;

  Index index;
  IndexList indexList;
  IndexListModel model;
  QModelIndex modelIndex;
  /*
   * Setup views
   */
  QTableView tableView;
  tableView.setModel(&model);
  tableView.resize(400, 200);
  QTreeView treeView;
  treeView.setModel(&model);
  treeView.resize(400, 200);
  QComboBox comboBox;
  comboBox.setModel(&model);
  comboBox.setModelColumn(IndexListModel::IndexNameColumn);
  /*
   * Setup indexes
   */
  // Index on Name field
  index.clear();
  index.setTableName("Connector_tbl");
  index.setName("Name_idx");
  index.addFieldName("Name");
  index.setUnique(true);
  indexList.append(index);
  // Index on A and B fields
  index.clear();
  index.setTableName("Connector_tbl");
  index.setName("AB_idx");
  index.addFieldName("A");
  index.addFieldName("B");
  indexList.append(index);

  /*
   * Initial state
   */
  QCOMPARE(model.columnCount(), 4);
  QCOMPARE(model.rowCount(), 0);
  /*
   * Set index list
   */
  model.setIndexList(indexList);
  QCOMPARE(model.rowCount(), 2);
  // Check row 0
  modelIndex = model.index(0, IndexListModel::IndexNameColumn);
  QVERIFY(modelIndex.isValid());
  QCOMPARE(model.data(modelIndex), QVariant("Name_idx"));
  modelIndex = model.index(0, IndexListModel::TableNameColumn);
  QVERIFY(modelIndex.isValid());
  QCOMPARE(model.data(modelIndex), QVariant("Connector_tbl"));
  modelIndex = model.index(0, IndexListModel::FieldNameListColumn);
  QVERIFY(modelIndex.isValid());
  QCOMPARE(model.data(modelIndex), QVariant("Name"));
  modelIndex = model.index(0, IndexListModel::IsUniqueColumn);
  QVERIFY(modelIndex.isValid());
  QCOMPARE(model.data(modelIndex), QVariant(true));
  // Check row 1
  modelIndex = model.index(1, IndexListModel::IndexNameColumn);
  QVERIFY(modelIndex.isValid());
  QCOMPARE(model.data(modelIndex), QVariant("AB_idx"));
  modelIndex = model.index(1, IndexListModel::TableNameColumn);
  QVERIFY(modelIndex.isValid());
  QCOMPARE(model.data(modelIndex), QVariant("Connector_tbl"));
  modelIndex = model.index(1, IndexListModel::FieldNameListColumn);
  QVERIFY(modelIndex.isValid());
  QCOMPARE(model.data(modelIndex), QVariant("A, B"));
  modelIndex = model.index(1, IndexListModel::IsUniqueColumn);
  QVERIFY(modelIndex.isValid());
  QCOMPARE(model.data(modelIndex), QVariant(false));

  /*
   * Play
   */
  tableView.show();
  tableView.resizeColumnsToContents();
  tableView.resizeRowsToContents();
  treeView.show();
  comboBox.show();
  /*
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
  */
}

void SchemaTest::parentTableFieldNameTest()
{
  using Mdt::Sql::Schema::ParentTableFieldName;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
//   using Mdt::Sql::Schema::SingleFieldPrimaryKey;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;

  /*
   * Setup fields
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Code_PK
//   SingleFieldPrimaryKey Code_PK;
//   Code_PK.setFieldName("Code_PK");
  // Name
  Field Name;
  Name.setName("Name");
  /*
   * Test
   */
  ParentTableFieldName aic(Id_PK);
  QCOMPARE(aic.fieldName(), QString("Id_PK"));
//   ParentTableFieldName sfpk(Code_PK);
//   QCOMPARE(sfpk.fieldName(), QString("Code_PK"));
  ParentTableFieldName f(Name);
  QCOMPARE(f.fieldName(), QString("Name"));
}

void SchemaTest::childTableFieldNameTest()
{
  using Mdt::Sql::Schema::ChildTableFieldName;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;

  /*
   * Setup fields
   */
  // Name
  Field Name;
  Name.setName("Name");
  /*
   * Tests
   */
  ChildTableFieldName f(Name);
  QCOMPARE(f.fieldName(), QString("Name"));
}

// void SchemaTest::foreignKeyActionTest()
// {
//   using Mdt::Sql::Schema::ForeignKey;
// 
//   /*
//    * Action -> string
//    */
//   QCOMPARE(ForeignKey::actionString(ForeignKey::NoAction), QString("NO ACTION"));
//   QCOMPARE(ForeignKey::actionString(ForeignKey::Restrict), QString("RESTRICT"));
//   QCOMPARE(ForeignKey::actionString(ForeignKey::SetNull), QString("SET NULL"));
//   QCOMPARE(ForeignKey::actionString(ForeignKey::SetDefault), QString("SET DEFAULT"));
//   QCOMPARE(ForeignKey::actionString(ForeignKey::Cascade), QString("CASCADE"));
//   /*
//    * String -> Action
//    */
//   QVERIFY(ForeignKey::actionFromString("") == ForeignKey::NoAction);
//   QVERIFY(ForeignKey::actionFromString("NO ACTION") == ForeignKey::NoAction);
//   QVERIFY(ForeignKey::actionFromString("no action") == ForeignKey::NoAction);
//   QVERIFY(ForeignKey::actionFromString("RESTRICT") == ForeignKey::Restrict);
//   QVERIFY(ForeignKey::actionFromString("restrict") == ForeignKey::Restrict);
//   QVERIFY(ForeignKey::actionFromString("SET NULL") == ForeignKey::SetNull);
//   QVERIFY(ForeignKey::actionFromString("set null") == ForeignKey::SetNull);
//   QVERIFY(ForeignKey::actionFromString("SET DEFAULT") == ForeignKey::SetDefault);
//   QVERIFY(ForeignKey::actionFromString("set default") == ForeignKey::SetDefault);
//   QVERIFY(ForeignKey::actionFromString("CASCADE") == ForeignKey::Cascade);
//   QVERIFY(ForeignKey::actionFromString("cascade") == ForeignKey::Cascade);
// }

void SchemaTest::foreignKeyTest()
{
  using Mdt::Sql::Schema::ForeignKey;
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::ParentTableFieldName;
  using Mdt::Sql::Schema::ChildTableFieldName;
  using Mdt::Sql::Schema::Index;

  Index index;
  Schema::Client_tbl client_tbl;
  Schema::Address_tbl address_tbl;
  /*
   * Setup fields
   */
  // Connector_Id_FK
  Field Connector_Id_FK;
  Connector_Id_FK.setName("Connector_Id_FK");
  Connector_Id_FK.setType(FieldType::Integer);
  Connector_Id_FK.setRequired(true);
  // Client_Id_FK
  Field Client_Id_FK;
  Client_Id_FK.setName("Client_Id_FK");
  Client_Id_FK.setType(FieldType::Integer);
  Client_Id_FK.setRequired(true);
  /*
   * Init Connector_tbl
   */
  Table Connector_tbl;
  Connector_tbl.setTableName("Connector_tbl");
  Connector_tbl.setAutoIncrementPrimaryKey("Id_PK");
  /*
   * Init Contact_tbl
   */
  Table Contact_tbl;
  Contact_tbl.setTableName("Contact_tbl");
  Contact_tbl.setAutoIncrementPrimaryKey("Id_PK");
  /*
   * Initial state
   */
  ForeignKey fk;
//   QVERIFY(fk.onDeleteAction() == ForeignKey::NoAction);
//   QVERIFY(fk.onUpdateAction() == ForeignKey::NoAction);
//   QVERIFY(!fk.createChildIndex());
  QVERIFY(fk.isNull());
  /*
   * Set/get
   */
  // Set
  fk.setParentTable(Connector_tbl);
  QVERIFY(fk.isNull());
  fk.setChildTable(Contact_tbl);
  QVERIFY(fk.isNull());
  fk.addKeyFields(ParentTableFieldName("Id_PK"), ChildTableFieldName(Connector_Id_FK));
  QVERIFY(!fk.isNull());
//   fk.setOnDeleteAction(ForeignKey::Restrict);
//   fk.setOnUpdateAction(ForeignKey::Cascade);
//   fk.setCreateChildIndex(true);
  // Check
  QCOMPARE(fk.parentTableName(), QString("Connector_tbl"));
  QCOMPARE(fk.childTableName(), QString("Contact_tbl"));
  QCOMPARE(fk.parentTableFieldNameList().size(), 1);
  QCOMPARE(fk.parentTableFieldNameList().at(0), QString("Id_PK"));
  QCOMPARE(fk.childTableFieldNameList().size(), 1);
  QCOMPARE(fk.childTableFieldNameList().at(0), QString("Connector_Id_FK"));
//   QVERIFY(fk.onDeleteAction() == ForeignKey::Restrict);
//   QVERIFY(fk.onUpdateAction() == ForeignKey::Cascade);
//   QVERIFY(fk.createChildIndex());
  // Check index for child table
  index = fk.getChildTableIndex();
  QCOMPARE(index.tableName(), QString("Contact_tbl"));
  QCOMPARE(index.fieldCount(), 1);
  QCOMPARE(index.fieldName(0), QString("Connector_Id_FK"));
  QCOMPARE(index.name(), QString("Contact_tbl_Connector_Id_FK_index"));
  /*
   * Clear
   */
  fk.clear();
  QVERIFY(fk.parentTableName().isEmpty());
  QVERIFY(fk.childTableName().isEmpty());
  QCOMPARE(fk.parentTableFieldNameList().size(), 0);
  QCOMPARE(fk.childTableFieldNameList().size(), 0);
//   QVERIFY(fk.onDeleteAction() == ForeignKey::NoAction);
//   QVERIFY(fk.onUpdateAction() == ForeignKey::NoAction);
//   QVERIFY(!fk.createChildIndex());
  QVERIFY(fk.isNull());
  /*
   * Setup fk for Address_tbl
   */
  fk.setParentTable(client_tbl);
  fk.setChildTable(address_tbl);
  fk.addKeyFields(ParentTableFieldName(client_tbl.Id_PK()), ChildTableFieldName(Client_Id_FK));
  // Check
  QCOMPARE(fk.parentTableName(), QString("Client_tbl"));
  QCOMPARE(fk.childTableName(), QString("Address_tbl"));
  QCOMPARE(fk.parentTableFieldNameList().size(), 1);
  QCOMPARE(fk.parentTableFieldNameList().at(0), QString("Id_PK"));
  QCOMPARE(fk.childTableFieldNameList().size(), 1);
  QCOMPARE(fk.childTableFieldNameList().at(0), QString("Client_Id_FK"));
  /*
   * Clear
   */
  fk.clear();
  QVERIFY(fk.parentTableName().isEmpty());
  QVERIFY(fk.childTableName().isEmpty());
  QCOMPARE(fk.parentTableFieldNameList().size(), 0);
  QCOMPARE(fk.childTableFieldNameList().size(), 0);
//   QVERIFY(fk.onDeleteAction() == ForeignKey::NoAction);
//   QVERIFY(fk.onUpdateAction() == ForeignKey::NoAction);
//   QVERIFY(!fk.createChildIndex());
  QVERIFY(fk.isNull());
}

void SchemaTest::foreignKeyListTest()
{
  using Mdt::Sql::Schema::ForeignKey;
  using Mdt::Sql::Schema::ForeignKeyList;
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::ParentTableFieldName;
  using Mdt::Sql::Schema::ChildTableFieldName;

  ForeignKey fk;
  /*
   * Setup fields
   */
  // Connector_Id_FK
  Field Connector_Id_FK;
  Connector_Id_FK.setName("Connector_Id_FK");
  Connector_Id_FK.setType(FieldType::Integer);
  Connector_Id_FK.setRequired(true);
  /*
   * Setup Connector_tbl
   */
  Table Connector_tbl;
  Connector_tbl.setTableName("Connector_tbl");
  Connector_tbl.setAutoIncrementPrimaryKey("Id_PK");
  /*
   * Init Contact_tbl
   */
  Table Contact_tbl;
  Contact_tbl.setTableName("Contact_tbl");
  Contact_tbl.setAutoIncrementPrimaryKey("Id_PK");
  /*
   * Setup fk_Connector_Id_FK
   */
  ForeignKey fk_Connector_Id_FK;
  fk_Connector_Id_FK.setParentTable(Connector_tbl);
  fk_Connector_Id_FK.setChildTable(Contact_tbl);
  fk_Connector_Id_FK.addKeyFields(ParentTableFieldName("Id_PK"), ChildTableFieldName(Connector_Id_FK));
  QVERIFY(!fk_Connector_Id_FK.isNull());
  /*
   * Initial state
   */
  ForeignKeyList list;
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  /*
   * Add 1 element
   */
  list.append(fk_Connector_Id_FK);
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.at(0).parentTableName(), QString("Connector_tbl"));
  QCOMPARE(list.at(0).childTableName(), QString("Contact_tbl"));
  for(const auto & fk : list){
    QCOMPARE(fk.parentTableName(), QString("Connector_tbl"));
    QCOMPARE(fk.childTableName(), QString("Contact_tbl"));
  }
  fk = list.foreignKeyReferencing("Contact_tbl");
  QVERIFY(fk.isNull());
  fk = list.foreignKeyReferencing("Connector_tbl");
  QVERIFY(!fk.isNull());
  QCOMPARE(fk.parentTableName(), QString("Connector_tbl"));
  QCOMPARE(fk.childTableName(), QString("Contact_tbl"));
  /*
   * Check updating child table name
   */
  list.updateChildTableName("NewChildTable_tbl");
  QCOMPARE(list.size(), 1);
  QCOMPARE(list.at(0).parentTableName(), QString("Connector_tbl"));
  QCOMPARE(list.at(0).childTableName(), QString("NewChildTable_tbl"));
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.size(), 0);
}

void SchemaTest::tablePopulationTest()
{
  using Mdt::Sql::Schema::TablePopulation;

  Schema::Client_tbl client;

  /*
   * Initial state
   */
  TablePopulation tp;
  /*
   * Set attributes
   */
  tp.setName("Client_tbl population");
  tp.setTable(client);
  tp.addField(client.Id_PK());
  tp.addField(client.Name());
  // Check
  QCOMPARE(tp.name(), QString("Client_tbl population"));
  QCOMPARE(tp.tableName(), client.tableName());
  QCOMPARE(tp.fieldcount(), 2);
  QCOMPARE(tp.fieldName(0), QString("Id_PK"));
  QCOMPARE(tp.fieldNameList().at(0), QString("Id_PK"));
  /*
   * Add data
   */
  QCOMPARE(tp.rowCount(), 0);
  tp << 1 << "Name 1";
  tp.commitCurrentRow();
  tp << 2 << "Name 2";
  tp.commitCurrentRow();
  tp << 3 << "Name 3";
  tp.commitCurrentRow();
  QCOMPARE(tp.rowCount(), 3);
  QCOMPARE(tp.data(0, 0), QVariant(1));
  QCOMPARE(tp.data(0, 1), QVariant("Name 1"));
  QCOMPARE(tp.data(1, 0), QVariant(2));
  QCOMPARE(tp.data(1, 1), QVariant("Name 2"));
  QCOMPARE(tp.data(2, 0), QVariant(3));
  QCOMPARE(tp.data(2, 1), QVariant("Name 3"));
  /*
   * Clear
   */
  tp.clear();
  QVERIFY(tp.name().isEmpty());
  QVERIFY(tp.tableName().isEmpty());
  QCOMPARE(tp.fieldcount(), 0);
  QCOMPARE(tp.rowCount(), 0);
}

void SchemaTest::tablePopulationListTest()
{
  using Mdt::Sql::Schema::TablePopulation;
  using Mdt::Sql::Schema::TablePopulationList;

  Schema::Client_tbl client;

  /*
   * Initial state
   */
  TablePopulationList list;
  QVERIFY(list.isEmpty());
  /*
   * Add 1 element
   */
  TablePopulation tp;
  tp.setName("Test");
  list.append(tp);
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.at(0).name(), QString("Test"));
  for(const auto & t : list){
    QCOMPARE(t.name(), QString("Test"));
  }
  /*
   * Clear
   */
  list.clear();
  QVERIFY(list.isEmpty());
}

void SchemaTest::triggerTest()
{
  using Mdt::Sql::Schema::Trigger;

  Schema::Client_tbl client;

  /*
   * Initial state
   */
  Trigger t;
  QVERIFY(!t.isTemporary());
  QVERIFY(t.event() == Trigger::Unknown);
  /*
   * Set
   */
  t.setName("TRG");
  t.setEvent(Trigger::AfterInsert);
  t.setTemporary(true);
  t.setTable(client);
  t.setScript("UPDATE Client_tbl SET Name = 'Some name';");
  // Check
  QCOMPARE(t.name(), QString("TRG"));
  QVERIFY(t.event() == Trigger::AfterInsert);
  QVERIFY(t.isTemporary());
  QCOMPARE(t.tableName(), client.tableName());
  QCOMPARE(t.script(), QString("UPDATE Client_tbl SET Name = 'Some name';"));
  /*
   * Clear
   */
  t.clear();
  QVERIFY(t.name().isEmpty());
  QVERIFY(t.event() == Trigger::Unknown);
  QVERIFY(!t.isTemporary());
  QVERIFY(t.tableName().isEmpty());
  QVERIFY(t.script().isEmpty());
}

void SchemaTest::triggerListTest()
{
  using Mdt::Sql::Schema::Trigger;
  using Mdt::Sql::Schema::TriggerList;

  /*
   * Initial state
   */
  TriggerList list;
  QVERIFY(list.isEmpty());
  /*
   * Add 1 element
   */
  Trigger t;
  t.setName("TRG");
  list.append(t);
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.at(0).name(), QString("TRG"));
  for(const auto & trg : list){
    QCOMPARE(trg.name(), QString("TRG"));
  }
  /*
   * Clear
   */
  list.clear();
  QVERIFY(list.isEmpty());
}

void SchemaTest::schemaTest()
{
  using Mdt::Sql::Schema::TablePopulation;
  using Mdt::Sql::Schema::Trigger;

  Mdt::Sql::Schema::Schema schema;
  Schema::Client_tbl client;
  Schema::Address_tbl address;
  Schema::ClientAdrressView clientAddressView;
  TablePopulation tp;
  Trigger trigger;

  /*
   * Initial state
   */
  QCOMPARE(schema.tableCount(), 0);
  QCOMPARE(schema.viewCount(), 0);
  QCOMPARE(schema.tablePopulationCount(), 0);
  /*
   * Add tables
   */
  schema.addTable(client);
  schema.addTable(address);
  QCOMPARE(schema.tableCount(), 2);
  QCOMPARE(schema.tableName(0), client.tableName());
  QCOMPARE(schema.tableName(1), address.tableName());
  /*
   * Add views
   */
  schema.addView(clientAddressView);
  QCOMPARE(schema.viewCount(), 1);
  QCOMPARE(schema.viewName(0), clientAddressView.name());
  /*
   * Add populations
   */
  tp.clear();
  tp.setName("TP1");
  schema.addTablePopulation(tp);
  QCOMPARE(schema.tablePopulationCount(), 1);
  QCOMPARE(schema.tablePopulationName(0), QString("TP1"));
  /*
   * Add triggers
   */
  trigger.clear();
  trigger.setName("TRG1");
  schema.addTrigger(trigger);
  QCOMPARE(schema.triggerCount(), 1);
  QCOMPARE(schema.triggerName(0), QString("TRG1"));
  /*
   * Clear
   */
  schema.clear();
  QCOMPARE(schema.tableCount(), 0);
  QCOMPARE(schema.viewCount(), 0);
  QCOMPARE(schema.tablePopulationCount(), 0);
  QCOMPARE(schema.triggerCount(), 0);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SchemaTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
