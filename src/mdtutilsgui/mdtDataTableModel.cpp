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
  int modelRowIndex;

  modelRowIndex = rowCount();
  if(!insertRows(modelRowIndex, 1)){
    return false;
  }
  for(it = data.constBegin(); it != data.constEnd(); it++){
    if(!QSqlTableModel::setData(index(modelRowIndex, fieldIndex(it.key())) , it.value(), role)){
      qDebug() << __FUNCTION__ << ": reverting row " << modelRowIndex;
      revertRow(modelRowIndex);
      return false;
    }
  }

  return doSubmit();
}

bool mdtDataTableModel::addRow(const QList<QVariant> &data, bool pkNotInData, int role)
{
  int modelRowIndex;
  int dataColumnIndex;
  int modelColumnIndex;

  modelRowIndex = rowCount();
  if(!insertRows(modelRowIndex, 1)){
    return false;
  }
  modelColumnIndex = 0;
  for(dataColumnIndex=0; dataColumnIndex<data.size(); dataColumnIndex++){
    // If PK is not in data, we must remap indexes
    if((pkNotInData)&&(pvPkIndexes.contains(modelColumnIndex))){
      if(modelColumnIndex >= columnCount()){
        qDebug() << __FUNCTION__ << ": reverting row " << modelRowIndex;
        revertRow(modelRowIndex);
        return false;
      }
      modelColumnIndex++;
    }
    if(!QSqlTableModel::setData(index(modelRowIndex, modelColumnIndex), data.at(dataColumnIndex), role)){
      qDebug() << __FUNCTION__ << ": reverting row " << modelRowIndex;
      revertRow(modelRowIndex);
      return false;
    }
    modelColumnIndex++;
  }

  return doSubmit();
}

bool mdtDataTableModel::addRow(const QStringList &data, bool pkNotInData, int role)
{
  int modelRowIndex;
  int dataColumnIndex;
  int modelColumnIndex;

  modelRowIndex = rowCount();
  if(!insertRows(modelRowIndex, 1)){
    return false;
  }
  modelColumnIndex = 0;
  for(dataColumnIndex=0; dataColumnIndex<data.size(); dataColumnIndex++){
    // If PK is not in data, we must remap indexes
    if((pkNotInData)&&(pvPkIndexes.contains(modelColumnIndex))){
      if(modelColumnIndex >= columnCount()){
        qDebug() << __FUNCTION__ << ": reverting row " << modelRowIndex;
        revertRow(modelRowIndex);
        return false;
      }
      modelColumnIndex++;
    }
    if(!QSqlTableModel::setData(index(modelRowIndex, modelColumnIndex), data.at(dataColumnIndex), role)){
      qDebug() << __FUNCTION__ << ": reverting row " << modelRowIndex;
      revertRow(modelRowIndex);
      return false;
    }
    modelColumnIndex++;
  }

  return doSubmit();
}

bool mdtDataTableModel::addRows(const QList<QStringList> &rows, const mdtFieldMap &fieldMap, int role)
{
  int dataRowIndex;
  int modelRowIndex;
  int modelColumnIndex;
  QVariant data;

  modelRowIndex = rowCount();
  if(!insertRows(modelRowIndex, rows.size())){
    return false;
  }
  for(dataRowIndex=0; dataRowIndex<rows.size(); dataRowIndex++){
    // Add data of each column
    for(modelColumnIndex=0; modelColumnIndex<columnCount(); modelColumnIndex++){
      data = fieldMap.dataForFieldIndex(rows.at(dataRowIndex), modelColumnIndex);
      // If data is empty, it's possible that we have auto values
      if(data.isValid()){
        if(!QSqlTableModel::setData(index(modelRowIndex, modelColumnIndex), data, role)){
          qDebug() << __FUNCTION__ << ": reverting row " << modelRowIndex;
          revertRow(modelRowIndex);
          return false;
        }
      }
    }
    modelRowIndex++;
  }

  return doSubmit();
}

bool mdtDataTableModel::setData(int row, int column, const QVariant &value, int role)
{
  if(!QSqlTableModel::setData(index(row, column), value, role)){
    qDebug() << __FUNCTION__ << ": reverting row " << row;
    ///revertRow(row);
    ///revertAll();
    return false;
  }

  return doSubmit();
}

bool mdtDataTableModel::setData(int row, const QString &field, const QVariant & value, int role)
{
  return setData(row, fieldIndex(field), value, role);
}

bool mdtDataTableModel::doSubmit()
{
  if(editStrategy() == QSqlTableModel::OnManualSubmit){
    return submitAll();
  }else{
    return submit();
  }
}
