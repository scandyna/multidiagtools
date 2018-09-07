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
#include "Mdt/Railway/VehicleTypeDataValidator.h"
#include "Mdt/ItemModel/ProxyModelContainer.h"
#include "Mdt/ItemModel/SortProxyModel.h"
#include "Mdt/Entity/AbstractReadOnlyCacheTableModel.h"
#include "Mdt/Entity/SetupWidget.h"
#include "Mdt/ErrorDialog.h"

using namespace Mdt::Entity;
using namespace Mdt::ItemModel;

namespace Mdt{ namespace Railway{

VehicleTypeEditionWindow::VehicleTypeEditionWindow(QWidget* parent)
 : QMainWindow(parent),
   mUi( std::make_unique<Ui::VehicleTypeEditionWindow>() )
{
  mUi->setupUi(this);

  constexpr Entity::VehicleTypeClassEntity vehicleTypeClass;
  setupWidget(mUi->vehicleTypeName, vehicleTypeClass.def().name().fieldAttributes());

  constexpr Entity::VehicleTypeEntity vehicleType;
  setupWidget(mUi->leManufacturerSerie, vehicleType.def().manufacturerSerie().fieldAttributes());

//   setupWidget(mUi->vehicleTypeName, mEditor.vehicleTypeNameFieldAttributes());
//   setupWidget(mUi->leManufacturerSerie, mEditor.manufacturerSerieFieldAttributes());

  connect(mUi->action_Save, &QAction::triggered, this, &VehicleTypeEditionWindow::save);

  setupChooseVehicleTypeClass();
}

VehicleTypeEditionWindow::~VehicleTypeEditionWindow()
{
}

void VehicleTypeEditionWindow::setQueryFactory(const std::shared_ptr<SelectQueryFactory> & factory)
{
  Q_ASSERT(factory.get() != nullptr);
//   Q_ASSERT(factory->isValid());

  mChooseVehicleTypeClassCache.setQueryFactory(factory);
  mChooseVehicleTypeClassCache.fetchAll();
}

void VehicleTypeEditionWindow::displayCreatedVehicleType(const CreateVehicleTypeResponse& response)
{
  mUi->vehicleTypeName->setCurrentText(response.className);
  mUi->leManufacturerSerie->setText(response.manufacturerSerie);
}

void VehicleTypeEditionWindow::handleError(const Railway::Error& error)
{
  displayError(error);
}

void VehicleTypeEditionWindow::save()
{
  const auto request = makeCreateVehicleTypeRequest();

  if(!validateRequest(request)){
    return;
  }

  emit createVehicleTypeRequested(request);
}

CreateVehicleTypeRequest VehicleTypeEditionWindow::makeCreateVehicleTypeRequest() const
{
  CreateVehicleTypeRequest request;

  request.className = mUi->vehicleTypeName->currentText();
  request.alias = QLatin1String("Fake");
  request.manufacturerSerie = mUi->leManufacturerSerie->text();

  return request;
}

bool VehicleTypeEditionWindow::validateRequest(const CreateVehicleTypeRequest& request)
{
  VehicleTypeDataValidator validator;

  if(!validator.validateManufacturerSerie(request.manufacturerSerie)){
    /// \todo Here we should use last state, mark the field (red), generate good message
    displayError(validator.lastError());
    return false;
  }

  return true;
}

void VehicleTypeEditionWindow::setupChooseVehicleTypeClass()
{
//   connect(&mChooseVehicleTypeClassCache, &ChooseVehicleTypeClassCache::errorOccured, this, &VehicleTypeEditionWindow::handleError);

  auto *cb = mUi->vehicleTypeName;
  auto *model = new AbstractReadOnlyCacheTableModel(cb);

  model->setCache(&mChooseVehicleTypeClassCache);

  ProxyModelContainer modelContainer;
  modelContainer.setSourceModel(model);
  auto *sortModel = new SortProxyModel(cb);
  sortModel->setSortLocaleAware(true);
  sortModel->addColumnToSortOrder(1, StringNumericMode::Natural, Qt::AscendingOrder);
  modelContainer.appendProxyModel(sortModel);

  cb->setModel(modelContainer.modelForView());
  cb->setModelColumn( mChooseVehicleTypeClassCache.nameColumn() );
}

void VehicleTypeEditionWindow::displayError(const Error& error)
{
  Mdt::ErrorDialog dialog(error);
  dialog.exec();
}

}} // namespace Mdt{ namespace Railway{
