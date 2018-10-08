/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_SQLITE_TEST_MAIN_H
#define MDT_SQLITE_TEST_MAIN_H

#include "TestBase.h"
#include "Mdt/Error.h"
#include <QString>

class ErrorDriverSQLiteTest : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void loadAndUseTest();
  void catchUniqueConstraintTest();

 private:

  bool createPersonTable();
  bool addPerson(int id, const QString & name);
  bool clearPersonTable();

  Mdt::Error lasteError() const
  {
    return mLastError;
  }

  void setLastError(const Mdt::Error & error);

  Mdt::Error mLastError;
};

#endif // #ifndef MDT_SQLITE_TEST_MAIN_H
