/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "EditVehicleTypeTableModelTest.h"
#include "Mdt/Railway/TaskList/EditVehicleTypeTableModel.h"
#include "Mdt/TestLib/ItemModel.h"
#include <QDebug>

using namespace Mdt::Railway::TaskList;
using namespace Mdt::TestLib;

void EditVehicleTypeTableModelTest::initTestCase()
{
}

/*
 * Helpers
 */

// void debugError(const Mdt::Error & error)
// {
//   qDebug() << error.functionName() << ": " << error.text();
// }

/*
 * Tests
 */

void EditVehicleTypeTableModelTest::fetchAllTest()
{
  
  EditVehicleTypeTableModel model;
  setupTableModel(&model);
  
  QFAIL("Not complete");
}

void EditVehicleTypeTableModelTest::insertAndSubmitTest()
{
  QFAIL("Not complete");
}

void EditVehicleTypeTableModelTest::setDataAndSubmitTest()
{
  QFAIL("Not complete");
}

/*
 * Helpers
 */

void EditVehicleTypeTableModelTest::setupTableModel(EditVehicleTypeTableModel *model)
{
  Q_ASSERT(model != nullptr);


//   auto *createVehicleType = new CreateVehicleType(mVehicleTypeRepository, model);
//   connect(model, &EditVehicleTypeTableModel::createVehicleTypeRequested, createVehicleType , &CreateVehicleType::execute);
//   connect(createVehicleType, &CreateVehicleType::succeeded, model, &EditVehicleTypeTableModel::onCreateVehicleTypeSucceeded);
//   connect(createVehicleType, &CreateVehicleType::failed, model, &EditVehicleTypeTableModel::onCreateVehicleTypeFailed);
// 
//   auto *updateVehicleType = new UpdateVehicleType(mVehicleTypeRepository, model);
//   connect(model, &EditVehicleTypeTableModel::updateVehicleTypeRequested, updateVehicleType , &UpdateVehicleType::execute);
//   connect(updateVehicleType, &UpdateVehicleType::succeeded, model, &EditVehicleTypeTableModel::onUpdateVehicleTypeSucceeded);
//   connect(updateVehicleType, &UpdateVehicleType::failed, model, &EditVehicleTypeTableModel::onUpdateVehicleTypeFailed);

//   UpdateVehicleType updateVehicleType(mVehicleTypeRepository);
//   connect(&model, &EditVehicleTypeTableModel::updateVehicleTypeRequested, &updateVehicleType , &UpdateVehicleType::execute);
//   connect(&updateVehicleType, &UpdateVehicleType::succeeded, &model, &EditVehicleTypeTableModel::onCreateVehicleTypeSucceded);
//   connect(&updateVehicleType, &UpdateVehicleType::failed, &model, &EditVehicleTypeTableModel::onCreateVehicleTypeFailed);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  EditVehicleTypeTableModelTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
