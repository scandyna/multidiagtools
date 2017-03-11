/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "MainWidget.h"

#include "Mdt/ItemModel/RelationFilterExpression.h"
#include "Mdt/ItemEditor/StandardWindow.h"
#include "Mdt/ItemEditor/StandardEditorLayoutWidget.h"
#include "ClientWidget.h"
#include "AddressWidget.h"
#include <QDebug>

using namespace Mdt::ItemEditor;
using namespace Mdt::ItemModel;

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
//   auto *clientWidget = new ClientWidget;
//   // Setup Address widget
//   auto *addressWidget = new AddressWidget;
//   // Setup editor widget
//   auto *editorWidget = new StandardEditorLayoutWidget;
//   editorWidget->setMainWidget(clientWidget);
//   editorWidget->addChildWidget(addressWidget, "Addresses");
//   // Setup relation
//   ParentModelColumn clientId(0);
//   FilterColumn addressClientId(1);
//   clientWidget->controller()->addChildController(addressWidget->controller(), addressClientId == clientId);

  // Setup main window
  StandardWindow mainWindow;
  auto *mainWidget = new MainWidget;
  mainWindow.setCentralWidget(mainWidget);
  mainWindow.setMainController(mainWidget->mainController());
  mainWindow.setWindowTitle("Client editor");
  mainWindow.showMaximized();

//   /// \todo to debug
//   TableViewWidget clientWidget2;
//   clientWidget2.controller()->setInsertLocation(TableViewController::InsertAtBeginning);
//   clientWidget2.setWindowTitle("Client 2");
//   clientWidget2.setModel(clientWidget->controller()->model());
//   clientWidget2.addEditionActionsToBottomArea();
//   clientWidget2.addInsertActionToBottomArea();
//   clientWidget2.addRemoveActionToBottomBar();
//   clientWidget2.show();
// 
//   TableViewWidget clientWidget3;
//   clientWidget3.controller()->setInsertLocation(TableViewController::InsertAtEnd);
//   clientWidget3.setWindowTitle("Client 3");
//   clientWidget3.setModel(clientWidget->controller()->model());
//   clientWidget3.addEditionActionsToBottomArea();
//   clientWidget3.addInsertActionToBottomArea();
//   clientWidget3.addRemoveActionToBottomBar();
//   clientWidget3.show();

  return app.exec();
}
