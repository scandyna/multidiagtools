/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/StandardWindow.h"
#include "Mdt/ItemEditor/StandardEditorLayoutWidget.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "AddressWidget.h"
#include "ClientEditorSchema.h"
#include <QSqlDatabase>
#include <QTemporaryFile>
#include <QDebug>

using namespace Mdt::ItemEditor;
namespace Sql = Mdt::Sql;

/*! \brief Main of SqlClientEditor example
 */
int main(int argc, char **argv)
{
  // Setup application
  Mdt::Application app(argc, argv);
  if(!app.init()){
    return 1;
  }
  // Setup database
  auto db = QSqlDatabase::addDatabase("QSQLITE");
  if(!db.isValid()){
    return 1;
  }
  QTemporaryFile file;
  if(!file.open()){
    qFatal("Unable to open temporary file");
    return 1;
  }
  db.setDatabaseName(file.fileName());
  if(!db.open()){
    qFatal("Unable to open database");
    return 1;
  }
  /// \todo Enable FK support + this and above should all be done with a helper class
  Sql::Schema::Driver driver(db);
  if(!driver.createSchema(ClientEditorSchema())){
    return 1;
  }
  // Setup Client widget
  
  // Setup Address widget
  auto *addressWidget = new AddressWidget(db);
  
  // Setup editor widget
  auto *editorWidget = new StandardEditorLayoutWidget;
  editorWidget->setMainWidget(addressWidget);
  // Setup main window
  StandardWindow mainWindow;
  mainWindow.setCentralWidget(editorWidget);
  mainWindow.setMainController(addressWidget->controller());
  mainWindow.show();

  return app.exec();
}
