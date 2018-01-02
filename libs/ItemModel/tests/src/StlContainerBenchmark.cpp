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
#include "StlContainerBenchmark.h"
#include "Mdt/ItemModel/StlContainer.h"
#include <QModelIndex>
#include <vector>
#include <list>
#include <ctime>
#include <cstdlib>
#include <algorithm>

using namespace Mdt::ItemModel;

void StlContainerBenchmark::initTestCase()
{
  std::srand(std::time(0));
}

void StlContainerBenchmark::cleanupTestCase()
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

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
    StlContainerBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
