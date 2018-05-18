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
#include "CachedRepositoryTableModelTest.h"
#include "Mdt/Entity/CachedRepositoryTableModel.h"
#include "Mdt/Entity/AbstractCachedRepository.h"
#include "Mdt/Entity/RepositoryHandle.h"
#include "Mdt/TestLib/ItemModel.h"
#include "Mdt/TestLib/ItemModelInsertRowTest.h"
#include "Mdt/TestLib/ItemModelRemoveRowTest.h"
#include <QSignalSpy>
#include <QVariantList>
#include <QStringList>
#include <vector>
#include <algorithm>

using namespace Mdt::Entity;
using namespace Mdt::TestLib;

/*
 * Entities and data
 */

class PersonData
{
 public:

  void setId(int id)
  {
    mId = id;
  }

  int id() const
  {
    return mId;
  }

  void setFirstName(const QString & name)
  {
    mFirstName = name;
    Q_ASSERT(!firstName().isEmpty());
  }

  QString firstName() const
  {
    return mFirstName;
  }

 private:

  int mId = 0;
  QString mFirstName;
};

/*
 * Repository
 */

class AbstractPersonRepository : public Mdt::Entity::AbstractCachedRepository<PersonData>
{
 public:

  int columnCount() const override
  {
    return 2;
  }

  QVariant data(int row, int column) const override
  {
    switch(column){
      case 0:
        return constRecordAt(row).id();
      case 1:
        return constRecordAt(row).firstName();
    }
    return QVariant();
  }

 private:

  void setDataToCache(int row, int column, const QVariant& data) override
  {
    switch(column){
      case 0:
        recordAt(row).setId(data.toULongLong());
      case 1:
        recordAt(row).setFirstName(data.toString());
    }
  }
};

using PersonRepositoryHandle = Mdt::Entity::RepositoryHandle<AbstractPersonRepository>;

/*
 * Table model
 */

class EditPersonTableModel : public Mdt::Entity::CachedRepositoryTableModel<AbstractPersonRepository>
{
 public:

  using ParentClass = Mdt::Entity::CachedRepositoryTableModel<AbstractPersonRepository>;
  using ParentClass::ParentClass;
};

/*
 * Repository implementations
 */

PersonData buildPerson(const QString & baseName)
{
  PersonData data;
  data.setFirstName("f" + baseName);
  return data;
}

class MemoryPersonRepository : public AbstractPersonRepository
{
 public:

  void populate(const QStringList & baseNames)
  {
    mMem.clear();
    for(const auto & baseName : baseNames){
      mMem.push_back( buildPerson(baseName) );
    }
  }

  bool fetchRecords(int count) override
  {
    const int n = std::min(count, storageRowCount());
    for(int i = 0; i < n; ++i){
      appendRecordToCache(mMem[i]);
    }
    return true;
  }

  int storageRowCount() const
  {
    return mMem.size();
  }

  bool insertRecordToStorage(const PersonData & record) override
  {
    mMem.push_back(record);
    return true;
  }

  std::vector<PersonData> mMem;
};

/*
 * Tests
 */

void CachedRepositoryTableModelTest::sandbox()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  personRepositoryHandle.repositoryImpl<MemoryPersonRepository>().populate({"A","B"});
  model.setRepository(personRepositoryHandle);
  QCOMPARE(model.rowCount(), 0);
  QVERIFY(personRepositoryHandle.repository().fetchAll());
  QCOMPARE(model.rowCount(), 2);
  
  /// \todo Call fetch all after setup and check that the model has rceived notification
}

void CachedRepositoryTableModelTest::setFetchedRepositoryTest()
{
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  personRepositoryHandle.repositoryImpl<MemoryPersonRepository>().populate({"A","B","C"});
  QVERIFY(personRepositoryHandle.repository().fetchAll());

  EditPersonTableModel model;
  QSignalSpy modelResetSpy(&model, &EditPersonTableModel::modelReset);
  QVERIFY(modelResetSpy.isValid());

  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
  QCOMPARE(modelResetSpy.count(), 0);
  model.setRepository(personRepositoryHandle);
  QCOMPARE(modelResetSpy.count(), 1);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
}

void CachedRepositoryTableModelTest::setRepositoryThenFetchTest()
{
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  personRepositoryHandle.repositoryImpl<MemoryPersonRepository>().populate({"A","B","C"});

  EditPersonTableModel model;
  QSignalSpy modelResetSpy(&model, &EditPersonTableModel::modelReset);
  QVERIFY(modelResetSpy.isValid());

  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
  QCOMPARE(modelResetSpy.count(), 0);
  model.setRepository(personRepositoryHandle);
  QCOMPARE(modelResetSpy.count(), 1);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);
  modelResetSpy.clear();

  QVERIFY(personRepositoryHandle.repository().fetchAll());
  QCOMPARE(modelResetSpy.count(), 1);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
}

void CachedRepositoryTableModelTest::dataTest()
{
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  personRepositoryHandle.repositoryImpl<MemoryPersonRepository>().populate({"A","B","C"});
  QVERIFY(personRepositoryHandle.repository().fetchAll());
  EditPersonTableModel model;
  model.setRepository(personRepositoryHandle);

  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("fA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("fB"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("fC"));

  QVERIFY(getModelData(model, 0, 1, Qt::DecorationRole).isNull());
  QVERIFY(model.data(QModelIndex()).isNull());
}

void CachedRepositoryTableModelTest::insertRowsTest()
{
  EditPersonTableModel model;
  QVERIFY(!appendRowToModel(model));

  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  model.setRepository(personRepositoryHandle);

  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);
  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 2);
}

void CachedRepositoryTableModelTest::itemModelInsertRowTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  model.setRepository(personRepositoryHandle);

  ItemModelInsertRowTest test(&model);
}

void CachedRepositoryTableModelTest::qtModelTest()
{
  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CachedRepositoryTableModelTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
