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
#include "mdtTtTestSystemUnitEditor.h"

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
  instance()->setupAndShowTestSystemEditor();
}

void mdtTtApplicationWidgets::slotEditTestSystems()
{
  setupAndShowTestSystemEditor();
}

void mdtTtApplicationWidgets::setupAndShowTestSystemEditor()
{
  if(pvTestSystemEditor == nullptr){
    if(!createTestSystemEditor()){
      return;
    }
  }
  Q_ASSERT(pvTestSystemEditor != nullptr);
  if(!pvTestSystemEditor->select()){
    displayError(pvTestSystemEditor->lastError());
    return;
  }
  showSqlWindow(pvTestSystemEditor, true, true);
}

bool mdtTtApplicationWidgets::createTestSystemEditor()
{
  Q_ASSERT(pvTestSystemEditor == nullptr);

  // Setup editor
  pvTestSystemEditor = new mdtTtTestSystemEditor(nullptr, pvDatabase);
  if(!pvTestSystemEditor->setupTables()){
    displayError(pvTestSystemEditor->lastError());
    delete pvTestSystemEditor;
    return false;
  }
  // Setup in a generic SQL window
  auto window = setupEditorInSqlWindow(pvTestSystemEditor);
  Q_ASSERT(window);
  window->setWindowTitle(tr("Test system edition"));
  window->resize(800, 600);

  return true;
}

void mdtTtApplicationWidgets::editTestSystemComponents()
{
  instance()->setupAndShowTestSystemComponentEditor();
}

void mdtTtApplicationWidgets::slotEditTestSystemComponents()
{
  setupAndShowTestSystemComponentEditor();
}

void mdtTtApplicationWidgets::setupAndShowTestSystemComponentEditor()
{
  if(pvTestSystemComponentEditor.isNull()){
    if(!createTestSystemComponentEditor()){
      return;
    }
  }
  Q_ASSERT(!pvTestSystemComponentEditor.isNull());
  if(!pvTestSystemComponentEditor->select()){
    displayError(pvTestSystemComponentEditor->lastError());
    return;
  }
  showSqlWindow(pvTestSystemComponentEditor, true, true);
}

bool mdtTtApplicationWidgets::createTestSystemComponentEditor()
{
  Q_ASSERT(pvTestSystemComponentEditor.isNull());

  // Setup editor
  pvTestSystemComponentEditor = new mdtTtTestSystemComponentEditor(nullptr, pvDatabase);
  if(!pvTestSystemComponentEditor->setupTables()){
    displayError(pvTestSystemComponentEditor->lastError());
    delete pvTestSystemComponentEditor;
    return false;
  }
  // Setup in a generic SQL window
  auto window = setupEditorInSqlWindow(pvTestSystemComponentEditor);
  Q_ASSERT(window);
  window->setWindowTitle(tr("Test system component edition"));
  window->resize(800, 600);

  return true;
}

void mdtTtApplicationWidgets::editTestSystemUnit(const QVariant& unitId)
{
  instance()->setupAndShowTestSystemUnitEditor(unitId);
}

void mdtTtApplicationWidgets::slotEditTestSystemUnits()
{
  setupAndShowTestSystemUnitEditor();
}

void mdtTtApplicationWidgets::setupAndShowTestSystemUnitEditor(const QVariant & unitId)
{
  if(pvTestSystemUnitEditor.isNull()){
    if(!createTestSystemUnitEditor()){
      return;
    }
  }
  Q_ASSERT(!pvTestSystemUnitEditor.isNull());
  if(!pvTestSystemUnitEditor->select()){
    displayError(pvTestSystemUnitEditor->lastError());
    return;
  }
  if(!unitId.isNull()){
    if(!pvTestSystemUnitEditor->setCurrentRow("Unit_Id_FK_PK", unitId)){
      displayError(pvTestSystemUnitEditor->lastError());
      return;
    }
  }
  showSqlWindow(pvTestSystemUnitEditor, true, true);
}

bool mdtTtApplicationWidgets::createTestSystemUnitEditor()
{
  Q_ASSERT(pvTestSystemUnitEditor.isNull());

  // Setup editor
  pvTestSystemUnitEditor = new mdtTtTestSystemUnitEditor(nullptr, pvDatabase);
  if(!pvTestSystemUnitEditor->setupTables()){
    displayError(pvTestSystemUnitEditor->lastError());
    delete pvTestSystemUnitEditor;
    return false;
  }
  // Setup in a generic SQL window
  auto window = setupEditorInSqlWindow(pvTestSystemUnitEditor);
  Q_ASSERT(window);
  window->disableInsertion();
  window->disableDeletion();
  window->setWindowTitle(tr("Test system unit edition"));
  window->resize(800, 600);

  return true;
}

void mdtTtApplicationWidgets::showTestNodeModbusIoTool(const QString& deviceAlias)
{
  instance()->setupAndShowTestNodeModbusIoTool(deviceAlias);
}

void mdtTtApplicationWidgets::setupAndShowTestNodeModbusIoTool(const QString& deviceAlias)
{
  // Create tool if needed
  if(pvTestNodeModbusIoTool.isNull()){
    if(!createTestNodeModbusIoTool()){
      return;
    }
  }
  Q_ASSERT(!pvTestNodeModbusIoTool.isNull());
  // .....
  if(!pvTestNodeModbusIoTool->setModbusDevice(deviceAlias)){
    displayError(pvTestNodeModbusIoTool->lastError());
    return;
  }
  pvTestNodeModbusIoTool->raise();
  pvTestNodeModbusIoTool->show();
}

bool mdtTtApplicationWidgets::createTestNodeModbusIoTool()
{
  Q_ASSERT(pvTestNodeModbusIoTool.isNull());

  pvTestNodeModbusIoTool = new mdtTtTestNodeModbusIoTool(nullptr, pvDatabase);

  return true;
}

// void mdtTtApplicationWidgets::editTestCable(const QVariant & testCableId)
// {
//   auto *aw = instance();
//
//   if(!aw->pvTestCableEditor){
//     if(!aw->createTestCableEditor()){
//       return;
//     }
//   }
//   Q_ASSERT(aw->pvTestCableEditor);
//   if(!aw->pvTestCableEditor->select()){
//     aw->displayError(aw->pvTestCableEditor->lastError());
//     return;
//   }
//   if(!aw->pvTestCableEditor->setCurrentRow("Id_PK", testCableId)){
//     aw->displayError(aw->pvTestCableEditor->lastError());
//     return;
//   }
//   aw->showSqlWindow(aw->pvTestCableEditor, false, true);
// }

// void mdtTtApplicationWidgets::editTestCables()
// {
//   instance()->slotEditTestCables();
// }

// void mdtTtApplicationWidgets::slotEditTestCables()
// {
//   if(!pvTestCableEditor){
//     if(!createTestCableEditor()){
//       return;
//     }
//   }
//   Q_ASSERT(pvTestCableEditor);
//   if(!pvTestCableEditor->select()){
//     displayError(pvTestCableEditor->lastError());
//     return;
//   }
//   showSqlWindow(pvTestCableEditor, true, true);
// }

// void mdtTtApplicationWidgets::slotEditTestNodes()
// {
//   if(!pvTestNodeEditor){
//     if(!createTestNodeEditor()){
//       return;
//     }
//   }
//   Q_ASSERT(pvTestNodeEditor);
//   if(!pvTestNodeEditor->select()){
//     displayError(pvTestNodeEditor->lastError());
//     return;
//   }
//   showSqlWindow(pvTestNodeEditor, true, true);
// }




// bool mdtTtApplicationWidgets::createTestCableEditor()
// {
//   Q_ASSERT(!pvTestCableEditor);
//
//   // Setup editor
//   pvTestCableEditor.reset(new mdtTtTestCableEditor(0, pvDatabase) );
//   if(!pvTestCableEditor->setupTables()){
//     displayError(pvTestCableEditor->lastError());
//     pvTestCableEditor.reset();
//     return false;
//   }
//   // Setup in a generic SQL window
//   auto window = setupEditorInSqlWindow(pvTestCableEditor);
//   Q_ASSERT(window);
//   window->setWindowTitle(tr("Physical test cable edition"));
//   window->resize(800, 600);
//
//   return true;
// }

// bool mdtTtApplicationWidgets::createTestNodeEditor()
// {
//   Q_ASSERT(!pvTestNodeEditor);
// 
//   // Setup editor
//   pvTestNodeEditor.reset(new mdtTtTestNodeEditor(0, pvDatabase));
//   if(!pvTestNodeEditor->setupTables()){
//     displayError(pvTestNodeEditor->lastError());
//     pvTestNodeEditor.reset();
//     return false;
//   }
//   // Setup in a generic SQL window
//   auto window = setupEditorInSqlWindow(pvTestNodeEditor);
//   Q_ASSERT(window);
//   // Add modbus I/O tool action
//   Q_ASSERT(window->menuBar() != nullptr);
//   QMenu *toolsMenu = window->menuBar()->addMenu(tr("&Tools"));
//   QAction *actShowTestNodeModbusIoTool = toolsMenu->addAction(tr("&MODBUS I/O tool"));
//   connect(actShowTestNodeModbusIoTool, SIGNAL(triggered()), pvTestNodeEditor.get(), SLOT(showTestNodeModbusIoTool()));
//   // Some other setup
//   window->setWindowTitle(tr("Test node edition"));
//   window->resize(800, 600);
//
//   return true;
// }

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
  /*
   * Delete all editors.
   * Note:
   *  thanks to QPointer, we can simply delete all
   *  without taking care if they are allready destroyed
   *  (typically by a parent).
   *  QPointer also becomes a nullptr automatically.
   */
  delete pvTestSystemEditor;
  delete pvTestSystemComponentEditor;
  delete pvTestSystemUnitEditor;
//   pvTestCableEditor.reset();
//   pvTestNodeEditor.reset();
  // Delete all tools
  delete pvTestNodeModbusIoTool;
}

mdtTtApplicationWidgets::mdtTtApplicationWidgets()
{
}
