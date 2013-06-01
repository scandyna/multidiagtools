/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtApplication.h"

#include "mdtSqlTableModel.h"
#include "mdtDataWidgetMapper.h"
#include "mdtApplication.h"
#include "mdtSqlRelation.h"
#include <QSqlDatabase>
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QWidget>
#include <QLineEdit>
#include <QString>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QFile>

#include <QTableView>
#include <QItemSelectionModel>

#include <QDebug>

void mdtDatabaseTest::initTestCase()
{
  QTemporaryFile tempFile;
  QSqlDatabase db;
  QSqlField fld;
  QString sql;

  // Create a temp file that will be used for database
  QVERIFY(tempFile.open());
  tempFile.close();
  pvDbFileInfo.setFile(tempFile.fileName() + ".db");
  qDebug() << "Init, DB file: " << pvDbFileInfo.filePath();

  // Open database
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(pvDbFileInfo.filePath());
  QVERIFY(db.open());

  // QSqlQuery must be created after db.open() was called.
  QSqlQuery q;

  /*
   * Create Client table
   */
  sql = "CREATE TABLE 'Client' (";
  sql += "'id_PK' INTEGER PRIMARY KEY AUTOINCREMENT, ";
  sql += "'first_name' VARCHAR(50) NOT NULL, ";
  sql += "'remarks' VARCHAR(500) )";
  QVERIFY(q.exec(sql));
  // Verify some attributes
  fld = db.record("Client").field("id_PK");
  QVERIFY(fld.isAutoValue());
  fld = db.record("Client").field("first_name");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);
  // Inert some data
  QVERIFY(q.exec("INSERT INTO 'Client' ('first_name') VALUES('Andy')"));
  QVERIFY(q.exec("INSERT INTO 'Client' ('first_name', 'remarks') VALUES('Bety', 'Remark on Bety')"));
  QVERIFY(q.exec("SELECT * FROM 'Client'"));
  while(q.next()){
    QVERIFY(!q.record().isEmpty());
  }

  /*
   * Create Addresses table
   */
  sql = "CREATE TABLE 'Address' (";
  sql += "'id_PK' INTEGER PRIMARY KEY AUTOINCREMENT, ";
  sql += "'street_name' VARCHAR(50) NOT NULL, ";
  sql += "'street_number' INTEGER NOT NULL, ";
  sql += "'id_client_FK' INTEGER NOT NULL)";
  QVERIFY(q.exec(sql));
  // Verify some attributes
  fld = db.record("Address").field("id_PK");
  QVERIFY(fld.isAutoValue());
  fld = db.record("Address").field("street_name");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);
  fld = db.record("Address").field("street_number");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);
  fld = db.record("Address").field("id_client_FK");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);

  // Inert some data
  QVERIFY(q.exec("INSERT INTO 'Address' ('street_name', 'street_number', 'id_client_FK') VALUES('Andy street 1', 56 , 1)"));
  QVERIFY(q.exec("INSERT INTO 'Address' ('street_name', 'street_number', 'id_client_FK') VALUES('Andy street 2', 87 , 1)"));
  QVERIFY(q.exec("INSERT INTO 'Address' ('street_name', 'street_number', 'id_client_FK') VALUES('Bety street 1', 45 , 2)"));
  QVERIFY(q.exec("SELECT * FROM 'Address'"));
  while(q.next()){
    QVERIFY(!q.record().isEmpty());
  }

}

/// \todo Make data checks - Nothing is finished !!
void mdtDatabaseTest::relationsTest()
{
  QSqlTableModel parentModel;
  QSqlTableModel childModel;
  mdtSqlRelation relation;

  // Setup parent model
  parentModel.setTable("Client");
  parentModel.select();
  // Setup child model
  childModel.setTable("Address");
  childModel.select();
  // Setup relation
  relation.setParentModel(&parentModel);
  relation.setChildModel(&childModel);
  QVERIFY(relation.addRelation("id_PK", "id_client_FK"));

  // Check data of each model
  
  // Setup parent view
  QTableView pv;
  pv.setModel(&parentModel);
  QVERIFY(pv.selectionModel() != 0);
  QObject::connect(pv.selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), \
                    &relation, SLOT(setParentCurrentIndex(const QModelIndex&, const QModelIndex&)));
  // Setup child view
  QTableView cv;
  cv.setModel(&childModel);

  // Play...
  pv.show();
  cv.show();
  while(pv.isVisible()){
    QTest::qWait(1000);
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



void mdtDatabaseTest::cleanupTestCase()
{
  QFile::remove(pvDbFileInfo.filePath());
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
