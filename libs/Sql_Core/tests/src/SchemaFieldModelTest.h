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
#ifndef MDT_SQL_SCHEMA_FIELD_MODEL_TEST_H
#define MDT_SQL_SCHEMA_FIELD_MODEL_TEST_H

#include "TestBase.h"
#include "Mdt/Sql/Schema/FieldType.h"
#include "Mdt/Sql/Schema/FieldList.h"

class SchemaFieldModelTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void fiedTypeListModelTest();

 private:

  // Get a FieldList with N items. Each field will be named 0,1,2,3,...,N-1
  Mdt::Sql::Schema::FieldList getFieldList(int N, Mdt::Sql::Schema::FieldType type);
};

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_MODEL_TEST_H
