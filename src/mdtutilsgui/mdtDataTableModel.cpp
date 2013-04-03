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

void mdtDataTableModel::setTable(const QString & tableName)
{
  int i;

  QSqlTableModel::setTable(tableName);
  // Get indexes of the primary key
  pvPkIndexes.clear();
  for(i=0; i<primaryKey().count(); i++){
    pvPkIndexes.append(fieldIndex(primaryKey().fieldName(i)));
  }
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

bool mdtDataTableModel::addRow(const QList<QVariant> &data, bool pkNotInData, int role, bool submitRow)
{
  int dataColumnIndex;
  int modelColumnIndex;

  if(!insertRows(0, 1)){
    return false;
  }
  modelColumnIndex = 0;
  for(dataColumnIndex=0; dataColumnIndex<data.size(); dataColumnIndex++){
    // If PK is not in data, we must remap indexes
    if((pkNotInData)&&(pvPkIndexes.contains(modelColumnIndex))){
      if(modelColumnIndex >= columnCount()){
        revertRow(0);
        return false;
      }
      modelColumnIndex++;
    }
    if(!QSqlTableModel::setData(index(0, modelColumnIndex), data.at(dataColumnIndex), role)){
      revertRow(0);
      return false;
    }
    modelColumnIndex++;
  }
  if(submitRow){
    return submit();
  }

  return true;
}

bool mdtDataTableModel::addRow(const QStringList &data, bool pkNotInData, int role, bool submitRow)
{
  int dataColumnIndex;
  int modelColumnIndex;

  if(!insertRows(0, 1)){
    return false;
  }
  modelColumnIndex = 0;
  for(dataColumnIndex=0; dataColumnIndex<data.size(); dataColumnIndex++){
    // If PK is not in data, we must remap indexes
    if((pkNotInData)&&(pvPkIndexes.contains(modelColumnIndex))){
      if(modelColumnIndex >= columnCount()){
        revertRow(0);
        return false;
      }
      modelColumnIndex++;
    }
    ///qDebug() << "Adding index" << index(0, modelColumnIndex) << " , data: " << data.at(dataColumnIndex);
    if(!QSqlTableModel::setData(index(0, modelColumnIndex), data.at(dataColumnIndex), role)){
      revertRow(0);
      return false;
    }
    modelColumnIndex++;
  }
  if(submitRow){
    return submit();
  }

  return true;
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
