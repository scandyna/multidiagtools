/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "StandardWidgetsTest.h"
#include "TestWindowEditor.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/StandardEditorLayoutWidget.h"
#include "Mdt/ItemEditor/StandardWindow.h"
#include <QSignalSpy>
#include <QItemSelectionModel>
#include <QStringListModel>
#include <QTableView>
#include <QPointer>
#include <QLineEdit>
#include <QLabel>

#include <QDebug>

void StandardWidgetsTest::initTestCase()
{
}

void StandardWidgetsTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void StandardWidgetsTest::standardEditorLayoutWidgetTest()
{
  using Mdt::ItemEditor::StandardEditorLayoutWidget;

  auto *lw = new StandardEditorLayoutWidget;
  QPointer<QLineEdit> widget1 = new QLineEdit;
  QPointer<QLabel> widget2 = new QLabel("Widget 2");
  QPointer<QLabel> widget3 = new QLabel("Widget 3");
  QPointer<QLabel> widget4 = new QLabel("Widget 4");

  /*
   * Initial state
   */
  // Tab widget must not exit now
  QVERIFY(lw->findChild<QTabWidget*>() == nullptr);
  /*
   * Set main widget first time
   */
  lw->setMainWidget(widget1);
  /*
   * Set main widget second time
   */
  lw->setMainWidget(widget2);
  QVERIFY(widget1.isNull());
  // Tab widget must not exit now
  QVERIFY(lw->findChild<QTabWidget*>() == nullptr);
  /*
   * Add child widgets
   */
  lw->addChildWidget(widget3, "W3");
  QVERIFY(lw->findChild<QTabWidget*>() != nullptr);
  lw->addChildWidget(widget4, "W4");
  /*
   * Play
   */
//   lw->show();
//   while(lw->isVisible()){
//     QTest::qWait(500);
//   }
  /*
   * Delete
   */
  delete lw;
  QVERIFY(widget2.isNull());
  QVERIFY(widget3.isNull());
  QVERIFY(widget4.isNull());
}

void StandardWidgetsTest::standardWindowTest()
{
  Mdt::ItemEditor::StandardWindow window;

  window.show();

  /*
   * Play
   */
  while(window.isVisible()){
    QTest::qWait(500);
  }
}

void StandardWidgetsTest::testWidowEditorTest()
{
  TestWindowEditor editor;

  /*
   * Play
   */
  editor.show();
  while(editor.isVisible()){
    QTest::qWait(500);
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  StandardWidgetsTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
