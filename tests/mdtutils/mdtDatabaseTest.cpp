/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtDatabaseTest.h"
#include "mdtSqlTableModel.h"
#include "mdtDataWidgetMapper.h"
#include "mdtApplication.h"
#include <QSqlDatabase>
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QWidget>
#include <QLineEdit>

#include <QDebug>

void mdtDatabaseTest::initTestCase()
{
  QTemporaryFile tempFile;
  QSqlDatabase db;
  QSqlField fld;

  // Create a temp file that will be used for database
  QVERIFY(tempFile.open());
  tempFile.close();

  qDebug() << "Init, tmp: " << tempFile.fileName();
  // Open database
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(tempFile.fileName());
  QVERIFY(db.open());

  // Create some tables - Note: QSqlQuery must be created after db.open() was called.
  QSqlQuery q;
  QVERIFY(q.exec("CREATE TABLE 'Client' ('id' INTEGER PRIMARY KEY AUTOINCREMENT, 'first_name' VARCHAR(50) NOT NULL, 'remarks' VARCHAR(500))"));

  // Verify some attributes
  fld = db.record("Client").field("id");
  QVERIFY(fld.isAutoValue());
  fld = db.record("Client").field("first_name");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);

  // Inert some data
  QVERIFY(q.exec("INSERT INTO 'Client' ('first_name') VALUES('Andy')"));
  QVERIFY(q.exec("INSERT INTO 'Client' ('first_name', 'remarks') VALUES('Bety', 'Remark one')"));
  
  QVERIFY(q.exec("SELECT * FROM 'Client'"));
  while(q.next()){
    QVERIFY(!q.record().isEmpty());
  }


}

void mdtDatabaseTest::mdtSqlTableModelTest()
{
}

void mdtDatabaseTest::mdtDataWidgetMapperTest()
{
  mdtDataWidgetMapper mapper;
  mdtSqlTableModel model;
  QLineEdit id, first_name, remarks;
  
  // Init model
  model.setTable("Client");
  model.select();

  // Init mapper
  mapper.setModel(&model);
  mapper.addMapping(&id, 0);
  mapper.addMapping(&first_name, 1);
  mapper.addMapping(&remarks, 2);
  mapper.toFirst();

  // Check that data are available in widgets
  QVERIFY(!id.text().isEmpty());
  QVERIFY(!first_name.text().isEmpty());
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtDatabaseTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
