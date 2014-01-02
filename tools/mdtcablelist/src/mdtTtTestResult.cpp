/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtTtTestResult.h"
#include "mdtTtTest.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QString>
#include <QList>

mdtTtTestResult::mdtTtTestResult(QSqlDatabase db)
 : mdtClBase(db)
{
}

bool mdtTtTestResult::setBaseTest(const QVariant & testResultId, const QVariant & testId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Remove previous result items
  if(!removeData("TestResultItem_tbl", "TestResult_Id_FK", testResultId)){
    return false;
  }
  // Prepare query for edition
  sql = "UPDATE TestResult_tbl SET "\
        " Test_Id_FK = :Test_Id_FK "\
        " WHERE Id_PK = " + testResultId.toString();
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for test result edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestResult");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Test_Id_FK", testId);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test result edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestResult");
    pvLastError.commit();
    return false;
  }
  // Add items
  if(!addItemsByTestId(testResultId, testId)){
    return false;
  }

  return true;
}

bool mdtTtTestResult::addItem(const QVariant & testResultId, const QVariant & testItemId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "INSERT INTO TestResultItem_tbl (TestResult_Id_FK, TestItem_Id_FK) "\
        "VALUES (:TestResult_Id_FK, :TestItem_Id_FK)";
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for test result item insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestResult");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":TestResult_Id_FK", testResultId);
  query.bindValue(":TestItem_Id_FK", testItemId);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test result item insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestResult");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTestResult::addItemsByTestId(const QVariant & testResultId, const QVariant & testId)
{
  mdtTtTest t(database());
  QList<QVariant> testItemIdList;
  int i;

  // Get base test items ID list
  testItemIdList = t.getListOfTestItemIdListByTestId(testId);
  if(testItemIdList.isEmpty()){
    pvLastError = t.lastError();
    return false;
  }
  // Add test result items
  for(i = 0; i < testItemIdList.size(); ++i){
    if(!addItem(testResultId, testItemIdList.at(i))){
      return false;
    }
  }

  return true;
}
