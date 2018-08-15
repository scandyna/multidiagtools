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
#ifndef PERSON_CACHE_H
#define PERSON_CACHE_H

#include "Mdt/Entity/AbstractReadOnlyCache.h"
#include "Mdt/Entity/AbstractEditableCache.h"
#include <QStringList>
#include <vector>

struct Person
{
  int id;
  QString name;
};

class PersonCacheCommonImpl
{
 public:

  int columnCount() const noexcept
  {
    return 2;
  }

  QString horizontalHeaderName(int column) const
  {
    switch(column){
      case 0:
        return "id";
      case 1:
        return "name";
    }
    return QString();
  }

  int storageRowCount() const
  {
    return mStorage.size();
  }

  void appendRecordToStorage(Person record);

  void clearStorage();

  std::vector<Mdt::Entity::VariantRecord> getRecordsFromStorage(int count);
  int getNextId() const;

 private:

  std::vector<Person> mStorage;
};

class PersonCache : public Mdt::Entity::AbstractReadOnlyCache
{
 Q_OBJECT

 public:

  int columnCount() const noexcept override
  {
    return mImpl.columnCount();
  }

  QString horizontalHeaderName(int column) const override
  {
    return mImpl.horizontalHeaderName(column);
  }

  int storageRowCount() const
  {
    return mImpl.storageRowCount();
  }

  void appendRecordToStorage(Person record)
  {
    mImpl.appendRecordToStorage(record);
  }

  void clearStorage()
  {
    mImpl.clearStorage();
  }

 private:

  bool fetchRecords(int count) override;

  PersonCacheCommonImpl mImpl;
};

class EditPersonCache : public Mdt::Entity::AbstractEditableCache
{
 Q_OBJECT

 public:

  int columnCount() const noexcept override
  {
    return mImpl.columnCount();
  }

  QString horizontalHeaderName(int column) const override
  {
    return mImpl.horizontalHeaderName(column);
  }

  void appendRecordToStorage(Person record)
  {
    mImpl.appendRecordToStorage(record);
  }

  void clearStorage()
  {
    mImpl.clearStorage();
  }

 private:

  bool fetchRecords(int count) override;
  bool addRecordToBackend(const Mdt::Entity::VariantRecord& record) override;

  PersonCacheCommonImpl mImpl;
};

void populatePersonStorage(PersonCache & pc, const QStringList & names);
void populatePersonStorage(EditPersonCache & pc, const QStringList & names);

#endif // #ifndef PERSON_CACHE_H
