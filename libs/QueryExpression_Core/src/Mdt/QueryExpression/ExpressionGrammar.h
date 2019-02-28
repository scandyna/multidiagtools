/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_QUERY_EXPRESSION_EXPRESSION_GRAMMAR_H
#define MDT_QUERY_EXPRESSION_EXPRESSION_GRAMMAR_H

#include "Comparison.h"
#include "MdtQueryExpression_CoreExport.h"

namespace Mdt{ namespace QueryExpression{

  /*! \brief Grammar of a where expression
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT ExpressionGrammar : boost::proto::or_<
                                                              boost::proto::logical_and< ExpressionGrammar , ExpressionGrammar > ,
                                                              boost::proto::logical_or< ExpressionGrammar , ExpressionGrammar > ,
                                                              Comparison
                                                            >
  {
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_EXPRESSION_GRAMMAR_H
