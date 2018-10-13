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
#include "SelectStatementTest.h"
#include "Mdt/QueryExpression/SelectStatement.h"
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
  boost::apply_visitor(visitor, field.internalVariant().internalVariant());
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
  boost::apply_visitor(visitor, field.internalVariant().internalVariant());
  return visitor.aliasOrName;
}

/*
 * Tests
 */

void SelectStatementTest::setEntityTest()
{
  SelectStatement stm;

  stm.setEntityName("Person");
  QCOMPARE(stm.entity().aliasOrName(), QString("Person"));

  stm.setEntityName(EntityName("Person"), "P");
  QCOMPARE(stm.entity().aliasOrName(), QString("P"));

  SelectEntity address( EntityName("Address"), "ADR");
  stm.setEntity(address);
  QCOMPARE(stm.entity().aliasOrName(), QString("ADR"));

  stm.clear();
  QVERIFY(stm.entity().isNull());
}

void SelectStatementTest::addFieldTest()
{
  SelectEntity person( EntityName("Person") );
  SelectEntity address( EntityName("Address"), "ADR");

  SelectField personId( person, FieldName("id") );
  SelectField personName( person, FieldName("name") );
  SelectField addressId( address, FieldName("id") );
  SelectField addressStreet( address, FieldName("street"), "AddressStreet" );

  SelectStatement stm;
  QCOMPARE(stm.fieldCount(), 0);
  stm.setEntity(person);
  stm.addField(personName);
  stm.addField(person, FieldName("remarks"), "PersonRemarks");
  stm.addField("notices");
  stm.addField(FieldName("age"), "A");
  stm.addField(addressStreet);
  stm.addField(address, FieldName("remarks"), "AddressRemarks");
  QCOMPARE(stm.fieldCount(), 6);
  auto fieldList = stm.fieldList();
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

  stm.selectAllFields();
  fieldList = stm.fieldList();
  QCOMPARE(fieldList.fieldCount(), 1);
  QVERIFY(getEntityAliasOrName(fieldList.at(0)).isEmpty());
  QVERIFY(getFieldAliasOrName(fieldList.at(0)).isEmpty());

  stm.clear();
  fieldList = stm.fieldList();
  QCOMPARE(fieldList.fieldCount(), 0);

  stm.setEntity(person);
  stm.addSelectAllFields(address);
  fieldList = stm.fieldList();
  QCOMPARE(fieldList.fieldCount(), 1);
  QCOMPARE(getEntityAliasOrName(fieldList.at(0)), QString("ADR"));
  QVERIFY(getFieldAliasOrName(fieldList.at(0)).isEmpty());
}

void SelectStatementTest::fieldIndexTest()
{
  SelectEntity person( EntityName("Person") );
  SelectEntity address( EntityName("Address"), "ADR");

  SelectField personId( person, FieldName("id") );
  SelectField personName( person, FieldName("name") );
  SelectField addressId( address, FieldName("id") );
  SelectField addressStreet( address, FieldName("street"), "AddressStreet" );

  SelectStatement stm;
  stm.setEntity(person);
  stm.addField(personName);
  stm.addField(person, FieldName("remarks"), "PersonRemarks");
  stm.addField("notices");
  stm.addField(FieldName("age"), "A");
  stm.addField(addressStreet);
  stm.addField(address, FieldName("remarks"), "AddressRemarks");
  QCOMPARE(stm.fieldIndex(personId), -1);
  QCOMPARE(stm.fieldIndex(personName), 0);
  QCOMPARE(stm.fieldIndex( SelectField(FieldName("PersonRemarks")) ), -1);
  QCOMPARE(stm.fieldIndex( SelectField(person, FieldName("remarks")) ), 1);
}

void SelectStatementTest::joinEntityTest()
{
  SelectEntity person( EntityName("Person") );
  SelectEntity address( EntityName("Address"), "ADR");

  SelectField personId( person, FieldName("id") );
  SelectField addressPersonId( address, FieldName("personId") );

  SelectStatement stm;
  stm.joinEntity(address, addressPersonId == personId);
  QCOMPARE(stm.joinClauseList().clauseCount(), 1);

  stm.clear();
  QCOMPARE(stm.joinClauseList().clauseCount(), 0);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SelectStatementTest test;

  return QTest::qExec(&test, argc, argv);
}
