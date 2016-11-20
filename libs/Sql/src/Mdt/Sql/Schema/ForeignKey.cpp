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
#include "ForeignKey.h"
#include "Table.h"
#include "Index.h"
#include "Field.h"
#include "FieldList.h"
#include "ForeignField.h"
#include "ForeignFieldList.h"
#include <QLatin1String>

namespace Mdt{ namespace Sql{ namespace Schema{

void ForeignKey::setTable(const Table & table)
{
  mTableName = table.tableName();
}

void ForeignKey::setChildTable(const Table & table)
{
  mTableName = table.tableName();
}

void ForeignKey::setForeignTable(const Table & table)
{
  mForeignTableName = table.tableName();
}

void ForeignKey::setParentTable(const Table & table)
{
  mForeignTableName = table.tableName();
}

void ForeignKey::setFields(const Field & field, const ForeignField & foreignTableField)
{
  Q_ASSERT(!field.isNull());

  mFieldNameList.clear();
  mForeignTableFieldNameList.clear();
  mFieldNameList.append(field.name());
  mForeignTableFieldNameList.append(foreignTableField.fieldName());
}

void ForeignKey::setFields(const FieldList & fieldList, const ForeignFieldList & foreignTableFieldList)
{
  Q_ASSERT(fieldList.size() == foreignTableFieldList.size());

  mFieldNameList = fieldList.toFieldNameList();
  mForeignTableFieldNameList = foreignTableFieldList.fieldNameList();
}

void ForeignKey::addKeyFields(const ParentTableFieldName & parentTableFieldName, const ChildTableFieldName & childTableFieldName)
{
  Q_ASSERT(!parentTableFieldName.fieldName().isEmpty());
  Q_ASSERT(!childTableFieldName.fieldName().isEmpty());

  mForeignTableFieldNameList.append(parentTableFieldName.fieldName());
  mFieldNameList.append(childTableFieldName.fieldName());
}

Index ForeignKey::getIndex() const
{
  Index index;

  index.setTableName(mTableName);
  for(const auto & fieldName : mFieldNameList){
    index.addFieldName(fieldName);
  }
  index.generateName();

  return index;
}

Index ForeignKey::getChildTableIndex() const
{
  return getIndex();
}

void ForeignKey::clear()
{
  mSettings.clear();
  mTableName.clear();
  mForeignTableName.clear();
  mForeignTableFieldNameList.clear();
  mFieldNameList.clear();
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{