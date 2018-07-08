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
#ifndef MDT_QUERY_EXPRESSION_BUILD_EXPRESSION_TREE_TRANSFORM_H
#define MDT_QUERY_EXPRESSION_BUILD_EXPRESSION_TREE_TRANSFORM_H

#include "../ExpressionGrammar.h"
#include "AddExpressionToTree.h"
#include "MdtQueryExpression_CoreExport.h"
#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>
#include <boost/proto/transform/when.hpp>
#include <boost/proto/transform.hpp>

namespace Mdt{ namespace QueryExpression{ namespace BuildExpressionTree{

  struct MDT_QUERYEXPRESSION_CORE_EXPORT EqualityComparisonTransform : boost::proto::or_<
    boost::proto::when<
      boost::proto::equal_to<LeftTerminal, RightTerminal>,
      boost::proto::call<AddEqualExpressionToTree( boost::proto::_value(boost::proto::_left), boost::proto::_value(boost::proto::_right), boost::proto::_data )>
    >,
    boost::proto::when<
      boost::proto::equal_to<LeftTerminal, LikeExpression>,
      boost::proto::call<AddEqualExpressionToTree( boost::proto::_value(boost::proto::_left), boost::proto::_value(boost::proto::_right), boost::proto::_data )>
    >,
    boost::proto::when<
      boost::proto::not_equal_to<LeftTerminal, RightTerminal>,
      boost::proto::call<AddNotEqualExpressionToTree( boost::proto::_value(boost::proto::_left), boost::proto::_value(boost::proto::_right), boost::proto::_data )>
    >
  >
  {
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT LessComparisonTransform : boost::proto::or_<
    boost::proto::when<
      boost::proto::less<LeftTerminal, RightTerminal>,
      boost::proto::call<AddLessExpressionToTree( boost::proto::_value(boost::proto::_left), boost::proto::_value(boost::proto::_right), boost::proto::_data )>
    >,
    boost::proto::when<
      boost::proto::less_equal<LeftTerminal, RightTerminal>,
      boost::proto::call<AddLessEqualExpressionToTree( boost::proto::_value(boost::proto::_left), boost::proto::_value(boost::proto::_right), boost::proto::_data )>
    >
  >
  {
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT GreaterComparisonTransform : boost::proto::or_<
    boost::proto::when<
      boost::proto::greater<LeftTerminal, RightTerminal>,
      boost::proto::call<AddGreaterExpressionToTree( boost::proto::_value(boost::proto::_left), boost::proto::_value(boost::proto::_right), boost::proto::_data )>
    >,
    boost::proto::when<
      boost::proto::greater_equal<LeftTerminal, RightTerminal>,
      boost::proto::call<AddGreaterEqualExpressionToTree( boost::proto::_value(boost::proto::_left), boost::proto::_value(boost::proto::_right), boost::proto::_data )>
    >
  >
  {
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT ComparisonTransform : boost::proto::or_<
    EqualityComparisonTransform,
    LessComparisonTransform,
    GreaterComparisonTransform
  >
  {
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT LogicalTransform : boost::proto::or_<
    boost::proto::when<
      boost::proto::logical_or<LogicalTransform, LogicalTransform>,
      boost::proto::call<AddLogicalOrExpressionToTree( LogicalTransform(boost::proto::_left), LogicalTransform(boost::proto::_right), boost::proto::_data )>
    >,
    boost::proto::when<
      boost::proto::logical_and<LogicalTransform, LogicalTransform>,
      boost::proto::call<AddLogicalAndExpressionToTree( LogicalTransform(boost::proto::_left), LogicalTransform(boost::proto::_right), boost::proto::_data )>
    >,
    ComparisonTransform
  >
  {
  };

}}} // namespace Mdt{ namespace QueryExpression{ namespace BuildExpressionTree{

#endif // #ifndef MDT_QUERY_EXPRESSION_BUILD_EXPRESSION_TREE_TRANSFORM_H
