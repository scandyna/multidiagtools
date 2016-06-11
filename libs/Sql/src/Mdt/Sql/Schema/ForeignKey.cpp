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
#include <QLatin1String>

namespace Mdt{ namespace Sql{ namespace Schema{

void ForeignKey::setParentTable(const Table & table)
{
  pvParentTableName = table.tableName();
}

void ForeignKey::setChildTable(const Table & table)
{
  pvChildTableName = table.tableName();
}

Index ForeignKey::getChildTableIndex() const
{
  Index index;

  index.setTableName(pvChildTableName);
  for(const auto & fieldName : pvChildTableFieldNameList){
    index.addFieldName(fieldName);
  }
  index.generateName();

  return index;
}

void ForeignKey::addKeyFields(const ParentTableFieldName & parentTableFieldName, const ChildTableFieldName & childTableFieldName)
{
  Q_ASSERT(!parentTableFieldName.fieldName().isEmpty());
  Q_ASSERT(!childTableFieldName.fieldName().isEmpty());

  pvParentTableFieldNameList.append(parentTableFieldName.fieldName());
  pvChildTableFieldNameList.append(childTableFieldName.fieldName());
}

void ForeignKey::clear()
{
  pvCreateChildIndex = false;
  pvOnDeleteAction = NoAction;
  pvOnUpdateAction = NoAction;
  pvParentTableName.clear();
  pvChildTableName.clear();
  pvParentTableFieldNameList.clear();
  pvChildTableFieldNameList.clear();
}

QString ForeignKey::actionString(ForeignKey::Action action)
{
  switch(action){
    case NoAction:
      return QStringLiteral("NO ACTION");
    case Restrict:
      return QStringLiteral("RESTRICT");
    case SetNull:
      return QStringLiteral("SET NULL");
    case SetDefault:
      return QStringLiteral("SET DEFAULT");
    case Cascade:
      return QStringLiteral("CASCADE");
  }
  return QString();
}

ForeignKey::Action ForeignKey::actionFromString(const QString& actionStr)
{
  Action action;
  const QString str = actionStr.trimmed().toUpper();

  if(str == QLatin1String("NO ACTION")){
    action = NoAction;
  }else if(str == QLatin1String("RESTRICT")){
    action = Restrict;
  }else if(str == QLatin1String("SET NULL")){
    action = SetNull;
  }else if(str == QLatin1String("SET DEFAULT")){
    action = SetDefault;
  }else if(str == QLatin1String("CASCADE")){
    action = Cascade;
  }else{
    action = NoAction;
  }

  return action;
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{