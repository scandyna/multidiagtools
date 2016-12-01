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
#include "FilterExpressionTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/FilterColumn.h"
#include "Mdt/ItemEditor/LikeExpression.h"
#include "Mdt/ItemEditor/LikeExpressionRegexTransform.h"
#include "Mdt/ItemEditor/Expression/LiteralValue.h"
#include "Mdt/ItemEditor/Expression/LeftTerminal.h"
#include <QRegularExpression>
#include <boost/proto/matches.hpp>
#include <boost/proto/literal.hpp>
#include <boost/proto/transform/arg.hpp>

// #include <QDebug>

/// \todo for sandbox
// #include <QString>
// #include <QRegularExpression>
// #include <QRegularExpressionMatch>
// #include <boost/proto/proto.hpp>

namespace ItemEditor = Mdt::ItemEditor;

void FilterExpressionTest::initTestCase()
{
}

void FilterExpressionTest::cleanupTestCase()
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

void FilterExpressionTest::literalValueTest()
{
  using ItemEditor::Expression::LiteralValue;
  using ItemEditor::FilterColumn;

  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , LiteralValue >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit("ID44") ) , LiteralValue >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(u8"éèj") ) , LiteralValue >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( FilterColumn(1) ) , LiteralValue >() , "" );
}

void FilterExpressionTest::leftTerminalTest()
{
  using ItemEditor::Expression::LeftTerminal;
  using ItemEditor::FilterColumn;

  FilterColumn A(1);

  static_assert(  expressionMatchesGrammar< decltype( A ) , LeftTerminal >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , LeftTerminal >() , "" );
}

/*
 * Runtime tests
 */

void FilterExpressionTest::filterColumnTest()
{
  using ItemEditor::FilterColumn;

  FilterColumn A(1);
  QCOMPARE( boost::proto::_value()(A).columnIndex() , 1 );
}

// void FilterExpressionTest::likeExpressionRegexTransformEscapeTest()
// {
//   using ItemEditor::LikeExpressionRegexTransform;
// 
//   QFETCH(QString, str);
//   QFETCH(QString, token);
//   QFETCH(QString, escapeStr);
//   QFETCH(QString, expectedResult);
// 
//   LikeExpressionRegexTransform::escape(str, token, escapeStr);
//   QCOMPARE( str, expectedResult );
// }

void FilterExpressionTest::likeExpressionRegexTransformEscapeTest()
{
  using ItemEditor::LikeExpressionRegexTransform;

  QFETCH(QString, str);
  QFETCH(QString, expectedResult);

  LikeExpressionRegexTransform::escapeRegexMetacharacters(str);
  QCOMPARE( str, expectedResult );
}

void FilterExpressionTest::likeExpressionRegexTransformEscapeTest_data()
{
  QTest::addColumn<QString>("str");
  QTest::addColumn<QString>("expectedResult");

  /*
   * Check without any metacharacters
   */
  QTest::newRow("A") << "A" << "A";
  QTest::newRow("AB") << "AB" << "AB";
  QTest::newRow("ABC") << "ABC" << "ABC";
  /*
   * Wildcards are transformed to regex metacharacters (later) if they are not escaped.
   * If already escaped, they must be keeped untouched.
   */
  // Check various combinaisons with token ?
  QTest::newRow("A") << "A" << "A";
  QTest::newRow("?") << "?" << "?";
  QTest::newRow("\\?") << "\\?" << "\\?";
  QTest::newRow("?A") << "?A" << "?A";
  QTest::newRow("\\?A") << "\\?A" << "\\?A";
  QTest::newRow("A?") << "A?" << "A?";
  QTest::newRow("A\\?") << "A\\?" << "A\\?";
  QTest::newRow("A?B") << "A?B" << "A?B";
  QTest::newRow("A\\?B") << "A\\?B" << "A\\?B";
  QTest::newRow("??") << "??" << "??";
  QTest::newRow("\\??") << "\\??" << "\\??";
  QTest::newRow("?\\?") << "?\\?" << "?\\?";
  QTest::newRow("\\?\\?") << "\\?\\?" << "\\?\\?";
  // Checks with other tokens
  QTest::newRow("A*") << "A*" << "A*";
  QTest::newRow("A\\*") << "A\\*" << "A\\*";
  QTest::newRow("A_") << "A_" << "A_";
  QTest::newRow("A\\_") << "A\\_" << "A\\_";
  QTest::newRow("A%") << "A%" << "A%";
  QTest::newRow("A\\%") << "A\\%" << "A\\%";
  /*
   * All regexp metacharacters (that are not wildcards) must simply be escaped
   * (note that \ is a metacharacter)
   */
  // Check escaping '\'
  QTest::newRow("\\") << "\\" << "\\\\";
  QTest::newRow("\\\\") << "\\\\" << "\\\\\\\\";
  QTest::newRow("\\A") << "\\A" << "\\\\A";
  QTest::newRow("\\AB") << "\\AB" << "\\\\AB";
  QTest::newRow("A\\") << "A\\" << "A\\\\";
  QTest::newRow("A\\B") << "A\\B" << "A\\\\B";
  QTest::newRow("A\\B\\") << "A\\B\\" << "A\\\\B\\\\";
  QTest::newRow("A\\B\\C") << "A\\B\\C" << "A\\\\B\\\\C";
  QTest::newRow("A\\B\\C\\") << "A\\B\\C\\" << "A\\\\B\\\\C\\\\";
  // Check escaping regex metacharacters
  QTest::newRow("A{") << "A{" << "A\\{";
  QTest::newRow("A\\{") << "A\\{" << "A\\\\\\{";   // Escape '\' and '{'
  QTest::newRow("A}") << "A}" << "A\\}";
  QTest::newRow("A\\}") << "A\\}" << "A\\\\\\}";   // Escape '\' and '}'
  QTest::newRow("A[") << "A[" << "A\\[";
  QTest::newRow("A\\[") << "A\\[" << "A\\\\\\[";   // Escape '\' and '['
  QTest::newRow("A]") << "A]" << "A\\]";
  QTest::newRow("A\\]") << "A\\]" << "A\\\\\\]";   // Escape '\' and ']'
  QTest::newRow("A(") << "A(" << "A\\(";
  QTest::newRow("A\\(") << "A\\(" << "A\\\\\\(";   // Escape '\' and '('
  QTest::newRow("A)") << "A)" << "A\\)";
  QTest::newRow("A\\)") << "A\\)" << "A\\\\\\)";   // Escape '\' and ')'
  /*
   * Check with some expressions that could have some sense
   */
  QTest::newRow("A?B") << "A?B" << "A?B";
  QTest::newRow("A??B") << "A??B" << "A??B";
  QTest::newRow("*A") << "*A" << "*A";
  QTest::newRow("A*") << "A*" << "A*";
  QTest::newRow("*A*") << "*A*" << "*A*";

//   QTest::newRow("A") << "A" << "d" << "a" << "A";
//   QTest::newRow("Ad") << "Ad" << "d" << "a" << "Aad";
//   QTest::newRow("Aad") << "Aad" << "d" << "a" << "Aad";
//   // Check various combinaisons with '\'
//   QTest::newRow("A") << "A" << "d" << "\\" << "A";
//   QTest::newRow("Ad") << "Ad" << "d" << "\\" << "A\\d";
//   QTest::newRow("A\\d") << "A\\d" << "d" << "\\" << "A\\d";
//   QTest::newRow("AdBd") << "AdBd" << "d" << "\\" << "A\\dB\\d";
//   QTest::newRow("A\\dBd") << "A\\dBd" << "d" << "\\" << "A\\dB\\d";
//   QTest::newRow("AdB\\d") << "AdB\\d" << "d" << "\\" << "A\\dB\\d";
//   QTest::newRow("AdB\\C") << "AdB\\C" << "d" << "\\" << "A\\dB\\C";

}

// void FilterExpressionTest::likeExpressionRegexTransformEscapeTest_data()
// {
//   QTest::addColumn<QString>("str");
//   QTest::addColumn<QString>("token");
//   QTest::addColumn<QString>("escapeStr");
//   QTest::addColumn<QString>("expectedResult");
// 
//   /*
//    * Check without any metacharacters
//    */
//   QTest::newRow("A") << "A" << "[" << "\\" << "A";
//   QTest::newRow("AB") << "AB" << "[" << "\\" << "AB";
//   QTest::newRow("ABC") << "ABC" << "[" << "\\" << "ABC";
//   QTest::newRow("A") << "A" << "*" << "\\" << "A";
//   QTest::newRow("AB") << "AB" << "*" << "\\" << "AB";
//   QTest::newRow("ABC") << "ABC" << "*" << "\\" << "ABC";
//   /*
//    * Wildcards are transformed to regex metacharacters (later) if they are not escaped.
//    * If already escaped, they must be keeped untouched.
//    */
//   // Check various combinaisons with token ?
//   QTest::newRow("A") << "A" << "?" << "\\" << "A";
//   QTest::newRow("?") << "?" << "?" << "\\" << "?";
//   QTest::newRow("\\?") << "\\?" << "?" << "\\" << "\\?";
//   QTest::newRow("?A") << "?A" << "?" << "\\" << "?A";
//   QTest::newRow("\\?A") << "\\?A" << "?" << "\\" << "\\?A";
//   QTest::newRow("A?") << "A?" << "?" << "\\" << "A?";
//   QTest::newRow("A\\?") << "A\\?" << "?" << "\\" << "A\\?";
//   // Checks with other tokens
//   QTest::newRow("A*") << "A*" << "*" << "\\" << "A*";
//   QTest::newRow("A\\*") << "A\\*" << "*" << "\\" << "A\\*";
//   QTest::newRow("A_") << "A_" << "_" << "\\" << "A_";
//   QTest::newRow("A\\_") << "A\\_" << "_" << "\\" << "A\\_";
//   QTest::newRow("A%") << "A%" << "%" << "\\" << "A%";
//   QTest::newRow("A\\%") << "A\\%" << "%" << "\\" << "A\\%";
//   /*
//    * All regexp metacharacters (that are not wildcards) must simply be escaped
//    * (note that \ is a metacharacter)
//    */
//   // Check escaping regex metacharacters
//   QTest::newRow("A") << "A" << "{" << "\\" << "A";
//   QTest::newRow("A{") << "A{" << "{" << "\\" << "A\\{";
//   QTest::newRow("A\\{") << "A\\{" << "{" << "\\" << "A\\\\\\{";   // Escape '\' and '{'
//   QTest::newRow("A") << "A" << "}" << "\\" << "A";
//   QTest::newRow("A}") << "A}" << "}" << "\\" << "A\\}";
//   QTest::newRow("A\\}") << "A\\}" << "}" << "\\" << "A\\\\\\}";   // Escape '\' and '}'
//   QTest::newRow("A") << "A" << "[" << "\\" << "A";
//   QTest::newRow("A[") << "A[" << "[" << "\\" << "A\\[";
//   QTest::newRow("A\\[") << "A\\[" << "[" << "\\" << "A\\\\\\[";   // Escape '\' and '['
//   QTest::newRow("A") << "A" << "]" << "\\" << "A";
//   QTest::newRow("A]") << "A]" << "]" << "\\" << "A\\]";
//   QTest::newRow("A\\]") << "A\\]" << "]" << "\\" << "A\\\\\\]";   // Escape '\' and ']'
//   QTest::newRow("A") << "A" << "(" << "\\" << "A";
//   QTest::newRow("A(") << "A(" << "(" << "\\" << "A\\(";
//   QTest::newRow("A\\(") << "A\\(" << "(" << "\\" << "A\\\\\\(";   // Escape '\' and '('
//   QTest::newRow("A") << "A" << ")" << "\\" << "A";
//   QTest::newRow("A)") << "A)" << ")" << "\\" << "A\\)";
//   QTest::newRow("A\\)") << "A\\)" << ")" << "\\" << "A\\\\\\)";   // Escape '\' and ')'
// 
// //   QTest::newRow("A") << "A" << "d" << "a" << "A";
// //   QTest::newRow("Ad") << "Ad" << "d" << "a" << "Aad";
// //   QTest::newRow("Aad") << "Aad" << "d" << "a" << "Aad";
// //   // Check various combinaisons with '\'
// //   QTest::newRow("A") << "A" << "d" << "\\" << "A";
// //   QTest::newRow("Ad") << "Ad" << "d" << "\\" << "A\\d";
// //   QTest::newRow("A\\d") << "A\\d" << "d" << "\\" << "A\\d";
// //   QTest::newRow("AdBd") << "AdBd" << "d" << "\\" << "A\\dB\\d";
// //   QTest::newRow("A\\dBd") << "A\\dBd" << "d" << "\\" << "A\\dB\\d";
// //   QTest::newRow("AdB\\d") << "AdB\\d" << "d" << "\\" << "A\\dB\\d";
// //   QTest::newRow("AdB\\C") << "AdB\\C" << "d" << "\\" << "A\\dB\\C";
// 
// }

void FilterExpressionTest::likeExpressionRegexTransformTest()
{
  using ItemEditor::LikeExpression;
  using ItemEditor::LikeExpressionRegexTransform;

  QFETCH(QString, likeExpression);
  QFETCH(QString, expectedRegex);
  QString regex;

  // Must not compile
//   QCOMPARE( LikeExpressionRegexTransform::getRegexPattern("A") , QString("A") );

  regex = LikeExpressionRegexTransform::getRegexPattern(LikeExpression(likeExpression));
  QCOMPARE(regex, expectedRegex);
}

void FilterExpressionTest::likeExpressionRegexTransformTest_data()
{
  QTest::addColumn<QString>("likeExpression");
  QTest::addColumn<QString>("expectedRegex");

  QTest::newRow("LIKE B") << "B" << "^B$";
  QTest::newRow("LIKE ?B") << "?B" << "^.B$";
  QTest::newRow("LIKE \\?B") << "\\?B" << "^\\?B$";
  QTest::newRow("LIKE '?B'") << "'?B'" << "^\\?B$";
  QTest::newRow("LIKE _B") << "_B" << "^.B$";
  QTest::newRow("LIKE *B") << "*B" << "^.*B$";
  QTest::newRow("LIKE %B") << "%B" << "^.*B$";
  QTest::newRow("LIKE %B?") << "%B?" << "^.*B.$";
  QTest::newRow("LIKE %B%") << "%B%" << "^.*B.*$";
  // Check unescaping wildcards that are not metacharacters in regexp
  QTest::newRow("LIKE \\_") << "\\_" << "^_$";
  // Check escaping regexp metacharacters
  QTest::newRow("LIKE {") << "{" << "^\\{$";
  QTest::newRow("LIKE \\{") << "\\{" << "^\\{$";
  QTest::newRow("LIKE }") << "}" << "^\\}$";
  QTest::newRow("LIKE [") << "[" << "^\\[$";
  QTest::newRow("LIKE ]") << "]" << "^\\]$";
  QTest::newRow("LIKE (") << "(" << "^\\($";
  QTest::newRow("LIKE )") << ")" << "^\\)$";
  QTest::newRow("LIKE ^") << "^" << "^\\^$";
  QTest::newRow("LIKE $") << "$" << "^\\$$";
  QTest::newRow("LIKE .") << "." << "^\\.$";
  QTest::newRow("LIKE |") << "|" << "^\\|$";
  QTest::newRow("LIKE +") << "+" << "^\\+$";
  QTest::newRow("LIKE \\") << "\\" << "^\\\\$";
  // Check escaping regexp abbreviations
  QTest::newRow("LIKE \\d") << "\\d" << "^\\\\d$";
  QTest::newRow("LIKE \\s") << "\\s" << "^\\\\s$";
  QTest::newRow("LIKE \\w") << "\\w" << "^\\\\w$";
  QTest::newRow("LIKE \\D") << "\\D" << "^\\\\D$";
  QTest::newRow("LIKE \\S") << "\\S" << "^\\\\S$";
  QTest::newRow("LIKE \\W") << "\\W" << "^\\\\W$";
  QTest::newRow("LIKE \\N") << "\\N" << "^\\\\N$";
  QTest::newRow("LIKE \\b") << "\\b" << "^\\\\b$";

}

void FilterExpressionTest::likeExpressionRegexTest()
{
  using ItemEditor::LikeExpression;
  using ItemEditor::LikeExpressionRegexTransform;

  QFETCH(QString , str);
  QFETCH(QString , likeExpression);
  QFETCH(bool , expectedMatches);
  QRegularExpression regex;
  QString pattern;

  pattern = LikeExpressionRegexTransform::getRegexPattern( LikeExpression(likeExpression) );
  regex.setPattern(pattern);
  qDebug() << "Pattern: " << pattern << " , error: " << regex.errorString();
  QVERIFY(regex.isValid());
  bool matches = regex.match(str).hasMatch();
  QCOMPARE( matches , expectedMatches );
}

void FilterExpressionTest::likeExpressionRegexTest_data()
{
  QTest::addColumn<QString>("str");
  QTest::addColumn<QString>("likeExpression");
  QTest::addColumn<bool>("expectedMatches");

  QTest::newRow("A LIKE A |y") << "A" << "A" << true;
  QTest::newRow("A LIKE B |n") << "A" << "B" << false;
  QTest::newRow("ABC LIKE A |n") << "ABC" << "A" << false;
  QTest::newRow("ABC LIKE B |n") << "ABC" << "B" << false;
  QTest::newRow("ABC LIKE ?B |n") << "ABC" << "?B" << false;
  QTest::newRow("ABC LIKE B? |n") << "ABC" << "B?" << false;
  QTest::newRow("ABC LIKE ?B? |y") << "ABC" << "?B?" << true;
  QTest::newRow("ABC LIKE _B |n") << "ABC" << "_B" << false;
  QTest::newRow("ABC LIKE B_ |n") << "ABC" << "B_" << false;
  QTest::newRow("ABC LIKE _B_ |y") << "ABC" << "_B_" << true;
  QTest::newRow("ABC LIKE *B |n") << "ABC" << "*B" << false;
  QTest::newRow("ABC LIKE B* |n") << "ABC" << "B*" << false;
  QTest::newRow("ABC LIKE *B* |y") << "ABC" << "*B*" << true;
  QTest::newRow("ABC LIKE %B |n") << "ABC" << "%B" << false;
  QTest::newRow("ABC LIKE B% |n") << "ABC" << "B%" << false;
  QTest::newRow("ABC LIKE %B% |y") << "ABC" << "%B%" << true;

}

// void testFunction(const QString & str)
// {
//   qDebug() << "str: " << str;
// }
// 
// void testFunction(const QRegularExpression & exp)
// {
//   qDebug() << "exp: " << exp;
// }
// 
// void checkMatches(const QString str, const QRegularExpression & exp)
// {
//   qDebug() << "str: " << str << " matches " << exp.match(str) << " : " << exp.match(str).hasMatch();
// }

void FilterExpressionTest::sandbox()
{
//   using ItemEditor::FilterColumn;
// 
//   FilterColumn a(1);
// 
//   ///boost::proto::display_expr( a % "A" );
//   
//   QRegularExpression exp;
//   exp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
//   exp.setPattern("A");
//   checkMatches("A" , exp);
//   checkMatches("ABC" , exp);
//   checkMatches("123ABC" , exp);
//   checkMatches("123BC" , exp);
//   checkMatches("123abc" , exp);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  FilterExpressionTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
