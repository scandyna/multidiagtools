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
#include "mdtClVehicleTypeEditor.h"
#include "mdtClConnectorEditor.h"
#include "mdtClArticleEditor.h"
#include "mdtClUnitEditor.h"
// Other data edition classes
#include "mdtClLinkVersion.h"
#include "mdtClLinkVersionDialog.h"
// Other classes
#include <QMessageBox>

#include <QDebug>

void mdtClApplicationWidgets::editVehicleTypes()
{
  instance()->setupAndShowVehicleTypeEditor();
}

void mdtClApplicationWidgets::setupAndShowVehicleTypeEditor()
{
  if(pvVehicleTypeEditor == nullptr){
    if(!createVehicleTypeEditor()){
      return;
    }
  }
  Q_ASSERT(pvVehicleTypeEditor != nullptr);
  if(!pvVehicleTypeEditor->select()){
    displayError(pvVehicleTypeEditor->lastError());
    return;
  }
  showSqlWindow(pvVehicleTypeEditor, true, true);
}

bool mdtClApplicationWidgets::createVehicleTypeEditor()
{
  Q_ASSERT(pvVehicleTypeEditor == nullptr);

  // Setup editor
  pvVehicleTypeEditor = new mdtClVehicleTypeEditor(nullptr, pvDatabase);
  if(!pvVehicleTypeEditor->setupTables()){
    displayError(pvVehicleTypeEditor->lastError());
    delete pvVehicleTypeEditor;
    return false;
  }
  // Setup in a generic SQL window
  auto window = setupEditorInSqlWindow(pvVehicleTypeEditor);
  Q_ASSERT(window);
  window->setWindowTitle(tr("Vehicle type edition"));
  window->resize(800, 600);

  return true;
}

void mdtClApplicationWidgets::editConnectors()
{
  instance()->setupAndShowConnectorEditor();
}

void mdtClApplicationWidgets::editConnector(const mdtClConnectorPkData & pk)
{
  instance()->setupAndShowConnectorEditor(pk);
}

void mdtClApplicationWidgets::setupAndShowConnectorEditor(const mdtClConnectorPkData & pk)
{
  if(pvConnectorEditor == nullptr){
    if(!createConnectorEditor()){
      return;
    }
  }
  Q_ASSERT(pvConnectorEditor != nullptr);
  if(!pvConnectorEditor->select()){
    displayError(pvConnectorEditor->lastError());
    return;
  }
  if(!pk.isNull()){
    if(!pvConnectorEditor->setCurrentConnector(pk)){
      displayError(pvConnectorEditor->lastError());
      return;
    }
  }
  showSqlWindow(pvConnectorEditor, true, true);
}

bool mdtClApplicationWidgets::createConnectorEditor()
{
  Q_ASSERT(pvConnectorEditor == nullptr);

  // Setup editor
  pvConnectorEditor = new mdtClConnectorEditor(nullptr, pvDatabase);
  if(!pvConnectorEditor->setupTables()){
    displayError(pvConnectorEditor->lastError());
    delete pvConnectorEditor;
    return false;
  }
  // Setup in a generic SQL window
  // Setup in a generic SQL window
  auto window = setupEditorInSqlWindow(pvConnectorEditor);
  Q_ASSERT(window);
  window->setWindowTitle(tr("Connector edition"));
  window->resize(800, 600);

  return true;
}

void mdtClApplicationWidgets::editArticles()
{
  instance()->setupAndShowArticleEditor();
}

void mdtClApplicationWidgets::editArticle(const QVariant& articleId)
{
  instance()->setupAndShowArticleEditor(articleId);
}

void mdtClApplicationWidgets::setupAndShowArticleEditor(const QVariant & articleId)
{
  if(pvArticleEditor == nullptr){
    if(!createArticleEditor()){
      return;
    }
  }
  Q_ASSERT(pvArticleEditor != nullptr);
  if(!pvArticleEditor->select()){
    displayError(pvArticleEditor->lastError());
    return;
  }
  if(!articleId.isNull()){
    if(!pvArticleEditor->setCurrentRow("Id_PK", articleId)){
      displayError(pvArticleEditor->lastError());
      return;
    }
  }
  showSqlWindow(pvArticleEditor, true, true);
}

bool mdtClApplicationWidgets::createArticleEditor()
{
  Q_ASSERT(pvArticleEditor == nullptr);

  // Setup editor
  pvArticleEditor = new mdtClArticleEditor(nullptr, pvDatabase);
  if(!pvArticleEditor->setupTables()){
    displayError(pvArticleEditor->lastError());
    delete pvArticleEditor;
    return false;
  }
  // Setup in a generic SQL window
  // Setup in a generic SQL window
  auto window = setupEditorInSqlWindow(pvArticleEditor);
  Q_ASSERT(window);
  window->setWindowTitle(tr("Article edition"));
  window->resize(800, 600);

  return true;
}

void mdtClApplicationWidgets::editUnit(const QVariant & unitId)
{
  instance()->setupAndShowUnitEditor(unitId);
}

void mdtClApplicationWidgets::editUnits()
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
  delete pvVehicleTypeEditor;
}

mdtClApplicationWidgets::mdtClApplicationWidgets()
{
}
