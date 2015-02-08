/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtTtApplicationWidgets.h"
// Editor classes
#include "mdtTtTestCableEditor.h"

#include <QDebug>

void mdtTtApplicationWidgets::editTestCable(const QVariant & testCableId)
{
  auto & aw = instance();

  if(!aw.pvTestCableEditor){
    if(!aw.createTestCableEditor()){
      return;
    }
  }
  Q_ASSERT(aw.pvTestCableEditor);
  if(!aw.pvTestCableEditor->select()){
    aw.displayError(aw.pvTestCableEditor->lastError());
    return;
  }
  if(!aw.pvTestCableEditor->setCurrentRow("Id_PK", testCableId)){
    aw.displayError(aw.pvTestCableEditor->lastError());
    return;
  }
  aw.showSqlWindow(aw.pvTestCableEditor, false, true);
}

void mdtTtApplicationWidgets::editTestCables()
{
  instance().slotEditTestCables();
}

void mdtTtApplicationWidgets::slotEditTestCables()
{
  if(!pvTestCableEditor){
    if(!createTestCableEditor()){
      return;
    }
  }
  Q_ASSERT(pvTestCableEditor);
  if(!pvTestCableEditor->select()){
    displayError(pvTestCableEditor->lastError());
    return;
  }
  showSqlWindow(pvTestCableEditor, true, true);
}

bool mdtTtApplicationWidgets::createTestCableEditor()
{
  Q_ASSERT(!pvTestCableEditor);

  // Setup editor
  pvTestCableEditor.reset(new mdtTtTestCableEditor(0, pvDatabase) );
  if(!pvTestCableEditor->setupTables()){
    displayError(pvTestCableEditor->lastError());
    pvTestCableEditor.reset();
    return false;
  }
  // Setup in a generic SQL window
  auto window = setupEditorInSqlWindow(pvTestCableEditor);
  Q_ASSERT(window);
  window->setWindowTitle(tr("Test cable edition"));
  window->resize(800, 600);

  return true;
}

void mdtTtApplicationWidgets::clearAllWidgets()
{
  // Delete all editors
  pvTestCableEditor.reset();
}
