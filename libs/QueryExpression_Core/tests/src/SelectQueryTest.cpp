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

using namespace Mdt::QueryExpression;

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

  QFAIL("Not complete");
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
