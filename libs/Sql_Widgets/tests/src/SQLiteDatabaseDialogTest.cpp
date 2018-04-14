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
#include "SQLiteDatabaseDialogTest.h"
#include "Mdt/Sql/SQLiteDatabaseFileDialog.h"

#include <QDebug>

using namespace Mdt::Sql;

void SQLiteDatabaseDialogTestTest::initTestCase()
{
}

void SQLiteDatabaseDialogTestTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SQLiteDatabaseDialogTestTest::constructTest()
{
  SQLiteDatabaseFileDialog dialog;
  QCOMPARE(dialog.dbOpenMode(), SQLiteDatabaseFileDialog::DbOpenExisting);

  SQLiteDatabaseFileDialog openExistingDialog(nullptr, SQLiteDatabaseFileDialog::DbOpenExisting);
  QCOMPARE(openExistingDialog.dbOpenMode(), SQLiteDatabaseFileDialog::DbOpenExisting);

  SQLiteDatabaseFileDialog createNewDialog(nullptr, SQLiteDatabaseFileDialog::DbCreateNew);
  QCOMPARE(createNewDialog.dbOpenMode(), SQLiteDatabaseFileDialog::DbCreateNew);
}

void SQLiteDatabaseDialogTestTest::setOpenModeTest()
{
  SQLiteDatabaseFileDialog dialog;
  QCOMPARE(dialog.dbOpenMode(), SQLiteDatabaseFileDialog::DbOpenExisting);
  QCOMPARE(dialog.acceptMode(), SQLiteDatabaseFileDialog::AcceptOpen);
  QCOMPARE(dialog.fileMode(), SQLiteDatabaseFileDialog::ExistingFile);

  dialog.setDbOpenMode(SQLiteDatabaseFileDialog::DbCreateNew);
  QCOMPARE(dialog.dbOpenMode(), SQLiteDatabaseFileDialog::DbCreateNew);
  QCOMPARE(dialog.acceptMode(), SQLiteDatabaseFileDialog::AcceptSave);
  QCOMPARE(dialog.fileMode(), SQLiteDatabaseFileDialog::AnyFile);

  dialog.setDbOpenMode(SQLiteDatabaseFileDialog::DbOpenExisting);
  QCOMPARE(dialog.dbOpenMode(), SQLiteDatabaseFileDialog::DbOpenExisting);
  QCOMPARE(dialog.acceptMode(), SQLiteDatabaseFileDialog::AcceptOpen);
  QCOMPARE(dialog.fileMode(), SQLiteDatabaseFileDialog::ExistingFile);
}

void SQLiteDatabaseDialogTestTest::setSuffixTest()
{
  SQLiteDatabaseFileDialog dialog;
  QCOMPARE(dialog.fileSuffix(), QString("sqlite"));

  dialog.setFileSuffix("abc", "ABC files");
  QCOMPARE(dialog.fileSuffix(), QString("abc"));
  QCOMPARE(dialog.fileSuffixLabel(), QString("ABC files"));
  QCOMPARE(dialog.nameFilters(), QStringList({"ABC files (*.abc)"}));

  dialog.setFileSuffix(".abc", "ABC files");
  QCOMPARE(dialog.fileSuffix(), QString("abc"));
  QCOMPARE(dialog.nameFilters(), QStringList({"ABC files (*.abc)"}));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SQLiteDatabaseDialogTestTest test;

  return QTest::qExec(&test, argc, argv);
}
