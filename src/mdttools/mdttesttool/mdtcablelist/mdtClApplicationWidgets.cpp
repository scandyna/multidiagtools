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
#include "mdtClApplicationWidgets.h"
// Common classes
#include "mdtSqlWindow.h"
#include "mdtSqlForm.h"
// Editor classes
#include "mdtClUnitEditor.h"

// Qt classes
#include <QMessageBox>
#include <QMutableListIterator>

#include <QDebug>

mdtClApplicationWidgets & mdtClApplicationWidgets::instance()
{
  static mdtClApplicationWidgets aw;
  return aw;
}

bool mdtClApplicationWidgets::closeOpenWidgets()
{
  return instance().closeAllOpenWidgets();
}

void mdtClApplicationWidgets::clear()
{
  instance().clearAllWidgets();
}

void mdtClApplicationWidgets::editUnit(const QVariant & unitId)
{
  auto & aw = instance();

  if(!aw.pvUnitEditor){
    if(!aw.createUnitEditor()){
      return;
    }
  }
  Q_ASSERT(aw.pvUnitEditor);
  if(!aw.pvUnitEditor->select()){
    aw.displayError(aw.pvUnitEditor->lastError());
    return;
  }
  if(!aw.pvUnitEditor->setCurrentRow("Id_PK", unitId)){
    aw.displayError(aw.pvUnitEditor->lastError());
    return;
  }
  aw.showSqlWindow(aw.pvUnitEditor, false, true);
}

void mdtClApplicationWidgets::editUnits()
{
  instance().slotEditUnits();
}

void mdtClApplicationWidgets::slotEditUnits()
{
  if(!pvUnitEditor){
    if(!createUnitEditor()){
      return;
    }
  }
  Q_ASSERT(pvUnitEditor);
  if(!pvUnitEditor->select()){
    displayError(pvUnitEditor->lastError());
    return;
  }
  showSqlWindow(pvUnitEditor, true, true);
}

bool mdtClApplicationWidgets::createUnitEditor()
{
  Q_ASSERT(!pvUnitEditor);

  // Setup editor
  pvUnitEditor.reset(new mdtClUnitEditor(0, pvDatabase) );
  if(!pvUnitEditor->setupTables()){
    displayError(pvUnitEditor->lastError());
    pvUnitEditor.reset();
    return false;
  }
  // Setup in a generic SQL window
  auto window = setupEditorInSqlWindow(pvUnitEditor);
  Q_ASSERT(window);
  window->setWindowTitle(tr("Unit edition"));
  window->resize(800, 600);

  return true;
}


std::shared_ptr<mdtSqlWindow> mdtClApplicationWidgets::setupEditorInSqlWindow(const std::shared_ptr<mdtSqlForm> & editor)
{
  Q_ASSERT(editor);

  std::shared_ptr<mdtSqlWindow> window(new mdtSqlWindow);

  window->setSqlForm(editor.get());
  window->enableEdition();
  pvOpenEditorWidows.append(window);

  return window;
}

void mdtClApplicationWidgets::showSqlWindow(const std::shared_ptr<mdtSqlForm> & form, bool enableNavigation, bool enableEdition)
{
  Q_ASSERT(form);

  auto window = getOpenSqlWindow(form);
  Q_ASSERT(window);
  window->setNavigationEnabled(enableNavigation);
  window->setEditionEnabled(enableEdition);
  window->raise();
  window->show();
}

std::shared_ptr<mdtSqlWindow> mdtClApplicationWidgets::getOpenSqlWindow(const std::shared_ptr<mdtSqlForm> & form)
{
  Q_ASSERT(form);

  for(auto & window : pvOpenEditorWidows){
    Q_ASSERT(window);
    if(window->sqlForm() == form.get()){
      return window;
    }
  }

  return std::shared_ptr<mdtSqlWindow>();
}

bool mdtClApplicationWidgets::closeAllOpenWidgets()
{
  mdtSqlForm *form;

  // Close all editors handled by a generic SQL window
  for(auto & window : pvOpenEditorWidows){
    Q_ASSERT(window);
    form = window->sqlForm();
    if(form != 0){
      if(!form->allDataAreSaved()){
        return false;
      }
    }
    window->close();
  }

  return true;
}

void mdtClApplicationWidgets::clearAllWidgets()
{
  // Delete all editors
  pvUnitEditor.reset();
  // Delete all windows - Must be done after having deleted editors
  pvOpenEditorWidows.clear();
}

void mdtClApplicationWidgets::displayError(const mdtError & error)
{
  QMessageBox msgBox;

  msgBox.setText(error.text());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
