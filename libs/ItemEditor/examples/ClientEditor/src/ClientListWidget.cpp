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
#include "ClientListWidget.h"
#include "Mdt/ItemEditor/TableViewController.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include <QLabel>
// #include <QComboBox>

using namespace Mdt::ItemEditor;

ClientListWidget::ClientListWidget(QWidget* parent)
 : TableViewWidget(parent)
{
  setObjectName("ClientListWidget");
  addResizeToContentsActionToTopBar();
  auto ctrl = controller();
  ctrl->setPrimaryKey({0});
  ctrl->setPrimaryKeyHidden(true);

//   auto *cb = new QComboBox;
//   cb->addItem("Filter");
//   cb->addItem("Navigate");
//   addWidgetToTopArea(cb);

  addWidgetToTopArea(new QLabel("State: "));
  mStateLabel = new QLabel;
  addWidgetToTopArea(mStateLabel);
//   connect(ctrl, &TableViewController::controllerStateChanged, this, &ClientListWidget::onControllerStateChanged);
  connect(ctrl->controllerStateMachine(), &ControllerStateMachine::currentStateChanged, this, &ClientListWidget::onControllerStateChanged);
  onControllerStateChanged();
}

void ClientListWidget::onControllerStateChanged()
{
  const auto state = controller()->controllerState();
  mStateLabel->setText( controllerStateText(state) );
}
