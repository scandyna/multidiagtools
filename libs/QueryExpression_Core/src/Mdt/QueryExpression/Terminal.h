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
#ifndef MDT_QUERY_EXPRESSION_TERMINAL_H
#define MDT_QUERY_EXPRESSION_TERMINAL_H

#include "SelectField.h"
#include "LiteralValue.h"
#include "MdtQueryExpression_CoreExport.h"
#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Terminal grammar for the left part of a binary expression
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT LeftTerminal : boost::proto::or_<
                                                          SelectField
                                                        >
  {
  };

  /*! \brief Terminal grammar for the right part of a binary expression
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT RightTerminal : boost::proto::or_<
                                                          SelectField,
                                                          LiteralValue
                                                        >
  {
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_TERMINAL_H
