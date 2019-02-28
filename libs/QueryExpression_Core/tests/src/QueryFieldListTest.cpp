/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "QueryFieldListTest.h"
#include "Mdt/QueryExpression/SelectFieldList.h"
#include <boost/variant.hpp>

using namespace Mdt::QueryExpression;

/*
 * Helper
 */

SelectAllField getSelectAllField(const SelectField & field)
{
  Q_ASSERT(boost::get<SelectAllField>(&field.internalVariant().internalVariant()) != nullptr);

  return boost::get<SelectAllField>(field.internalVariant().internalVariant());
}

EntityAndField getEntityAndField(const SelectField & field)
{
  Q_ASSERT(boost::get<EntityAndField>(&field.internalVariant().internalVariant()) != nullptr);

  return boost::get<EntityAndField>(field.internalVariant().internalVariant());
}

/*
 * Tests
 */

void QueryFieldListTest::addFieldTest()
{
  SelectEntity person(EntityName("Person"), "P");

  SelectFieldList list;
  QCOMPARE(list.fieldCount(), 0);
  QVERIFY(list.isEmpty());

  list.addField( SelectAllField(person) );
  list.addField(FieldName("id"));
  list.addField(FieldName("age"), "A");
  list.addField(person, FieldName("name"), "PersonName");
  QCOMPARE(list.fieldCount(), 4);
  QVERIFY(!list.isEmpty());
  QCOMPARE( getSelectAllField(list.at(0)).entityAliasOrName(), QString("P") );
  auto ef = getEntityAndField(list.at(1));
  QVERIFY(ef.entityAliasOrName().isEmpty());
  QCOMPARE(ef.fieldAliasOrName(), QString("id"));
  ef = getEntityAndField(list.at(2));
  QVERIFY(ef.entityAliasOrName().isEmpty());
  QCOMPARE(ef.fieldAliasOrName(), QString("A"));
  ef = getEntityAndField(list.at(3));
  QCOMPARE(ef.entityAliasOrName(), QString("P"));
  QCOMPARE(ef.fieldAliasOrName(), QString("PersonName"));

  list.clear();
  QVERIFY(list.isEmpty());
}

void QueryFieldListTest::fieldIndexTest()
{
  SelectEntity person(EntityName("Person"), "P");
  SelectEntity address(EntityName("Address"), "ADR");

  SelectFieldList list;
  list.addField(FieldName("id"));
  list.addField(person, FieldName("id"));
  list.addField(address, FieldName("id"));
  QCOMPARE(list.fieldIndex( SelectField(FieldName("None")) ), -1);
  QCOMPARE(list.fieldIndex( SelectField(FieldName("id")) ), 0);
  QCOMPARE(list.fieldIndex( SelectField(person, FieldName("id")) ), 1);
  QCOMPARE(list.fieldIndex( SelectField(address, FieldName("id")) ), 2);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  QueryFieldListTest test;

  return QTest::qExec(&test, argc, argv);
}
