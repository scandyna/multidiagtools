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
#include "mdtTtTestNodeEditor.h"
// Tools classes
#include "mdtTtTestNodeModbusIoTool.h"
// Qt classes
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

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

void mdtTtApplicationWidgets::showTestNodeModbusIoTool(const QString& deviceAlias)
{
  auto & aw = instance();

  // Create tool if needed
  if(!aw.pvTestNodeModbusIoTool){
    if(!aw.createTestNodeModbusIoTool()){
      return;
    }
  }
  Q_ASSERT(aw.pvTestNodeModbusIoTool);
  // .....
  if(!aw.pvTestNodeModbusIoTool->setModbusDevice(deviceAlias)){
    /// \todo display error
    return;
  }
  aw.pvTestNodeModbusIoTool->raise();
  aw.pvTestNodeModbusIoTool->show();
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

void mdtTtApplicationWidgets::slotEditTestNodes()
{
  if(!pvTestNodeEditor){
    if(!createTestNodeEditor()){
      return;
    }
  }
  Q_ASSERT(pvTestNodeEditor);
  if(!pvTestNodeEditor->select()){
    displayError(pvTestNodeEditor->lastError());
    return;
  }
  showSqlWindow(pvTestNodeEditor, true, true);
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

bool mdtTtApplicationWidgets::createTestNodeEditor()
{
  Q_ASSERT(!pvTestNodeEditor);

  // Setup editor
  pvTestNodeEditor.reset(new mdtTtTestNodeEditor(0, pvDatabase));
  if(!pvTestNodeEditor->setupTables()){
    displayError(pvTestNodeEditor->lastError());
    pvTestNodeEditor.reset();
    return false;
  }
  // Setup in a generic SQL window
  auto window = setupEditorInSqlWindow(pvTestNodeEditor);
  Q_ASSERT(window);
  // Add modbus I/O tool action
  Q_ASSERT(window->menuBar() != nullptr);
  QMenu *toolsMenu = window->menuBar()->addMenu(tr("&Tools"));
  QAction *actShowTestNodeModbusIoTool = toolsMenu->addAction(tr("&MODBUS I/O tool"));
  connect(actShowTestNodeModbusIoTool, SIGNAL(triggered()), pvTestNodeEditor.get(), SLOT(showTestNodeModbusIoTool()));
  // Some other setup
  window->setWindowTitle(tr("Test node edition"));
  window->resize(800, 600);

  return true;
}

bool mdtTtApplicationWidgets::createTestNodeModbusIoTool()
{
  Q_ASSERT(!pvTestNodeModbusIoTool);

  pvTestNodeModbusIoTool.reset(new mdtTtTestNodeModbusIoTool(0, pvDatabase));

  return true;
}

bool mdtTtApplicationWidgets::closeCustomWidgets()
{
  qDebug() << "Closing custom widgets ...";
  
  if(pvTestNodeModbusIoTool){
    pvTestNodeModbusIoTool->close();
  }

  return true;
}

void mdtTtApplicationWidgets::clearAllWidgets()
{
  // Delete all editors
  pvTestCableEditor.reset();
  pvTestNodeEditor.reset();
  // Delete all tools
  pvTestNodeModbusIoTool.reset();
}
