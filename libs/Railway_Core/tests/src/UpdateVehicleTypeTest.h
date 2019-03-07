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
#ifndef RW_TEST_MAIN_H
#define RW_TEST_MAIN_H

#include "TestBase.h"
#include "TestVehicleTypeRepository.h"
#include "TestVehicleTypeClassRepository.h"
#include "Mdt/Railway/VehicleTypeId.h"
#include "Mdt/Railway/VehicleTypeClassId.h"
#include "Mdt/Railway/CreateVehicleType.h"
#include "Mdt/Railway/CreateVehicleTypeClass.h"
#include <memory>

using namespace Mdt::Railway;

class UpdateVehicleTypeTest : public QObject
{
 Q_OBJECT

 public:

  UpdateVehicleTypeTest();

 private slots:

  void simpleUpdateTest();

 private:

  VehicleTypeClassId createVehicleTypeClass(const CreateVehicleTypeClassRequest & request);
  VehicleTypeId createVehicleType(const CreateVehicleTypeRequest & request);
  VehicleTypeId createVehicleTypeAndClass(const QString & name, const QString & alias, const QString & manufacturerSerie);

  std::shared_ptr<TestVehicleTypeClassRepository> mVehicleTypeClassRepository;
  std::shared_ptr<TestVehicleTypeRepository> mVehicleTypeRepository;
};

#endif // #ifndef RW_TEST_MAIN_H
