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
#include "mdtTtTest.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QString>

#include <QDebug>

mdtTtTest::mdtTtTest(QSqlDatabase db)
 : mdtClBase(db)
{
}

bool mdtTtTest::addTestItem(const QVariant & testId, const QVariant & testLinkBusAId, const QVariant & testLinkBusBId, const QVariant & expectedValue)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "INSERT INTO TestItem_tbl (Test_Id_FK, TestLinkBusA_Id_FK, TestLinkBusB_Id_FK, ExpectedValue) "\
        "VALUES (:Test_Id_FK, :TestLinkBusA_Id_FK, :TestLinkBusB_Id_FK, :ExpectedValue)";
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for test item insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Test_Id_FK", testId);
  query.bindValue(":TestLinkBusA_Id_FK", testLinkBusAId);
  query.bindValue(":TestLinkBusB_Id_FK", testLinkBusBId);
  query.bindValue(":ExpectedValue", expectedValue);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test item insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }

  return true;
}
