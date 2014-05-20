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
#include "mdtSqlTableSelection.h"
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QVector>

mdtSqlTableSelection::mdtSqlTableSelection() 
{
}

void mdtSqlTableSelection::clear() 
{
  pvRowList.clear();
}

void mdtSqlTableSelection::setIndexList(const QModelIndexList & indexList, const QStringList & fieldList, QSqlQueryModel *model) 
{
  Q_ASSERT(model != 0);

  QVector<int> selectedRows;
  mdtSqlTableSelectionRow selectionRow;
  QModelIndex index;
  QString fieldName;
  int row;
  int i, j;

  clear();
  // Build a list of selected rows
  for(i = 0; i < indexList.size(); ++i){
    row = indexList.at(i).row();
    Q_ASSERT(row >= 0);
    if(!selectedRows.contains(row)){
      selectedRows.append(row);
    }
  }
  qSort(selectedRows.begin(), selectedRows.end());
  // Build list with only interresting fields
  for(i = 0; i < selectedRows.size(); ++i){
    selectionRow.clear();
    row = selectedRows.at(i);
    for(j = 0; j < fieldList.size(); ++j){
      fieldName = fieldList.at(j);
      index = model->index(row, model->record().indexOf(fieldName));
      Q_ASSERT(index.isValid());
      selectionRow.addIndex(index, fieldName);
    }
    pvRowList.append(selectionRow);
  }
}

QVariant mdtSqlTableSelection::data(int row, const QString & fieldName) const 
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < pvRowList.size());

  QModelIndex index;

  index = pvRowList.at(row).index(fieldName);

  return index.data();
}

QStringList mdtSqlTableSelection::fields(int row) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < pvRowList.size());

  return pvRowList.at(row).fields();
}

QList<QVariant> mdtSqlTableSelection::dataList(const QString & fieldName) const 
{
  QList<QVariant> list;
  int i;

  for(i = 0; i < pvRowList.size(); ++i){
    list.append(pvRowList.at(i).index(fieldName).data());
  }

  return list;
}
