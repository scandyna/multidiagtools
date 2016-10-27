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
#ifndef MDT_SQL_EXPRESSION_WHERE_EXPRESSION_GRAMMAR_H
#define MDT_SQL_EXPRESSION_WHERE_EXPRESSION_GRAMMAR_H

#include "Mdt/Sql/WhereField.h"
#include "Comparison.h"

namespace Mdt{ namespace Sql{ namespace Expression{

  /*! \brief Grammar of a where expression
   */
  struct WhereExpressionGrammar : boost::proto::or_<
                                    boost::proto::logical_and< WhereExpressionGrammar , WhereExpressionGrammar > ,
                                    boost::proto::logical_or< WhereExpressionGrammar , WhereExpressionGrammar > ,
                                    Comparison
                                  >
  {
  };

}}} // namespace Mdt{ namespace Sql{ namespace Expression{

#endif // #ifndef MDT_SQL_EXPRESSION_WHERE_EXPRESSION_GRAMMAR_H
