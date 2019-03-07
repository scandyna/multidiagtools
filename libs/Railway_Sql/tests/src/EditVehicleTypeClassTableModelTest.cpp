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
#include "EditVehicleTypeClassTableModelTest.h"
#include "Mdt/Railway/CreateVehicleTypeClass.h"
#include "Mdt/Railway/UpdateVehicleTypeClass.h"
#include "Mdt/Railway/RemoveVehicleTypeClass.h"
#include "Mdt/TestLib/ItemModel.h"
#include <QDebug>

using namespace Mdt::Railway;
using namespace Mdt::TestLib;
using Mdt::QueryExpression::SqlSelectQuery;

/*
 * EditVehicleTypeClassTableModelTest ctor, init
 */

EditVehicleTypeClassTableModelTest::EditVehicleTypeClassTableModelTest()
 : mVehicleTypeClassRepository( std::make_shared<SqlVehicleTypeClassRepository>() )
{
}

void EditVehicleTypeClassTableModelTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  mVehicleTypeClassRepository->setDatabase(database());
//   QVERIFY( mSelectQueryFactory->setup(connectionParameters()) );
  QVERIFY(createTestSchema());
  mSelectQuery.setDatabase(database());
}

/*
 * Helpers
 */

void debugError(const Mdt::Error & error)
{
  qDebug() << error.functionName() << ": " << error.text();
}

/*
 * Tests
 */

void EditVehicleTypeClassTableModelTest::insertAndSubmitAndFetchAllTest()
{
  QVERIFY(clearStorage());

  EditVehicleTypeClassTableModel model;
  setupTableModel(&model);
  QCOMPARE(model.columnCount(), 3);
  QCOMPARE(model.rowCount(), 0);

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
  QVERIFY( setModelData(model, 0, model.vehicleTypeNameColumn(), "RBDe 560 DO") );
  QVERIFY( setModelData(model, 0, model.vehicleTypeAliasColumn(), "DOMINO") );
  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, model.vehicleTypeNameColumn()), QVariant("RBDe 560 DO"));
  QCOMPARE(getModelData(model, 0, model.vehicleTypeAliasColumn()), QVariant("DOMINO"));

  QVERIFY(model.fetchAll());
  QCOMPARE(model.columnCount(), 3);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, model.vehicleTypeNameColumn()), QVariant("RBDe 560 DO"));
  QCOMPARE(getModelData(model, 0, model.vehicleTypeAliasColumn()), QVariant("DOMINO"));
}

void EditVehicleTypeClassTableModelTest::setDataAndSubmitTest()
{
  QVERIFY(clearStorage());

  const auto vehicleTypeClassId1 = createVehicleTypeClass("RBDe 560 DO", "DOMINO");
  QVERIFY(!vehicleTypeClassId1.isNull());
  const auto vehicleTypeClassId2 = createVehicleTypeClass("RABe 514", "DTZ");
  QVERIFY(!vehicleTypeClassId2.isNull());

  EditVehicleTypeClassTableModel model;
  setupTableModel(&model);
  QVERIFY(model.fetchAll());
  QCOMPARE(model.columnCount(), 3);
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(getModelData(model, 0, model.idColumn()).toInt() > 0);
  QCOMPARE(getModelData(model, 0, model.vehicleTypeNameColumn()), QVariant("RBDe 560 DO"));
  QCOMPARE(getModelData(model, 0, model.vehicleTypeAliasColumn()), QVariant("DOMINO"));
  QVERIFY(getModelData(model, 1, model.idColumn()).toInt() > 0);
  QCOMPARE(getModelData(model, 1, model.vehicleTypeNameColumn()), QVariant("RABe 514"));
  QCOMPARE(getModelData(model, 1, model.vehicleTypeAliasColumn()), QVariant("DTZ"));

  QVERIFY( setModelData(model, 0, model.vehicleTypeNameColumn(), "RABDe 500") );
  QVERIFY( setModelData(model, 0, model.vehicleTypeAliasColumn(), "ICN") );
  QVERIFY(getModelData(model, 0, model.idColumn()).toInt() > 0);
  QCOMPARE(getModelData(model, 0, model.vehicleTypeNameColumn()), QVariant("RABDe 500"));
  QCOMPARE(getModelData(model, 0, model.vehicleTypeAliasColumn()), QVariant("ICN"));
  QVERIFY(getModelData(model, 1, model.idColumn()).toInt() > 0);
  QCOMPARE(getModelData(model, 1, model.vehicleTypeNameColumn()), QVariant("RABe 514"));
  QCOMPARE(getModelData(model, 1, model.vehicleTypeAliasColumn()), QVariant("DTZ"));

  QVERIFY(model.submitChanges());
  QVERIFY(getModelData(model, 0, model.idColumn()).toInt() > 0);
  QCOMPARE(getModelData(model, 0, model.vehicleTypeNameColumn()), QVariant("RABDe 500"));
  QCOMPARE(getModelData(model, 0, model.vehicleTypeAliasColumn()), QVariant("ICN"));
  QVERIFY(getModelData(model, 1, model.idColumn()).toInt() > 0);
  QCOMPARE(getModelData(model, 1, model.vehicleTypeNameColumn()), QVariant("RABe 514"));
  QCOMPARE(getModelData(model, 1, model.vehicleTypeAliasColumn()), QVariant("DTZ"));

  auto vehicleTypeClass = mVehicleTypeClassRepository->getById(vehicleTypeClassId1);
  QVERIFY(vehicleTypeClass);
  QCOMPARE(vehicleTypeClass->name(), QString("RABDe 500"));
  QCOMPARE(vehicleTypeClass->alias(), QString("ICN"));
  vehicleTypeClass = mVehicleTypeClassRepository->getById(vehicleTypeClassId2);
  QVERIFY(vehicleTypeClass);
  QCOMPARE(vehicleTypeClass->name(), QString("RABe 514"));
  QCOMPARE(vehicleTypeClass->alias(), QString("DTZ"));
}

void EditVehicleTypeClassTableModelTest::removeTest()
{
  QVERIFY(clearStorage());

  const auto vehicleTypeClassId1 = createVehicleTypeClass("RBDe 560 DO", "DOMINO");
  QVERIFY(!vehicleTypeClassId1.isNull());
  const auto vehicleTypeClassId2 = createVehicleTypeClass("RABe 514", "DTZ");
  QVERIFY(!vehicleTypeClassId2.isNull());

  EditVehicleTypeClassTableModel model;
  setupTableModel(&model);
  QVERIFY(model.fetchAll());
  QCOMPARE(model.columnCount(), 3);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, model.vehicleTypeNameColumn()), QVariant("RBDe 560 DO"));
  QCOMPARE(getModelData(model, 0, model.vehicleTypeAliasColumn()), QVariant("DOMINO"));
  QCOMPARE(getModelData(model, 1, model.vehicleTypeNameColumn()), QVariant("RABe 514"));
  QCOMPARE(getModelData(model, 1, model.vehicleTypeAliasColumn()), QVariant("DTZ"));

  QVERIFY(model.removeRow(0));
  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, model.vehicleTypeNameColumn()), QVariant("RABe 514"));
  QCOMPARE(getModelData(model, 0, model.vehicleTypeAliasColumn()), QVariant("DTZ"));
}

/*
 * Helpers
 */

void EditVehicleTypeClassTableModelTest::setupTableModel(EditVehicleTypeClassTableModel *model)
{
  Q_ASSERT(model != nullptr);

  connect(model, &EditVehicleTypeClassTableModel::fetchVehicleTypeClassesRequested, &mSelectQuery, &SqlSelectQuery::execAndFetchAll);
  connect(&mSelectQuery, &SqlSelectQuery::newRecordAvailable, model, &EditVehicleTypeClassTableModel::fromBackendAppendRecord);
  connect(model, &EditVehicleTypeClassTableModel::errorOccured, debugError);

  auto *createVehicleTypeClass = new CreateVehicleTypeClass(mVehicleTypeClassRepository, model);
  connect(model, &EditVehicleTypeClassTableModel::createVehicleTypeClassRequested, createVehicleTypeClass , &CreateVehicleTypeClass::execute);
  connect(createVehicleTypeClass, &CreateVehicleTypeClass::succeeded, model, &EditVehicleTypeClassTableModel::onCreateVehicleTypeClassSucceded);
  connect(createVehicleTypeClass, &CreateVehicleTypeClass::failed, model, &EditVehicleTypeClassTableModel::onCreateVehicleTypeClassFailed);
//   auto *createVehicleTypeClassResponseHandler = new ResponseHandler<CreateVehicleTypeClassRequest, CreateVehicleTypeClassResponse>(model);
//   createVehicleTypeClassResponseHandler->setUseCase(*createVehicleTypeClass);

  auto *updateVehicleTypeClass = new UpdateVehicleTypeClass(mVehicleTypeClassRepository, model);
  connect(model, &EditVehicleTypeClassTableModel::updateVehicleTypeClassRequested, updateVehicleTypeClass , &UpdateVehicleTypeClass::execute);
  connect(updateVehicleTypeClass, &UpdateVehicleTypeClass::succeeded, model, &EditVehicleTypeClassTableModel::onUpdateVehicleTypeClassSucceded);
  connect(updateVehicleTypeClass, &UpdateVehicleTypeClass::failed, model, &EditVehicleTypeClassTableModel::onUpdateVehicleTypeClassFailed);
//   auto *updateVehicleTypeClassResponseHandler = new ResponseHandler<UpdateVehicleTypeClassRequest, UpdateVehicleTypeClassResponse>(model);
//   updateVehicleTypeClassResponseHandler->setUseCase(*updateVehicleTypeClass);

  auto *removeVehicleTypeClass = new RemoveVehicleTypeClass(mVehicleTypeClassRepository, model);
  connect(model, &EditVehicleTypeClassTableModel::removeVehicleTypeClassRequested, removeVehicleTypeClass , &RemoveVehicleTypeClass::execute);
  connect(removeVehicleTypeClass, &RemoveVehicleTypeClass::succeeded, model, &EditVehicleTypeClassTableModel::onRemoveVehicleTypeClassSucceeded);
  connect(removeVehicleTypeClass, &RemoveVehicleTypeClass::failed, model, &EditVehicleTypeClassTableModel::onRemoveVehicleTypeClassFailed);
}

bool EditVehicleTypeClassTableModelTest::clearStorage()
{
  if(!mVehicleTypeClassRepository->removeAll()){
    mVehicleTypeClassRepository->lastError().commit();
    return false;
  }
  return true;
}

VehicleTypeClassId EditVehicleTypeClassTableModelTest::createVehicleTypeClass(const QString& name, const QString& alias)
{
  CreateVehicleTypeClass cvtc(mVehicleTypeClassRepository);
  CreateVehicleTypeClassRequest request;

  request.name = name;
  request.alias = alias;
  cvtc.execute(request);

  return cvtc.response().id;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  EditVehicleTypeClassTableModelTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
