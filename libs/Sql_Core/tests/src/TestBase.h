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

#include "Mdt/Sql/ConnectionParameters.h"

#include "Mdt/Sql/SQLiteConnectionParameters.h"
#include "Mdt/Sql/Connection.h"
#include "Mdt/CoreApplication.h"
#include <QTemporaryFile>
#include <QSqlDatabase>
#include <QObject>
#include <QtTest/QtTest>
#include <Qt>
#include <memory>

struct Client
{
  int id = 0;
  QString name;
};

class TestBase : public QObject
{
 Q_OBJECT

 public:

  explicit TestBase();
  ~TestBase();

 protected:

  bool initDatabaseTemporaryFile();
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

  Mdt::Sql::SQLiteConnectionParameters sqliteConnectionParameters() const
  {
    return mConnectionParameters;
  }

  /// bool initDatabaseSqliteAsync();

  bool createClientTable();
  bool insertClient(int id, const QString & name);
  Client getClient(int id);
  bool cleanupClientTable();

  bool createTestSchema();

 private:

  QString mConnectionName;
  Mdt::Sql::SQLiteConnectionParameters mConnectionParameters;
  QTemporaryFile mTempFile;  // We keep it as member, so file is destroyed automatically
};

#endif // #ifndef MDT_SQL_TEST_BASE_H
