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
#include "FilterExpression.h"
#include <QAbstractItemModel>

namespace Mdt{ namespace ItemModel{

FilterExpression::~FilterExpression()
{
}

// void FilterExpression::setExpression(const auto & expr)
// {
//   static_assert( boost::proto::matches< decltype(expr), Expression::FilterExpressionGrammar>::value , "Type of expr is not a valid filter expression." );
// }

bool FilterExpression::eval(const QAbstractItemModel*const model, int row) const
{
  Q_ASSERT(!isNull());
  Q_ASSERT(model != nullptr);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < model->rowCount());

  return mContainer->eval(model, row);
}

// void FilterExpression::storeExpression(const auto & expr)
// {
//   mContainer.reset( new Expression::FilterExpressionContainer< typename boost::proto::result_of::deep_copy<decltype(expr)>::type >(expr) );
// }

}} // namespace Mdt{ namespace ItemModel{