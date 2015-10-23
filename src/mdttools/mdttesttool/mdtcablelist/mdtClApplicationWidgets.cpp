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
// Editor classes
#include "mdtClUnitEditor.h"
// Other data edition classes
#include "mdtClLinkVersion.h"
#include "mdtClLinkVersionDialog.h"
// Other classes
#include <QMessageBox>

#include <QDebug>

void mdtClApplicationWidgets::editUnit(const QVariant & unitId)
{
  instance()->setupAndShowUnitEditor(unitId);
}

void mdtClApplicationWidgets::editUnits()
{
  instance()->setupAndShowUnitEditor();
}

void mdtClApplicationWidgets::slotEditUnits()
{
  instance()->setupAndShowUnitEditor();
}

void mdtClApplicationWidgets::setupAndShowUnitEditor(const QVariant & unitId)
{
  if(pvUnitEditor == nullptr){
    if(!createUnitEditor()){
      return;
    }
  }
  Q_ASSERT(pvUnitEditor != nullptr);
  if(!pvUnitEditor->select()){
    displayError(pvUnitEditor->lastError());
    return;
  }
  if(!unitId.isNull()){
    if(!pvUnitEditor->setCurrentRow("Id_PK", unitId)){
      displayError(pvUnitEditor->lastError());
      return;
    }
  }
  showSqlWindow(pvUnitEditor, true, true);
}

bool mdtClApplicationWidgets::createUnitEditor()
{
  Q_ASSERT(pvUnitEditor == nullptr);

  // Setup editor
  pvUnitEditor = new mdtClUnitEditor(nullptr, pvDatabase);
  if(!pvUnitEditor->setupTables()){
    displayError(pvUnitEditor->lastError());
    delete pvUnitEditor;
    ///pvUnitEditor = nullptr;
    return false;
  }
  // Setup in a generic SQL window
  auto window = setupEditorInSqlWindow(pvUnitEditor);
  Q_ASSERT(window);
  window->setWindowTitle(tr("Unit edition"));
  window->resize(800, 600);

  return true;
}

void mdtClApplicationWidgets::createLinkVersion()
{
  mdtClLinkVersion lv(pvDatabase);
  mdtClLinkVersionDialog dialog;
  mdtClLinkVersionData data;
  bool ok;

  // Setup and show dialog
  data = lv.getLastVersionData(ok);
  if(!ok){
    displayError(lv.lastError());
    return;
  }
  dialog.setLastVersionData(data);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add to database
  data = dialog.versionData();
  if(!lv.addVersion(data)){
    displayError(lv.lastError());
    return;
  }
  // Inform user about success
  QMessageBox msgBox;
  QString msg = tr("New version") + " '" + QString::number(data.version(), 'f', 3) + "' " + tr("was successfully created.");
  msgBox.setText(msg);
  msgBox.setIcon(QMessageBox::Information);
  msgBox.exec();
}

void mdtClApplicationWidgets::clearAllWidgets()
{
  /*
   * Delete all editors.
   * Note:
   *  thanks to QPointer, we can simply delete all
   *  without taking care if they are allready destroyed
   *  (typically by a parent).
   *  QPointer also becomes a nullptr automatically.
   */
  delete pvUnitEditor;
}

mdtClApplicationWidgets::mdtClApplicationWidgets()
{
}
