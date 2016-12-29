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
#include "ClientWidget.h"
#include "AddressWidget.h"
#include <QDebug>

using namespace Mdt::ItemEditor;

/*! \brief Main of ClientEditor example
 */
int main(int argc, char **argv)
{
  // Setup application
  Mdt::Application app(argc, argv);
  if(!app.init()){
    return 1;
  }
  // Setup Client widget
  auto *clientWidget = new ClientWidget;
  // Setup Address widget
  auto *addressWidget = new AddressWidget;
  // Setup editor widget
  auto *editorWidget = new StandardEditorLayoutWidget;
  editorWidget->setMainWidget(clientWidget);
  editorWidget->addChildWidget(addressWidget, "Addresses");
  // Setup main window
  StandardWindow mainWindow;
  mainWindow.setCentralWidget(editorWidget);
  mainWindow.setMainController(clientWidget->controller());
  mainWindow.setWindowTitle("Client editor");
  mainWindow.show();

  return app.exec();
}
