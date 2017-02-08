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
#include "FilterExpression.h"
#include <QAbstractItemModel>

namespace Mdt{ namespace ItemModel{

FilterExpression::~FilterExpression()
{
}

bool FilterExpression::eval(const QAbstractItemModel*const model, int row, Qt::CaseSensitivity caseSensitivity) const
{
  Q_ASSERT(!isNull());
  Q_ASSERT(model != nullptr);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < model->rowCount());

  return mContainer->eval(model, row, caseSensitivity);
}

void FilterExpression::setGreatestColumn()
{
  Q_ASSERT(!isNull());

  mGreatestColumn = mContainer->getGreatestColumn();
}

}} // namespace Mdt{ namespace ItemModel{
