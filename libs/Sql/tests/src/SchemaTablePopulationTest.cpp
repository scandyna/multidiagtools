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
#include "SchemaTablePopulationTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Schema/TablePopulation.h"
#include "Mdt/Sql/Schema/TablePopulationList.h"
#include "Schema/Client.h"
#include "Schema/ClientPopulation.h"

namespace Sql = Mdt::Sql;

void SchemaTablePopulationTest::initTestCase()
{
}

void SchemaTablePopulationTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SchemaTablePopulationTest::tablePopulationTest()
{
  using Sql::Schema::TablePopulation;

  Schema::Client client;

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
  tp.addRow(1, "Name 1");
  tp.addRow(2, "Name 2");
  tp.addRow(3, "Name 3");
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
  /*
   * Check using non typed methods
   */
  tp.setName("tp");
  tp.setTableName("tp_tbl");
  tp.addFieldName("a");
  QCOMPARE(tp.name(), QString("tp"));
  QCOMPARE(tp.tableName(), QString("tp_tbl"));
  QCOMPARE(tp.fieldcount(), 1);
  QCOMPARE(tp.fieldName(0), QString("a"));
}

void SchemaTablePopulationTest::clientPopulationTest()
{
  Schema::ClientPopulation p;

  QCOMPARE(p.name(), QString("Client data"));
  QCOMPARE(p.tableName(), QString("Client_tbl"));
  QCOMPARE(p.fieldcount(), 2);
  QCOMPARE(p.fieldName(0), QString("Id_PK"));
  QCOMPARE(p.fieldName(1), QString("Name"));
  QCOMPARE(p.fieldNameList(), QStringList({"Id_PK","Name"}));
  QCOMPARE(p.rowCount(), 0);
  /*
   * Check adding clients
   */
  p.addClient(1, "A");
  QCOMPARE(p.rowCount(), 1);
  QCOMPARE(p.data(0, 0), QVariant(1));
  QCOMPARE(p.data(0, 1), QVariant("A"));
  p.addClient("B");
  QCOMPARE(p.rowCount(), 2);
  QVERIFY(p.data(1, 0).isNull());
  QCOMPARE(p.data(1, 1), QVariant("B"));
}

void SchemaTablePopulationTest::tablePopulationListTest()
{
  using Sql::Schema::TablePopulation;
  using Sql::Schema::TablePopulationList;

  Schema::Client client;
  Schema::ClientPopulation clientPopulation;

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
  /*
   * Check editing
   */
  // Setup list
  TablePopulation tp1;
  tp1.setName("tp1");
  tp1.setTableName("tp1_tbl");
  tp1.addFieldName("a");
  TablePopulation tp2;
  tp2.setName("tp2");
  tp2.setTableName("tp2_tbl");
  tp2.addFieldName("b");
  list.append(tp1);
  list.append(tp2);
  // Check
  QCOMPARE(list.size(), 2);
  QCOMPARE(list.at(0).name(), QString("tp1"));
  QCOMPARE(list.at(1).name(), QString("tp2"));
  // Edit tp2
  list[1].setName("tp22");
  QCOMPARE(list.at(1).name(), QString("tp22"));
  /*
   * Check adding table population template
   */
  list.clear();
  list.append(clientPopulation);
  QCOMPARE(list.size(), 1);
  QCOMPARE(list.at(0).tableName(), QString("Client_tbl"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SchemaTablePopulationTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
