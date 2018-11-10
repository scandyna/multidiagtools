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

void PersonTableModelCommonImpl::appendRecordToStorage(Person record)
{
  record.id = getNextId();
  mStorage.push_back(record);
}

void PersonTableModelCommonImpl::updateRecordInStorage(int row, const Person& record)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < storageRowCount());

  mStorage[row] = record;
}

void PersonTableModelCommonImpl::removeRecordFromStorage(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < storageRowCount());

  mStorage.erase( mStorage.begin() + row );
}

void PersonTableModelCommonImpl::clearStorage()
{
  mStorage.clear();
}

std::vector<VariantRecord> PersonTableModelCommonImpl::getRecordsFromStorage(int count)
{
  std::vector<VariantRecord> list;

  const int n = std::min(count, storageRowCount());
  for(int i = 0; i < n; ++i){
    const auto & person = mStorage[i];
    list.push_back( makeVariantRecord(person) );
  }

  return list;
}

int PersonTableModelCommonImpl::getNextId() const
{
  const auto cmp = [](const Person & a, const Person & b){
    return a.id < b.id;
  };
  const auto it = std::max_element(mStorage.cbegin(), mStorage.cend(), cmp);
  if(it == mStorage.cend()){
    return 1;
  }
  return it->id + 1;
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
  appendRecordToStorage(mPendingPerson);
  const VariantRecord record = makeVariantRecord( mImpl.lastRecordFromStorage() );

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

}

void EditPersonTableModel::updateRecordInBackendSucceeded()
{
}

/*
 * Population functions
 */

template<typename Model>
void populatePersonStorageImpl(Model & model, const QStringList & names)
{
  model.clearStorage();
  for(const auto & name : names){
    Person record;
    record.name = name;
    model.appendRecordToStorage(record);
  }
}

void populatePersonStorage(ListPersonTableModel & model, const QStringList& names)
{
  populatePersonStorageImpl(model, names);
}

void populatePersonStorage(EditPersonTableModel& model, const QStringList& names)
{
  populatePersonStorageImpl(model, names);
}
