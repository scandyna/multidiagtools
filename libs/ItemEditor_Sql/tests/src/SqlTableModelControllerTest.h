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
#ifndef MDT_ITEM_EDITOR_SQL_TABLE_MODEL_CONTROLLER_TEST_H
#define MDT_ITEM_EDITOR_SQL_TABLE_MODEL_CONTROLLER_TEST_H

#include "SqlTestBase.h"
#include "Schema/ClientAddressSchema.h"
#include "Schema/ClientPopulation.h"
#include <QVariant>

class SqlTableModelControllerTest : public SqlTestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void setModelTest();
  void setDefaultModelTest();
  void setTableTest();
  void selectTest();
  void removeTest();
  void removeTest_data();
  void editSubmitTest();
  void editSelectTest();

 private:

  void createSchema(const Schema::ClientAddressSchema & schema);
  bool deleteClientData();
  bool repopulateClientTable(const Schema::ClientPopulation & tp);
  QVariant getClientNameFromDatabase(int id); 

};

#endif // #ifndef MDT_ITEM_EDITOR_SQL_TABLE_MODEL_CONTROLLER_TEST_H
