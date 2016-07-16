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
#include "TestWindowEditor.h"
#include "Mdt/ItemEditor/StandardEditorLayoutWidget.h"
#include "TestTableEditionWidget.h"
#include "TestFormEditionWidget.h"

#include <QDebug>

TestWindowEditor::TestWindowEditor(QWidget* parent)
 : StandardWindow(parent)
{
  using Mdt::ItemEditor::StandardEditorLayoutWidget;
//   using Mdt::ItemEditor::TableViewWidget;

  auto *layoutWidget = new StandardEditorLayoutWidget;
  pvFormEditionWidget = new TestFormEditionWidget;
  pvTableEditionWidget = new TestTableEditionWidget;

  layoutWidget->setMainWidget(pvFormEditionWidget);
  layoutWidget->addChildWidget(pvTableEditionWidget, tr("Table"));
  setCentralWidget(layoutWidget);
  setMainEditorWidget(pvTableEditionWidget);
}

void TestWindowEditor::setModel(QAbstractTableModel* model)
{
  Q_ASSERT(model != nullptr);

  pvFormEditionWidget->setModel(model);
  pvTableEditionWidget->setModel(model);
}
