/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "SchemaTableBenchmark.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Schema/Client.h"
#include "Schema/Address.h"

using namespace Mdt::Sql::Schema;

void SchemaTableBenchmark::initTestCase()
{
}

void SchemaTableBenchmark::cleanupTestCase()
{
}

/*
 * Tests
 */

void SchemaTableBenchmark::tablePrimaryKeyAicBenchmark()
{
  /*
   * Setup fields
   */
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);

  QBENCHMARK{
    Table table;
    table.setAutoIncrementPrimaryKey("Id_PK");
    table.addField(Name);
    QCOMPARE(table.fieldCount(), 2);
    QCOMPARE(table.fieldName(0), QString("Id_PK"));
    QCOMPARE(table.fieldName(1), QString("Name"));
    QVERIFY(table.isFieldPartOfPrimaryKey(0));
    QVERIFY(!table.isFieldPartOfPrimaryKey(1));
    QVERIFY(table.isFieldAutoIncrement(0));
    QVERIFY(!table.isFieldAutoIncrement(1));
    QVERIFY(table.fieldType(0) == FieldType::Integer);
    QVERIFY(table.fieldType(1) == FieldType::Varchar);
    QCOMPARE(table.fieldLength(0), -1);
    QCOMPARE(table.fieldLength(1), 100);
    QCOMPARE(table.fieldIndex("Id_PK"), 0);
    QCOMPARE(table.fieldIndex("Name"), 1);
  }
}

void SchemaTableBenchmark::tablePrimaryKeyMcBenchmark()
{
  /*
   * Setup fields
   */
  // Id_A
  Field Id_A;
  Id_A.setName("Id_A");
  Id_A.setType(FieldType::Integer);
  // Id_B
  Field Id_B;
  Id_B.setName("Id_B");
  Id_B.setType(FieldType::Integer);
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);

  QBENCHMARK{
    Table table;
    table.setPrimaryKey(Id_A, Id_B);
    table.addField(Name);
    QCOMPARE(table.fieldCount(), 3);
    QCOMPARE(table.fieldName(0), QString("Id_A"));
    QCOMPARE(table.fieldName(1), QString("Id_B"));
    QCOMPARE(table.fieldName(2), QString("Name"));
    QVERIFY(table.isFieldPartOfPrimaryKey(0));
    QVERIFY(table.isFieldPartOfPrimaryKey(1));
    QVERIFY(!table.isFieldPartOfPrimaryKey(2));
    QVERIFY(!table.isFieldAutoIncrement(0));
    QVERIFY(!table.isFieldAutoIncrement(1));
    QVERIFY(!table.isFieldAutoIncrement(2));
    QVERIFY(table.fieldType(0) == FieldType::Integer);
    QVERIFY(table.fieldType(1) == FieldType::Integer);
    QVERIFY(table.fieldType(2) == FieldType::Varchar);
    QCOMPARE(table.fieldLength(0), -1);
    QCOMPARE(table.fieldLength(1), -1);
    QCOMPARE(table.fieldLength(2), 100);
    QCOMPARE(table.fieldIndex("Id_A"), 0);
    QCOMPARE(table.fieldIndex("Id_B"), 1);
    QCOMPARE(table.fieldIndex("Name"), 2);
  }
}

void SchemaTableBenchmark::createClientTableBenchmark()
{
  QString tableName;
  QString idPkFieldName;
  QString nameFieldName;

  QBENCHMARK{
    Schema::Client client;
    tableName = client.tableName();
    idPkFieldName = client.Id_PK().fieldName();
    nameFieldName = client.Name().name();
  }
  QCOMPARE(tableName, QString("Client_tbl"));
  QCOMPARE(idPkFieldName, QString("Id_PK"));
  QCOMPARE(nameFieldName, QString("Name"));
}

void SchemaTableBenchmark::createAddressTableBenchmark()
{
  QString tableName;
  QString idPkFieldName;
  QString streetFieldName;

  QBENCHMARK{
    Schema::Address address;
    tableName = address.tableName();
    idPkFieldName = address.Id_PK().fieldName();
    streetFieldName = address.Street().name();
  }
  QCOMPARE(tableName, QString("Address_tbl"));
  QCOMPARE(idPkFieldName, QString("Id_PK"));
  QCOMPARE(streetFieldName, QString("Street"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaTableBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
