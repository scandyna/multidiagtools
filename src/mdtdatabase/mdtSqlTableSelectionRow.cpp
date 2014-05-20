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
#include "mdtSqlTableSelectionRow.h"

mdtSqlTableSelectionRow::mdtSqlTableSelectionRow() 
{
}

void mdtSqlTableSelectionRow::clear() 
{
  pvItemList.clear();
}

void mdtSqlTableSelectionRow::addIndex(const QModelIndex & index, const QString & fieldName) 
{
  mdtSqlTableSelectionItem item(index, fieldName);
  pvItemList.append(item);
}

QModelIndex mdtSqlTableSelectionRow::index(int internalColumnIndex) const 
{
  Q_ASSERT(internalColumnIndex >= 0);
  Q_ASSERT(internalColumnIndex < pvItemList.size());

  return pvItemList.at(internalColumnIndex).index();
}

QModelIndex mdtSqlTableSelectionRow::index(const QString & fieldName) const 
{
  int i;

  for(i = 0; i < pvItemList.size(); ++i){
    if(pvItemList.at(i).fieldName() == fieldName){
      return pvItemList.at(i).index();
    }
  }

  return QModelIndex();
}

QStringList mdtSqlTableSelectionRow::fields() const
{
  QStringList lst;
  int i;

  for(i = 0; i < pvItemList.size(); ++i){
    lst.append(pvItemList.at(i).fieldName());
  }

  return lst;
}
