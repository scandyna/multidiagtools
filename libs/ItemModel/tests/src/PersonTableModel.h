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
#ifndef PERSON_TABLE_MODEL_H
#define PERSON_TABLE_MODEL_H

#include "Mdt/ItemModel/AbstractReadOnlyCachedTableModel.h"
#include "Mdt/ItemModel/AbstractEditableCachedTableModel.h"
#include "Mdt/Container/VariantRecord.h"
#include <QString>
#include <QStringList>
#include <vector>

using namespace Mdt::Container;

struct Person
{
  int id;
  QString name;
};

class PersonTableModelCommonImpl
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

  QString storageNameAt(int row) const
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < storageRowCount());

    return mStorage[row].name;
  }

  const Person & recordFromStorage(int row) const
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < storageRowCount());

    return mStorage[row];
  }

  void appendRecordToStorage(Person record);
  void updateRecordInStorage(int row, const Person & record);
  void removeRecordFromStorage(int row);

  void clearStorage();

  std::vector<Mdt::Container::VariantRecord> getRecordsFromStorage(int count);
  int getNextId() const;

 private:

  std::vector<Person> mStorage;
};

class ListPersonTableModel : public Mdt::ItemModel::AbstractReadOnlyCachedTableModel
{
 Q_OBJECT

 using BaseClass = AbstractReadOnlyCachedTableModel;

 public:

  ListPersonTableModel(QObject *parent = nullptr);

  int columnCount(const QModelIndex & parent = QModelIndex()) const override
  {
    Q_UNUSED(parent)
    return mImpl.columnCount();
  }

  void fromBackendInsertRecords(int row, int count, const Mdt::Container::VariantRecord & record)
  {
    BaseClass::fromBackendInsertRecords(row, count, record);
  }

  void fromBackendAppendRecord(const Mdt::Container::VariantRecord & record)
  {
    BaseClass::fromBackendAppendRecord(record);
  }

  void fromBackendSetData(int row, int column, const QVariant & data)
  {
    BaseClass::fromBackendSetData(row, column, data);
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

  QVariant horizontalHeaderDisplayRoleData(int column) const override
  {
    return mImpl.horizontalHeaderName(column);
  }

  bool fetchRecords(int count) override;

  PersonTableModelCommonImpl mImpl;
};


class EditPersonTableModel : public Mdt::ItemModel::AbstractEditableCachedTableModel
{
 Q_OBJECT

 using BaseClass = AbstractEditableCachedTableModel;

 public:

  EditPersonTableModel(QObject *parent = nullptr);

  int columnCount(const QModelIndex & parent = QModelIndex()) const override
  {
    Q_UNUSED(parent)
    return mImpl.columnCount();
  }

  void fromBackendInsertRecords(int row, int count, const Mdt::Container::VariantRecord & record)
  {
    BaseClass::fromBackendInsertRecords(row, count, record);
  }

  void fromBackendAppendRecord(const Mdt::Container::VariantRecord & record)
  {
    BaseClass::fromBackendAppendRecord(record);
  }

  void fromBackendSetData(int row, int column, const QVariant & data)
  {
    BaseClass::fromBackendSetData(row, column, data);
  }

  bool addRecordToBackend(const Mdt::Container::TableCacheRowTransaction & rowTransaction) override;

  void addRecordToBackendSucceeded();

  void updateRecordInBackendSucceeded();

  int storageRowCount() const
  {
    return mImpl.storageRowCount();
  }

  QString storageNameAt(int row) const
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < storageRowCount());

    return mImpl.storageNameAt(row);
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

  QVariant horizontalHeaderDisplayRoleData(int column) const override
  {
    return mImpl.horizontalHeaderName(column);
  }

  bool fetchRecords(int count) override;

  PersonTableModelCommonImpl mImpl;
  int mPendingTransactionId = 0;
  Person mPendingPerson;
};

void populatePersonStorage(ListPersonTableModel & model, const QStringList & names);
void populatePersonStorage(EditPersonTableModel & model, const QStringList & names);

#endif // #ifndef PERSON_TABLE_MODEL_H
