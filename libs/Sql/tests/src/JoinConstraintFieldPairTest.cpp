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
#include "JoinConstraintFieldPairTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/JoinConstraintFieldPair.h"
#include "Mdt/Sql/JoinConstraintFieldPairList.h"
#include "Mdt/Sql/JoinConstraintFieldPairListSqlTransform.h"
#include "Mdt/Sql/SelectTable.h"
#include "Mdt/Sql/Schema/ForeignKey.h"
#include "Mdt/Sql/Schema/ParentTableFieldName.h"
#include "Mdt/Sql/Schema/ChildTableFieldName.h"
#include "Mdt/Sql/Schema/AutoIncrementPrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKey.h"
#include "Mdt/Sql/Schema/Table.h"

namespace Sql = Mdt::Sql;

void JoinConstraintFieldPairTest::initTestCase()
{
  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!mDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
}

void JoinConstraintFieldPairTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void JoinConstraintFieldPairTest::fieldPairTest()
{
  using Sql::JoinConstraintFieldPair;

  JoinConstraintFieldPair fp("a", "b");
  QCOMPARE(fp.leftField(), QString("a"));
  QCOMPARE(fp.rightField(), QString("b"));
}

void JoinConstraintFieldPairTest::fieldPairListTest()
{
  using Sql::JoinConstraintFieldPairList;

  /*
   * Initial state
   */
  JoinConstraintFieldPairList list("A", "B");
  QCOMPARE(list.leftTable(), QString("A"));
  QCOMPARE(list.rightTable(), QString("B"));
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  QVERIFY(list.isNull());
  /*
   * Build
   */
  list.addFieldPair("a1", "b1");
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QVERIFY(!list.isNull());
  QCOMPARE(list.at(0).leftField(), QString("a1"));
  QCOMPARE(list.at(0).rightField(), QString("b1"));
}

void JoinConstraintFieldPairTest::fromForeignKeyTest()
{
  using Sql::JoinConstraintFieldPairList;
  using Sql::Schema::ParentTableFieldName;
  using Sql::Schema::ChildTableFieldName;
  using Sql::Schema::ForeignKey;

  /*
   * Setup foreign key
   */
  ForeignKey fk;
  fk.addKeyFields( ParentTableFieldName("p1"), ChildTableFieldName("c1") );
}

void JoinConstraintFieldPairTest::fromTablesTest()
{
  using Sql::JoinConstraintFieldPairList;
  using Sql::SelectTable;
  using Sql::Schema::ParentTableFieldName;
  using Sql::Schema::ChildTableFieldName;
  using Sql::Schema::ForeignKey;
  using Sql::Schema::Table;
  using Sql::Schema::PrimaryKey;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;

  /*
   * Common table fields
   */
  Field p1;
  p1.setType(FieldType::Integer);
  p1.setName("p1_PK");
  Field p2;
  p2.setType(FieldType::Integer);
  p2.setName("p2_PK");
  Field c1;
  c1.setType(FieldType::Integer);
  c1.setName("c1_FK");
  Field c2;
  c2.setType(FieldType::Integer);
  c2.setName("c2_FK");
  /*
   * 1 field relation
   */
  // Parent table
  Table PA;
  PA.setTableName("PA");
  PA.setAutoIncrementPrimaryKey("Id_PK");
  // Child table
  Table CA;
  CA.setTableName("CA");
  CA.setAutoIncrementPrimaryKey("Id_PK");
  CA.addField(c1);
  ForeignKey fkCA;
  fkCA.setParentTable(PA);
  fkCA.addKeyFields( ParentTableFieldName("Id_PK"), ChildTableFieldName(c1) );
  CA.addForeignKey(fkCA);
  // Select tables
  SelectTable SPA(PA, "SPA");
  SelectTable SCA(CA, "SCA");
  // Check with left: SPA and right: SCA
  JoinConstraintFieldPairList list1 = JoinConstraintFieldPairList::fromTables(SPA, SCA);
  QCOMPARE(list1.leftTable(), QString("SPA"));
  QCOMPARE(list1.rightTable(), QString("SCA"));
  QCOMPARE(list1.size(), 1);
  QCOMPARE(list1.at(0).leftField(), QString("Id_PK"));
  QCOMPARE(list1.at(0).rightField(), QString("c1_FK"));
  // Check with left: SCA and right: SPA
  list1 = JoinConstraintFieldPairList::fromTables(SCA, SPA);
  QCOMPARE(list1.leftTable(), QString("SCA"));
  QCOMPARE(list1.rightTable(), QString("SPA"));
  QCOMPARE(list1.size(), 1);
  QCOMPARE(list1.at(0).leftField(), QString("c1_FK"));
  QCOMPARE(list1.at(0).rightField(), QString("Id_PK"));
  /*
   * 2 fields relation
   */
  // Parent table
  Table PB;
  PB.setTableName("PB");
  PB.addField(p1);
  PB.addField(p2);
  PrimaryKey pkPB;
  pkPB.addField(p1);
  pkPB.addField(p2);
  PB.setPrimaryKey(pkPB);
  // Child table
  Table CB;
  CB.setAutoIncrementPrimaryKey("Id_PK");
  CB.setTableName("CB");
  CB.addField(c1);
  CB.addField(c2);
  ForeignKey fkCB;
  fkCB.setParentTable(PB);
  fkCB.setChildTable(CB);
  fkCB.addKeyFields( ParentTableFieldName(p1), ChildTableFieldName(c1) );
  fkCB.addKeyFields( ParentTableFieldName(p2), ChildTableFieldName(c2) );
  CB.addForeignKey(fkCB);
  // Select tables
  SelectTable SPB(PB, "SPB");
  SelectTable SCB(CB, "SCB");
  // Check with left: SPB and right: SCB
  JoinConstraintFieldPairList list2 = JoinConstraintFieldPairList::fromTables(SPB, SCB);
  QCOMPARE(list2.leftTable(), QString("SPB"));
  QCOMPARE(list2.rightTable(), QString("SCB"));
  QCOMPARE(list2.size(), 2);
  QCOMPARE(list2.at(0).leftField(), QString("p1_PK"));
  QCOMPARE(list2.at(0).rightField(), QString("c1_FK"));
  QCOMPARE(list2.at(1).leftField(), QString("p2_PK"));
  QCOMPARE(list2.at(1).rightField(), QString("c2_FK"));
  // Check with left: SCB and right: SPB
  list2 = JoinConstraintFieldPairList::fromTables(SCB, SPB);
  QCOMPARE(list2.leftTable(), QString("SCB"));
  QCOMPARE(list2.rightTable(), QString("SPB"));
  QCOMPARE(list2.size(), 2);
  QCOMPARE(list2.at(0).leftField(), QString("c1_FK"));
  QCOMPARE(list2.at(0).rightField(), QString("p1_PK"));
  QCOMPARE(list2.at(1).leftField(), QString("c2_FK"));
  QCOMPARE(list2.at(1).rightField(), QString("p2_PK"));
}

void JoinConstraintFieldPairTest::sqlTransformTest()
{
  using Sql::JoinConstraintFieldPairList;
  using Sql::JoinConstraintFieldPairListSqlTransform;

  auto db = mDatabase;
  QString expectedSql;
  JoinConstraintFieldPairList list("A", "B");

  // 1 pair of fields
  list.addFieldPair("a1", "b1");
  expectedSql = "\"A\".\"a1\"=\"B\".\"b1\"";
  QCOMPARE( JoinConstraintFieldPairListSqlTransform::getSql(list, db) , expectedSql );
  // 2 pair of fields
  list.addFieldPair("a2", "b2");
  expectedSql = "\"A\".\"a1\"=\"B\".\"b1\" AND \"A\".\"a2\"=\"B\".\"b2\"";
  QCOMPARE( JoinConstraintFieldPairListSqlTransform::getSql(list, db) , expectedSql );
  // 3 pair of fields
  list.addFieldPair("a3", "b3");
  expectedSql = "\"A\".\"a1\"=\"B\".\"b1\" AND \"A\".\"a2\"=\"B\".\"b2\" AND \"A\".\"a3\"=\"B\".\"b3\"";
  QCOMPARE( JoinConstraintFieldPairListSqlTransform::getSql(list, db) , expectedSql );
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  JoinConstraintFieldPairTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
