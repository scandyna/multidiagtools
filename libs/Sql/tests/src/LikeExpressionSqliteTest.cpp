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
#include "LikeExpressionSqliteTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/LikeExpression.h"
#include "Mdt/Sql/Expression/LikeExpressionSqlTransform.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Schema/LikeExpressionTestSchema.h"
#include <QSqlQuery>
#include <QSqlError>

namespace Sql = Mdt::Sql;

void LikeExpressionSqliteTest::initTestCase()
{
  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!mDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
  // Create a database
  QVERIFY(mTempFile.open());
  mTempFile.close();
  mDatabase.setDatabaseName(mTempFile.fileName());
  QVERIFY(mDatabase.open());
  QSqlQuery query(mDatabase);
  // Specify encoding (is important for some tests)
  QVERIFY(query.exec("PRAGMA encoding = \"UTF-8\""));
}

void LikeExpressionSqliteTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void LikeExpressionSqliteTest::sqlTransformTest()
{
  using Sql::Expression::LikeExpressionSqlTransform;

  QFETCH(QString, likeExpression);
  QFETCH(QString, expectedSql);
  QString sql;
  auto db = mDatabase;

  sql = LikeExpressionSqlTransform::getSql(likeExpression, db);
  QCOMPARE(sql, expectedSql);
}

void LikeExpressionSqliteTest::sqlTransformTest_data()
{
  QTest::addColumn<QString>("likeExpression");
  QTest::addColumn<QString>("expectedSql");

  /*
   * Check without any metacharacters
   */
  QTest::newRow("LIKE A") << "A" << "LIKE 'A'";
  QTest::newRow("LIKE AB") << "AB" << "LIKE 'AB'";
  QTest::newRow("LIKE ABC") << "ABC" << "LIKE 'ABC'";
  /*
   * LikeExpression supports wildcards ? and *
   * In SQL LIKE, ? becomes _ and * becomes %
   * If wildcard ? or * is escaped, it must be unescaped
   * SQL LIKE keywords _ and % must be escaped
   */
  // Check simple transforms
  QTest::newRow("LIKE ?") << "?" << "LIKE '_'";
  QTest::newRow("LIKE *") << "*" << "LIKE '%'";
  // Check simple transforms with escaped wildcards
  QTest::newRow("LIKE \\?") << "\\?" << "LIKE '?'";
  QTest::newRow("LIKE \\*") << "\\*" << "LIKE '*'";
  // Check that SQL LIKE keywords are escaped
  QTest::newRow("LIKE _") << "_" << "LIKE '\\_' ESCAPE '\\'";
  QTest::newRow("LIKE %") << "%" << "LIKE '\\%' ESCAPE '\\'";
  // Check alternate escaped and not escaped wildcards with *
  QTest::newRow("LIKE *\\**") << "*\\**" << "LIKE '%*%'";
  QTest::newRow("LIKE *\\***") << "*\\***" << "LIKE '%*%%'";
  QTest::newRow("LIKE *\\**\\*") << "*\\**\\*" << "LIKE '%*%*'";
  // Check with '
  QTest::newRow("LIKE '") << "'" << "LIKE ''''";
  // Check with '\'
  QTest::newRow("LIKE \\") << "\\" << "LIKE '\\'";
}

void LikeExpressionSqliteTest::sqlTransformBenchmark()
{
  using Sql::Expression::LikeExpressionSqlTransform;

  QString sql;

  QBENCHMARK{
    sql = LikeExpressionSqlTransform::getSql("ABCD?EF\\?GH*IJKL\\*M%2", mDatabase);
  }
  QCOMPARE( sql, QString("LIKE 'ABCD_EF?GH%IJKL*M\\%2' ESCAPE '\\'") );
}

void LikeExpressionSqliteTest::matchTest()
{
  using Like = Sql::LikeExpression;

  Sql::Schema::Driver driver(mDatabase);
  Schema::LikeExpressionTestSchema schema;

  /*
   * Setup data set
   */
  schema.addStrRow("A");
  schema.addStrRow("AB");
  schema.addStrRow("ABC");
  schema.addStrRow("ABCD");
  schema.addMetaRow("?");
  schema.addMetaRow("*");
  schema.addMetaRow("'");
  schema.addMetaRow("_");
  schema.addMetaRow("%");
  schema.addMetaRow("\\");
  schema.addMetaRow("(");
  schema.addMetaRow(")");
  schema.addMetaRow("{");
  schema.addMetaRow("}");
  schema.addMetaRow("[");
  schema.addMetaRow("]");
  schema.addMetaRow("$");
  QVERIFY(driver.dropSchema(schema));
  QVERIFY(driver.createSchema(schema));
  /*
   * Checks
   */
  // No metacharacters in expression
  QCOMPARE( getStrData(Like("A")) , QVariantList({"A"}) );
  QCOMPARE( getMetaData(Like("A")) , QVariantList() );
  // Check with wildcards in expression
  QCOMPARE( getStrData(Like("?")) , QVariantList({"A"}) );
  QCOMPARE( getMetaData(Like("?")) , QVariantList({"?","*","'","_","%","\\","(",")","{","}","[","]","$"}) );
  QCOMPARE( getStrData(Like("\\?")) , QVariantList() );
  QCOMPARE( getMetaData(Like("\\?")) , QVariantList({"?"}) );
  QCOMPARE( getStrData(Like("*")) , QVariantList({"A","AB","ABC","ABCD"}) );
  QCOMPARE( getMetaData(Like("*")) , QVariantList({"?","*","'","_","%","\\","(",")","{","}","[","]","$"}) );
  QCOMPARE( getStrData(Like("\\*")) , QVariantList() );
  QCOMPARE( getMetaData(Like("\\*")) , QVariantList({"*"}) );
  // Check matching '\'
  QCOMPARE( getStrData(Like("\\")) , QVariantList() );
  QCOMPARE( getMetaData(Like("\\")) , QVariantList({"\\"}) );
  // Check matching '
  QCOMPARE( getStrData(Like("'")) , QVariantList() );
  QCOMPARE( getMetaData(Like("'")) , QVariantList({"'"}) );
  // Check with SQL LIKE metacharacters in expression
  QCOMPARE( getStrData(Like("_")) , QVariantList() );
  QCOMPARE( getMetaData(Like("_")) , QVariantList({"_"}) );
  QCOMPARE( getStrData(Like("%")) , QVariantList() );
  QCOMPARE( getMetaData(Like("%")) , QVariantList({"%"}) );
  // Check matching other special chars
  QCOMPARE( getStrData(Like("(")) , QVariantList() );
  QCOMPARE( getMetaData(Like("(")) , QVariantList({"("}) );
  QCOMPARE( getStrData(Like("[")) , QVariantList() );
  QCOMPARE( getMetaData(Like("[")) , QVariantList({"["}) );
  QCOMPARE( getStrData(Like("[a-Z]")) , QVariantList() );
  QCOMPARE( getMetaData(Like("[a-Z]")) , QVariantList() );
  QCOMPARE( getStrData(Like("{")) , QVariantList() );
  QCOMPARE( getMetaData(Like("{")) , QVariantList({"{"}) );
  // Check some more realistic expressions
  QCOMPARE( getStrData(Like("B")) , QVariantList() );
  QCOMPARE( getMetaData(Like("B")) , QVariantList() );
  QCOMPARE( getStrData(Like("?B")) , QVariantList({"AB"}) );
  QCOMPARE( getMetaData(Like("?B")) , QVariantList() );
  QCOMPARE( getStrData(Like("B?")) , QVariantList() );
  QCOMPARE( getMetaData(Like("B?")) , QVariantList() );
  QCOMPARE( getStrData(Like("*B")) , QVariantList({"AB"}) );
  QCOMPARE( getMetaData(Like("*B")) , QVariantList() );
  QCOMPARE( getStrData(Like("B*")) , QVariantList() );
  QCOMPARE( getMetaData(Like("B*")) , QVariantList() );
  QCOMPARE( getStrData(Like("?B*")) , QVariantList({"AB","ABC","ABCD"}) );
  QCOMPARE( getMetaData(Like("?B*")) , QVariantList() );
}

/*
 * Helper
 */

QVariantList LikeExpressionSqliteTest::getStrData(const Sql::LikeExpression& expr)
{
  return getData("StrData_tbl", expr);
}

QVariantList LikeExpressionSqliteTest::getMetaData(const Sql::LikeExpression& expr)
{
  return getData("MetaData_tbl", expr);
}

QVariantList LikeExpressionSqliteTest::getData(const QString& tableName, const Sql::LikeExpression& expr)
{
  using Sql::Expression::LikeExpressionSqlTransform;

  QVariantList data;
  QString sql = "SELECT Data FROM " + tableName;
  sql += " WHERE Data " + LikeExpressionSqlTransform::getSql(expr.expression(), mDatabase);
  QSqlQuery query(mDatabase);
  if(!query.exec(sql)){
    qDebug() << "Error: " << query.lastError();
    return data;
  }
  while(query.next()){
    data << query.value(0);
  }

  return data;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  LikeExpressionSqliteTest test;

  return QTest::qExec(&test, argc, argv);
}
