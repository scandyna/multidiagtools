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
#include "AutoIncrementPrimaryKey.h"
#include "ForeignTable.h"
#include <algorithm>

namespace Mdt{ namespace Sql{ namespace Schema{

void Table::setTableName(const QString & name)
{
  pvTableName = name;
  pvForeignKeyList.updateChildTableName(name);
  pvIndexList.updateTableName(name);
}

void Table::setAutoIncrementPrimaryKey(const QString & fieldName)
{
  Q_ASSERT(!fieldName.isEmpty());
  Q_ASSERT(!contains(fieldName));

  mPrimaryKeyFieldIndex = 0;
  mPrimaryKey.setPrimaryKey( AutoIncrementPrimaryKey(fieldName) );
}

void Table::addForeignKey(const Field & field, const ForeignTable & foreignTable, const ForeignField & foreignField, const ForeignKeySettings & settings)
{

}

void Table::addForeignKey(const FieldList & fieldList, const ForeignTable & foreignTable, const ForeignFieldList & ForeignFieldList, const ForeignKeySettings & settings)
{

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

int Table::fieldIndex(const QString & fieldName) const
{
  if(mPrimaryKeyFieldIndex == 0){
    if(QString::compare(mPrimaryKey.fieldName(), fieldName, Qt::CaseInsensitive) == 0){
      return 0;
    }else{
      int index = mFieldList.fieldIndex(fieldName);
      if(index < 0){
        return index;
      }
      return index + 1;
    }
  }else{
    return mFieldList.fieldIndex(fieldName);
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
  if(index == mPrimaryKeyFieldIndex){
    return mPrimaryKey.fieldName();
  }
  return refFieldConst(index).name();
}

FieldType Table::fieldType(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  if(index == mPrimaryKeyFieldIndex){
    return mPrimaryKey.fieldType();
  }
  return refFieldConst(index).type();
}

int Table::fieldLength(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  if(index == mPrimaryKeyFieldIndex){
    return mPrimaryKey.fieldLength();
  }
  return refFieldConst(index).length();
}

bool Table::isFieldPartOfPrimaryKey(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  /*
   * If primary key has a field definition
   * (a AutoIncrementPrimaryKey),
   * we allways represent it as first field in table.
   * Else, (for multi column PrimaryKey),
   * we check if index exists in pvPrimaryKeyFieldIndexList.
   */
  if(index == mPrimaryKeyFieldIndex){
    return true;
  }
  if(mPrimaryKeyFieldIndexList.empty()){
    return false;
  }
  return (std::find(mPrimaryKeyFieldIndexList.cbegin(), mPrimaryKeyFieldIndexList.cend(), index) != mPrimaryKeyFieldIndexList.cend());
}

bool Table::isFieldAutoIncrement(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  /*
   * The only case for which
   *  is a AutoIncrementPrimaryKey
   */
  if(index == mPrimaryKeyFieldIndex){
    return (mPrimaryKey.primaryKeyType() == PrimaryKeyContainer::AutoIncrementPrimaryKeyType);
  }
  return false;
}

bool Table::isFieldRequired(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  if(index == mPrimaryKeyFieldIndex){
    return true;
  }
  return refFieldConst(index).isRequired();
}

bool Table::isFieldUnique(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  if(index == mPrimaryKeyFieldIndex){
    return true;
  }
  return refFieldConst(index).isUnique();
}

QVariant Table::fieldDefaultValue(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  if(index == mPrimaryKeyFieldIndex){
    return QVariant();
  }
  return refFieldConst(index).defaultValue();
}

Field Table::field(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());
  Q_ASSERT(index != mPrimaryKeyFieldIndex);

  return refFieldConst(index);
}

bool Table::isNull() const
{
  return ( pvTableName.isEmpty() || (fieldCount() < 1) );
}

void Table::clear()
{
  mPrimaryKeyFieldIndex = -1;
  mPrimaryKeyFieldIndexList.clear();
  mPrimaryKey.clear();
  pvIsTemporary = false;
  pvTableName.clear();
  mFieldList.clear();
  pvForeignKeyList.clear();
  pvIndexList.clear();
}

void Table::updatePrimaryKeyFlags(const PrimaryKey & pk)
{
  mPrimaryKeyFieldIndex = -1;
  // Update mPrimaryKeyFieldIndexList
  mPrimaryKeyFieldIndexList.clear();
  const auto fieldNameList = pk.fieldNameList();
  for(const auto & fieldName : fieldNameList){
    int idx = fieldIndex(fieldName);
    Q_ASSERT_X(idx >= 0, "Table::updatePrimaryKeyFlags()", "pk contains a field that not exists in table");
    mPrimaryKeyFieldIndexList.emplace_back(idx);
    mFieldList[idx].setRequired(true);
  }
}

const Field & Table::refFieldConst(int index) const
{
  /*
   * If primary key has a field definition
   * (a AutoIncrementPrimaryKey or a SingleFieldPrimaryKey),
   * we allways represent it as first field in table.
   */
  if(mPrimaryKeyFieldIndex == 0){
    Q_ASSERT(index > 0);
    return mFieldList.at(index - 1);
  }else{
    return mFieldList.at(index);
  }
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
