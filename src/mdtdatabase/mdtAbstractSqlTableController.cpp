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
#include "mdtAbstractSqlTableController.h"
#include <QSqlError>

using namespace std;

mdtAbstractSqlTableController::mdtAbstractSqlTableController(QObject* parent)
 : QObject(parent)
{
}

mdtAbstractSqlTableController::~mdtAbstractSqlTableController()
{
}

void mdtAbstractSqlTableController::setTableName(const QString& tableName, QSqlDatabase db, const QString& userFriendlyTableName)
{
  shared_ptr<QSqlTableModel> m(new QSqlTableModel(0, db));
  m->setTable(tableName);
  setModel(m, userFriendlyTableName);
}

void mdtAbstractSqlTableController::setModel(shared_ptr< QSqlTableModel > m, const QString& userFriendlyTableName)
{
  Q_ASSERT(m);

  pvModel = m;
  if(userFriendlyTableName.isEmpty()){
    pvUserFriendlyTableName = m->tableName();
  }else{
    pvUserFriendlyTableName = userFriendlyTableName;
  }
}

bool mdtAbstractSqlTableController::select()
{
  Q_ASSERT(pvModel);

  bool ok;

  /// \todo emit signals to diseable UI actions
  
  // Call select on model
  ok = pvModel->select();
  if(!ok){
    QSqlError sqlError = pvModel->lastError();
    pvLastError.setError(tr("Select failed for") + "'" + pvUserFriendlyTableName + "' (" + pvModel->tableName() + ")" , mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
  }
  
  /// \todo emit signals to enable UI actions

  return ok;
}
