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
#include "Mdt/Railway/EditVehicleTypeTableModel.h"
#include "Mdt/ItemModel/SortProxyModel.h"
#include "Mdt/Entity/AbstractReadOnlyCacheTableModel.h"
#include "Mdt/Entity/SetupWidget.h"
#include "Mdt/Error.h"
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
  setupVehicleTypeModels();
  setupChooseVehicleType();
  setupEditVehicleType();
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

//   mChooseVehicleTypeCache.setQueryFactory(factory);
//   mChooseVehicleTypeCache.fetchAll();
  
  mEditVehicleTypeTableModel.setQueryFactory(factory);
  mEditVehicleTypeTableModel.fetchAll();
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
  mWidgetMapper->setDataToModel();

  const auto request = makeCreateVehicleTypeRequest();

  if(!validateRequest(request)){
    return;
  }

  emit createVehicleTypeRequested(request);
}

VehicleTypeClassId VehicleTypeEditionWindow::selectedVehicleTypeClassId() const
{
  auto *cb = mUi->vehicleTypeName;

  const auto row = cb->currentIndex();
  if(row < 0){
    return VehicleTypeClassId();
  }

  return mChooseVehicleTypeClassCache.id(row);
}

CreateVehicleTypeRequest VehicleTypeEditionWindow::makeCreateVehicleTypeRequest() const
{
  CreateVehicleTypeRequest request;

  request.className = mUi->vehicleTypeName->currentText();
  request.vehicleTypeClassId = selectedVehicleTypeClassId();
  request.alias = QLatin1String("Fake");
  request.manufacturerSerie = mUi->leManufacturerSerie->text();

  return request;
}

bool VehicleTypeEditionWindow::validateRequest(const CreateVehicleTypeRequest& request)
{
  VehicleTypeDataValidator validator;

  if(request.vehicleTypeClassId.isNull()){
    const auto msg = tr("Please choose a vehicle type class");
    auto error = mdtErrorNewQ(msg, Mdt::Error::Warning, this);
    displayError(error);
    return false;
  }
  if(!validator.validateManufacturerSerie(request.manufacturerSerie)){
    /// \todo Here we should use last state, mark the field (red), generate good message
    displayError(validator.lastError());
    return false;
  }

  return true;
}

void VehicleTypeEditionWindow::setupChooseVehicleTypeClass()
{
  connect(&mChooseVehicleTypeClassCache, &ChooseVehicleTypeClassCache::errorOccured, this, &VehicleTypeEditionWindow::handleError);

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

void VehicleTypeEditionWindow::setupVehicleTypeModels()
{
  mEditVehicleTypeModelContainer.setSourceModel(&mEditVehicleTypeTableModel);
  auto *sortModel = new SortProxyModel(this);
  sortModel->setSortLocaleAware(true);
  sortModel->addColumnToSortOrder(1, StringNumericMode::Natural, Qt::AscendingOrder);
  mEditVehicleTypeModelContainer.appendProxyModel(sortModel);
}

void VehicleTypeEditionWindow::setupChooseVehicleType()
{
//   connect(&mChooseVehicleTypeCache, &ChooseVehicleTypeCache::errorOccured, this, &VehicleTypeEditionWindow::handleError);

  auto *view = mUi->availableVehicleTypeView;
//   auto *model = new AbstractReadOnlyCacheTableModel(view);

//   model->setCache(&mChooseVehicleTypeCache);

//   ProxyModelContainer modelContainer;
//   modelContainer.setSourceModel(&mEditVehicleTypeTableModel);
//   auto *sortModel = new SortProxyModel(view);
//   sortModel->setSortLocaleAware(true);
//   sortModel->addColumnToSortOrder(1, StringNumericMode::Natural, Qt::AscendingOrder);
//   modelContainer.appendProxyModel(sortModel);

//   view->setModel(modelContainer.modelForView());

  view->setModel(mEditVehicleTypeModelContainer.modelForView());
}

void VehicleTypeEditionWindow::setupEditVehicleType()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  Q_ASSERT(mWidgetMapper == nullptr);

  mWidgetMapper = new DataWidgetMapper(this);
  mWidgetMapper->setModel(mEditVehicleTypeModelContainer.modelForView());
  mWidgetMapper->addMapping( mUi->vehicleTypeName, mEditVehicleTypeTableModel.vehicleTypeNameColumn() );
  mWidgetMapper->addMapping( mUi->leManufacturerSerie, mEditVehicleTypeTableModel.manufacturerSerieColumn() );

  auto *view = mUi->availableVehicleTypeView;
  mVehicleTypeCurrentRowChanged.setSelectionModel( view->selectionModel() );
  connect( &mVehicleTypeCurrentRowChanged, &SelectionModelCurrentRowChanged::currentRowChanged, mWidgetMapper, &DataWidgetMapper::setCurrentRow );
}

void VehicleTypeEditionWindow::displayError(const Error& error)
{
  Mdt::ErrorDialog dialog(error);
  dialog.exec();
}

}} // namespace Mdt{ namespace Railway{
