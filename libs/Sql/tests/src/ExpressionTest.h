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
#ifndef MDT_EXPRESSION_TEST_H
#define MDT_EXPRESSION_TEST_H

#include <QObject>
#include <QtTest/QtTest>
#include <QSqlDatabase>

class ExpressionTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

 private:

  /*
   * Compile time tests
   */

  void terminalTest();
  void comparisonTest();

 private slots:

  void fieldTest();

  void terminalSqlTransformTest();
  void comparisonSqlTransformTest();

 private:

  QSqlDatabase pvDatabase;
};


#endif // #ifndef MDT_EXPRESSION_TEST_H
