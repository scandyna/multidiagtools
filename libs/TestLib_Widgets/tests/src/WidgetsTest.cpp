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
#include "WidgetsTest.h"
#include "Mdt/TestLib/Widgets.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QTableView>
#include <QComboBox>
#include <QString>
#include <QStringList>
#include <QStringListModel>

#include <QDebug>

using namespace Mdt::TestLib;
using Mdt::ItemModel::VariantTableModel;

/*
 * Tests
 */

void WidgetsTest::mouseClickOnItemTableViewTest()
{
  QModelIndex index;
  VariantTableModel model;
  QTableView view;
  view.setModel(&model);

  model.resize(5, 2);

  index = model.index(0, 0);
  mouseClickOnItem(view, index);
  QVERIFY(isItemSelected(view, index));

  index = model.index(2, 1);
  mouseClickOnItem(view, index);
  QVERIFY(isItemSelected(view, index));
}

void WidgetsTest::mouseSelectRowComboBoxTest()
{
  QStringListModel model({"A","B","C"});
  QComboBox cb;
  cb.setModel(&model);

  cb.setCurrentIndex(0);
  QCOMPARE(cb.currentIndex(), 0);
  mouseSelectRow(cb, 1);
  QCOMPARE(cb.currentIndex(), 1);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  WidgetsTest test;

  return QTest::qExec(&test, argc, argv);
}
