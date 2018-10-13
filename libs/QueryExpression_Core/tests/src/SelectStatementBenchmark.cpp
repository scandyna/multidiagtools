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
#include "SelectStatementBenchmark.h"
#include "Mdt/QueryExpression/SelectStatement.h"

using namespace Mdt::QueryExpression;

/*
 * Tests
 */

void SelectStatementBenchmark::fieldIndexBenchmark()
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

  QBENCHMARK{
    QCOMPARE(stm.fieldIndex(addressStreet), 4);
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SelectStatementBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
