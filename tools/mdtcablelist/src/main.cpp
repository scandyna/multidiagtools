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
#include "mdtApplication.h"

#include <QObject>
#include <QLocale>
#include <QSqlDatabase>
///#include "mdtSqlWindow.h"

#include "mdtClLinkEditor.h"
#include "mdtClTerminalBlockEditor.h"


#include <QInputDialog>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QStringList>

#include <QDir>
#include <QFileInfo>

#include "mdtClMainWindow.h"

#include <QDebug>

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  QSqlDatabase db;
  QString password;
  bool ok;
  int retVal;
  ///mdtSqlWindow vehicleTypeEditorWindow;
  ///mdtClVehicleTypeEditor *vehicleTypeEditor;
  ///mdtClArticleEditor *articleEditor;
  ///mdtClUnitEditor *unitEditor;
  // Terminal block editor
  ///mdtSqlWindow terminalBlockEditorWindow;
  mdtClTerminalBlockEditor *terminalBlockEditor;
  // Link editor
  mdtClLinkEditor *linkEditor;
  ///mdtSqlWindow linkEditorWindow;

  // Init app, we allow multiple instances
  if(!app.init()){
    return 1;
  }

  ///QObject::connect(&app, SIGNAL(languageChanged()), &modbusIoTool, SLOT(retranslate()));
  ///modbusIoTool.setAvailableTranslations(app.availableTranslations(), app.currentTranslationKey());

  mdtClMainWindow window;

  window.show();
  
  ///mdtSqlDatabaseManager dbManager;

  
  // Setup DB connection
  /**
  db = QSqlDatabase::addDatabase("QMYSQL");
  db.setHostName("127.0.0.1");
  db.setDatabaseName("cablelist");
  db.setUserName("scandyna");
  password = QInputDialog::getText(0, "Password for DB connection", \
                                      "Please enter password to connect to DB", \
                                      QLineEdit::Password, "", &ok);
  if(!ok){
    return 0;
  }
  db.setPassword(password);
  if(!db.open()){
    QMessageBox::warning(0, "DB connection error", "Cannot connect to database");
    return 1;
  }
  */
  // Setup vehicle type editor
  
  ///vehicleTypeEditor->setupUi(&vehicleTypeEditorWindow);
  
  /**
  vehicleTypeEditorWindow.setWindowTitle(QObject::tr("Vehicle type edition"));
  vehicleTypeEditorWindow.show();
  */
  // Setup article editor
  /**
  articleEditor = new mdtClArticleEditor(0, db);
  if(!articleEditor->setupTables(true)){
    QMessageBox::warning(0, "DB setup error", "Cannot setup tables for article editor");
    return 1;
  }
  articleEditor->form()->show();
  */
  // Setup Unit editor
  /**
  unitEditor = new mdtClUnitEditor(0, db);
  if(!unitEditor->setupTables(true)){
    QMessageBox::warning(0, "DB setup error", "Cannot setup tables for unit editor");
    return 1;
  }
  unitEditor->form()->sqlWindow()->resize(1000, 600);
  unitEditor->form()->show();
  */
  // Setup link editor
  /**
  linkEditor = new mdtClLinkEditor(0, db);
  if(!linkEditor->setupTables()){
    QMessageBox::warning(0, "DB setup error", "Cannot setup tables for link editor");
    return 1;
  }
  linkEditor->setupUi(&linkEditorWindow);
  linkEditorWindow.setWindowTitle("Link edition");
  linkEditorWindow.show();
  */
  // Setup terminal block editor
  /**
  terminalBlockEditor = new mdtClTerminalBlockEditor(0, db);
  if(!terminalBlockEditor->setupTables()){
    QMessageBox::warning(0, "DB setup error", "Cannot setup tables for terminal block editor");
    return 1;
  }
  terminalBlockEditor->setupUi(&terminalBlockEditorWindow);
  terminalBlockEditorWindow.setWindowTitle(QObject::tr("Terminal block edition"));
  terminalBlockEditorWindow.show();
  */

  retVal = app.exec();
  db.close();
  ///delete unitEditor;
  ///delete articleEditor;
  ///delete vehicleTypeEditor;
  ///delete linkEditor;
  delete terminalBlockEditor;

  return retVal;
}
