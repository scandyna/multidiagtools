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
#include "Mdt/QueryExpression/AbstractSelectQuery.h"
#include <boost/variant.hpp>

/*
 * Select query implementation for tests
 */

class MemorySelectQuery : public Mdt::QueryExpression::AbstractSelectQuery
{
 public:

  MemorySelectQuery(QObject *parent = nullptr)
   : AbstractSelectQuery(parent)
  {
  }

  bool exec(const Mdt::QueryExpression::SelectStatement& statement) override
  {
  }

  bool next() override
  {
  }

  int fieldCount() const override
  {
  }

  QVariant value(int fieldIndex) const override
  {
  }

 private:

  int fieldIndexImpl(const Mdt::QueryExpression::EntityAndField & field) const override
  {
  }

};

using namespace Mdt::QueryExpression;

/*
 * Tests
 */

void SelectQueryTest::instanceTest()
{
//   auto query = SelectQuery::make<MemorySelectQuery>();
//   auto & memoryQuery = query.impl<MemorySelectQuery>();

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
