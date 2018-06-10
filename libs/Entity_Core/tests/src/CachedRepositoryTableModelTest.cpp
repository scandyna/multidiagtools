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
#include "Mdt/Container/StlContainer.h"
#include <QSignalSpy>
#include <QVariantList>
#include <QStringList>
#include <QVector>
#include <QModelIndex>
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

using PersonDataTable = std::vector<PersonData>;

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

  const PersonDataTable & storageDataTable() const
  {
    return mMem;
  }

  bool insertRecordToStorage(const PersonData & record, QVariant & autoId) override
  {
    mMem.push_back(record);
    return true;
  }

  bool removeRecordFromStorage(int row) override
  {
    Mdt::Container::removeFromContainer(mMem, row, 1);
    return true;
  }

  bool updateRecordInStorage(int row) override
  {
    mMem[row] = constRecordAt(row);
    return true;
  }

  PersonDataTable mMem;
};

/*
 * Helpers
 */

int repositoryStorageRowCount(const PersonRepositoryHandle & handle)
{
  Q_ASSERT(!handle.isNull());

  return handle.constRepositoryImpl<MemoryPersonRepository>().storageRowCount();
}

PersonDataTable repositoryStorageDataTable(const PersonRepositoryHandle & handle)
{
  Q_ASSERT(!handle.isNull());

  return  handle.constRepositoryImpl<MemoryPersonRepository>().storageDataTable();
}

QStringList repositoryStorageToFirstNameList(const PersonRepositoryHandle & handle)
{
  Q_ASSERT(!handle.isNull());

  QStringList fnList;

  const auto dataTable = repositoryStorageDataTable(handle);
  for(const auto & person : dataTable){
    fnList.append( person.firstName() );
  }

  return fnList;
}

void populateRepository(PersonRepositoryHandle & handle, const QStringList & baseNames)
{
  Q_ASSERT(!handle.isNull());

  handle.repositoryImpl<MemoryPersonRepository>().populate(baseNames);
}

/*
 * Tests
 */

void CachedRepositoryTableModelTest::sandbox()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  populateRepository(personRepositoryHandle, {"A","B"});
  model.setRepository(personRepositoryHandle);
  QCOMPARE(model.rowCount(), 0);
  QVERIFY(personRepositoryHandle.repository().fetchAll());
  QCOMPARE(model.rowCount(), 2);
  
  /// \todo Call fetch all after setup and check that the model has rceived notification
}

void CachedRepositoryTableModelTest::initTestCase()
{
  qRegisterMetaType<Qt::Orientation>();
}

void CachedRepositoryTableModelTest::setFetchedRepositoryTest()
{
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  populateRepository(personRepositoryHandle, {"A","B","C"});
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
  populateRepository(personRepositoryHandle, {"A","B","C"});

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
  populateRepository(personRepositoryHandle, {"A","B","C"});
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

void CachedRepositoryTableModelTest::insertRowsAndSubmitTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  model.setRepository(personRepositoryHandle);

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);

  /*
   * We not want submit() to submit changes
   * See API doc for the reasons
   */
  QVERIFY(model.submit());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(repositoryStorageRowCount(personRepositoryHandle), 0);

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(repositoryStorageRowCount(personRepositoryHandle), 1);
}

void CachedRepositoryTableModelTest::insertRowsAndSubmitHeaderTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  model.setRepository(personRepositoryHandle);
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());
  QVariantList arguments;

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * We not want submit() to submit changes
   * See API doc for the reasons
   */
  QVERIFY(model.submit());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(headerDataChangedSpy.count(), 0);

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
}

void CachedRepositoryTableModelTest::insertRowsAndSubmitResizeSignalsTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  model.setRepository(personRepositoryHandle);
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &EditPersonTableModel::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &EditPersonTableModel::rowsInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QVariantList arguments;

  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
}

void CachedRepositoryTableModelTest::insertRowsFromRepositoryAndSubmitTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  model.setRepository(personRepositoryHandle);
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());
  QVariantList arguments;

  QCOMPARE(model.rowCount(), 0);
  personRepositoryHandle.repository().insertRecords(0, 1, PersonData());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(repositoryStorageRowCount(personRepositoryHandle), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  QVERIFY(personRepositoryHandle.repository().submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
  QCOMPARE(repositoryStorageRowCount(personRepositoryHandle), 1);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
}

void CachedRepositoryTableModelTest::insertRowsFromRepositoryAndSubmitResizeSignalsTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  model.setRepository(personRepositoryHandle);
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &EditPersonTableModel::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &EditPersonTableModel::rowsInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QVariantList arguments;

  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);

  personRepositoryHandle.repository().insertRecords(0, 1, PersonData());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
}

void CachedRepositoryTableModelTest::removeRowsAndSubmitTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  model.setRepository(personRepositoryHandle);

  populateRepository(personRepositoryHandle, {"A","B","C","D","E"});
  QVERIFY(personRepositoryHandle.repository().fetchAll());
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(getModelData(model, 0, 1), QVariant("fA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("fB"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("fC"));
  QCOMPARE(getModelData(model, 3, 1), QVariant("fD"));
  QCOMPARE(getModelData(model, 4, 1), QVariant("fE"));
  QCOMPARE(repositoryStorageRowCount(personRepositoryHandle), 5);
  QCOMPARE(repositoryStorageToFirstNameList(personRepositoryHandle), QStringList({"fA","fB","fC","fD","fE"}));

  /*
   * Remove 1 row
   */

  QVERIFY(removeLastRowFromModel(model));
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(getModelData(model, 0, 1), QVariant("fA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("fB"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("fC"));
  QCOMPARE(getModelData(model, 3, 1), QVariant("fD"));
  QCOMPARE(getModelData(model, 4, 1), QVariant("fE"));
  QCOMPARE(repositoryStorageRowCount(personRepositoryHandle), 5);
  QCOMPARE(repositoryStorageToFirstNameList(personRepositoryHandle), QStringList({"fA","fB","fC","fD","fE"}));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(getModelData(model, 0, 1), QVariant("fA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("fB"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("fC"));
  QCOMPARE(getModelData(model, 3, 1), QVariant("fD"));
  QCOMPARE(repositoryStorageRowCount(personRepositoryHandle), 4);
  QCOMPARE(repositoryStorageToFirstNameList(personRepositoryHandle), QStringList({"fA","fB","fC","fD"}));

  /*
   * Remove 2 non consecutive rows
   */

  QVERIFY(model.removeRow(0));
  QVERIFY(model.removeRow(2));
  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(getModelData(model, 0, 1), QVariant("fA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("fB"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("fC"));
  QCOMPARE(getModelData(model, 3, 1), QVariant("fD"));
  QCOMPARE(repositoryStorageRowCount(personRepositoryHandle), 4);
  QCOMPARE(repositoryStorageToFirstNameList(personRepositoryHandle), QStringList({"fA","fB","fC","fD"}));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("fB"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("fD"));
  QCOMPARE(repositoryStorageRowCount(personRepositoryHandle), 2);
  QCOMPARE(repositoryStorageToFirstNameList(personRepositoryHandle), QStringList({"fB","fD"}));
}

void CachedRepositoryTableModelTest::removeRowsAndSubmitHeaderTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  model.setRepository(personRepositoryHandle);
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());
  QVariantList arguments;

  populateRepository(personRepositoryHandle, {"A","B","C"});
  QVERIFY(personRepositoryHandle.repository().fetchAll());
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("2"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("3"));

  QVERIFY(removeLastRowFromModel(model));
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(2)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("2"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("2"));
}

void CachedRepositoryTableModelTest::removeRowsAndSubmitResizeSignalsTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  model.setRepository(personRepositoryHandle);
  QVariantList arguments;
  QSignalSpy rowsAboutToBeRemovedSpy(&model, &EditPersonTableModel::rowsAboutToBeRemoved);
  QSignalSpy rowsRemovedSpy(&model, &EditPersonTableModel::rowsRemoved);
  QVERIFY(rowsAboutToBeRemovedSpy.isValid());
  QVERIFY(rowsRemovedSpy.isValid());

  populateRepository(personRepositoryHandle, {"A","B","C","D","E"});
  QVERIFY(personRepositoryHandle.repository().fetchAll());
  QCOMPARE(model.rowCount(), 5);
  rowsAboutToBeRemovedSpy.clear();
  rowsRemovedSpy.clear();

  /*
   * Remove 1 row
   */

  QVERIFY(removeLastRowFromModel(model));
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(4));             // first
  QCOMPARE(arguments.at(2), QVariant(4));             // last
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(4));             // first
  QCOMPARE(arguments.at(2), QVariant(4));             // last

  /*
   * Remove 2 non consecutive rows
   */

  QVERIFY(model.removeRow(0));
  QVERIFY(model.removeRow(2));
  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(2));             // first
  QCOMPARE(arguments.at(2), QVariant(2));             // last
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(rowsRemovedSpy.count(), 2);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(2));             // first
  QCOMPARE(arguments.at(2), QVariant(2));             // last
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
}

void CachedRepositoryTableModelTest::removeRowsFromRepositoryAndSubmitTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  model.setRepository(personRepositoryHandle);
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());
  QVariantList arguments;

  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
  headerDataChangedSpy.clear();
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));

  QVERIFY(removeLastRowFromModel(model));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
}

void CachedRepositoryTableModelTest::insertRecordsRemoveAndSubmitHeaderTest()
{

  QFAIL("Not complete");
}

void CachedRepositoryTableModelTest::setDataAndSubmitTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  populateRepository(personRepositoryHandle, {"A"});
  QVERIFY(personRepositoryHandle.repository().fetchAll());
  model.setRepository(personRepositoryHandle);

  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(repositoryStorageToFirstNameList(personRepositoryHandle), QStringList({"fA"}));
  QCOMPARE(getModelData(model, 0, 1), QVariant("fA"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));

  QVERIFY(setModelData(model, 0, 1, "E"));
  QCOMPARE(repositoryStorageToFirstNameList(personRepositoryHandle), QStringList({"fA"}));
  QCOMPARE(getModelData(model, 0, 1), QVariant("E"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("e"));

  QVERIFY(model.submitChanges());
  QCOMPARE(repositoryStorageToFirstNameList(personRepositoryHandle), QStringList({"E"}));
  QCOMPARE(getModelData(model, 0, 1), QVariant("E"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
}

void CachedRepositoryTableModelTest::setDataAndSubmitSignalTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  populateRepository(personRepositoryHandle, {"A"});
  QVERIFY(personRepositoryHandle.repository().fetchAll());
  model.setRepository(personRepositoryHandle);
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &EditPersonTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  QVERIFY(setModelData(model, 0, 1, "E"));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 0);
  QCOMPARE(topLeft.column(), 0);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 0);
  QCOMPARE(bottomRight.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last

  QVERIFY(model.submitChanges());
  QVERIFY(personRepositoryHandle.repository().submitChanges());
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
}

void CachedRepositoryTableModelTest::setDataFromRepositoryAndSubmitTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  populateRepository(personRepositoryHandle, {"A"});
  QVERIFY(personRepositoryHandle.repository().fetchAll());
  model.setRepository(personRepositoryHandle);

  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(repositoryStorageToFirstNameList(personRepositoryHandle), QStringList({"fA"}));
  QCOMPARE(getModelData(model, 0, 1), QVariant("fA"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));

  personRepositoryHandle.repository().setData(0, 1, "E");
  QCOMPARE(repositoryStorageToFirstNameList(personRepositoryHandle), QStringList({"fA"}));
  QCOMPARE(getModelData(model, 0, 1), QVariant("E"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("e"));


  QVERIFY(personRepositoryHandle.repository().submitChanges());
  QCOMPARE(repositoryStorageToFirstNameList(personRepositoryHandle), QStringList({"E"}));
  QCOMPARE(getModelData(model, 0, 1), QVariant("E"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
}

void CachedRepositoryTableModelTest::setDataFromRepositoryAndSubmitSignalTest()
{
  EditPersonTableModel model;
  auto personRepositoryHandle = PersonRepositoryHandle::make<MemoryPersonRepository>();
  populateRepository(personRepositoryHandle, {"A"});
  QVERIFY(personRepositoryHandle.repository().fetchAll());
  model.setRepository(personRepositoryHandle);
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &EditPersonTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  personRepositoryHandle.repository().setData(0, 1, "E");
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 0);
  QCOMPARE(topLeft.column(), 0);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 0);
  QCOMPARE(bottomRight.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last

  QVERIFY(personRepositoryHandle.repository().submitChanges());
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
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
