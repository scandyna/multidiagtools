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

void RelationFilterExpression::setGreatestColumns()
{
  Q_ASSERT(!isNull());

  mGreatestColumn = mContainer->getGreatestColumn();
  mGreatestParentModelColumn = mContainer->getGreatestParentModelColumn();
}

}} // namespace Mdt{ namespace ItemModel{
