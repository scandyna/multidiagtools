/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "ForeignKeyList.h"
#include "Field.h"
#include "FieldList.h"
#include "ForeignTable.h"
#include "ForeignField.h"
#include "ForeignFieldList.h"

namespace Mdt{ namespace Sql{ namespace Schema{

void ForeignKeyList::addForeignKey(const QString & tableName, const Field & field, const ForeignTable & foreignTable, const ForeignField & foreignField, const ForeignKeySettings & settings)
{
  ForeignKey fk;

  fk.setTableName(tableName);
  fk.setForeignTableName(foreignTable.tableName());
  fk.setFields(field, foreignField);
  fk.setSettings(settings);
  mForeignKeyList.append(fk);
}

void ForeignKeyList::addForeignKey(const QString & tableName, const FieldList & fieldList, const ForeignTable & foreignTable, const ForeignFieldList & foreignFieldList, const ForeignKeySettings & settings)
{
  ForeignKey fk;

  fk.setTableName(tableName);
  fk.setForeignTableName(foreignTable.tableName());
  fk.setFields(fieldList, foreignFieldList);
  fk.setSettings(settings);
  mForeignKeyList.append(fk);
}

void ForeignKeyList::updateTableName(const QString& tableName)
{
  for(auto & fk : mForeignKeyList){
    fk.setChildTableName(tableName);
  }
}

void ForeignKeyList::updateChildTableName(const QString& tableName)
{
  updateTableName(tableName);
}

ForeignKey ForeignKeyList::foreignKeyReferencing(const QString& tableName) const
{
  ForeignKey fk;
  for(const auto & _fk : mForeignKeyList){
    if( QString::compare(_fk.parentTableName(), tableName, Qt::CaseInsensitive) == 0){
      fk = _fk;
      break;
    }
  }
  return fk;
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{