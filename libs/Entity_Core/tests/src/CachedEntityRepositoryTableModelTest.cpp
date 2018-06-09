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
#include "CachedEntityRepositoryTableModelTest.h"
#include "Mdt/Entity/CachedEntityRepositoryTableModel.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/AbstractCachedEntityRepository.h"
#include "Mdt/Entity/RepositoryHandle.h"
#include "Mdt/TestLib/ItemModel.h"
#include "Mdt/Container/StlContainer.h"
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

using PersonRepositoryHandle = Mdt::Entity::RepositoryHandle<AbstractPersonRepository>;

/*
 * Table model
 */

class EditPersonTableModel : public Mdt::Entity::CachedEntityRepositoryTableModel<AbstractPersonRepository>
{
 public:

  using ParentClass = Mdt::Entity::CachedRepositoryTableModel<AbstractPersonRepository>;
//   using ParentClass::ParentClass;

  EditPersonTableModel(QObject *parent = nullptr)
   : CachedEntityRepositoryTableModel(parent)
  {
    setModelEditable(true);
  }

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

  bool insertRecordToStorage(const PersonData & record, QVariant & autoId) override
  {
    mMem.push_back(record);
    return true;
  }

  bool updateRecordInStorage(int row) override
  {
    mMem[row] = constRecordAt(row);
    return true;
  }

  bool removeRecordFromStorage(int row) override
  {
    Mdt::Container::removeFromContainer(mMem, row, 1);
    return true;
  }

  std::vector<PersonData> mMem;
};

/*
 * Tests
 */

void CachedEntityRepositoryTableModelTest::defTest()
{
  EditPersonTableModel model;
  QCOMPARE(model.def().entityName(), QString("Person"));
}

void CachedEntityRepositoryTableModelTest::fieldIndexTest()
{
  EditPersonTableModel model;
  QCOMPARE(model.fieldIndex(model.def().id()), 0);
  QCOMPARE(model.fieldIndex(model.def().firstName()), 1);
}

void CachedEntityRepositoryTableModelTest::horizontalHeaderTest()
{
  EditPersonTableModel model;
  QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant("id"));
  QVERIFY(model.headerData(0, Qt::Horizontal, Qt::DecorationRole).isNull());
  QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant("firstName"));
}

void CachedEntityRepositoryTableModelTest::setDataTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  model.setRepository(personRepositoryHandle);

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 2);

  const auto flags = getModelFlags(model, 0, 0);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));

  QVERIFY(setModelData(model, 0, 0, 15));
  QVERIFY(setModelData(model, 0, 1, "A"));
  QCOMPARE(getModelData(model, 0, 0), QVariant(15));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
}

void CachedEntityRepositoryTableModelTest::qtModelTest()
{
  QFAIL("Not implemented");
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CachedEntityRepositoryTableModelTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
