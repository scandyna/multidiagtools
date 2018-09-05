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
#include "PersonCache.h"
#include <algorithm>

using namespace Mdt::Entity;
using Mdt::Container::VariantRecord;

void PersonCacheCommonImpl::appendRecordToStorage(Person record)
{
  record.id = getNextId();
  mStorage.push_back(record);
}

void PersonCacheCommonImpl::updateRecordInStorage(int row, const Person& record)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < storageRowCount());

  mStorage[row] = record;
}

void PersonCacheCommonImpl::removeRecordFromStorage(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < storageRowCount());

  mStorage.erase( mStorage.begin() + row );
}

void PersonCacheCommonImpl::clearStorage()
{
  mStorage.clear();
}

std::vector<VariantRecord> PersonCacheCommonImpl::getRecordsFromStorage(int count)
{
  std::vector<VariantRecord> list;
  const int n = std::min(count, storageRowCount());
  for(int i = 0; i < n; ++i){
    const auto & person = mStorage[i];
    VariantRecord record(2);
    record.setValue(0, person.id);
    record.setValue(1, person.name);
    list.push_back(record);
  }

  return list;
}

int PersonCacheCommonImpl::getNextId() const
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

bool PersonCache::fetchRecords(int count)
{
  const auto records = mImpl.getRecordsFromStorage(count);
  for(const auto & record : records){
    fromBackendAppendRecord(record);
  }
  return true;
}

bool EditPersonCache::fetchRecords(int count)
{
  const auto records = mImpl.getRecordsFromStorage(count);
  for(const auto & record : records){
    fromBackendAppendRecord(record);
  }
  return true;
}

bool EditPersonCache::addRecordToBackend(int row)
{
  Person person;
  person.name = data(row, 1).toString();
  appendRecordToStorage(person);

  person = mImpl.recordFromStorage(row);
  VariantRecord record(2);
  record.setValue(0, person.id);
  record.setValue(1, person.name);
  fromBackendSetRecord(row, record);

  return true;
}

bool EditPersonCache::updateRecordInBackend(int row)
{
  Person person;

  person.id = data(row, 0).toInt();
  person.name = data(row, 1).toString();

  mImpl.updateRecordInStorage(row, person);
  VariantRecord record(2);
  record.setValue(0, person.id);
  record.setValue(1, person.name);
  fromBackendSetRecord(row, record);

  return true;
}

bool EditPersonCache::removeRecordFromBackend(int row)
{
  mImpl.removeRecordFromStorage(row);

  return true;
}


void populatePersonStorage(PersonCache & pc, const QStringList & names)
{
  populatePersonStorageImpl(pc, names);
}

void populatePersonStorage(EditPersonCache & pc, const QStringList & names)
{
  populatePersonStorageImpl(pc, names);
}
