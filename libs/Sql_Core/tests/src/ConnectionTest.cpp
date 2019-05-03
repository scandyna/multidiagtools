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
#include "ConnectionTest.h"
#include "Mdt/Sql/Connection.h"
#include <QSqlDatabase>
#include <QTemporaryFile>

using namespace Mdt::Sql;

/*
 * Tests
 */

void ConnectionTest::constructAndGetTest()
{
  Connection cnxA("A");
  QCOMPARE(cnxA.name(), QLatin1String("A"));
}

void ConnectionTest::generateConnectionNameTest()
{
  QFETCH(QStringList, existingNames);
  QFETCH(QString, namePrefix);
  QFETCH(QString, expectedName);

  QCOMPARE(Connection::generateConnectionName(existingNames, namePrefix), expectedName);
}

void ConnectionTest::generateConnectionNameTest_data()
{
  QTest::addColumn<QStringList>("existingNames");
  QTest::addColumn<QString>("namePrefix");
  QTest::addColumn<QString>("expectedName");

  QTest::newRow("1") << QStringList{}
                     << "TestConnection"
                     << "TestConnection_1";

  QTest::newRow("1") << QStringList{"a"}
                     << "TestConnection"
                     << "TestConnection_1";

  QTest::newRow("2") << QStringList{"TestConnection_1"}
                     << "TestConnection"
                     << "TestConnection_2";

  QTest::newRow("3") << QStringList{"TestConnection_1","TestConnection_2"}
                     << "TestConnection"
                     << "TestConnection_3";
}

void ConnectionTest::addOpenCloseRemoveTest()
{
  Connection cnnA("A");

  QVERIFY(!QSqlDatabase::connectionNames().contains("A"));
  QVERIFY(QSqlDatabase::addDatabase("QSQLITE", cnnA.name()).isValid());
  QVERIFY(QSqlDatabase::connectionNames().contains("A"));

  QTemporaryFile file;
  QVERIFY(file.open());
  file.close();
  {
    auto db = QSqlDatabase::database(cnnA.name());
    QVERIFY(db.isValid());
    db.setDatabaseName(file.fileName());
    QVERIFY(db.open());
  }

  QVERIFY(QSqlDatabase::database(cnnA.name(), false).isOpen());
  Connection::close(cnnA);
  QVERIFY(!QSqlDatabase::database(cnnA.name(), false).isOpen());

  QVERIFY(QSqlDatabase::connectionNames().contains("A"));
  Connection::remove(cnnA);
  QVERIFY(!QSqlDatabase::connectionNames().contains("A"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ConnectionTest test;

  return QTest::qExec(&test, argc, argv);
}
