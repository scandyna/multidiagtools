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
#include "Mdt/Railway/CreateVehicleType.h"
#include "Mdt/Railway/UpdateVehicleType.h"
#include "Mdt/Railway/CreateVehicleTypeClass.h"
#include "Mdt/Railway/VehicleTypeData.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/TestLib/ItemModel.h"
#include <QDebug>

using namespace Mdt::Railway;
using namespace Mdt::TestLib;
using Mdt::QueryExpression::SqlSelectQuery;
using Mdt::Entity::EntitySelectStatement;

EditVehicleTypeTableModelTest::EditVehicleTypeTableModelTest()
 : mVehicleTypeClassRepository( std::make_shared<SqlVehicleTypeClassRepository>() ),
   mVehicleTypeRepository( std::make_shared<SqlVehicleTypeRepository>() )
{
}

void EditVehicleTypeTableModelTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  mVehicleTypeClassRepository->setDatabase(database());
  mVehicleTypeRepository->setDatabase(database());
  mSelectQuery.setDatabase(database());
  QVERIFY(createTestSchema());
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

void EditVehicleTypeTableModelTest::fetchAllTest()
{
  QVERIFY(clearStorage());

  EditVehicleTypeTableModel model;
  setupTableModel(&model);

  QVERIFY( !createVehicleType("RBDe 560 DO","DOMINO","1").isNull() );
  QVERIFY( !createVehicleType("RABe 514","DTZ","2").isNull() );

  QCOMPARE(model.rowCount(), 0);

  QVERIFY(model.fetchAll());
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, model.vehicleTypeNameColumn()), QVariant("RBDe 560 DO"));
  QCOMPARE(getModelData(model, 0, model.vehicleTypeAliasColumn()), QVariant("DOMINO"));
  QCOMPARE(getModelData(model, 0, model.manufacturerSerieColumn()), QVariant("1"));
  QCOMPARE(getModelData(model, 1, model.vehicleTypeNameColumn()), QVariant("RABe 514"));
  QCOMPARE(getModelData(model, 1, model.vehicleTypeAliasColumn()), QVariant("DTZ"));
  QCOMPARE(getModelData(model, 1, model.manufacturerSerieColumn()), QVariant("2"));
}

void EditVehicleTypeTableModelTest::setVehicleTypeClassTest()
{
  QVERIFY(clearStorage());

  EditVehicleTypeTableModel model;
  setupTableModel(&model);

  QVERIFY( !createVehicleType("RBDe 560 DO","DOMINO","1").isNull() );
  QVERIFY( !createVehicleType("RABe 514","DTZ","3").isNull() );

  QVERIFY(model.fetchAll());
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, model.vehicleTypeNameColumn()), QVariant("RBDe 560 DO"));
  QCOMPARE(getModelData(model, 0, model.vehicleTypeAliasColumn()), QVariant("DOMINO"));
  QCOMPARE(getModelData(model, 0, model.manufacturerSerieColumn()), QVariant("1"));
  QCOMPARE(getModelData(model, 1, model.vehicleTypeNameColumn()), QVariant("RABe 514"));
  QCOMPARE(getModelData(model, 1, model.vehicleTypeAliasColumn()), QVariant("DTZ"));
  QCOMPARE(getModelData(model, 1, model.manufacturerSerieColumn()), QVariant("3"));

  const int vehicleTypeClassId = getModelData(model, 1, model.vehicleTypeClassIdColumn()).toInt();
  QVERIFY(vehicleTypeClassId > 0);
  QVERIFY(setModelData(model, 0, model.vehicleTypeClassIdColumn(), vehicleTypeClassId));
  QCOMPARE(getModelData(model, 0, model.vehicleTypeNameColumn()), QVariant("RABe 514"));
  QCOMPARE(getModelData(model, 0, model.vehicleTypeAliasColumn()), QVariant("DTZ"));
  QCOMPARE(getModelData(model, 0, model.manufacturerSerieColumn()), QVariant("1"));
  QCOMPARE(getModelData(model, 1, model.vehicleTypeNameColumn()), QVariant("RABe 514"));
  QCOMPARE(getModelData(model, 1, model.vehicleTypeAliasColumn()), QVariant("DTZ"));
  QCOMPARE(getModelData(model, 1, model.manufacturerSerieColumn()), QVariant("3"));
}

void EditVehicleTypeTableModelTest::insertAndSubmitTest()
{
  QVERIFY(clearStorage());

  EditVehicleTypeTableModel model;
  setupTableModel(&model);
  QCOMPARE(model.rowCount(), 0);

  VehicleTypeClassId vehicleTypeClassId = createVehicleTypeClass("RBDe 560 DO", "DOMINO");
  QVERIFY(!vehicleTypeClassId.isNull());

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
  QVERIFY( setModelData(model, 0, model.vehicleTypeClassIdColumn(), vehicleTypeClassId.value()) );
  QVERIFY( setModelData(model, 0, model.manufacturerSerieColumn(), "2") );
  QVERIFY(model.submitChanges());

  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, model.vehicleTypeNameColumn()), QVariant("RBDe 560 DO"));
  QCOMPARE(getModelData(model, 0, model.vehicleTypeAliasColumn()), QVariant("DOMINO"));
  QCOMPARE(getModelData(model, 0, model.manufacturerSerieColumn()), QVariant("2"));
  QVERIFY( getModelData(model, 0, model.vehicleTypeIdColumn()).toInt() > 0 );
  QCOMPARE(getModelData(model, 0, model.vehicleTypeClassIdColumn()).toULongLong(), vehicleTypeClassId.value());
}

void EditVehicleTypeTableModelTest::setDataAndSubmitTest()
{
  QVERIFY(clearStorage());

  

  QFAIL("Not complete");
}

/*
 * Helpers
 */

void EditVehicleTypeTableModelTest::setupTableModel(Mdt::Railway::EditVehicleTypeTableModel *model)
{
  Q_ASSERT(model != nullptr);

  connect(model, &EditVehicleTypeTableModel::fetchAllRequested, &mSelectQuery, &SqlSelectQuery::execAndFetchAll);
  connect(&mSelectQuery, &SqlSelectQuery::newRecordAvailable, model, &EditVehicleTypeTableModel::fromBackendAppendRecord);
  connect(model, &EditVehicleTypeTableModel::errorOccured, debugError);

  auto *createVehicleType = new CreateVehicleType(mVehicleTypeRepository, model);
  connect(model, &EditVehicleTypeTableModel::createVehicleTypeRequested, createVehicleType , &CreateVehicleType::execute);
  connect(createVehicleType, &CreateVehicleType::succeeded, model, &EditVehicleTypeTableModel::onCreateVehicleTypeSucceeded);
  connect(createVehicleType, &CreateVehicleType::failed, model, &EditVehicleTypeTableModel::onCreateVehicleTypeFailed);

  auto *updateVehicleType = new UpdateVehicleType(mVehicleTypeRepository, model);
  connect(model, &EditVehicleTypeTableModel::updateVehicleTypeRequested, updateVehicleType , &UpdateVehicleType::execute);
  connect(updateVehicleType, &UpdateVehicleType::succeeded, model, &EditVehicleTypeTableModel::onUpdateVehicleTypeSucceeded);
  connect(updateVehicleType, &UpdateVehicleType::failed, model, &EditVehicleTypeTableModel::onUpdateVehicleTypeFailed);

//   UpdateVehicleType updateVehicleType(mVehicleTypeRepository);
//   connect(&model, &EditVehicleTypeTableModel::updateVehicleTypeRequested, &updateVehicleType , &UpdateVehicleType::execute);
//   connect(&updateVehicleType, &UpdateVehicleType::succeeded, &model, &EditVehicleTypeTableModel::onCreateVehicleTypeSucceded);
//   connect(&updateVehicleType, &UpdateVehicleType::failed, &model, &EditVehicleTypeTableModel::onCreateVehicleTypeFailed);
}

bool EditVehicleTypeTableModelTest::clearStorage()
{
  if(!mVehicleTypeRepository->removeAll()){
    mVehicleTypeRepository->lastError().commit();
    return false;
  }
  if(!mVehicleTypeClassRepository->removeAll()){
    mVehicleTypeClassRepository->lastError().commit();
    return false;
  }
  return true;
}

VehicleTypeClassId EditVehicleTypeTableModelTest::createVehicleTypeClass(const QString& name, const QString& alias)
{
  CreateVehicleTypeClass cvtc(mVehicleTypeClassRepository);
  CreateVehicleTypeClassRequest request;

  request.name = name;
  request.alias = alias;
  cvtc.execute(request);

  return cvtc.response().id;
}

Mdt::Railway::VehicleTypeClassId EditVehicleTypeTableModelTest::getVehicleTypeClass(const QString& name, const QString& alias)
{
  VehicleTypeClassId vehicleTypeClassId;

  EntitySelectStatement<Entity::VehicleTypeClassEntity> statement;
  const auto nameField = statement.makeSelectField( statement.def().name() );
  const auto aliasField = statement.makeSelectField( statement.def().alias() );
  statement.addField(statement.def().id());
  statement.setFilter( (nameField == name) && (aliasField == alias) );
  if(!mSelectQuery.exec(statement)){
    return vehicleTypeClassId;
  }
  if(mSelectQuery.next()){
    Q_ASSERT(mSelectQuery.fieldCount() == 1);
    vehicleTypeClassId = VehicleTypeClassId::fromQVariant( mSelectQuery.value(0) );
  }

  return vehicleTypeClassId;
}

VehicleTypeId EditVehicleTypeTableModelTest::createVehicleType(const QString& name, const QString& alias, const QString& manufacturerSerie)
{
  VehicleTypeClassId vehicleTypeClassId = getVehicleTypeClass(name, alias);
  if(vehicleTypeClassId.isNull()){
    vehicleTypeClassId = createVehicleTypeClass(name, alias);
    if(vehicleTypeClassId.isNull()){
      qWarning() << "createVehicleType(): error while creating vehicle type class";
      return VehicleTypeId();
    }
  }
  VehicleTypeData vehicleType;
  vehicleType.setVehicleTypeClassId(vehicleTypeClassId);
  vehicleType.setManufacturerSerie(manufacturerSerie);
  const auto vehicleTypeId = mVehicleTypeRepository->add(vehicleType);
  if(!vehicleTypeId){
    qWarning() << "createVehicleType(): error while creating vehicle type: " << vehicleTypeId.error().text();
  }

  return *vehicleTypeId;
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
