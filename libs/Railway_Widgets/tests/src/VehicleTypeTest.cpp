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
#include "VehicleTypeTest.h"
#include "Mdt/Railway/VehicleTypeClassEditionWindow.h"
#include "Mdt/Railway/VehicleTypeEditionWidget.h"
#include "Mdt/Railway/VehicleTypeEditionWindow.h"
#include "Mdt/Railway/CreateVehicleTypeClass.h"
#include "Mdt/Railway/UpdateVehicleTypeClass.h"
#include "Mdt/Railway/RemoveVehicleTypeClass.h"
#include "Mdt/Railway/VehicleTypeClassSqlRepository.h"
#include "Mdt/Railway/EditVehicleTypeClassTableModel.h"
#include "Mdt/Railway/EditVehicleTypeTableModel.h"

#include "Mdt/Railway/CreateVehicleType.h"
#include "Mdt/Railway/SqlVehicleTypeRepository.h"
#include "Mdt/Railway/SqlVehicleTypeClassRepository.h"

#include "Mdt/Railway/VehicleTypeClassRepository.h"
#include "Mdt/Railway/SqlSchema.h"
#include "Mdt/Sql/SQLiteDatabase.h"
#include "Mdt/QueryExpression/SqlSelectQueryFactory.h"
// #include "Mdt/QueryExpression/SqlAsyncSelectQuery.h"
#include "Mdt/QueryExpression/SqlAsyncSelectQueryFactory.h"
#include "Mdt/Sql/SQLiteConnectionParameters.h"
// #include "Mdt/Sql/SQLiteDatabaseFileDialog.h"
#include <QSqlDatabase>
#include <QFile>
#include <QTemporaryFile>
#include <vector>
#include <algorithm>
#include <memory>

#include <QDebug>

using namespace Mdt::Railway;
using namespace Mdt::Sql;
using namespace Mdt::QueryExpression;

void VehicleTypeTest::initTestCase()
{
}

void VehicleTypeTest::cleanupTestCase()
{
}

class MemoryVehicleTypeClassRepository : public AbstractVehicleTypeClassRepository
{
 public:

  bool fetchRecords(int count) override
  {
    const int n = std::min(count, storageRowCount());
    for(int i = 0; i < n; ++i){
      appendRecordToCache(mStorage[i]);
    }
    return true;
  }

 private:

  int storageRowCount() const
  {
    return mStorage.size();
  }

  std::vector<VehicleTypeClassData> mStorage;
};

void setupEditVehicleTypeClassTableModel(EditVehicleTypeClassTableModel *model,
                                         const std::shared_ptr<VehicleTypeClassRepository> & repository,
                                         const std::unique_ptr<AbstractAsyncSelectQuery> & selectQuery)
{
  Q_ASSERT(model != nullptr);

  QObject::connect(model, &EditVehicleTypeClassTableModel::fetchVehicleTypeClassesRequested, selectQuery.get(), &AbstractAsyncSelectQuery::submitStatement);
  QObject::connect(selectQuery.get(), &AbstractAsyncSelectQuery::newRecordAvailable, model, &EditVehicleTypeClassTableModel::fromBackendAppendRecord);

  auto *createVehicleTypeClass = new CreateVehicleTypeClass(repository, model);
  QObject::connect(model, &EditVehicleTypeClassTableModel::createVehicleTypeClassRequested, createVehicleTypeClass, &CreateVehicleTypeClass::execute);
  QObject::connect(createVehicleTypeClass, &CreateVehicleTypeClass::succeeded, model, &EditVehicleTypeClassTableModel::onCreateVehicleTypeClassSucceded);
  QObject::connect(createVehicleTypeClass, &CreateVehicleTypeClass::failed, model, &EditVehicleTypeClassTableModel::onCreateVehicleTypeClassFailed);

  auto *updateVehicleTypeClass = new UpdateVehicleTypeClass(repository, model);
  QObject::connect(model, &EditVehicleTypeClassTableModel::updateVehicleTypeClassRequested, updateVehicleTypeClass, &UpdateVehicleTypeClass::execute);
  QObject::connect(updateVehicleTypeClass, &UpdateVehicleTypeClass::succeeded, model, &EditVehicleTypeClassTableModel::onUpdateVehicleTypeClassSucceded);
  QObject::connect(updateVehicleTypeClass, &UpdateVehicleTypeClass::failed, model, &EditVehicleTypeClassTableModel::onUpdateVehicleTypeClassFailed);

  auto *removeVehicleTypeClass = new RemoveVehicleTypeClass(repository, model);
  QObject::connect(model, &EditVehicleTypeClassTableModel::removeVehicleTypeClassRequested, removeVehicleTypeClass , &RemoveVehicleTypeClass::execute);
  QObject::connect(removeVehicleTypeClass, &RemoveVehicleTypeClass::succeeded, model, &EditVehicleTypeClassTableModel::onRemoveVehicleTypeClassSucceeded);
  QObject::connect(removeVehicleTypeClass, &RemoveVehicleTypeClass::failed, model, &EditVehicleTypeClassTableModel::onRemoveVehicleTypeClassFailed);
}

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

/**
* @brief ${...}
* 
*/
void VehicleTypeTest::sandbox()
{
//   VehicleTypeEditionWidget w;
//   w.show();
//   VehicleTypeEditionWindow window;

  SqlSchema schema;
  SQLiteDatabase sqliteDb;
  const QString dbFile = "railway.sqlite";
  if(!sqliteDb.openExisting(dbFile)){
    QVERIFY(sqliteDb.createNew(dbFile));
    QVERIFY(schema.createSchema(sqliteDb.database()));
  }
  auto dbConnection = sqliteDb.database();
  QVERIFY(dbConnection.isValid());

  auto vehicleTypeClassRepository = VehicleTypeClassRepositoryHandle::make<VehicleTypeClassSqlRepository>();
  vehicleTypeClassRepository.repositoryImpl<VehicleTypeClassSqlRepository>().setDbConnection(dbConnection);

  auto vehicleTypeClassRepository2 = std::make_shared<SqlVehicleTypeClassRepository>();
  vehicleTypeClassRepository2->setDatabase(dbConnection);

  auto queryFactory = std::make_shared<SqlSelectQueryFactory>();
  queryFactory->setDatabase(dbConnection);
//   SelectQueryFactory queryFactory;
//   auto & sqlQueryFactory = queryFactory.makeImpl<SqlSelectQueryFactory>();
//   sqlQueryFactory.setDatabase(dbConnection);

  SQLiteConnectionParameters parameters;
  parameters.setDatabaseFile( dbConnection.databaseName() );
  auto asyncQueryFactory = std::make_shared<SqlAsyncSelectQueryFactory>();
  asyncQueryFactory->setup(parameters.toConnectionParameters());

  VehicleTypeClassEditionWindow window;
//   window.setVehicleTypeClassRepository(vehicleTypeClassRepository);
//   window.setVehicleTypeClassRepository(vehicleTypeClassRepository2);
//   window.setSelectQueryFactory(queryFactory);
//   window.setAsyncSelectQueryFactory(asyncQueryFactory);
  window.show();
//   while(window.isVisible()){
//     QTest::qWait(500);
//   }

  auto editVehicleTypeClassTableModel = std::make_shared<EditVehicleTypeClassTableModel>();
  auto editVehicleTypeClassSelectQuery = asyncQueryFactory->createSelectQuery();
  setupEditVehicleTypeClassTableModel(editVehicleTypeClassTableModel.get(), vehicleTypeClassRepository2, editVehicleTypeClassSelectQuery);

  window.setModel(editVehicleTypeClassTableModel);
  QObject::connect(editVehicleTypeClassTableModel.get(), &EditVehicleTypeClassTableModel::errorOccured, &window, &VehicleTypeClassEditionWindow::displayError);

  auto vehicleTypeRepository = std::make_shared<SqlVehicleTypeRepository>();
  vehicleTypeRepository->setDatabase(dbConnection);

  auto editVehicleTypeTableModel = std::make_shared<EditVehicleTypeTableModel>();
  auto editVehicleTypeSelectQuery = asyncQueryFactory->createSelectQuery();
  setupEditVehicleTypeTableModel(editVehicleTypeTableModel.get(), vehicleTypeRepository, editVehicleTypeSelectQuery);

  VehicleTypeEditionWindow vtWindow;
  vtWindow.setVehicleTypeClassTableModel(editVehicleTypeClassTableModel);
  vtWindow.setEditVehicleTypeTableModel(editVehicleTypeTableModel);
  QObject::connect(editVehicleTypeTableModel.get(), &EditVehicleTypeTableModel::errorOccured, &vtWindow, &VehicleTypeEditionWindow::displayError);

//   QObject::connect(&vtWindow, &VehicleTypeEditionWindow::createVehicleTypeRequested, &cvt, &CreateVehicleType::execute);
//   QObject::connect(&cvt, &CreateVehicleType::succeed, &vtWindow, &VehicleTypeEditionWindow::displayCreatedVehicleType);
//   QObject::connect(&createVehicleType, &CreateVehicleType::failed, &vtWindow, &VehicleTypeEditionWindow::handleError);
  vtWindow.show();

  QTest::qWait(1000);

//   vtWindow.setQueryFactory(asyncQueryFactory);

//   QVERIFY(vehicleTypeClassRepository.repository().fetchAll());
  QVERIFY(editVehicleTypeClassTableModel->fetchAll());
  QVERIFY(editVehicleTypeTableModel->fetchAll());

  while(vtWindow.isVisible()){
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
  VehicleTypeTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
