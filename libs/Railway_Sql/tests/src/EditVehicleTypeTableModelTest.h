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
#include "Mdt/Railway/EditVehicleTypeTableModel.h"
#include "Mdt/Railway/VehicleTypeId.h"
#include "Mdt/Railway/VehicleTypeClassId.h"
#include "Mdt/Railway/VehicleTypeClassRepository.h"
#include "Mdt/Railway/VehicleTypeRepository.h"
#include "Mdt/Railway/SqlVehicleTypeClassRepository.h"
#include "Mdt/Railway/SqlVehicleTypeRepository.h"
#include "Mdt/QueryExpression/SqlSelectQuery.h"
#include <memory>

class EditVehicleTypeTableModelTest : public TestBase
{
 Q_OBJECT

 public:

  EditVehicleTypeTableModelTest();

 private slots:

  void initTestCase();

  void fetchAllTest();
  void setVehicleTypeClassTest();
  void insertAndSubmitTest();
  void setDataAndSubmitTest();

 private:

  void setupTableModel(Mdt::Railway::EditVehicleTypeTableModel *model);
  bool clearStorage();
  Mdt::Railway::VehicleTypeClassId createVehicleTypeClass(const QString & name, const QString & alias);
  Mdt::Railway::VehicleTypeClassId getVehicleTypeClass(const QString & name, const QString & alias);
  Mdt::Railway::VehicleTypeId createVehicleType(const QString & name, const QString & alias, const QString & manufacturerSerie);

  std::shared_ptr<Mdt::Railway::SqlVehicleTypeClassRepository> mVehicleTypeClassRepository;
  std::shared_ptr<Mdt::Railway::SqlVehicleTypeRepository> mVehicleTypeRepository;
  Mdt::QueryExpression::SqlSelectQuery mSelectQuery;
  
//   std::shared_ptr<Mdt::QueryExpression::SqlAsyncSelectQueryFactory> mSelectQueryFactory;
};

#endif // #ifndef RW_TEST_MAIN_H
