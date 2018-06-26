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
#ifndef MDT_QUERY_EXPRESSION_COMPARISON_H
#define MDT_QUERY_EXPRESSION_COMPARISON_H

#include "Terminal.h"
#include "LikeExpression.h"
#include "MdtQueryExpression_CoreExport.h"
#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Equality or inequality comparison grammar
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT CompareEquality : boost::proto::or_<
                                                            boost::proto::equal_to< LeftTerminal , RightTerminal > ,
                                                            boost::proto::equal_to< LeftTerminal , LikeExpression > ,
                                                            boost::proto::not_equal_to< LeftTerminal , RightTerminal >
                                                          >
  {
  };

  /*! \brief Less or less equal comparison grammar
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT CompareLess : boost::proto::or_<
                                                        boost::proto::less< LeftTerminal , RightTerminal > ,
                                                        boost::proto::less_equal< LeftTerminal , RightTerminal >
                                                      >
  {
  };

  /*! \brief Greater or greater equal comparison grammar
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT CompareGreater : boost::proto::or_<
                                                            boost::proto::greater< LeftTerminal , RightTerminal > ,
                                                            boost::proto::greater_equal< LeftTerminal , RightTerminal >
                                                          >
  {
  };

  /*! \brief Comparison grammar
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT Comparison : boost::proto::or_<
                                                        CompareEquality ,
                                                        CompareLess ,
                                                        CompareGreater
                                                      >
  {
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_COMPARISON_H
