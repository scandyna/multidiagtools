/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "EditVehicleTypeWindow.h"
#include "ui_EditVehicleTypeWindow.h"
#include <QTableView>

namespace Mdt{ namespace Railway{ namespace TaskList{

EditVehicleTypeWindow::EditVehicleTypeWindow(QWidget* parent)
 : QMainWindow(parent),
   mUi( std::make_unique<Ui::EditVehicleTypeWindow>() )
{
  mUi->setupUi(this);
}

EditVehicleTypeWindow::~EditVehicleTypeWindow()
{
}

void EditVehicleTypeWindow::setEditVehicleTypeTableModel(const std::shared_ptr<EditVehicleTypeTableModel> & model)
{
  mEditVehicleTypeTableModel = model;
  Q_ASSERT(mEditVehicleTypeTableModel.get() != nullptr);
  mUi->availableVehicleTypeView->setModel(mEditVehicleTypeTableModel.get());
}

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{
