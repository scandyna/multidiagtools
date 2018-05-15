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
#include "VehicleTypeEditionWindow.h"
#include "ui_VehicleTypeEditionWindow.h"

// #include "Mdt/Railway/VehicleTypeClassTableModel.h"
#include "Mdt/Railway/VehicleTypeData.h"

#include "Mdt/Entity/SetupWidget.h"
#include "Mdt/ErrorDialog.h"

#include "Mdt/Entity/FieldAttributes.h"

using namespace Mdt::Entity;

namespace Mdt{ namespace Railway{

VehicleTypeEditionWindow::VehicleTypeEditionWindow(QWidget* parent)
 : QMainWindow(parent),
   mUi( std::make_unique<Ui::VehicleTypeEditionWindow>() )
{
  mUi->setupUi(this);

  mUi->vehicleTypeName->setModel( mEditor.vehicleTypeClassTableModel() );
  mUi->vehicleTypeName->setModelColumn( mEditor.vehicleTypeNameColumn() );
  mUi->vehicleTypeName->setCurrentIndex(-1);

  setupWidget(mUi->vehicleTypeName, mEditor.vehicleTypeNameFieldAttributes());
  setupWidget(mUi->leManufacturerSerie, mEditor.manufacturerSerieFieldAttributes());

  connect(mUi->action_Save, &QAction::triggered, this, &VehicleTypeEditionWindow::save);
}

VehicleTypeEditionWindow::~VehicleTypeEditionWindow()
{
}

void VehicleTypeEditionWindow::save()
{
  if( !mEditor.setVehicleTypeName(mUi->vehicleTypeName->currentText()) ){
    displayError(mEditor.lastError());
    return;
  }
  mUi->vehicleTypeName->setCurrentText( mEditor.vehicleTypeName() );

  if( !mEditor.setManufacturerSerie(mUi->leManufacturerSerie->text()) ){
    displayError(mEditor.lastError());
    return;
  }
  mUi->leManufacturerSerie->setText( mEditor.manufacturerSerie() );
}

void VehicleTypeEditionWindow::displayError(const Error& error)
{
  Mdt::ErrorDialog dialog(error);
  dialog.exec();
}

}} // namespace Mdt{ namespace Railway{
