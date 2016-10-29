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
#include "Table.h"
#include <algorithm>

namespace Mdt{ namespace Sql{ namespace Schema{

void Table::setTableName(const QString & name)
{
  pvTableName = name;
  pvForeignKeyList.updateChildTableName(name);
  pvIndexList.updateTableName(name);
}

void Table::setPrimaryKey(const PrimaryKey& pk)
{
  pvPrimaryKeyFieldIndex = -1;
  pvPrimaryKey.setPrimaryKey(pk);
  // Update pvPrimaryKeyFieldIndexList
  pvPrimaryKeyFieldIndexList.clear();
  const auto & fieldNameList = pk.fieldNameList();
  for(const auto & fieldName : fieldNameList){
    int idx = fieldIndex(fieldName);
    Q_ASSERT_X(idx >= 0, "Table::setPrimaryKey()", "pk contains a field that not exists in table");
    pvPrimaryKeyFieldIndexList.emplace_back(idx);
  }
}

void Table::addForeignKey(ForeignKey fk)
{
  Q_ASSERT(!fk.parentTableName().isEmpty());
#ifndef QT_NO_DEBUG
  for(const auto & fieldName : fk.childTableFieldNameList()){
    Q_ASSERT(contains(fieldName));
  }
#endif // #ifndef QT_NO_DEBUG

  fk.setChildTable(*this);
  pvForeignKeyList.append(fk);
}

ForeignKey Table::foreignKeyReferencing(const QString & tableName) const
{
  return pvForeignKeyList.foreignKeyReferencing(tableName);
}

void Table::addIndex(Index index)
{
#ifndef QT_NO_DEBUG
  for(const auto & fieldName : index.fieldNameList()){
    Q_ASSERT(contains(fieldName));
  }
#endif // #ifndef QT_NO_DEBUG

  index.setTableName(pvTableName);
  pvIndexList.append(index);
}

int Table::fieldIndex(const QString& fieldName) const
{
  if(pvPrimaryKeyFieldIndex == 0){
    if(QString::compare(pvPrimaryKey.fieldName(), fieldName, Qt::CaseInsensitive) == 0){
      return 0;
    }else{
      int index = pvFieldList.fieldIndex(fieldName);
      if(index < 0){
        return index;
      }
      return index + 1;
    }
  }else{
    return pvFieldList.fieldIndex(fieldName);
  }
}

QString Table::fieldName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  /*
   * If primary key has a field definition
   * (a AutoIncrementPrimaryKey or a SingleFieldPrimaryKey),
   * we allways represent it as first field in table.
   */
  if(index == pvPrimaryKeyFieldIndex){
    return pvPrimaryKey.fieldName();
  }
  return refFieldConst(index).name();
}

FieldType Table::fieldType(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  if(index == pvPrimaryKeyFieldIndex){
    return pvPrimaryKey.fieldType();
  }
  return refFieldConst(index).type();
}

int Table::fieldLength(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  if(index == pvPrimaryKeyFieldIndex){
    return pvPrimaryKey.fieldLength();
  }
  return refFieldConst(index).length();
}

bool Table::isFieldPartOfPrimaryKey(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  /*
   * If primary key has a field definition
   * (a AutoIncrementPrimaryKey or a SingleFieldPrimaryKey),
   * we allways represent it as first field in table.
   * Else, (for multi column PrimaryKey),
   * we check if index exists in pvPrimaryKeyFieldIndexList.
   */
  if(index == pvPrimaryKeyFieldIndex){
    return true;
  }
  if(pvPrimaryKeyFieldIndexList.empty()){
    return false;
  }
  return (std::find(pvPrimaryKeyFieldIndexList.cbegin(), pvPrimaryKeyFieldIndexList.cend(), index) != pvPrimaryKeyFieldIndexList.cend());
}

bool Table::isFieldAutoIncrement(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  /*
   * The only case for which
   *  is a AutoIncrementPrimaryKey
   */
  if(index == pvPrimaryKeyFieldIndex){
    return (pvPrimaryKey.primaryKeyType() == PrimaryKeyContainer::AutoIncrementPrimaryKeyType);
  }
  return false;
}

bool Table::isFieldRequired(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  if(index == pvPrimaryKeyFieldIndex){
    return true;
  }
  return refFieldConst(index).isRequired();
}

bool Table::isFieldUnique(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  if(index == pvPrimaryKeyFieldIndex){
    return true;
  }
  return refFieldConst(index).isUnique();
}

QVariant Table::fieldDefaultValue(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  if(index == pvPrimaryKeyFieldIndex){
    return QVariant();
  }
  return refFieldConst(index).defaultValue();
}

Field Table::field(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());
  Q_ASSERT(index != pvPrimaryKeyFieldIndex);

  return refFieldConst(index);
}

bool Table::isNull() const
{
  return ( pvTableName.isEmpty() || (fieldCount() < 1) );
}

void Table::clear()
{
  pvPrimaryKeyFieldIndex = -1;
  pvPrimaryKeyFieldIndexList.clear();
  pvPrimaryKey.clear();
  pvIsTemporary = false;
  pvTableName.clear();
  pvFieldList.clear();
  pvForeignKeyList.clear();
  pvIndexList.clear();
}

const Field & Table::refFieldConst(int index) const
{
  /*
   * If primary key has a field definition
   * (a AutoIncrementPrimaryKey or a SingleFieldPrimaryKey),
   * we allways represent it as first field in table.
   */
  if(pvPrimaryKeyFieldIndex == 0){
    Q_ASSERT(index > 0);
    return pvFieldList.at(index - 1);
  }else{
    return pvFieldList.at(index);
  }
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
