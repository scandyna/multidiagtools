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

  connect(mUi->action_Save, &QAction::triggered, this, &VehicleTypeEditionWindow::save);
  connect(mUi->action_New, &QAction::triggered, this, &VehicleTypeEditionWindow::addNew);
}

VehicleTypeEditionWindow::~VehicleTypeEditionWindow()
{
}

void VehicleTypeEditionWindow::setVehicleTypeClassTableModel(const std::shared_ptr<EditVehicleTypeClassTableModel> & model)
{
  Q_ASSERT(model.get() != nullptr);

  mVehicleTypeClassTableModel = model;
  setupChooseVehicleTypeClass();
}

void VehicleTypeEditionWindow::setEditVehicleTypeTableModel(const std::shared_ptr<EditVehicleTypeTableModel> & model)
{
  Q_ASSERT(model.get() != nullptr);

  mEditVehicleTypeTableModel = model;
  setupVehicleTypeModels();
  setupChooseVehicleType();
  setupEditVehicleType();
}

void VehicleTypeEditionWindow::displayError(const Error& error)
{
  Mdt::ErrorDialog dialog(error);
  dialog.exec();
}

void VehicleTypeEditionWindow::addNew()
{
  mEditVehicleTypeTableModel->insertRow( mEditVehicleTypeTableModel->rowCount() );
}

void VehicleTypeEditionWindow::save()
{
  mWidgetMapper->setDataToModel();

  mEditVehicleTypeTableModel->submitChanges();
}

void VehicleTypeEditionWindow::setSelectedVehicleTypeClassIdToModel()
{
  const int row = mWidgetMapper->currentRow();
  Q_ASSERT(row >= 0);
  const auto id = selectedVehicleTypeClassId();
  
  
  /// \todo see if DataWidgetMapper could handle this
}

VehicleTypeClassId VehicleTypeEditionWindow::selectedVehicleTypeClassId() const
{
  auto *cb = mUi->vehicleTypeName;

  const auto row = cb->currentIndex();
  if(row < 0){
    return VehicleTypeClassId();
  }

//   return mChooseVehicleTypeClassCache.id(row);
}

void VehicleTypeEditionWindow::setupChooseVehicleTypeClass()
{
  Q_ASSERT(mVehicleTypeClassTableModel.get() != nullptr);

  auto *cb = mUi->vehicleTypeName;

  ProxyModelContainer modelContainer;
  modelContainer.setSourceModel(mVehicleTypeClassTableModel.get());
  auto *sortModel = new SortProxyModel(cb);
  sortModel->setSortLocaleAware(true);
  sortModel->addColumnToSortOrder(1, StringNumericMode::Natural, Qt::AscendingOrder);
  modelContainer.appendProxyModel(sortModel);

  cb->setModel(modelContainer.modelForView());
  cb->setModelColumn( mVehicleTypeClassTableModel->vehicleTypeNameColumn() );
}

void VehicleTypeEditionWindow::setupVehicleTypeModels()
{
  mEditVehicleTypeModelContainer.setSourceModel(mEditVehicleTypeTableModel.get());
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
//   mWidgetMapper->addMapping( mUi->vehicleTypeName, mEditVehicleTypeTableModel->vehicleTypeNameColumn() );
  mWidgetMapper->addRelationalMapping( mUi->vehicleTypeName, mEditVehicleTypeTableModel->vehicleTypeClassIdColumn(), mVehicleTypeClassTableModel->idColumn() );
  mWidgetMapper->addMapping( mUi->leManufacturerSerie, mEditVehicleTypeTableModel->manufacturerSerieColumn() );

  auto *view = mUi->availableVehicleTypeView;
  mVehicleTypeCurrentRowChanged.setSelectionModel( view->selectionModel() );
  connect( &mVehicleTypeCurrentRowChanged, &SelectionModelCurrentRowChanged::currentRowChanged, mWidgetMapper, &DataWidgetMapper::setCurrentRow );
}

}} // namespace Mdt{ namespace Railway{
