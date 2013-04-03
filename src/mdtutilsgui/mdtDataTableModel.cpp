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
#include "mdtDataTableModel.h"
#include "mdtError.h"
///#include <QSqlError>
///#include <QSqlQuery>
///#include <QStringList>

#include <QDebug>

mdtDataTableModel::mdtDataTableModel(QObject *parent, QSqlDatabase db)
 : QSqlTableModel(parent, db)
{
}

mdtDataTableModel::~mdtDataTableModel()
{
}

bool mdtDataTableModel::addRow(const QMap<QString,QVariant> &data, int role)
{
  QMap<QString,QVariant>::const_iterator it;

  if(!insertRows(0, 1)){
    return false;
  }
  for(it = data.constBegin(); it != data.constEnd(); it++){
    if(!QSqlTableModel::setData(index(0, fieldIndex(it.key())) , it.value(), role)){
      revertRow(0);
      return false;
    }
  }

  return submit();
}

bool mdtDataTableModel::setData(int row, int column, const QVariant &value, int role)
{
  if(!QSqlTableModel::setData(index(row, column), value, role)){
    revertRow(row);
    return false;
  }

  return submit();
}

bool mdtDataTableModel::setData(int row, const QString &field, const QVariant & value, int role)
{
  return setData(row, fieldIndex(field), value, role);
}
