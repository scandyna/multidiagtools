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
#include "DataWidgetMapperBenchmark.h"
#include "Mdt/ItemEditor/DataWidgetMapper.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QLineEdit>

using namespace Mdt::ItemEditor;
using namespace Mdt::ItemModel;

/*
 * Benchmarks
 */

void DataWidgetMapperBenchmark::setCurrentRowBenchmark()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QLineEdit editor0;

  model.populate(10, 1);
  mapper.setModel(&model);
  mapper.addMapping(&editor0, 0);
  editor0.show();
  QBENCHMARK{
    mapper.setCurrentRow(0);
    QCOMPARE(mapper.currentRow(), 0);
    mapper.setCurrentRow(1);
    QCOMPARE(mapper.currentRow(), 1);
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  DataWidgetMapperBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
