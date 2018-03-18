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
#include "DataListItemModelTest.h"
#include "Mdt/TestLib/ItemModelInsertRowTest.h"
#include "Mdt/TestLib/ItemModelRemoveRowTest.h"
#include <QVector>
#include <QList>
#include <vector>
#include <list>

using namespace Mdt::Entity;
using namespace Mdt::TestLib;

/*
 * Tests
 */

void DataListItemModelTest::rowResziableTableModelInsertTest()
{
  ArticleTableModel model;
  ItemModelInsertRowTest test(&model);
}

void DataListItemModelTest::rowResziableTableModelRemoveTest()
{
  ArticleTableModel model;
  ItemModelRemoveRowTest test(&model);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  DataListItemModelTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
