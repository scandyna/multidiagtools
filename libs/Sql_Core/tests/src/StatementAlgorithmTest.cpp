/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "StatementAlgorithmTest.h"
#include "Mdt/Sql/StatementAlgorithm.h"
#include "Mdt/Sql/QueryExpressionSqlTransform.h"
#include <QLatin1String>
#include <QString>

using namespace Mdt::Sql;

void StatementAlgorithmTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
}

void StatementAlgorithmTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void StatementAlgorithmTest::filterExpressionFromPrimaryKeyRecordTest()
{
  Mdt::QueryExpression::FilterExpression filter;
  const auto db = database();
  QString expectedSql;

  PrimaryKeyRecord pkr1;
  pkr1.addValue(FieldName("Id_PK"), 12);
  filter = filterExpressionFromPrimaryKeyRecord(pkr1);
  expectedSql = "\"Id_PK\"=12";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  PrimaryKeyRecord pkr2;
  pkr2.addValue(FieldName("Id_A_PK"), 1);
  pkr2.addValue(FieldName("Id_B_PK"), 2);
  filter = filterExpressionFromPrimaryKeyRecord(pkr2);
  expectedSql = "(\"Id_A_PK\"=1)AND(\"Id_B_PK\"=2)";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  PrimaryKeyRecord pkr3;
  pkr3.addValue(FieldName("Id_A_PK"), 1);
  pkr3.addValue(FieldName("Id_B_PK"), 2);
  pkr3.addValue(FieldName("Id_C_PK"), 3);
  filter = filterExpressionFromPrimaryKeyRecord(pkr3);
  expectedSql = "((\"Id_A_PK\"=1)AND(\"Id_B_PK\"=2))AND(\"Id_C_PK\"=3)";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  PrimaryKeyRecord pkr4;
  pkr4.addValue(FieldName("Id_A_PK"), 1);
  pkr4.addValue(FieldName("Id_B_PK"), 2);
  pkr4.addValue(FieldName("Id_C_PK"), 3);
  pkr4.addValue(FieldName("Id_D_PK"), 4);
  filter = filterExpressionFromPrimaryKeyRecord(pkr4);
  expectedSql = "(((\"Id_A_PK\"=1)AND(\"Id_B_PK\"=2))AND(\"Id_C_PK\"=3))AND(\"Id_D_PK\"=4)";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);
}

void StatementAlgorithmTest::filterExpressionFromPrimaryKeyRecordWithTableNameTest()
{
  Mdt::QueryExpression::FilterExpression filter;
  const auto db = database();
  QString expectedSql;

  PrimaryKeyRecord pkr1;
  pkr1.addValue(FieldName("Id_PK"), 12);
  filter = filterExpressionFromPrimaryKeyRecord("Person_tbl", pkr1);
  expectedSql = "\"Person_tbl\".\"Id_PK\"=12";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  PrimaryKeyRecord pkr2;
  pkr2.addValue(FieldName("Id_A_PK"), 1);
  pkr2.addValue(FieldName("Id_B_PK"), 2);
  filter = filterExpressionFromPrimaryKeyRecord("Person_tbl", pkr2);
  expectedSql = "(\"Person_tbl\".\"Id_A_PK\"=1)AND(\"Person_tbl\".\"Id_B_PK\"=2)";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  PrimaryKeyRecord pkr3;
  pkr3.addValue(FieldName("Id_A_PK"), 1);
  pkr3.addValue(FieldName("Id_B_PK"), 2);
  pkr3.addValue(FieldName("Id_C_PK"), 3);
  filter = filterExpressionFromPrimaryKeyRecord("Person_tbl", pkr3);
  expectedSql = "((\"Person_tbl\".\"Id_A_PK\"=1)AND(\"Person_tbl\".\"Id_B_PK\"=2))AND(\"Person_tbl\".\"Id_C_PK\"=3)";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  PrimaryKeyRecord pkr4;
  pkr4.addValue(FieldName("Id_A_PK"), 1);
  pkr4.addValue(FieldName("Id_B_PK"), 2);
  pkr4.addValue(FieldName("Id_C_PK"), 3);
  pkr4.addValue(FieldName("Id_D_PK"), 4);
  filter = filterExpressionFromPrimaryKeyRecord("Person_tbl", pkr4);
  expectedSql = "(((\"Person_tbl\".\"Id_A_PK\"=1)AND(\"Person_tbl\".\"Id_B_PK\"=2))AND(\"Person_tbl\".\"Id_C_PK\"=3))AND(\"Person_tbl\".\"Id_D_PK\"=4)";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  StatementAlgorithmTest test;

  return QTest::qExec(&test, argc, argv);
}

