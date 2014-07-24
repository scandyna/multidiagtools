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
#include "mdtTtAbstractTestWidget.h"
#include <QSqlError>

#include <QDebug>

using namespace std;

mdtTtAbstractTestWidget::mdtTtAbstractTestWidget(QSqlDatabase db, QWidget* parent)
 : QWidget(parent),
   pvDatabase(db),
   pvTestItemTableModel(new QSqlTableModel(0, db)),
   pvTestNodeManager(new mdtTtTestNodeManager(0, db))
{
}

bool mdtTtAbstractTestWidget::init()
{
  QSqlError sqlError;

  // Setup test data
  if(!pvTestData.setup(database(), true)){
    pvLastError = pvTestData.lastError();
    return false;
  }
  // Setup test item table model
  testItemTableModel()->setTable("TestItem_view");
  if(!testItemTableModel()->select()){
    sqlError = testItemTableModel()->lastError();
    pvLastError.setError(tr("Cannot select data from table 'TestItem_view'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtAbstractTestWidget");
    pvLastError.commit();
    return false;
  }
  emit testItemTableSet();

  return true;
}

void mdtTtAbstractTestWidget::setTestData(const mdtTtTestData& data)
{
  pvTestData = data;
  emit testDataChanged(pvTestData);
}

void mdtTtAbstractTestWidget::setTestDataValue(const QString& fieldName, const QVariant& value)
{
  pvTestData.setValue(fieldName, value);
  emit testDataChanged(pvTestData);
}

