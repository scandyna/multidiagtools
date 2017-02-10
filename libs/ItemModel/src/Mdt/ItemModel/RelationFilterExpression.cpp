/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "RelationFilterExpression.h"
#include <QAbstractItemModel>

namespace Mdt{ namespace ItemModel{

RelationFilterExpression::~RelationFilterExpression()
{
}

bool RelationFilterExpression::eval(const QAbstractItemModel*const model, int row, const Expression::ParentModelEvalData& parentModelData, Qt::CaseSensitivity caseSensitivity) const
{
  Q_ASSERT(!isNull());
  Q_ASSERT(model != nullptr);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < model->rowCount());
  Q_ASSERT(!parentModelData.isNull());

  return mContainer->eval(model, row, parentModelData, caseSensitivity);
}

RelationKey RelationFilterExpression::getRelationKeyForEquality() const
{
  Q_ASSERT(!isNull());

  return mContainer->getRelationKeyForEquality();
}

RelationFilterExpression RelationFilterExpression::fromRelationKey(const RelationKey & relationKey)
{
  Q_ASSERT(!relationKey.isNull());
  Q_ASSERT(relationKey.columnPairCount() <= 4);

  switch(relationKey.columnPairCount()){
    case 1:
      return fromColumnPairs(relationKey.columnPairAt(0));
      break;
    case 2:
      return fromColumnPairs(relationKey.columnPairAt(0), relationKey.columnPairAt(1));
      break;
    case 3:
      return fromColumnPairs(relationKey.columnPairAt(0), relationKey.columnPairAt(1), relationKey.columnPairAt(2));
      break;
    case 4:
      return fromColumnPairs(relationKey.columnPairAt(0), relationKey.columnPairAt(1), relationKey.columnPairAt(2), relationKey.columnPairAt(3));
      break;
    default:
      ;
  }

  return RelationFilterExpression();  // Will assert if called
}

void RelationFilterExpression::setGreatestColumns()
{
  Q_ASSERT(!isNull());

  mGreatestColumn = mContainer->getGreatestColumn();
  mGreatestParentModelColumn = mContainer->getGreatestParentModelColumn();
}

RelationFilterExpression RelationFilterExpression::fromColumnPairs(RelationColumnPair pair1)
{
  return ( FilterColumn(pair1.childModelColumn()) == ParentModelColumn(pair1.parentModelColumn()) );
}

RelationFilterExpression RelationFilterExpression::fromColumnPairs(RelationColumnPair pair1, RelationColumnPair pair2)
{
  return (
    ( FilterColumn(pair1.childModelColumn()) == ParentModelColumn(pair1.parentModelColumn()) ) &&
    ( FilterColumn(pair2.childModelColumn()) == ParentModelColumn(pair2.parentModelColumn()) )
  );
}

RelationFilterExpression RelationFilterExpression::fromColumnPairs(RelationColumnPair pair1, RelationColumnPair pair2,
                                                                   RelationColumnPair pair3)
{
  return (
    ( FilterColumn(pair1.childModelColumn()) == ParentModelColumn(pair1.parentModelColumn()) ) &&
    ( FilterColumn(pair2.childModelColumn()) == ParentModelColumn(pair2.parentModelColumn()) ) &&
    ( FilterColumn(pair3.childModelColumn()) == ParentModelColumn(pair3.parentModelColumn()) )
  );
}

RelationFilterExpression RelationFilterExpression::fromColumnPairs(RelationColumnPair pair1, RelationColumnPair pair2,
                                                                   RelationColumnPair pair3, RelationColumnPair pair4)
{
  return (
    ( FilterColumn(pair1.childModelColumn()) == ParentModelColumn(pair1.parentModelColumn()) ) &&
    ( FilterColumn(pair2.childModelColumn()) == ParentModelColumn(pair2.parentModelColumn()) ) &&
    ( FilterColumn(pair3.childModelColumn()) == ParentModelColumn(pair3.parentModelColumn()) ) &&
    ( FilterColumn(pair4.childModelColumn()) == ParentModelColumn(pair4.parentModelColumn()) )
  );
}

}} // namespace Mdt{ namespace ItemModel{
