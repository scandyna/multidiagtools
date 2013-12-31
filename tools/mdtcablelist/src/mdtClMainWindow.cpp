/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtClMainWindow.h"
#include "mdtSqlWindow.h"
#include "mdtSqlDatabaseManager.h"
#include "mdtClDatabaseSchema.h"
#include "mdtClVehicleTypeEditor.h"
#include "mdtClConnectorEditor.h"
#include "mdtClUnitEditor.h"
#include "mdtClArticleEditor.h"
#include "mdtCcTestConnectionCableEditor.h"
#include "mdtTtTestNodeEditor.h"
#include "mdtTtTestEditor.h"
#include "mdtTtTestItemEditor.h"
#include <QAction>
#include <QMessageBox>
#include <QApplication>
#include <QFileInfo>

#include <QDebug>

mdtClMainWindow::mdtClMainWindow()
{
  setupUi(this);
  // Database manager
  pvDatabaseManager = new mdtSqlDatabaseManager(this);
  pvDatabaseManager->setForDialogParentWidget(this);
  /// \todo provisoire
  initWorkDirectory();
  ///openDatabaseSqlite();
  // Editors - Will be setup on first call
  pvVehicleTypeEditor = 0;
  pvConnectorEditor = 0;
  pvUnitEditor = 0;
  pvArticleEditor = 0;
  pvTestNodeEditor = 0;
  pvTestEditor = 0;
  pvTestItemEditor = 0;
  pvTestItemEditorWindow = 0;

  createActions();
}

mdtClMainWindow::~mdtClMainWindow()
{
}

void mdtClMainWindow::openDatabase()
{
  openDatabaseSqlite();
}

void mdtClMainWindow::closeDatabase()
{
  /// \todo Check about data saving !!
  delete pvVehicleTypeEditor;
  pvVehicleTypeEditor = 0;
  delete pvConnectorEditor;
  pvConnectorEditor = 0;
  delete pvArticleEditor;
  pvArticleEditor = 0;
  delete pvUnitEditor;
  pvUnitEditor = 0;
  pvDatabaseManager->database().close();
}

void mdtClMainWindow::createNewDatabase()
{
  createDatabaseSqlite();
}

void mdtClMainWindow::importDatabase()
{
  importDatabaseSqlite();
}

void mdtClMainWindow::editVehicleType()
{
  if(pvVehicleTypeEditor == 0){
    pvVehicleTypeEditor = new mdtClVehicleTypeEditor(this, pvDatabaseManager->database());
    if(!pvVehicleTypeEditor->setupTables()){
      QMessageBox msgBox(this);
      msgBox.setText(tr("Cannot setup vehicle editor."));
      msgBox.setInformativeText(tr("This can happen if selected database has wrong format (is also not a database made for ")\
                                + qApp->applicationName() + tr(")"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      delete pvVehicleTypeEditor;
      pvVehicleTypeEditor = 0;
      return;
    }
  }
  Q_ASSERT(pvVehicleTypeEditor != 0);
  pvVehicleTypeEditor->form()->show();
}

void mdtClMainWindow::editConnector()
{
  if(pvConnectorEditor == 0){
    pvConnectorEditor = new mdtClConnectorEditor(this, pvDatabaseManager->database());
    if(!pvConnectorEditor->setupAsWindow()){
      QMessageBox msgBox(this);
      msgBox.setText(tr("Cannot setup connector editor."));
      msgBox.setInformativeText(tr("This can happen if selected database has wrong format (is also not a database made for ")\
                                + qApp->applicationName() + tr(")"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      delete pvConnectorEditor;
      pvConnectorEditor = 0;
      return;
    }
  }
  Q_ASSERT(pvConnectorEditor != 0);
  pvConnectorEditor->show();
}

void mdtClMainWindow::editUnit()
{
  if(pvUnitEditor == 0){
    pvUnitEditor = new mdtClUnitEditor(this, pvDatabaseManager->database());
    if(!pvUnitEditor->setupAsWindow()){
      QMessageBox msgBox(this);
      msgBox.setText(tr("Cannot setup unit editor."));
      msgBox.setInformativeText(tr("This can happen if selected database has wrong format (is also not a database made for ")\
                                + qApp->applicationName() + tr(")"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      delete pvUnitEditor;
      pvUnitEditor = 0;
      return;
    }
  }
  Q_ASSERT(pvUnitEditor != 0);
  pvUnitEditor->show();
}

void mdtClMainWindow::editArticle()
{
  if(pvArticleEditor == 0){
    pvArticleEditor = new mdtClArticleEditor(this, pvDatabaseManager->database());
    if(!pvArticleEditor->setupAsWindow()){
      QMessageBox msgBox(this);
      msgBox.setText(tr("Cannot setup article editor."));
      msgBox.setInformativeText(tr("This can happen if selected database has wrong format (is also not a database made for ")\
                                + qApp->applicationName() + tr(")"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      delete pvArticleEditor;
      pvArticleEditor = 0;
      return;
    }
  }
  Q_ASSERT(pvArticleEditor != 0);
  pvArticleEditor->show();
}

void mdtClMainWindow::editTestNode()
{
  if(pvTestNodeEditor == 0){
    pvTestNodeEditor = new mdtTtTestNodeEditor(this, pvDatabaseManager->database());
    if(!pvTestNodeEditor->setupAsWindow()){
      QMessageBox msgBox(this);
      msgBox.setText(tr("Cannot setup test node editor."));
      msgBox.setInformativeText(tr("This can happen if selected database has wrong format (is also not a database made for ")\
                                + qApp->applicationName() + tr(")"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      delete pvTestNodeEditor;
      pvTestNodeEditor = 0;
      return;
    }
  }
  Q_ASSERT(pvTestNodeEditor != 0);
  pvTestNodeEditor->show();
}

void mdtClMainWindow::editTest()
{
  if(pvTestEditor == 0){
    pvTestEditor = new mdtTtTestEditor(this, pvDatabaseManager->database());
    if(!pvTestEditor->setupAsWindow()){
      QMessageBox msgBox(this);
      msgBox.setText(tr("Cannot setup test editor."));
      msgBox.setInformativeText(tr("This can happen if selected database has wrong format (is also not a database made for ")\
                                + qApp->applicationName() + tr(")"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      delete pvTestEditor;
      pvTestEditor = 0;
      return;
    }
  }
  Q_ASSERT(pvTestEditor != 0);
  pvTestEditor->show();
}

void mdtClMainWindow::editTestItem()
{
  if(pvTestItemEditor == 0){
    pvTestItemEditor = new mdtTtTestItemEditor(0, pvDatabaseManager->database());
    if(!pvTestItemEditor->setupTables()){
      QMessageBox msgBox(this);
      msgBox.setText(tr("Cannot setup test item editor."));
      msgBox.setInformativeText(tr("This can happen if selected database has wrong format (is also not a database made for ")\
                                + qApp->applicationName() + tr(")"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      delete pvTestItemEditor;
      pvTestItemEditor = 0;
      return;
    }
    pvTestItemEditorWindow = new mdtSqlWindow(this);
    pvTestItemEditorWindow->setSqlForm(pvTestItemEditor);
    pvTestItemEditorWindow->resize(800, 500);
    pvTestItemEditorWindow->enableNavigation();
    pvTestItemEditorWindow->enableEdition();
  }
  Q_ASSERT(pvTestItemEditor != 0);
  Q_ASSERT(pvTestItemEditorWindow != 0);
  pvTestItemEditor->select();
  pvTestItemEditorWindow->show();
}

void mdtClMainWindow::createTestConnectionCable()
{
  mdtCcTestConnectionCableEditor editor(this, pvDatabaseManager->database());

  editor.createCable();
}

void mdtClMainWindow::connectTestCable()
{
  mdtCcTestConnectionCableEditor editor(this, pvDatabaseManager->database());

  editor.connectTestCable();
}

void mdtClMainWindow::disconnectTestCable()
{
  mdtCcTestConnectionCableEditor editor(this, pvDatabaseManager->database());

  editor.disconnectTestCable();
}

void mdtClMainWindow::createActions()
{
  // Open database
  connect(actOpenDatabase, SIGNAL(triggered()), this, SLOT(openDatabase()));
  // Close database
  connect(actCloseDatabase, SIGNAL(triggered()), this, SLOT(closeDatabase()));
  // Create new database
  connect(actCreateNewDatabase, SIGNAL(triggered()), this, SLOT(createNewDatabase()));
  // Import database
  connect(actImportDatabase, SIGNAL(triggered()), this, SLOT(importDatabase()));

  // Article edition
  connect(actEditArticle, SIGNAL(triggered()), this, SLOT(editArticle()));
  connect(pbEditArticle, SIGNAL(clicked()), this, SLOT(editArticle()));
  // Vehicle type edition
  connect(actEditVehicleType, SIGNAL(triggered()), this, SLOT(editVehicleType()));
  connect(pbEditVehicleType, SIGNAL(clicked()), this, SLOT(editVehicleType()));
  // Connector editor
  connect(actEditConnector, SIGNAL(triggered()), this, SLOT(editConnector()));
  // Unit edition
  connect(actEditUnit, SIGNAL(triggered()), this, SLOT(editUnit()));
  connect(pbEditUnit, SIGNAL(clicked()), this, SLOT(editUnit()));

  // Test connection cable
  connect(actCreateTestConnectionCable, SIGNAL(triggered()), this, SLOT(createTestConnectionCable()));
  connect(actConnectTestCable, SIGNAL(triggered()), this, SLOT(connectTestCable()));
  connect(actDisconnectTestCable, SIGNAL(triggered()), this, SLOT(disconnectTestCable()));
  // Test node edition
  connect(actEditTestNode, SIGNAL(triggered()), this, SLOT(editTestNode()));
  // Test edition
  connect(actEditTest, SIGNAL(triggered()), this, SLOT(editTest()));
  // Test item edition
  connect(actEditTestItem, SIGNAL(triggered()), this, SLOT(editTestItem()));

}

bool mdtClMainWindow::initWorkDirectory()
{
  pvWorkDirectory = QDir::home();
  if(!pvWorkDirectory.cd("mdtcablelist")){
    if(!pvWorkDirectory.mkdir("mdtcablelist")){
      QMessageBox msgBox(this);
      mdtError e(tr("Cannot create 'mdtcablelist' directory."), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtClMainWindow");
      e.commit();
      msgBox.setText(e.text());
      /// \todo Let the user a chance to choose another place to create this directory !
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      return false;
    }
    if(!pvWorkDirectory.cd("mdtcablelist")){
      ///QMessageBox msgBox(this);
      mdtError e(tr("Cannot create 'mdtcablelist' directory."), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtClMainWindow");
      e.commit();
      displayWarning(e.text());
      /**
      msgBox.setText(e.text());
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      */
      return false;
    }
  }

  return true;
}

bool mdtClMainWindow::openDatabaseSqlite()
{
  QFileInfo fileInfo;

  // Check that no database is allready open
  if(pvDatabaseManager->database().isOpen()){
    displayWarning(tr("A database is allready open."), tr("Close current database and try again."));
    return false;
  }
  // Let the user choose a file
  fileInfo = pvDatabaseManager->chooseDatabaseSqlite(pvWorkDirectory);
  if(fileInfo.fileName().isEmpty()){
    return false;
  }
  /// \todo Mange, let user choose, etc...
  ///QFileInfo fileInfo(pvWorkDirectory, "test01.sqlite");
  // Open database
  if(!pvDatabaseManager->openDatabaseSqlite(fileInfo)){
    displayWarning(pvDatabaseManager->lastError().text());
    /// \todo Dangerous !!
    /**
    mdtClDatabaseSchema dbSchema(pvDatabaseManager);
    if(!dbSchema.createSchemaSqlite(fileInfo)){
      QMessageBox msgBox(this);
      mdtError e(tr("Cannot create database schema."), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtClMainWindow");
      e.commit();
      msgBox.setText(e.text());
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      return false;
    }
    */
    return false;
  }
  // Check that we have open a cable list schema
  mdtClDatabaseSchema dbSchema(pvDatabaseManager);
  if(!dbSchema.checkSchema()){
    displayWarning(tr("Choosen file does not contain a valid cable list database."));
    closeDatabase();
    return false;
  }

  return true;
}

bool mdtClMainWindow::createDatabaseSqlite()
{
  // Check that no database is allready open
  if(pvDatabaseManager->database().isOpen()){
    displayWarning(tr("A database is allready open."), tr("Close current database and try again."));
    return false;
  }
  // Create database
  mdtClDatabaseSchema dbSchema(pvDatabaseManager);
  if(!dbSchema.createSchemaSqlite(pvWorkDirectory)){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Database creation failed.") + "                ");
    msgBox.setDetailedText(dbSchema.lastError().text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return false;
  }

  return true;
}

bool mdtClMainWindow::importDatabaseSqlite()
{
  // Check that we have currently a database open
  if(!pvDatabaseManager->database().isOpen()){
    displayWarning(tr("Cannot import a database."), tr("Please open a database and try again."));
    return false;
  }
  /// \todo Save current edition !
  /// \todo Create a backup from current database !
  // Let the user a chance to cancel import
  QMessageBox msgBox(this);
  msgBox.setText(tr("You are about to import a database into current database."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  if(msgBox.exec() != QMessageBox::Yes){
    return false;
  }
  // Import ...
  mdtClDatabaseSchema dbSchema(pvDatabaseManager);

  return dbSchema.importDatabase(pvWorkDirectory);
}

void mdtClMainWindow::displayWarning(const QString & text , const QString & informativeText)
{
  QMessageBox msgBox(this);
  msgBox.setText(text);
  msgBox.setInformativeText(informativeText);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.exec();
}
