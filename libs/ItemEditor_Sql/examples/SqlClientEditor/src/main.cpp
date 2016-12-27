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
#include "AddressWidget.h"

using namespace Mdt::ItemEditor;

/*! \brief Main of SqlClientEditor example
 */
int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  StandardWindow mainWindow;
  auto *editorWidget = new StandardEditorLayoutWidget;
  auto *addressWidget = new AddressWidget;

  // Setup editor widget
  editorWidget->setMainWidget(addressWidget);
  // Setup main window
  mainWindow.setCentralWidget(editorWidget);
  mainWindow.setMainController(addressWidget->controller());
  mainWindow.show();
  // Setup and run application
  if(!app.init()){
    return 1;
  }

  return app.exec();
}
