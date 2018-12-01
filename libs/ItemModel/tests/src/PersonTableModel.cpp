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
#include "PersonTableModel.h"
#include <algorithm>

using namespace Mdt::ItemModel;
using Mdt::Container::VariantRecord;
using Mdt::Container::TableCacheRowTask;
using Mdt::Container::TableCacheTask;

/*
 * Helpers
 */

VariantRecord makeVariantRecord(const Person & person)
{
  return VariantRecord{person.id, person.name};
}

Person makePerson(const VariantRecord & record)
{
  Q_ASSERT(record.columnCount() == 2);

  return Person{record.value(0).toInt(), record.value(1).toString()};
}

/*
 * PersonTableModelCommonImpl
 */

void PersonTableModelCommonImpl::clearStorage()
{
  mStorage.clear();
}

std::vector<VariantRecord> PersonTableModelCommonImpl::getRecordsFromStorage(int count)
{
  std::vector<VariantRecord> recordList;
  const auto personList = mStorage.getCountPersons(count);

  for(const Person & person : personList){
    VariantRecord record({person.id, person.name});
    recordList.push_back(record);
  }

  return recordList;
}

/*
 * ListPersonTableModel
 */

ListPersonTableModel::ListPersonTableModel(QObject* parent)
 : BaseClass(parent)
{
}

bool ListPersonTableModel::fetchRecords(int count)
{
  const auto records = mImpl.getRecordsFromStorage(count);
  for(const auto & record : records){
    fromBackendAppendRecord(record);
  }
  return true;
}

bool ListPersonTableModel::fetchRecordFromBackend(const Mdt::Container::TableCacheRowTask& rowTask)
{
  mFetchingPerson.taskId = rowTask.taskId();
  mFetchingPerson.personId = index(rowTask.row(), 0).data().toInt();

  return true;
}

void ListPersonTableModel::fetchRecordFromBackendSucceeded()
{
  TableCacheTask task(mFetchingPerson.taskId);
  const auto person = mImpl.getPersonFromStorage(mFetchingPerson.personId);
  const auto record = makeVariantRecord(person);

  taskSucceeded(task, record);
}

void ListPersonTableModel::fetchRecordFromBackendFailed()
{
  TableCacheTask task(mFetchingPerson.taskId);
  const auto error = mdtErrorNew("fetchRecordFromBackend failed", Mdt::Error::Critical, "Test");

  taskFailed(task, error);
}

void ListPersonTableModel::updateStoragePersonNameAt(int row, const QString & name)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  Person person = makePerson(record(row));
  Q_ASSERT(person.id > 0);
  person.name = name;
  mImpl.updatePersonInStorage(person);
}

/*
 * EditPersonTableModel
 */

EditPersonTableModel::EditPersonTableModel(QObject* parent)
 : BaseClass(parent)
{
}

bool EditPersonTableModel::fetchRecords(int count)
{
  const auto records = mImpl.getRecordsFromStorage(count);
  for(const auto & record : records){
    fromBackendAppendRecord(record);
  }
  return true;
}

bool EditPersonTableModel::fetchRecordFromBackend(const Mdt::Container::TableCacheRowTask& rowTask)
{
  mFetchingPerson.taskId = rowTask.taskId();
  mFetchingPerson.personId = index(rowTask.row(), 0).data().toInt();

  return true;
}

void EditPersonTableModel::fetchRecordFromBackendSucceeded()
{
  TableCacheTask task(mFetchingPerson.taskId);
  const auto person = mImpl.getPersonFromStorage(mFetchingPerson.personId);
  const auto record = makeVariantRecord(person);

  taskSucceeded(task, record);
}

void EditPersonTableModel::fetchRecordFromBackendFailed()
{
  TableCacheTask task(mFetchingPerson.taskId);
  const auto error = mdtErrorNew("fetchRecordFromBackend failed", Mdt::Error::Critical, "Test");

  taskFailed(task, error);
}

bool EditPersonTableModel::addRecordToBackend(const TableCacheRowTransaction & rowTransaction)
{
  Q_ASSERT(rowTransaction.row() >= 0);
  Q_ASSERT(rowTransaction.row() < rowCount());
  Q_ASSERT(!rowTransaction.isNull());

  mPendingTransactionId = rowTransaction.transactionId();
  mPendingPerson = makePerson( record(rowTransaction.row()) );

  return true;
}

void EditPersonTableModel::addRecordToBackendSucceeded()
{
  const int id = mImpl.addPersonToStorage(mPendingPerson);
  const VariantRecord record = makeVariantRecord( mImpl.getPersonFromStorage(id) );
  const TableCacheTransaction transaction(mPendingTransactionId);

  transactionSucceeded(transaction, record);
}

void EditPersonTableModel::addRecordToBackendFailed()
{
  const TableCacheTransaction transaction(mPendingTransactionId);
  const auto error = mdtErrorNewQ("Fail for test", Mdt::Error::Critical, this);

  transactionFailed(transaction, error);
}

bool EditPersonTableModel::updateRecordInBackend(const TableCacheRowTransaction & rowTransaction)
{
  Q_ASSERT(rowTransaction.row() >= 0);
  Q_ASSERT(rowTransaction.row() < rowCount());
  Q_ASSERT(!rowTransaction.isNull());

  mPendingTransactionId = rowTransaction.transactionId();
  mPendingPerson = makePerson( record(rowTransaction.row()) );

  return true;
}

void EditPersonTableModel::updateRecordInBackendSucceeded()
{
  mImpl.updatePersonInStorage(mPendingPerson);
  const VariantRecord record = makeVariantRecord(mPendingPerson);
  const TableCacheTransaction transaction(mPendingTransactionId);

  transactionSucceeded(transaction, record);
}

void EditPersonTableModel::updateStoragePersonNameAt(int row, const QString & name)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  Person person = makePerson(record(row));
  Q_ASSERT(person.id > 0);
  person.name = name;
  mImpl.updatePersonInStorage(person);
}

/*
 * Population functions
 */

void populatePersonStorage(ListPersonTableModel & model, const std::initializer_list<Person> & list)
{
  model.populatePersonStorage(list);
}

void populatePersonStorageByNames(ListPersonTableModel & model, const QStringList& names)
{
  model.populatePersonStorageByNames(names);
//   populatePersonStorageImpl(model, names);
}

void populatePersonStorage(EditPersonTableModel & model, const std::initializer_list<Person> & list)
{
  model.populatePersonStorage(list);
}

void populatePersonStorageByNames(EditPersonTableModel& model, const QStringList& names)
{
  model.populatePersonStorageByNames(names);
//   populatePersonStorageImpl(model, names);
}
