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
#include "JoinConstraintExpressionTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/JoinConstraintField.h"
#include "Mdt/Sql/JoinConstraintExpression.h"
#include "Mdt/Sql/Expression/JoinConstraint/Grammar.h"
#include <QSqlDatabase>
#include <boost/proto/matches.hpp>

#include <boost/proto/proto.hpp>

/*
 * Init / cleanup
 */

void JoinConstraintExpressionTest::initTestCase()
{
}

void JoinConstraintExpressionTest::cleanupTestCase()
{
}

/*
 * Compile time tests
 */

template<typename Expr, typename Grammar>
constexpr bool expressionMatchesGrammar()
{
  return boost::proto::matches< Expr, Grammar >::value;
}

void JoinConstraintExpressionTest::grammarComparisonTest()
{
  namespace JoinConstraint = Mdt::Sql::Expression::JoinConstraint;
  namespace Sql = Mdt::Sql;

  using JoinConstraint::Comparison;
  using Sql::JoinConstraintField;
  using Sql::TableName;
  using Sql::FieldName;

  JoinConstraintField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
  JoinConstraintField adrClientId(TableName("Address_tbl"), FieldName("Client_Id_FK"));

  static_assert(  expressionMatchesGrammar< decltype( clientId == 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 == clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId == "ID25" ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId == u8"ID25" ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( "ID25" == clientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( u8"ID25" == clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId == adrClientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( adrClientId == clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId != 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 != clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId != "ID25" ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId != u8"ID25" ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( "ID25" != clientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( u8"ID25" != clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId != adrClientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( adrClientId != clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId < 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 < clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId < "ID25" ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( "ID25" < clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId < adrClientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( adrClientId < clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId <= 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 <= clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId <= "ID25" ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( "ID25" <= clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId <= adrClientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( adrClientId <= clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId > 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 > clientId) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId > "ID25" ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( "ID25" > clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId > adrClientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( adrClientId > clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId >= 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 >= clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId >= "ID25" ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( "ID25" >= clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId >= adrClientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( adrClientId >= clientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId + 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId - 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId << 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId >> 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId % 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId / 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId * 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId && 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId || 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId = 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId + adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId - adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId << adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId >> adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId % adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId / adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId * adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId && adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId || adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId = adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId == (adrClientId < 25) ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( (clientId == adrClientId) == 32 ) , Comparison >() , "" );
//   static_assert( !expressionMatchesGrammar< decltype( clientId == clientId ) , Comparison >() , "" );
}

void JoinConstraintExpressionTest::grammarTest()
{
  namespace JoinConstraint = Mdt::Sql::Expression::JoinConstraint;
  namespace Sql = Mdt::Sql;

  using JoinConstraint::Grammar;
  using Sql::JoinConstraintField;
  using Sql::TableName;
  using Sql::FieldName;

  JoinConstraintField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
  JoinConstraintField adrClientId(TableName("Address_tbl"), FieldName("Client_Id_FK"));

  static_assert(  expressionMatchesGrammar< decltype( clientId == 25 ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId == adrClientId ) , Grammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId && 25 ) , Grammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId && adrClientId ) , Grammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId || 25 ) , Grammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId || adrClientId ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == 25) && (adrClientId == 52) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (adrClientId < 20) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId < 25) && (adrClientId == clientId) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (clientId > 10) && (adrClientId < 100) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (clientId > 10) && (adrClientId < 100) && (adrClientId > 2) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (clientId > 10) && (clientId < 1000) && (adrClientId < 100) && (adrClientId > 2) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == 25) || (adrClientId == 52) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (adrClientId < 20) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId < 25) || (adrClientId == clientId) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (clientId > 10) || (adrClientId < 100) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (clientId > 10) || (adrClientId < 100) || (adrClientId > 2) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (clientId > 10) || (clientId < 1000) || (adrClientId < 100) || (adrClientId > 2) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && ( (adrClientId < 20) || (clientId < 50) ) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( ( (clientId == adrClientId) || (clientId == 2) ) && ( (adrClientId < 20) || (clientId < 50) ) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || ( (adrClientId > 20) && (adrClientId < 50) ) ) , Grammar >() , "" );
}

/*
 * Rutime tests
 */

void JoinConstraintExpressionTest::fieldTest()
{
  namespace Sql = Mdt::Sql;

  using Sql::JoinConstraintField;
  using Sql::TableName;
  using Sql::FieldName;


  JoinConstraintField A(TableName("A_tbl"), FieldName("id_A"));
  QCOMPARE(A.tableName(), QString("A_tbl"));
  QCOMPARE(A.fieldName(), QString("id_A"));
}

void JoinConstraintExpressionTest::expressionContructCopySqliteTest()
{
  namespace Sql = Mdt::Sql;

  using Sql::TableName;
  using Sql::FieldName;
  using Sql::JoinConstraintField;
  using Sql::JoinConstraintExpression;

  auto db = QSqlDatabase::addDatabase("QSQLITE");
  if(!db.isValid()){
    QSKIP("QSQLITE driver is not available - Skip test");
  }

  JoinConstraintField A(TableName("A"), FieldName("a"));
  JoinConstraintField B(TableName("B"), FieldName("b"));
  QString expectedSql;

  /*
   * Default construct
   */
  JoinConstraintExpression exp;
  /*
   * Construct
   */
  JoinConstraintExpression exp1( A == 2 );
  expectedSql = "\"A\".\"a\"=2";
  QCOMPARE(exp1.toSql(db), expectedSql);

}

void JoinConstraintExpressionTest::expressionAssignSqliteTest()
{
  auto db = QSqlDatabase::addDatabase("QSQLITE");
  if(!db.isValid()){
    QSKIP("QSQLITE driver is not available - Skip test");
  }

}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  JoinConstraintExpressionTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
