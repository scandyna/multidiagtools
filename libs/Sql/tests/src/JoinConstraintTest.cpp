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
#include "JoinConstraintTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/JoinConstraint.h"
#include "Mdt/Sql/SelectTable.h"
#include "Schema/Client_tbl.h"
#include "Schema/Address_tbl.h"

namespace Sql = Mdt::Sql;

/*
 * Init and cleanup
 */

void JoinConstraintTest::initTestCase()
{
  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!mDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
}

void JoinConstraintTest::cleanupTestCase()
{
}


/*
 * Tests
 */

void JoinConstraintTest::setOnExpressionTest()
{
  using Sql::JoinConstraint;
  using Sql::JoinConstraintOperator;
  using Sql::JoinConstraintField;
  using Sql::SelectTable;
  using Sql::TableName;
  using Sql::FieldName;

  SelectTable CLI(TableName("Client_tbl"), "CLI");
  SelectTable ADR(TableName("Address_tbl"), "ADR");
  JoinConstraintField clientId(CLI, FieldName("Id_PK"));
  JoinConstraintField adrClientId(ADR, FieldName("Client_Id_FK"));

  /*
   * Initial state
   */
  JoinConstraint jc;
  QVERIFY(jc.isNull());
  /*
   * Set expression
   */
  jc.setOnExpression(adrClientId == clientId);
  QVERIFY(!jc.isNull());
  QVERIFY(jc.constraintOperator() == JoinConstraintOperator::On);
  
}

void JoinConstraintTest::onExpressionSqlTransformTest()
{

}

void JoinConstraintTest::setOnExpressionFromTablesTest()
{
  using Sql::JoinConstraint;
  using Sql::JoinConstraintOperator;
  using Sql::SelectTable;

  Schema::Client_tbl client;
  Schema::Address_tbl address;
  SelectTable CLI(client, "CLI");
  SelectTable ADR(address, "ADR");

  /*
   * Initial state
   */
  JoinConstraint jc;
  QVERIFY(jc.isNull());
  /*
   * Set expression
   */
  jc.setOnExpression(CLI, ADR);
  QVERIFY(!jc.isNull());
  QVERIFY(jc.constraintOperator() == JoinConstraintOperator::On);

}

void JoinConstraintTest::onExpressionFromTablesSqlTransformTest()
{

}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  JoinConstraintTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
