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
#include "mdtSqlRelationInfo.h"

void mdtSqlRelationInfo::setRelationType(mdtSqlRelationInfo::relationType_t type)
{
  pvRelationType = type;
}

void mdtSqlRelationInfo::addRelation(const QString& parentFieldName, const QString& childFieldName, bool copyParentToChildOnInsertion, const QString& operatorWithPreviousItem)
{
  mdtSqlRelationInfoItem item;

  item.parentFieldName = parentFieldName;
  item.childFieldName = childFieldName;
  item.copyParentToChildOnInsertion = copyParentToChildOnInsertion;
  item.relationOperatorWithPreviousItem = operatorWithPreviousItem;

  pvItems.append(item);
}

void mdtSqlRelationInfo::clear()
{
  pvParentTableName.clear();
  pvChildTableName.clear();
  pvItems.clear();
}
