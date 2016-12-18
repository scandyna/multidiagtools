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
#ifndef MDT_ITEM_MODEL_EXPRESSION_RELATION_FILTER_EXPRESSION_GRAMMAR_H
#define MDT_ITEM_MODEL_EXPRESSION_RELATION_FILTER_EXPRESSION_GRAMMAR_H

#include "RelationFilterComparison.h"

namespace Mdt{ namespace ItemModel{ namespace Expression{

  /*! \brief Grammar of a FilterExpression
   */
  struct RelationFilterExpressionGrammar : boost::proto::or_<
                                            boost::proto::logical_and< RelationFilterExpressionGrammar , RelationFilterExpressionGrammar > ,
                                            boost::proto::logical_or< RelationFilterExpressionGrammar , RelationFilterExpressionGrammar > ,
                                            RelationFilterComparison
                                          >
  {
  };

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{

#endif // #ifndef MDT_ITEM_MODEL_EXPRESSION_RELATION_FILTER_EXPRESSION_GRAMMAR_H
