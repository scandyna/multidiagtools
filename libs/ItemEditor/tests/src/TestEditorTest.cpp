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
#include "TestEditorTest.h"
#include "TestWindowEditor.h"
#include "TestFormEditionWidget.h"
#include "TestTableEditionWidget.h"
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

void TestEditorTest::initTestCase()
{
}

void TestEditorTest::cleanupTestCase()
{
}

/*
 * Tests
 */


void TestEditorTest::widowEditorTest()
{
  using Mdt::ItemEditor::StandardWindow;

//   TestWindowEditor editor;
  StandardWindow formEditorWindow;
  TestFormEditionWidget formEditor;
  StandardWindow tableEditorWindow;
  TestTableEditionWidget tableEditor;
//   TestTableModel model;
  QStringListModel model;

  formEditor.setModel(&model);
  formEditorWindow.setCentralWidget(&formEditor);
  ///formEditorWindow.setMainController(formEditor.controller());
  ///formEditorWindow.setMainEditorWidget(&formEditor);
  formEditorWindow.show();
  tableEditor.setModel(&model);
  tableEditorWindow.setCentralWidget(&tableEditor);
  ///tableEditorWindow.setMainEditorWidget(&tableEditor);
  tableEditorWindow.setMainController(tableEditor.controller());
  tableEditorWindow.show();
//   model.populate(3, 1);

  /*
   * Play
   */
  while(formEditorWindow.isVisible()){
    QTest::qWait(500);
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  TestEditorTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
