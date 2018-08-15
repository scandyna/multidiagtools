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

void PersonCacheCommonImpl::appendRecordToStorage(Person record)
{
  record.id = getNextId();
  mStorage.push_back(record);
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
    appendRecordFromBackend(record);
  }
  return true;
}

bool EditPersonCache::fetchRecords(int count)
{
  const auto records = mImpl.getRecordsFromStorage(count);
  for(const auto & record : records){
    appendRecordFromBackend(record);
  }
  return true;
}

bool EditPersonCache::addRecordToBackend(const VariantRecord& record)
{
  Q_ASSERT(record.columnCount() == columnCount());

  Person person;
  person.name = record.value(1).toString();
  appendRecordToStorage(person);

  return true;
}

template<typename Cache>
void populatePersonStorageImpl(Cache & cache, const QStringList & names)
{
  cache.clearStorage();
  for(const auto & name : names){
    Person record;
    record.name = name;
    cache.appendRecordToStorage(record);
  }
}

void populatePersonStorage(PersonCache & pc, const QStringList & names)
{
  populatePersonStorageImpl(pc, names);
}

void populatePersonStorage(EditPersonCache & pc, const QStringList & names)
{
  populatePersonStorageImpl(pc, names);
}
