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
#ifndef MDT_ERROR_TEST_H
#define MDT_ERROR_TEST_H

#include <QObject>
#include <QtTest/QtTest>

class ErrorTest : public QObject
{
 Q_OBJECT

 private slots:

  void constructAndCopyTest();
  void errorStackTest();
  void setSourceTest();

  void errorLoggerConsoleBackendTest();
  void errorLoggerFileBackendTest();
  void errorLoggerTest();
  void errorLoggerTest_data();
  void errorLoggerConcurrentAccessTest();
  void errorLoggerConcurrentAccessTest_data();

  /*
   * Bug discovered at 20160611
   *
   * This code can produce a core dump:
   * void foo()
   * {
   *   auto error = mdtErrorNew("test", Mdt::Error::Critical, "MyClass");
   *   error.commit();
   * }
   */
  void coreDumpAfterCommitBugTest();
};

#endif // #ifndef MDT_ERROR_TEST_H
