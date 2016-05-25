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
#include "SchemaTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Schema/FieldTypeInfo.h"
#include "Mdt/Sql/Schema/FieldTypeInfoList.h"
#include "Mdt/Sql/Schema/FieldTypeInfoModel.h"
#include "Mdt/Sql/Schema/Driver.h"
#include <QSqlDatabase>
#include <QComboBox>

void SchemaTest::initTestCase()
{
  QSqlDatabase::addDatabase("QSQLITE", "SQLITE_1");
}

void SchemaTest::cleanupTestCase()
{
  QSqlDatabase::removeDatabase("SQLITE_1");
}

/*
 * Tests
 */

void SchemaTest::fieldTypeInfoTest()
{
  using Mdt::Sql::Schema::FieldTypeInfo;
  using Mdt::Sql::Schema::FieldType;

  FieldTypeInfo fti(FieldType::Boolean, "BOOLEAN");
  QVERIFY(fti.type() == FieldType::Boolean);
  QCOMPARE(fti.name(), QString("BOOLEAN"));
}

void SchemaTest::fieldTypeInfoListTest()
{
  using Mdt::Sql::Schema::FieldTypeInfo;
  using Mdt::Sql::Schema::FieldTypeInfoList;
  using Mdt::Sql::Schema::FieldType;

  /*
   * Initial state
   */
  FieldTypeInfoList ftil;
  QCOMPARE(ftil.size(), 0);
  /*
   * Add one element
   */
  ftil.append(FieldTypeInfo(FieldType::Boolean, "BOOLEAN"));
  QCOMPARE(ftil.size(), 1);
  QVERIFY(ftil.at(0).type() == FieldType::Boolean);
  QCOMPARE(ftil.at(0).name(), QString("BOOLEAN"));
  for(const auto & fti : ftil){
    QVERIFY(fti.type() == FieldType::Boolean);
    QCOMPARE(fti.name(), QString("BOOLEAN"));
  }
  
}

void SchemaTest::fieldTypeInfoModelTest()
{
  using Mdt::Sql::Schema::FieldTypeInfo;
  using Mdt::Sql::Schema::FieldTypeInfoModel;
  using Mdt::Sql::Schema::FieldType;

  FieldTypeInfoModel model;
  QModelIndex index;
  QComboBox cb;
  QSqlDatabase db = QSqlDatabase::database("SQLITE_1", false);
  if(!db.isValid()){
    QSKIP("SQLite driver not available");
  }
  /*
   * Create a SQLite driver
   */
  QVERIFY(db.isValid());
  Mdt::Sql::Schema::Driver driver(db);
  QVERIFY(driver.isValid());
  /*
   * Initial state
   */
  QCOMPARE(model.rowCount(), 0);
  /*
   * Populate
   */
  model.setDriver(driver);
  QVERIFY(model.rowCount() > 0);
  index = model.index(0, 0);
  QCOMPARE(model.data(index), QVariant("BOOLEAN"));
  /*
   * Setup combobox
   */
  cb.setModel(&model);
  /*
   * Play
   */
//   cb.show();
//   while(cb.isVisible()){
//     QTest::qWait(500);
//   }
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SchemaTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
