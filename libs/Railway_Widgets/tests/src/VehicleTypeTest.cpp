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
#include "Mdt/Railway/VehicleTypeClassSqlRepository.h"

#include "Mdt/Railway/VehicleTypeClassRepository.h"
#include "Mdt/Railway/SqlSchema.h"
#include "Mdt/Sql/SQLiteDatabase.h"
// #include "Mdt/Sql/SQLiteDatabaseFileDialog.h"
#include <QSqlDatabase>
#include <QFile>
#include <QTemporaryFile>
#include <vector>
#include <algorithm>

#include <QDebug>

using namespace Mdt::Railway;
using namespace Mdt::Sql;

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

void VehicleTypeTest::sandbox()
{
//   VehicleTypeEditionWidget w;
//   w.show();
//   VehicleTypeEditionWindow window;

//   QTemporaryFile dbFile;
//   QFile dbFile("railway.sqlite");
//   QVERIFY(dbFile.open(QFile::ReadWrite));
//   qDebug() << "DB file: " << dbFile.fileName();
  SqlSchema schema;
  SQLiteDatabase sqliteDb;
  const QString dbFile = "railway.sqlite";
  if(!sqliteDb.openExisting(dbFile)){
    QVERIFY(sqliteDb.createNew(dbFile));
    QVERIFY(schema.createSchema(sqliteDb.database()));
  }
  auto dbConnection = sqliteDb.database();
//   auto dbConnection = QSqlDatabase::addDatabase("QSQLITE");
  QVERIFY(dbConnection.isValid());
//   dbConnection.setDatabaseName("railway.sqlite");
//   qDebug() << "DB file: " << dbConnection.databaseName();
//   QVERIFY(dbConnection.open());
// 
//   SqlSchema schema;
//   QVERIFY(schema.createSchema(dbConnection));

  auto vehicleTypeClassRepository = VehicleTypeClassRepository::make<VehicleTypeClassSqlRepository>();
  vehicleTypeClassRepository.repositoryImpl<VehicleTypeClassSqlRepository>().setDbConnection(dbConnection);
  QVERIFY(vehicleTypeClassRepository.repository().fetchAll());

  VehicleTypeClassEditionWindow window;
  window.setVehicleTypeClassRepository(vehicleTypeClassRepository);
  window.show();
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
  VehicleTypeTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
