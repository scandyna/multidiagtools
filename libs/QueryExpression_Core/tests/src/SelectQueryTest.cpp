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
#include "SelectQueryTest.h"
#include "Mdt/QueryExpression/SelectQuery.h"
#include <boost/variant.hpp>

using namespace Mdt::QueryExpression;

/*
 * Helper to get entity alias or name
 */

struct GetEntityAliasOrNameVisitor : public boost::static_visitor<>
{
  void operator()(const SelectAllField & field)
  {
    aliasOrName = field.entityAliasOrName();
  }

  void operator()(const EntityAndField & field)
  {
    aliasOrName = field.entityAliasOrName();
  }

  QString aliasOrName;
};

QString getEntityAliasOrName(const SelectField & field)
{
  GetEntityAliasOrNameVisitor visitor;
  boost::apply_visitor(visitor, field.internalVariant());
  return visitor.aliasOrName;
}

/*
 * Helper to get field alias or name
 */

struct GetFieldAliasOrNameVisitor : public boost::static_visitor<>
{
  void operator()(const SelectAllField &)
  {
  }

  void operator()(const EntityAndField & field)
  {
    aliasOrName = field.fieldAliasOrName();
  }

  QString aliasOrName;
};

QString getFieldAliasOrName(const SelectField & field)
{
  GetFieldAliasOrNameVisitor visitor;
  boost::apply_visitor(visitor, field.internalVariant());
  return visitor.aliasOrName;
}

/*
 * Tests
 */

void SelectQueryTest::setEntityTest()
{
  SelectQuery query;

  query.setEntityName("Person");
  QCOMPARE(query.entity().aliasOrName(), QString("Person"));

  query.setEntityName(EntityName("Person"), "P");
  QCOMPARE(query.entity().aliasOrName(), QString("P"));

  SelectEntity address( EntityName("Address"), "ADR");
  query.setEntity(address);
  QCOMPARE(query.entity().aliasOrName(), QString("ADR"));

  query.clear();
  QVERIFY(query.entity().isNull());
}

void SelectQueryTest::addFieldTest()
{
  SelectEntity person( EntityName("Person") );
  SelectEntity address( EntityName("Address"), "ADR");

  SelectField personId( person, FieldName("id") );
  SelectField personName( person, FieldName("name") );
  SelectField addressId( address, FieldName("id") );
  SelectField addressStreet( address, FieldName("street"), "AddressStreet" );

  SelectQuery query;
  query.setEntity(person);
  query.addField(personName);
  query.addField(person, FieldName("remarks"), "PersonRemarks");
  query.addField("notices");
  query.addField(FieldName("age"), "A");
  query.addField(addressStreet);
  query.addField(address, FieldName("remarks"), "AddressRemarks");
  auto fieldList = query.fieldList();
  QCOMPARE(fieldList.fieldCount(), 6);
  QCOMPARE(getEntityAliasOrName(fieldList.at(0)), QString("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(0)), QString("name"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(1)), QString("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(1)), QString("PersonRemarks"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(2)), QString());
  QCOMPARE(getFieldAliasOrName(fieldList.at(2)), QString("notices"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(3)), QString());
  QCOMPARE(getFieldAliasOrName(fieldList.at(3)), QString("A"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(4)), QString("ADR"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(4)), QString("AddressStreet"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(5)), QString("ADR"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(5)), QString("AddressRemarks"));

  query.selectAllFields();
  fieldList = query.fieldList();
  QCOMPARE(fieldList.fieldCount(), 1);
  QVERIFY(getEntityAliasOrName(fieldList.at(0)).isEmpty());
  QVERIFY(getFieldAliasOrName(fieldList.at(0)).isEmpty());

  query.clear();
  fieldList = query.fieldList();
  QCOMPARE(fieldList.fieldCount(), 0);

  query.setEntity(person);
  query.addSelectAllFields(address);
  fieldList = query.fieldList();
  QCOMPARE(fieldList.fieldCount(), 1);
  QCOMPARE(getEntityAliasOrName(fieldList.at(0)), QString("ADR"));
  QVERIFY(getFieldAliasOrName(fieldList.at(0)).isEmpty());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SelectQueryTest test;

  return QTest::qExec(&test, argc, argv);
}
