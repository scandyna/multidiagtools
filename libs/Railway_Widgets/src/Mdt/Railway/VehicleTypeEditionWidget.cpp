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
#include "VehicleTypeEditionWidget.h"
#include "ui_VehicleTypeEditionWidget.h"
#include "Mdt/Railway/VehicleTypeModel.h"
#include <QTableView>
#include <QPushButton>

namespace Mdt{ namespace Railway{

VehicleTypeEditionWidget::VehicleTypeEditionWidget(QWidget* parent)
 : QWidget(parent),
   mUi( std::make_unique<Ui::VehicleTypeEditionWidget>() )
{
  auto *model = new VehicleTypeModel(this);
  mUi->setupUi(this);
  mUi->tableView->setModel(model);
  connect( mUi->pbAdd, &QPushButton::clicked, model, &VehicleTypeModel::addVehicleType );
}

VehicleTypeEditionWidget::~VehicleTypeEditionWidget()
{
}

}} // namespace Mdt{ namespace Railway{
