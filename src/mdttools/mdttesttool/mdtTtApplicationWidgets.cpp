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
#include "mdtTtTestSystemEditor.h"
#include "mdtTtTestSystemComponentEditor.h"

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

void mdtTtApplicationWidgets::editTestSystems()
{
  auto & aw = instance();

  if(!aw.pvTestSystemEditor){
    if(!aw.createTestSystemEditor()){
      return;
    }
  }
  Q_ASSERT(aw.pvTestSystemEditor);
  if(!aw.pvTestSystemEditor->select()){
    aw.displayError(aw.pvTestSystemEditor->lastError());
    return;
  }
  aw.showSqlWindow(aw.pvTestSystemEditor, false, true);
}

void mdtTtApplicationWidgets::editTestSystemComponents()
{
  auto & aw = instance();

  if(!aw.pvTestSystemComponentEditor){
    if(!aw.createTestSystemComponentEditor()){
      return;
    }
  }
  Q_ASSERT(aw.pvTestSystemComponentEditor);
  if(!aw.pvTestSystemComponentEditor->select()){
    aw.displayError(aw.pvTestSystemComponentEditor->lastError());
    return;
  }
  aw.showSqlWindow(aw.pvTestSystemComponentEditor, false, true);
}



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
    aw.displayError(aw.pvTestNodeModbusIoTool->lastError());
    return;
  }
  aw.pvTestNodeModbusIoTool->raise();
  aw.pvTestNodeModbusIoTool->show();
}


void mdtTtApplicationWidgets::slotEditTestSystems()
{
  if(!pvTestSystemEditor){
    if(!createTestSystemEditor()){
      return;
    }
  }
  Q_ASSERT(pvTestSystemEditor);
  if(!pvTestSystemEditor->select()){
    displayError(pvTestSystemEditor->lastError());
    return;
  }
  showSqlWindow(pvTestSystemEditor, true, true);
}

void mdtTtApplicationWidgets::slotEditTestSystemComponents()
{
  if(!pvTestSystemComponentEditor){
    if(!createTestSystemComponentEditor()){
      return;
    }
  }
  Q_ASSERT(pvTestSystemComponentEditor);
  if(!pvTestSystemComponentEditor->select()){
    displayError(pvTestSystemComponentEditor->lastError());
    return;
  }
  showSqlWindow(pvTestSystemComponentEditor, true, true);
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


bool mdtTtApplicationWidgets::createTestSystemEditor()
{
  Q_ASSERT(!pvTestSystemEditor);

  // Setup editor
  pvTestSystemEditor.reset(new mdtTtTestSystemEditor(0, pvDatabase) );
  if(!pvTestSystemEditor->setupTables()){
    displayError(pvTestSystemEditor->lastError());
    pvTestSystemEditor.reset();
    return false;
  }
  // Setup in a generic SQL window
  auto window = setupEditorInSqlWindow(pvTestSystemEditor);
  Q_ASSERT(window);
  window->setWindowTitle(tr("Test system edition"));
  window->resize(800, 600);

  return true;
}

bool mdtTtApplicationWidgets::createTestSystemComponentEditor()
{
  Q_ASSERT(!pvTestSystemComponentEditor);

  // Setup editor
  pvTestSystemComponentEditor.reset(new mdtTtTestSystemComponentEditor(0, pvDatabase) );
  if(!pvTestSystemComponentEditor->setupTables()){
    displayError(pvTestSystemComponentEditor->lastError());
    pvTestSystemComponentEditor.reset();
    return false;
  }
  // Setup in a generic SQL window
  auto window = setupEditorInSqlWindow(pvTestSystemComponentEditor);
  Q_ASSERT(window);
  window->setWindowTitle(tr("Test system component edition"));
  window->resize(800, 600);

  return true;
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
  window->setWindowTitle(tr("Physical test cable edition"));
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
  pvTestSystemEditor.reset();
  pvTestSystemComponentEditor.reset();
  
  pvTestCableEditor.reset();
  pvTestNodeEditor.reset();
  // Delete all tools
  pvTestNodeModbusIoTool.reset();
}
