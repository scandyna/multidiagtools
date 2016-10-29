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
#include "SelectStatementTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/SelectStatement.h"
#include "Mdt/Sql/FieldName.h"
#include "Mdt/Sql/TableName.h"
#include "Mdt/Sql/Schema/Field.h"
#include "Mdt/Sql/Schema/AutoIncrementPrimaryKey.h"
#include "Mdt/Sql/Schema/SingleFieldPrimaryKey.h"
#include "Schema/Client_tbl.h"

namespace Sql = Mdt::Sql;

void SelectStatementTest::initTestCase()
{
}

void SelectStatementTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SelectStatementTest::addFieldTest()
{
  using Sql::SelectStatement;
  using Sql::SelectField;
  using Sql::SelectTable;
  using Sql::FieldName;
  using Sql::TableName;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::AutoIncrementPrimaryKey;
  using Sql::Schema::SingleFieldPrimaryKey;

  SelectTable CLI(TableName("Client_tbl"), "CLI");
  Field field;
  field.setType(FieldType::Integer);
  SingleFieldPrimaryKey sfpk;
  sfpk.setFieldType(FieldType::Integer);
  SelectStatement stm;

  /*
   * Add fields using avaliable overloads
   */
  stm.addField(CLI, SelectField("A", "A_alias") );
  stm.addField(CLI, FieldName("B"), "B_alias");
  stm.addField(CLI, FieldName("C"));
  field.setName("D");
  stm.addField(CLI, field, "D_alias");
  field.setName("E");
  stm.addField(CLI, field);
  stm.addField(CLI, AutoIncrementPrimaryKey("F"), "F_alias");
  stm.addField(CLI, AutoIncrementPrimaryKey("G"));
  sfpk.setFieldName("H");
  stm.addField(CLI, sfpk, "H_alias");
  sfpk.setFieldName("I");
  stm.addField(CLI, sfpk);
  /*
   * Check
   */
  auto list = stm.fieldList();
  QCOMPARE(list.size(), 9);
  QCOMPARE(list.selectFieldAt(0).fieldName(), QString("A"));
  QCOMPARE(list.selectFieldAt(0).alias(), QString("A_alias"));
  QCOMPARE(list.selectFieldAt(1).fieldName(), QString("B"));
  QCOMPARE(list.selectFieldAt(1).alias(), QString("B_alias"));
  QCOMPARE(list.selectFieldAt(2).fieldName(), QString("C"));
  QVERIFY(list.selectFieldAt(2).alias().isEmpty());
  QCOMPARE(list.selectFieldAt(3).fieldName(), QString("D"));
  QCOMPARE(list.selectFieldAt(3).alias(), QString("D_alias"));
  QCOMPARE(list.selectFieldAt(4).fieldName(), QString("E"));
  QVERIFY(list.selectFieldAt(4).alias().isEmpty());
  QCOMPARE(list.selectFieldAt(5).fieldName(), QString("F"));
  QCOMPARE(list.selectFieldAt(5).alias(), QString("F_alias"));
  QCOMPARE(list.selectFieldAt(6).fieldName(), QString("G"));
  QVERIFY(list.selectFieldAt(6).alias().isEmpty());
  QCOMPARE(list.selectFieldAt(7).fieldName(), QString("H"));
  QCOMPARE(list.selectFieldAt(7).alias(), QString("H_alias"));
  QCOMPARE(list.selectFieldAt(8).fieldName(), QString("I"));
  QVERIFY(list.selectFieldAt(8).alias().isEmpty());
}

void SelectStatementTest::addAllFieldsTest()
{
  QFAIL("Not implemented");
}

void SelectStatementTest::simpleSelectTest()
{
  using Sql::SelectStatement;
  using Sql::SelectField;
  using Sql::SelectTable;
  using Sql::FieldName;
  using Sql::TableName;

  SelectTable CLI(TableName("Client_tbl"), "CLI");
  SelectStatement stm;

  stm.addField(CLI, SelectField("Id_PK", "Client_Id") );
  stm.addField(CLI, FieldName("Name"), "ClientName");
  stm.addField(CLI, FieldName("Remarks"));
}

void SelectStatementTest::simpleSelectWithSchemaTest()
{
  using Sql::SelectStatement;
  using Sql::SelectField;
  using Sql::SelectTable;

  Schema::Client_tbl client;
  SelectTable CLI(client, "CLI");
  SelectStatement stm;

  stm.addField(CLI, client.Id_PK(), "Client_Id");
  stm.addField(CLI, client.Name());
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SelectStatementTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
