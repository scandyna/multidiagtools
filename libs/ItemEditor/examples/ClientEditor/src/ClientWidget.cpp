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
#include "ClientWidget.h"
#include "ClientModel.h"
#include "Mdt/ItemModel/FormatProxyModel.h"
#include "Mdt/ItemEditor/WidgetMapperController.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include <QWidget>
#include <QLabel>

using namespace Mdt::ItemModel;
using namespace Mdt::ItemEditor;

ClientWidget::ClientWidget(QWidget* parent)
 : AbstractFormWidget(parent)
{
  setObjectName("ClientWidget");
  setModel(new ClientModel(this));
  auto *centralWidget = new QWidget;
  setupUi(centralWidget);
  setCentralWidget(centralWidget);
  auto *ctrl = controller();
  ctrl->setPrimaryKey({0});
//   ctrl->setPrimaryKeyEditable(false);
//   ctrl->setPrimaryKeyItemsEnabled(false);
  ctrl->addMapping(fld_Id, 0);
  ctrl->addMapping(fld_Name, 1);
  ctrl->setInsertLocation(WidgetMapperController::InsertAtEnd);
  /*
   * Setup formatting
   */
  auto *formatModel = new FormatProxyModel(this);
  formatModel->setTextAlignmentForColumn(0, Qt::AlignCenter);
  formatModel->setTextFontForColumn(1, QFont("Times", 14));
  formatModel->setTextColorForColumn(1, QColor(0,0,255));
  appendProxyModel(formatModel);

  addNavigationActionsToTopArea();
  addEditionActionsToTopArea();
  addInsertActionToTopArea();
  setInsertActionText(tr("New client"));
  addRemoveActionToTopArea();
  setRemoveActionText(tr("Remove client"));

  addWidgetToTopArea(new QLabel("State: "));
  mStateLabel = new QLabel;
  addWidgetToTopArea(mStateLabel);
  connect(ctrl->controllerStateMachine(), &ControllerStateMachine::currentStateChanged, this, &ClientWidget::onControllerStateChanged);
  onControllerStateChanged();
}

void ClientWidget::onControllerStateChanged()
{
  const auto state = controller()->controllerState();
  mStateLabel->setText( controllerStateText(state) );
}
