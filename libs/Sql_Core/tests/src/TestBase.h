/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_SQL_TEST_BASE_H
#define MDT_SQL_TEST_BASE_H

#include "Mdt/Sql/SQLiteConnectionParameters.h"
#include "Mdt/Sql/Connection.h"
#include "Mdt/Sql/AsyncQueryConnection.h"
#include "Mdt/Container/VariantRecord.h"
#include "Mdt/CoreApplication.h"
#include <QTemporaryFile>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlField>
#include <QObject>
#include <QMetaType>
#include <QtTest/QtTest>
#include <Qt>
#include <memory>

struct Client
{
  int id = 0;
  QString name;
};
Q_DECLARE_METATYPE(Client)

class TestBase : public QObject
{
 Q_OBJECT

 public:

  explicit TestBase();
  ~TestBase();

 protected:

  bool initDatabaseTemporaryFile();

  Mdt::Sql::SQLiteConnectionParameters sqliteConnectionParameters() const
  {
    return mConnectionParameters;
  }

  bool initDatabaseSqlite();

  bool openDatabaseIfNot();
  void closeDatabase();

  Mdt::Sql::Connection connection() const
  {
    Q_ASSERT(!mConnectionName.isEmpty());
    return Mdt::Sql::Connection(mConnectionName);
  }

  QSqlDatabase database() const
  {
    Q_ASSERT(!mConnectionName.isEmpty());
    return connection().database();
  }

  bool isDatabaseOpen() const
  {
    if(mConnectionName.isEmpty()){
      return false;
    }
    return database().isOpen();
  }

  bool initDatabaseSqliteAsync();

  bool openDatabaseAsyncIfNot();
  void closeDatabaseAsync();

  std::shared_ptr<Mdt::Sql::AsyncQueryConnection> asyncQueryConnection()
  {
    Q_ASSERT(mAsyncQueryConnection.get() != nullptr);

    return mAsyncQueryConnection;
  }

  bool createClientTable();
  bool insertClient(int id, const QString & name);
  bool clientExists(int id);
  Client getClient(int id);
  bool cleanupClientTable();

  bool createTestSchema();

 private:

  QTemporaryFile mTempFile;  // We keep it as member, so file is destroyed automatically
  Mdt::Sql::SQLiteConnectionParameters mConnectionParameters;
  QString mConnectionName;
  std::shared_ptr<Mdt::Sql::AsyncQueryConnection> mAsyncQueryConnection;
};

/*
 * Helper plain functions
 */

QSqlField sqlFieldFromVariantValue(int index, const QVariant & value);

QSqlRecord sqlRecordFromVariantRecord(const Mdt::Container::VariantRecord & record);

#endif // #ifndef MDT_SQL_TEST_BASE_H
