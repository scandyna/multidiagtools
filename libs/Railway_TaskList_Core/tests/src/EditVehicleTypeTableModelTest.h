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
#include "Mdt/Railway/TaskList/EditVehicleTypeTableModel.h"
#include "Mdt/Railway/TaskList/VehicleTypeRepository.h"
#include <memory>

class EditVehicleTypeTableModelTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();

  void fetchAllTest();
  void insertAndSubmitTest();
  void setDataAndSubmitTest();

 private:

  void setupTableModel(Mdt::Railway::TaskList::EditVehicleTypeTableModel *model);

//   std::shared_ptr<Mdt::Railway::SqlVehicleTypeClassRepository> mVehicleTypeClassRepository;
//   std::shared_ptr<Mdt::Railway::SqlVehicleTypeRepository> mVehicleTypeRepository;
//   Mdt::QueryExpression::SqlSelectQuery mSelectQuery;
};

#endif // #ifndef RW_TEST_MAIN_H
