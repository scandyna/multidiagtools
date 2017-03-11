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
#include "MainWidget.h"
#include "ClientWidget.h"
#include "ClientListWidget.h"
#include "AddressWidget.h"
#include "Mdt/ItemModel/RelationFilterExpression.h"
#include "Mdt/ItemEditor/StandardEditorLayoutWidget.h"

using namespace Mdt::ItemModel;
using namespace Mdt::ItemEditor;

MainWidget::MainWidget(QWidget* parent)
 : QSplitter(parent)
{
  auto *clientWidget = new ClientWidget;
  auto *clientListWidget = new ClientListWidget;
  auto *addressWidget = new AddressWidget;
  // Setup editor widget
  auto *editorWidget = new StandardEditorLayoutWidget;
  editorWidget->setMainWidget(clientWidget);
  editorWidget->addChildWidget(addressWidget, "Addresses");
  // Setup relation
  clientListWidget->controller()->addChildController(clientWidget->controller(), ChildModelColumn(0) == ParentModelColumn(0));
  clientWidget->controller()->addChildController(addressWidget->controller());

  clientListWidget->setModel(clientWidget->controller()->model());
  addWidget(clientListWidget);
  addWidget(editorWidget);
  setStretchFactor(0, 1);
  setStretchFactor(1, 4);

  mMainController = clientWidget->controller();
  Q_ASSERT(!mMainController.isNull());
}
