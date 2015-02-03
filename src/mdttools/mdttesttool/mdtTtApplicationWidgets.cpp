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
// Common classes
// #include "mdtSqlWindow.h"
// #include "mdtSqlForm.h"
// Editor classes
#include "mdtTtTestCableEditor.h"

// Qt classes
// #include <QMessageBox>
// #include <QMutableListIterator>

#include <QDebug>

// mdtTtApplicationWidgets & mdtTtApplicationWidgets::instance()
// {
//   static mdtTtApplicationWidgets aw;
//   return aw;
// }
// 
// bool mdtTtApplicationWidgets::closeOpenWidgets()
// {
//   return instance().closeAllOpenWidgets();
// }

// void mdtTtApplicationWidgets::clear()
// {
//   instance().clearAllWidgets();
// }

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

// std::shared_ptr<mdtSqlWindow> mdtTtApplicationWidgets::setupEditorInSqlWindow(const std::shared_ptr<mdtSqlForm> & editor)
// {
//   Q_ASSERT(editor);
// 
//   std::shared_ptr<mdtSqlWindow> window(new mdtSqlWindow);
// 
//   window->setSqlForm(editor.get());
//   window->enableEdition();
//   pvOpenEditorWidows.append(window);
// 
//   return window;
// }

// void mdtTtApplicationWidgets::showSqlWindow(const std::shared_ptr<mdtSqlForm> & form, bool enableNavigation, bool enableEdition)
// {
//   Q_ASSERT(form);
// 
//   auto window = getOpenSqlWindow(form);
//   Q_ASSERT(window);
//   window->setNavigationEnabled(enableNavigation);
//   window->setEditionEnabled(enableEdition);
//   window->raise();
//   window->show();
// }

// std::shared_ptr<mdtSqlWindow> mdtTtApplicationWidgets::getOpenSqlWindow(const std::shared_ptr<mdtSqlForm> & form)
// {
//   Q_ASSERT(form);
// 
//   for(auto & window : pvOpenEditorWidows){
//     Q_ASSERT(window);
//     if(window->sqlForm() == form.get()){
//       return window;
//     }
//   }
// 
//   return std::shared_ptr<mdtSqlWindow>();
// }

// bool mdtTtApplicationWidgets::closeAllOpenWidgets()
// {
//   mdtSqlForm *form;
// 
//   // Close all editors handled by a generic SQL window
//   for(auto & window : pvOpenEditorWidows){
//     Q_ASSERT(window);
//     form = window->sqlForm();
//     if(form != 0){
//       if(!form->allDataAreSaved()){
//         return false;
//       }
//     }
//     window->close();
//   }
// 
//   return true;
// }


// void mdtTtApplicationWidgets::displayError(const mdtError & error)
// {
//   QMessageBox msgBox;
// 
//   msgBox.setText(error.text());
//   msgBox.setDetailedText(error.systemText());
//   msgBox.setIcon(error.levelIcon());
//   msgBox.exec();
// }

void mdtTtApplicationWidgets::clearAllWidgets()
{
  // Delete all editors
  pvTestCableEditor.reset();
  // Delete all windows - Must be done after having deleted editors
//   pvOpenEditorWidows.clear();
}
