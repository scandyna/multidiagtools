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
#ifndef SELECT_QUERY_TABLE_MODEL_TEST_H
#define SELECT_QUERY_TABLE_MODEL_TEST_H

#include "SqlTestBase.h"
#include "Mdt/QueryExpression/CachedSelectQuery.h"

class SelectQueryTableModelTest : public SqlTestBase
{
 Q_OBJECT

 public:

  SelectQueryTableModelTest();

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void setupTest();
  void setupSignalTest();
  void setStatementTest();
  void setStatementSignalTest();
  void qtModelTest();

 private:

  bool createTestSchema();

  bool insertPerson(int id, const QString & firstName, int age, const QString & remarks);
  bool cleanupPersonTable();

  Mdt::QueryExpression::CachedSelectQuery mQuery;
};

#endif // #ifndef SELECT_QUERY_TABLE_MODEL_TEST_H
