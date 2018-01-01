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
#include "StlListModelBenchmark.h"
#include "Mdt/ItemModel/ReadOnlyStlListModel.h"
#include "Mdt/ItemModel/EditableStlListModel.h"
#include <QModelIndex>
#include <vector>
#include <list>
#include <ctime>
#include <cstdlib>
#include <algorithm>

#include <QDebug>

using namespace Mdt::ItemModel;

void StlListModelBenchmark::initTestCase()
{
  std::srand(std::time(0));
}

void StlListModelBenchmark::cleanupTestCase()
{
}

/*
 * Helpers
 */

template<typename Container>
int fillContainer(Container & container)
{
  const int n = 100;

  container.resize(n);
  std::generate(container.begin(), container.end(), std::rand);

  return n;
}

/*
 * Tests
 */

void StlListModelBenchmark::stdVectorSizeBenchmark()
{
  std::vector<int> v;
  const int n = fillContainer(v);
  int size;
  QBENCHMARK{
    size = v.size();
  }
  QCOMPARE(size, n);
}

void StlListModelBenchmark::stlModelVectorRowCountBenchmark()
{
  std::vector<int> v;
  ReadOnlyStlListModel<std::vector<int>> model;
  const int n = fillContainer(v);
  model.setContainer(v);
  int size;
  QBENCHMARK{
    size = model.rowCount();
  }
  QCOMPARE(size, n);
}

void StlListModelBenchmark::stlModelVectorRowCountQModelIndexBenchmark()
{
  std::vector<int> v;
  ReadOnlyStlListModel<std::vector<int>> model;
  const int n = fillContainer(v);
  model.setContainer(v);
  const QModelIndex invalidParent;
  int size;
  QBENCHMARK{
    size = model.rowCount(invalidParent);
  }
  QCOMPARE(size, n);
}

void StlListModelBenchmark::stdListSizeBenchmark()
{
  std::list<int> l;
  const int n = fillContainer(l);
  int size;
  QBENCHMARK{
    size = l.size();
  }
  QCOMPARE(size, n);
}

void StlListModelBenchmark::stlModelListRowCountBenchmark()
{
  std::list<int> l;
  ReadOnlyStlListModel<std::list<int>> model;
  const int n = fillContainer(l);
  model.setContainer(l);
  int size;
  QBENCHMARK{
    size = model.rowCount();
  }
  QCOMPARE(size, n);
}

// void StlListModelBenchmark::stdVectorAtBenchmark()
// {
//   QFAIL("Not complete");
// }

// void StlListModelBenchmark::stdListAtBenchmark()
// {
//   QFAIL("Not complete");
// }


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  StlListModelBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
