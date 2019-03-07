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
#include "EditVehicleTypeTest.h"
#include "Mdt/Railway/TaskList/EditVehicleTypeTableModel.h"
#include "Mdt/Railway/TaskList/EditVehicleTypeWindow.h"
#include "Mdt/Railway/TaskList/VehicleTypeRepository.h"
#include "Mdt/Railway/TaskList/CreateVehicleType.h"

#include "Mdt/Railway/TaskList/SqlSchema.h"

#include "Mdt/Sql/SQLiteDatabase.h"
// #include "Mdt/QueryExpression/SqlAsyncSelectQuery.h"
// #include "Mdt/QueryExpression/SqlAsyncSelectQueryFactory.h"
#include "Mdt/Sql/SQLiteConnectionParameters.h"
// #include "Mdt/Sql/SQLiteDatabaseFileDialog.h"
#include <QSqlDatabase>
#include <QFile>
#include <QTemporaryFile>
#include <vector>
#include <algorithm>
#include <memory>

#include <QDebug>

using namespace Mdt::Railway::TaskList;
using namespace Mdt::Sql;
// using namespace Mdt::QueryExpression;

void EditVehicleTypeTest::initTestCase()
{
}

void EditVehicleTypeTest::cleanupTestCase()
{
}

/*
void setupEditVehicleTypeTableModel(EditVehicleTypeTableModel *model,
                                         const std::shared_ptr<VehicleTypeRepository> & repository,
                                         const std::unique_ptr<AbstractAsyncSelectQuery> & selectQuery)
{
  Q_ASSERT(model != nullptr);

  QObject::connect(model, &EditVehicleTypeTableModel::fetchAllRequested, selectQuery.get(), &AbstractAsyncSelectQuery::submitStatement);
  QObject::connect(selectQuery.get(), &AbstractAsyncSelectQuery::newRecordAvailable, model, &EditVehicleTypeTableModel::fromBackendAppendRecord);

  auto *createVehicleType = new CreateVehicleType(repository, model);
  QObject::connect(model, &EditVehicleTypeTableModel::createVehicleTypeRequested, createVehicleType , &CreateVehicleType::execute);
  QObject::connect(createVehicleType, &CreateVehicleType::succeeded, model, &EditVehicleTypeTableModel::onCreateVehicleTypeSucceeded);
  QObject::connect(createVehicleType, &CreateVehicleType::failed, model, &EditVehicleTypeTableModel::onCreateVehicleTypeFailed);

}
*/

void EditVehicleTypeTest::sandbox()
{
  EditVehicleTypeWindow window;
  window.show();

  auto editVehicleTypeTableModel = std::make_shared<EditVehicleTypeTableModel>();
  window.setEditVehicleTypeTableModel(editVehicleTypeTableModel);
//   auto editVehicleTypeSelectQuery = asyncQueryFactory->createSelectQuery();
//   setupEditVehicleTypeTableModel(editVehicleTypeTableModel.get(), vehicleTypeRepository, editVehicleTypeSelectQuery);

  SqlSchema schema;
  SQLiteDatabase sqliteDb;
  const QString dbFile = "railway_tasklist.sqlite";
  if(!sqliteDb.openExisting(dbFile)){
    QVERIFY(sqliteDb.createNew(dbFile));
    QVERIFY(schema.create(sqliteDb.database()));
  }
  auto dbConnection = sqliteDb.database();
  QVERIFY(dbConnection.isValid());


  SQLiteConnectionParameters parameters;
  parameters.setDatabaseFile( dbConnection.databaseName() );
//   auto asyncQueryFactory = std::make_shared<SqlAsyncSelectQueryFactory>();
//   asyncQueryFactory->setup(parameters.toConnectionParameters());

//   auto vehicleTypeRepository = std::make_shared<SqlVehicleTypeRepository>();
//   vehicleTypeRepository->setDatabase(dbConnection);


  QTest::qWait(1000);

//   QVERIFY(editVehicleTypeTableModel->fetchAll());

  while(window.isVisible()){
    QTest::qWait(500);
  }

}

/*
 * Tests
 */

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  EditVehicleTypeTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
