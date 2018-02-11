/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "AutoIncrementPrimaryKey.h"
#include "ForeignTable.h"
#include <algorithm>

namespace Mdt{ namespace Sql{ namespace Schema{

void Table::setTableName(const QString & name)
{
  mTableName = name;
  mForeignKeyList.updateChildTableName(name);
  pvIndexList.updateTableName(name);
}

void Table::setAutoIncrementPrimaryKey(const QString & fieldName)
{
  Q_ASSERT(!fieldName.isEmpty());

  Field field;
  int idx = fieldIndex(fieldName);
  if(idx < 0){
    setAutoIncrementPrimaryKey(fieldName, FieldType::Integer);
  }else{
    const auto fldType = fieldType(idx);
    Q_ASSERT(AutoIncrementPrimaryKey::isValidFieldType(fldType));
    setAutoIncrementPrimaryKey(fieldName, fldType);
  }
}

void Table::setAutoIncrementPrimaryKey(const QString& fieldName, FieldType fieldType)
{
  Q_ASSERT(!fieldName.isEmpty());
  Q_ASSERT(AutoIncrementPrimaryKey::isValidFieldType(fieldType));

  Field field;
  int idx = fieldIndex(fieldName);
  if(idx < 0){
    idx = fieldCount();
    field.setName(fieldName);
    field.setType(fieldType);
    field.setRequired(true);
    field.setUnique(true);
    addField(field);
  }else{
    Q_ASSERT(mFieldList.at(idx).type() == fieldType);
    mFieldList[idx].setRequired(true);
    mFieldList[idx].setUnique(true);
  }
  Q_ASSERT(idx >= 0);
  Q_ASSERT(idx < fieldCount());
  mPrimaryKeyFieldIndexList.clear();
  mPrimaryKeyFieldIndexList.addFieldIndex(idx);
  mPrimaryKeyType = PrimaryKeyType::AutoIncrementPrimaryKey;
}

void Table::setPrimaryKey(const PrimaryKey & primaryKey)
{
  Q_ASSERT(!primaryKey.isNull());

  mPrimaryKeyFieldIndexList.clear();
  const auto & fieldNameList = primaryKey.fieldNameList();
  for(const auto & fieldName : fieldNameList){
    const int idx = fieldIndex(fieldName);
    Q_ASSERT(idx >= 0);
    mFieldList[idx].setRequired(true);
    mPrimaryKeyFieldIndexList.addFieldIndex(idx);
  }
  mPrimaryKeyType = PrimaryKeyType::PrimaryKey;
}

void Table::setPrimaryKeyContainer(const PrimaryKeyContainer& primaryKeyContainer)
{
  Q_ASSERT(primaryKeyContainer.primaryKeyType() != PrimaryKeyType::Unknown);
  switch(primaryKeyContainer.primaryKeyType()){
    case PrimaryKeyType::AutoIncrementPrimaryKey:
      setAutoIncrementPrimaryKey(primaryKeyContainer.fieldName(), primaryKeyContainer.fieldType());
      break;
    case PrimaryKeyType::PrimaryKey:
      setPrimaryKey(primaryKeyContainer.primaryKey());
      break;
    case PrimaryKeyType::Unknown:
      break;
  }
}

void Table::addForeignKey(const Field & field, const ForeignTable & foreignTable, const ForeignField & foreignField, const ForeignKeySettings & settings)
{
  Q_ASSERT(!field.isNull());

  if(!contains(field)){
    addField(field);
  }
  mForeignKeyList.addForeignKey(mTableName, field, foreignTable, foreignField, settings);
}

void Table::addForeignKey(const FieldList & fieldList, const ForeignTable & foreignTable, const ForeignFieldList & foreignFieldList, const ForeignKeySettings & settings)
{
  Q_ASSERT(fieldList.size() == foreignFieldList.size());
  Q_ASSERT(!fieldList.isEmpty());

  for(const auto & field : fieldList){
    if(!contains(field)){
      addField(field);
    }
  }
  mForeignKeyList.addForeignKey(mTableName, fieldList, foreignTable, foreignFieldList, settings);
}

ForeignKey Table::foreignKeyReferencing(const QString & tableName) const
{
  return mForeignKeyList.foreignKeyReferencing(tableName);
}

void Table::addIndex(Index index)
{
#ifndef QT_NO_DEBUG
  for(const auto & fieldName : index.fieldNameList()){
    Q_ASSERT(contains(fieldName));
  }
#endif // #ifndef QT_NO_DEBUG

  index.setTableName(mTableName);
  pvIndexList.append(index);
}

QString Table::fieldName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  return refFieldConst(index).name();
}

FieldType Table::fieldType(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  return refFieldConst(index).type();
}

int Table::fieldLength(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  return refFieldConst(index).length();
}

bool Table::isFieldPartOfPrimaryKey(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  return mPrimaryKeyFieldIndexList.containsFieldIndex(index);
}

bool Table::isFieldAutoIncrement(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  if(isFieldPartOfPrimaryKey(index)){
    return (mPrimaryKeyType == PrimaryKeyType::AutoIncrementPrimaryKey);
  }
  return false;
}

bool Table::isFieldRequired(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  return refFieldConst(index).isRequired();
}

bool Table::isFieldUnique(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  return refFieldConst(index).isUnique();
}

QVariant Table::fieldDefaultValue(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  return refFieldConst(index).defaultValue();
}

Field Table::field(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  return refFieldConst(index);
}

bool Table::isNull() const
{
  return ( mTableName.isEmpty() || (fieldCount() < 1) );
}

void Table::clear()
{
  mPrimaryKeyFieldIndexList.clear();
  mPrimaryKeyType = PrimaryKeyType::Unknown;
  pvIsTemporary = false;
  mTableName.clear();
  mFieldList.clear();
  mForeignKeyList.clear();
  pvIndexList.clear();
}

const Field & Table::refFieldConst(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  return mFieldList.at(index);
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
