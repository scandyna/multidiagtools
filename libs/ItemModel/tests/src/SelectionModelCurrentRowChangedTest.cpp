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
#include "SelectionModelCurrentRowChangedTest.h"
#include "Mdt/ItemModel/SelectionModelCurrentRowChanged.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QTableView>
#include <QAbstractItemView>
#include <QSignalSpy>

using namespace Mdt::ItemModel;

/*
 * Helpers
 */

int currentRowFromSignalSpy(QSignalSpy & spy)
{
  Q_ASSERT(spy.count() == 1);

  const auto arguments = spy.takeFirst();
  Q_ASSERT(arguments.count() == 1);

  return arguments.at(0).toInt();
}

void setViewCurrentIndex(QAbstractItemView & view, int row, int column)
{
  Q_ASSERT(view.model() != nullptr);

  QModelIndex index = view.model()->index(row, column);
  Q_ASSERT(index.isValid());

  view.setCurrentIndex(index);
}

void setViewCurrentRow(QAbstractItemView & view, int row)
{
  Q_ASSERT(view.model() != nullptr);

  setViewCurrentIndex(view, row, 0);
}

void displayViewAndWaitClosed(QAbstractItemView & view)
{
  view.show();

  while(view.isVisible()){
    QTest::qWait(500);
  }
}

/*
 * Tests
 */

void SelectionModelCurrentRowChangedTest::tableViewTest()
{
  QStringListModel model({"A","B","C","D"});
  QTableView view;
  view.setModel(&model);

  SelectionModelCurrentRowChanged rc;
  QSignalSpy spy(&rc, &SelectionModelCurrentRowChanged::currentRowChanged);
  QVERIFY(spy.isValid());
  QCOMPARE(spy.count(), 0);

  view.setCurrentIndex(QModelIndex());
  rc.setSelectionModel(view.selectionModel());
  QCOMPARE(spy.count(), 1);
  QCOMPARE(currentRowFromSignalSpy(spy), -1);

  setViewCurrentRow(view, 0);
  QCOMPARE(spy.count(), 1);
  QCOMPARE(currentRowFromSignalSpy(spy), 0);

  setViewCurrentRow(view, 0);
  QCOMPARE(spy.count(), 0);

  setViewCurrentRow(view, 1);
  QCOMPARE(spy.count(), 1);
  QCOMPARE(currentRowFromSignalSpy(spy), 1);

  VariantTableModel model2;
  model2.resize(3, 2);
  model2.populateColumn(0, {1,2,3});

  view.setModel(&model2);
  QCOMPARE(spy.count(), 0);
  setViewCurrentIndex(view, 2, 0);
  rc.setSelectionModel(view.selectionModel());
  QCOMPARE(spy.count(), 1);
  QCOMPARE(currentRowFromSignalSpy(spy), 2);

  setViewCurrentIndex(view, 2, 1);
  QCOMPARE(spy.count(), 0);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SelectionModelCurrentRowChangedTest test;

  return QTest::qExec(&test, argc, argv);
}
