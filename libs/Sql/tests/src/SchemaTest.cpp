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
#include "Mdt/Sql/Schema/Field.h"
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

void SchemaTest::collationTest()
{
  using Mdt::Sql::Schema::Collation;

  Collation collation;

  /*
   * Initial state
   */
  QVERIFY(collation.isCaseSensitive());
  QVERIFY(collation.country() == QLocale::AnyCountry);
  QVERIFY(collation.language() == QLocale::AnyLanguage);
  QVERIFY(collation.isNull());
  /*
   * Simple set/get
   */
  collation.setCaseSensitive(false);
  collation.setCountry(QLocale::Switzerland);
  collation.setLanguage(QLocale::French);
  collation.setCharset("utf8");
  QVERIFY(!collation.isCaseSensitive());
  QVERIFY(collation.country() == QLocale::Switzerland);
  QVERIFY(collation.language() == QLocale::French);
  QCOMPARE(collation.charset(), QString("utf8"));
  QVERIFY(!collation.isNull());
  /*
   * Clear
   */
  collation.clear();
  QVERIFY(collation.isCaseSensitive());
  QVERIFY(collation.country() == QLocale::AnyCountry);
  QVERIFY(collation.language() == QLocale::AnyLanguage);
  QVERIFY(collation.charset().isEmpty());
  QVERIFY(collation.isNull());
}

void SchemaTest::fieldTest()
{
  using Mdt::Sql::Schema::Collation;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;

  Field field;

  /*
   * Initial state
   */
  QVERIFY(field.type() == FieldType::UnknownType);
  QVERIFY(!field.isRequired());
  QCOMPARE(field.length(), -1);
  QVERIFY(field.isNull());
  /*
   * Simple set/get check
   */
  // Setup a required integer field
  field.setType(FieldType::Integer);
  QVERIFY(field.isNull());
  field.setName("Id_PK");
  QVERIFY(!field.isNull());
  field.setRequired(true);
  // Check
  QVERIFY(field.type() == FieldType::Integer);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.isRequired());
  /*
   * Clear
   */
  field.clear();
  QVERIFY(field.type() == FieldType::UnknownType);
  QVERIFY(field.name().isEmpty());
  QVERIFY(!field.isRequired());
  QVERIFY(field.defaultValue().isNull());
  QCOMPARE(field.length(), -1);
  QVERIFY(field.collation().isNull());
  QVERIFY(field.isNull());
  /*
   * Simple set/get check
   */
  // Setup a text field
  field.setName("Name");
  QVERIFY(field.isNull());
  field.setType(FieldType::Varchar);
  QVERIFY(!field.isNull());
  field.setLength(50);
  field.setDefaultValue("Empty");
  field.setCaseSensitive(false);
  // Check
  QVERIFY(field.type() == FieldType::Varchar);
  QCOMPARE(field.name(), QString("Name"));
  QVERIFY(!field.isRequired());
  QCOMPARE(field.length(), 50);
  QCOMPARE(field.defaultValue(), QVariant("Empty"));
  QVERIFY(!field.collation().isCaseSensitive());
  /*
   * Clear
   */
  field.clear();
  QVERIFY(field.type() == FieldType::UnknownType);
  QVERIFY(field.name().isEmpty());
  QVERIFY(!field.isRequired());
  QVERIFY(field.defaultValue().isNull());
  QCOMPARE(field.length(), -1);
  QVERIFY(field.collation().isNull());
  QVERIFY(field.isNull());
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
