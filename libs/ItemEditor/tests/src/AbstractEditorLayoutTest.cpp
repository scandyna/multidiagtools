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
#include "AbstractEditorLayoutTest.h"
#include "Mdt/ItemEditor/AbstractEditorLayout.h"
#include <QWidget>
#include <QPointer>
#include <QScopedPointer>
#include <QLabel>
#include <QToolBar>

using namespace Mdt::ItemEditor;

/*
 * Init/Cleanup
 */

void AbstractEditorLayoutTest::initTestCase()
{
}

void AbstractEditorLayoutTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void AbstractEditorLayoutTest::setChangeCentralWidgetTest()
{
  QWidget widget;
  /*
   * Initial state
   */
  auto *layout = new AbstractEditorLayout;
  QVERIFY(layout->centralWidget() == nullptr);
  QCOMPARE(layout->count(), 0);
  /*
   * Set a central widget
   */
  QPointer<QLabel> centralWidget1 = new QLabel("centralWidget1");
  layout->setCentralWidget(centralWidget1);
  QCOMPARE(layout->centralWidget(), centralWidget1.data());
  QCOMPARE(layout->count(), 1);
  /*
   * Change central widget
   */
  QPointer<QLabel> centralWidget2 = new QLabel("centralWidget2");
  layout->setCentralWidget(centralWidget2);
  QCOMPARE(layout->centralWidget(), centralWidget2.data());
  QVERIFY(centralWidget1.isNull());
  QCOMPARE(layout->count(), 1);
  /*
   * Set layout to widget
   */
  widget.setLayout(layout);
}

void AbstractEditorLayoutTest::setCentralWidgetThenBarsTest()
{
  QWidget widget;
  /*
   * Initial state
   */
  auto *layout = new AbstractEditorLayout;
  QVERIFY(layout->centralWidget() == nullptr);
  QCOMPARE(layout->count(), 0);
  /*
   * Set central widget
   */
  auto *centralWidget = new QLabel("Central widget");
  layout->setCentralWidget(centralWidget);
  QCOMPARE(layout->count(), 1);
  QCOMPARE(layout->indexOf(centralWidget), 0);
  /*
   * Add tool bar to top area
   */
  auto *topBar = layout->addToolBarToTopArea();
  QVERIFY(topBar != nullptr);
  QCOMPARE(layout->count(), 2);
  QCOMPARE(layout->indexOf(centralWidget), 1);
  /*
   * Add tool bar to bottom area
   */
  auto *bottomBar = layout->addToolBarToBottomArea();
  QVERIFY(bottomBar != nullptr);
  QCOMPARE(layout->count(), 3);
  QCOMPARE(layout->indexOf(centralWidget), 1);
  /*
   * Set layout to widget
   */
  widget.setLayout(layout);
}

void AbstractEditorLayoutTest::setBarsThenCentralWidgetTest()
{
  QWidget widget;
  /*
   * Initial state
   */
  auto *layout = new AbstractEditorLayout;
  QVERIFY(layout->centralWidget() == nullptr);
  QCOMPARE(layout->count(), 0);
  /*
   * Add tool bar to top area
   */
  auto *topBar = layout->addToolBarToTopArea();
  QVERIFY(topBar != nullptr);
  QCOMPARE(layout->count(), 1);
  /*
   * Add tool bar to bottom area
   */
  auto *bottomBar = layout->addToolBarToBottomArea();
  QVERIFY(bottomBar != nullptr);
  QCOMPARE(layout->count(), 2);
  /*
   * Set central widget
   */
  auto *centralWidget = new QLabel("Central widget");
  layout->setCentralWidget(centralWidget);
  QCOMPARE(layout->count(), 3);
  QCOMPARE(layout->indexOf(centralWidget), 1);
  /*
   * Set layout to widget
   */
  widget.setLayout(layout);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  AbstractEditorLayoutTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
