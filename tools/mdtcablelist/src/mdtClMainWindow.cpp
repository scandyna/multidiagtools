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
#include "mdtSqlDatabaseManager.h"
#include "mdtClDatabaseSchema.h"
#include "mdtClVehicleTypeEditor.h"
#include "mdtClUnitEditor.h"
#include "mdtClArticleEditor.h"
#include "mdtError.h"
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
  openDatabaseSqlite();
  // Editors - Will be setup on first call
  pvVehicleTypeEditor = 0;
  pvUnitEditor = 0;
  pvArticleEditor = 0;

  createActions();
}

mdtClMainWindow::~mdtClMainWindow()
{
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

void mdtClMainWindow::editUnit()
{
  if(pvUnitEditor == 0){
    pvUnitEditor = new mdtClUnitEditor(this, pvDatabaseManager->database());
    if(!pvUnitEditor->setupTables()){
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
  pvUnitEditor->form()->show();
}

void mdtClMainWindow::editArticle()
{
  if(pvArticleEditor == 0){
    pvArticleEditor = new mdtClArticleEditor(this, pvDatabaseManager->database());
    if(!pvArticleEditor->setupTables()){
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
  pvArticleEditor->form()->show();
}

void mdtClMainWindow::createActions()
{
  // Vehicle type edition
  pvActEditVehicleType = new QAction(tr("Edit vehicle types"), this);
  connect(pvActEditVehicleType, SIGNAL(triggered()), this, SLOT(editVehicleType()));
  connect(pbEditVehicleType, SIGNAL(clicked()), this, SLOT(editVehicleType()));
  // Unit edition
  pvActEditUnit = new QAction(tr("Edit units"), this);
  connect(pvActEditUnit, SIGNAL(triggered()), this, SLOT(editUnit()));
  connect(pbEditUnit, SIGNAL(clicked()), this, SLOT(editUnit()));
  // Article edition
  pvActEditArticle = new QAction(tr("Edit articles"), this);
  connect(pvActEditArticle, SIGNAL(triggered()), this, SLOT(editArticle()));
  connect(pbEditArticle, SIGNAL(clicked()), this, SLOT(editArticle()));

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
      QMessageBox msgBox(this);
      mdtError e(tr("Cannot create 'mdtcablelist' directory."), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtClMainWindow");
      e.commit();
      msgBox.setText(e.text());
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      return false;
    }
  }

  return true;
}

bool mdtClMainWindow::openDatabaseSqlite()
{
  /// \todo Mange, let user choose, etc...
  QFileInfo fileInfo(pvWorkDirectory, "test01.sqlite");
  if(!pvDatabaseManager->openDatabaseSqlite(fileInfo)){
    /// \todo Dangerous !!
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
  }

  return true;
}
