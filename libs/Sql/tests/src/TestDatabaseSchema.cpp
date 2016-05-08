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
#include "TestDatabaseSchema.h"
#include "mdtSqlTablePopulationSchema.h"
#include "mdtSqlTableSchema.h"
#include "mdtSqlViewSchema.h"
#include <QtTest/QtTest>

TestDatabaseSchema::TestDatabaseSchema()
{
  using mdtSqlViewSchema::SelectField;

  mdtSqlTableSchema table;
  mdtSqlField field;
  mdtSqlField Client_Id_FK;
  mdtSqlForeignKey fk_Client_Id_FK;

  /*
   * Create Client_tbl
   */
  table.clear();
  table.setTableName("Client_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  table.addField(field, true);
  // Name
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(100);
  table.addField(field, false);
  // FieldA
  field.clear();
  field.setName("FieldA");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // FieldB
  field.clear();
  field.setName("FieldB");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  pvSchema.addTable(table);
  /*
   * Create Address_tbl
   */
  table.clear();
  table.setTableName("Address_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  table.addField(field, true);
  // Client_Id_FK
  Client_Id_FK.clear();
  Client_Id_FK.setName("Client_Id_FK");
  Client_Id_FK.setType(mdtSqlFieldType::Integer);
  Client_Id_FK.setRequired(true);
  table.addField(Client_Id_FK, false);
  // Street
  field.clear();
  field.setName("Street");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(150);
  table.addField(field, false);
  // FieldAA
  field.clear();
  field.setName("FieldAA");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // FieldAB
  field.clear();
  field.setName("FieldAB");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // fk_Client_Id_FK
  fk_Client_Id_FK.clear();
  fk_Client_Id_FK.setParentTableName("Client_tbl");
  fk_Client_Id_FK.addKeyFields("Id_PK", Client_Id_FK);
  table.addForeignKey(fk_Client_Id_FK);
  pvSchema.addTable(table);
  /*
   * Create Address_Client_view
   */
  mdtSqlViewSchema::Schema ADR_CLI_view;
  mdtSqlViewSchema::Table CLI("Client_tbl", "CLI");
  mdtSqlViewSchema::Table ADR("Address_tbl", "ADR");
  mdtSqlViewSchema::JoinClause ADR_CLI_join;
  ADR_CLI_view.setName("Address_Client_view");
  ADR_CLI_view.setTable(ADR);
  ADR_CLI_view.addSelectField(ADR, SelectField("Id_PK"));
  ADR_CLI_view.addSelectField(ADR, SelectField("Client_Id_FK"));
  ADR_CLI_view.addSelectField(ADR, SelectField("Street"));
  ADR_CLI_view.addSelectField(ADR, SelectField("FieldAA"));
  ADR_CLI_view.addSelectField(ADR, SelectField("FieldAB"));
  ADR_CLI_view.addSelectField(CLI, SelectField("Name", "ClientName"));
  ADR_CLI_view.addSelectField(CLI, SelectField("FieldA", "ClientFieldA"));
  ADR_CLI_view.addSelectField(CLI, SelectField("FieldB", "ClientFieldB"));
  ADR_CLI_join = pvSchema.joinClause(ADR, CLI);
  QVERIFY(!ADR_CLI_join.isNull());
  ADR_CLI_view.addJoinClause(ADR_CLI_join);
  pvSchema.addView(ADR_CLI_view);
  /*
   * Create Client2_tbl
   */
  table.clear();
  table.setTableName("Client2_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  table.addField(field, true);
  // Name
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(100);
  table.addField(field, false);
  // FieldA
  field.clear();
  field.setName("FieldA");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // FieldB
  field.clear();
  field.setName("FieldB");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  pvSchema.addTable(table);
  /*
   * Create Address2_tbl
   */
  table.clear();
  table.setTableName("Address2_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  table.addField(field, true);
  // Client_Id_FK
  Client_Id_FK.clear();
  Client_Id_FK.setName("Client_Id_FK");
  Client_Id_FK.setType(mdtSqlFieldType::Integer);
  Client_Id_FK.setRequired(true);
  table.addField(Client_Id_FK, false);
  // Street
  field.clear();
  field.setName("Street");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(150);
  table.addField(field, false);
  // FieldAA
  field.clear();
  field.setName("FieldAA");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // FieldAB
  field.clear();
  field.setName("FieldAB");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // fk_Client_Id_FK
  fk_Client_Id_FK.clear();
  fk_Client_Id_FK.setParentTableName("Client2_tbl");
  fk_Client_Id_FK.addKeyFields("Id_PK", Client_Id_FK);
  table.addForeignKey(fk_Client_Id_FK);
  pvSchema.addTable(table);
}

bool TestDatabaseSchema::createSchema(const QSqlDatabase & db)
{
  return pvSchema.createSchema(db);
}
