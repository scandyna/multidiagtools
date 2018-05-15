/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "VehicleTypeClassEditionWindow.h"
#include "ui_VehicleTypeClassEditionWindow.h"
#include "Mdt/Railway/Entity/VehicleTypeClass.h"
#include "Mdt/Railway/VehicleTypeClassTableModel.h"
#include "Mdt/Entity/FieldAttributesItemDelegate.h"
#include "Mdt/ItemModel/RemoveRows.h"
#include "Mdt/ErrorDialog.h"

namespace Mdt{ namespace Railway{

VehicleTypeClassEditionWindow::VehicleTypeClassEditionWindow(QWidget* parent)
 : QMainWindow(parent),
   mUi( std::make_unique<Ui::VehicleTypeClassEditionWindow>() ),
   mTableModel( new VehicleTypeClassTableModel(this) )
{
  mUi->setupUi(this);

  mUi->tableView->setModel( mTableModel );
  using Delegate = Mdt::Entity::FieldAttributesItemDelegate<Entity::VehicleTypeClassEntity>;
  mUi->tableView->setItemDelegate( new Delegate(this) );

  connect( mUi->pbAdd, &QPushButton::clicked, this, &VehicleTypeClassEditionWindow::addVehicleTypeClass );
  connect( mUi->pbRemove, &QPushButton::clicked, this, &VehicleTypeClassEditionWindow::removeSelectedVehicleTypeClasses );
  connect( mUi->action_Save, &QAction::triggered, this, &VehicleTypeClassEditionWindow::save );
}

VehicleTypeClassEditionWindow::~VehicleTypeClassEditionWindow()
{
}

void VehicleTypeClassEditionWindow::setVehicleTypeClassRepository(const VehicleTypeClassRepository& repository)
{
  mRepository = repository;
  mTableModel->setRepository(repository);
}

void VehicleTypeClassEditionWindow::addVehicleTypeClass()
{
  ///mTableModel->appendRow();
  mTableModel->insertRow(mTableModel->rowCount());
}

void VehicleTypeClassEditionWindow::removeSelectedVehicleTypeClasses()
{
  Mdt::ItemModel::removeRows(mTableModel, mUi->tableView->selectionModel());
}

void VehicleTypeClassEditionWindow::save()
{
  if(!mRepository.repository().submitChanges()){
    displayError(mRepository.constRepository().lastError());
  }
}

void VehicleTypeClassEditionWindow::displayError(const Error& error)
{
  Mdt::ErrorDialog dialog(error);
  dialog.exec();
}

}} // namespace Mdt{ namespace Railway{
