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
#ifndef MDT_RAILWAY_TASK_LIST_SQL_TEST_BASE_H
#define MDT_RAILWAY_TASK_LIST_SQL_TEST_BASE_H

#include "Mdt/Sql/ConnectionParameters.h"
#include "Mdt/CoreApplication.h"
#include <QObject>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QSqlDatabase>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QtTest/QtTest>
#include <Qt>
#include <initializer_list>


class TestBase : public QObject
{
 Q_OBJECT

 protected:

  bool initDatabaseSqlite();

  QSqlDatabase database() const
  {
    return mDatabase;
  }

  Mdt::Sql::ConnectionParameters connectionParameters() const
  {
    return mConnectionParameters;
  }

  bool createTestSchema();

 private:

  QTemporaryFile mTempFile;  // We keep it as member, so file is destroyed automatically
  QSqlDatabase mDatabase;
  Mdt::Sql::ConnectionParameters mConnectionParameters;
};

#endif // #ifndef MDT_RAILWAY_TASK_LIST_SQL_TEST_BASE_H
