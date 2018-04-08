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
#include "CachedRepositoryStlTableProxyTest.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/AbstractCachedRepository.h"
#include "Mdt/Entity/RepositoryHandle.h"
#include "Mdt/Entity/CachedRepositoryStlTableProxy.h"
#include "Mdt/Entity/RowResizableTableModel.h"

#include "Mdt/Entity/EditableTableModel.h"
#include "Mdt/Entity/ReadOnlyTableModel.h"

#include "Mdt/TestLib/ItemModel.h"
#include <QStringList>
#include <vector>

#include <QDebug>

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

class AbstractPersonRepository : public Mdt::Entity::AbstractCachedRepository<PersonData>
{
 public:

  virtual void populate(const QStringList & baseNames) = 0;

};

using PersonRepository = Mdt::Entity::RepositoryHandle<AbstractPersonRepository>;
using PersonRepositoryStlTableProxy = Mdt::Entity::CachedRepositoryStlTableProxy<AbstractPersonRepository>;

class EditPersonTableModel : public Mdt::Entity::RowResizableTableModel<PersonRepositoryStlTableProxy>
{
 public:

  using ParentClass = Mdt::Entity::RowResizableTableModel<PersonRepositoryStlTableProxy>;
  using ParentClass::ParentClass;
};

PersonData buildPerson(const QString & baseName)
{
  PersonData data;
  data.setFirstName("f" + baseName);
  return data;
}

class MemoryPersonRepository : public AbstractPersonRepository
{
 public:

  void populate(const QStringList & baseNames) override
  {
    mMem.clear();
    for(const auto & baseName : baseNames){
      mMem.push_back( buildPerson(baseName) );
    }
  }

 private:

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

  std::vector<PersonData> mMem;
};

/*
 * Tests
 */

void CachedRepositoryStlTableProxyTest::noRepositorySetTest()
{
  PersonRepositoryStlTableProxy proxy;
  QCOMPARE(proxy.rowCount(), 0);
  QVERIFY(proxy.cbegin() == proxy.cend());
}

void CachedRepositoryStlTableProxyTest::simpleSetGetTest()
{
  auto personRepository = PersonRepository::make<MemoryPersonRepository>();
  PersonRepositoryStlTableProxy proxy;
  proxy.setRepository(personRepository);

  QCOMPARE(proxy.rowCount(), 0);
  personRepository.repository().populate({"A","B","C"});
  QVERIFY(personRepository.repository().fetchAll());
  QCOMPARE(proxy.rowCount(), 3);

  auto cit = proxy.cbegin();
  QCOMPARE(cit->firstName(), QString("fA"));
  ++cit;
  QCOMPARE(cit->firstName(), QString("fB"));
  ++cit;
  QCOMPARE(cit->firstName(), QString("fC"));
  ++cit;
  QVERIFY(cit == proxy.cend());

  auto it = proxy.begin();
  it->setFirstName("EfA");
  QCOMPARE(it->firstName(), QString("EfA"));
  ++it;
  it->setFirstName("EfB");
  QCOMPARE(it->firstName(), QString("EfB"));
  ++it;
  it->setFirstName("EfC");
  QCOMPARE(it->firstName(), QString("EfC"));
  ++it;
  QVERIFY(it == proxy.end());
}

void CachedRepositoryStlTableProxyTest::insertTest()
{
  auto personRepository = PersonRepository::make<MemoryPersonRepository>();
  PersonRepositoryStlTableProxy proxy;
  proxy.setRepository(personRepository);

  QCOMPARE(proxy.rowCount(), 0);
  auto it = proxy.insert( proxy.begin(), 2, buildPerson("Z") );
  QCOMPARE(proxy.rowCount(), 2);
  QVERIFY(it == proxy.begin());
  QCOMPARE(it->firstName(), QString("fZ"));
  ++it;
  QCOMPARE(it->firstName(), QString("fZ"));
}

void CachedRepositoryStlTableProxyTest::eraseTest()
{
  auto personRepository = PersonRepository::make<MemoryPersonRepository>();
  PersonRepositoryStlTableProxy proxy;
  proxy.setRepository(personRepository);
  personRepository.repository().populate({"A","B","C"});
  QVERIFY(personRepository.repository().fetchAll());

  QCOMPARE(proxy.rowCount(), 3);
  auto it = proxy.erase(std::next(proxy.begin(), 1), std::next(proxy.begin(), 2));
  QCOMPARE(proxy.rowCount(), 2);
  QVERIFY(it == proxy.end());
  it = proxy.erase(proxy.begin(), proxy.end());
  QCOMPARE(proxy.rowCount(), 0);
  QVERIFY(it == proxy.end());
}

void CachedRepositoryStlTableProxyTest::tableModelTest()
{
  auto personRepository = PersonRepository::make<MemoryPersonRepository>();
  EditPersonTableModel model;
  model.table().setRepository(personRepository);

  QCOMPARE(model.rowCount(), 0);
  personRepository.repository().populate({"A"});
  QVERIFY(personRepository.repository().fetchAll());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 1), QVariant("fA"));
  QVERIFY(setModelData(model, 0, 1, "EfA"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("EfA"));
  appendRowToModel(model);
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(setModelData(model, 1, 1, "EfB"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("EfA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("EfB"));
  removeLastRowFromModel(model);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 1), QVariant("EfA"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CachedRepositoryStlTableProxyTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
