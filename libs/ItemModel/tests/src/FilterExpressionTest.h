/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_FILTER_EXPRESSION_TEST_H
#define MDT_ITEM_MODEL_FILTER_EXPRESSION_TEST_H

#include <QObject>
#include <QtTest/QtTest>

class FilterExpressionTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

 private:

  /*
   * Compile time tests
   */

  void literalValueTest();
  void leftTerminalTest();
  void rightTerminalTest();
  void filterComparisonTest();
  void filterExpressionGrammarTest();
  void relationFilterComparisonTest();
  void relationExpressionGrammarTest();

 private slots:

  /*
   * Runtime tests
   */

  void filterColumnTest();
  void parentModelColumnTest();

  void parentModelEvalDataTest();
  void evalDataTest();
  void comparisonEvalTest();
  void filterEvalTest();
  void relationFilterEvalTest();

  void expressionCopyTest();
  void expressionTest();
  void expressionRelationTest();
  void expressionBenchmark();

  void relationExpressionCopyTest();
  void relationExpressionTest();
};

#endif // #ifndef MDT_ITEM_MODEL_FILTER_EXPRESSION_TEST_H
