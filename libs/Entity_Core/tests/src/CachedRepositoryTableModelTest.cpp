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
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/AbstractCachedEntityRepository.h"
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

struct PersonDataStruct
{
  qulonglong id;
  QString firstName;
};

MDT_ENTITY_DEF(
  (PersonDataStruct),
  Person,
  (id, FieldFlag::IsPrimaryKey),
  (firstName, FieldMaxLength(5))
)

class PersonData : public Mdt::Entity::DataTemplate<PersonEntity>
{
 public:

  void setFirstName(const QString & name)
  {
    dataStruct().firstName = name.trimmed();
    Q_ASSERT(!firstName().isEmpty());
  }

  QString firstName() const
  {
    return constDataStruct().firstName;
  }
};

/*
 * Repository
 */

class AbstractPersonRepository : public Mdt::Entity::AbstractCachedEntityRepository<PersonData>
{
 public:

};

using PersonRepository = Mdt::Entity::RepositoryHandle<AbstractPersonRepository>;

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

class PersonMemoryRepository : public AbstractPersonRepository
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
    return false;
  }

  std::vector<PersonData> mMem;
};

/*
 * Tests
 */

void CachedRepositoryTableModelTest::sandbox()
{
  EditPersonTableModel model;
  auto personRepository = PersonRepository::make<PersonMemoryRepository>();
  personRepository.repositoryImpl<PersonMemoryRepository>().populate({"A","B"});
  model.setRepository(personRepository);
  QCOMPARE(model.rowCount(), 0);
  QVERIFY(personRepository.repository().fetchAll());
  QCOMPARE(model.rowCount(), 2);
  
  /// \todo Call fetch all after setup and check that the model has rceived notification
}

void CachedRepositoryTableModelTest::setFetchedRepositoryTest()
{
  auto personRepository = PersonRepository::make<PersonMemoryRepository>();
  personRepository.repositoryImpl<PersonMemoryRepository>().populate({"A","B","C"});
  QVERIFY(personRepository.repository().fetchAll());

  EditPersonTableModel model;
  QSignalSpy modelResetSpy(&model, &EditPersonTableModel::modelReset);
  QVERIFY(modelResetSpy.isValid());

  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
  QCOMPARE(modelResetSpy.count(), 0);
  model.setRepository(personRepository);
  QCOMPARE(modelResetSpy.count(), 1);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
}

void CachedRepositoryTableModelTest::setRepositoryThenFetchTest()
{
  auto personRepository = PersonRepository::make<PersonMemoryRepository>();
  personRepository.repositoryImpl<PersonMemoryRepository>().populate({"A","B","C"});

  EditPersonTableModel model;
  QSignalSpy modelResetSpy(&model, &EditPersonTableModel::modelReset);
  QVERIFY(modelResetSpy.isValid());

  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
  QCOMPARE(modelResetSpy.count(), 0);
  model.setRepository(personRepository);
  QCOMPARE(modelResetSpy.count(), 1);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);
  modelResetSpy.clear();

  QVERIFY(personRepository.repository().fetchAll());
  QCOMPARE(modelResetSpy.count(), 1);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
}

void CachedRepositoryTableModelTest::dataTest()
{
  auto personRepository = PersonRepository::make<PersonMemoryRepository>();
  personRepository.repositoryImpl<PersonMemoryRepository>().populate({"A","B","C"});
  QVERIFY(personRepository.repository().fetchAll());
  EditPersonTableModel model;
  model.setRepository(personRepository);

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

  auto personRepositoryHandle = PersonRepository::make<PersonMemoryRepository>();
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
  auto personRepositoryHandle = PersonRepository::make<PersonMemoryRepository>();
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
