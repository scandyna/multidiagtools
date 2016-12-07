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
#ifndef MDT_ITEM_EDITOR_EXPRESSION_COMPARISON_H
#define MDT_ITEM_EDITOR_EXPRESSION_COMPARISON_H

#include "LeftTerminal.h"
#include "RightTerminal.h"
#include "../LikeExpression.h"
#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>

namespace Mdt{ namespace ItemEditor{ namespace Expression{

  /*! \brief Equality or inequality comparison grammar
   */
  struct CompareEquality : boost::proto::or_<
                              boost::proto::equal_to< LeftTerminal , RightTerminal > ,
                              boost::proto::equal_to< LeftTerminal , LikeExpression > ,
                              boost::proto::not_equal_to< LeftTerminal , RightTerminal >
                            >
  {
  };

  /*! \brief Less or less equal comparison grammar
   */
  struct CompareLess : boost::proto::or_<
                          boost::proto::less< LeftTerminal , RightTerminal > ,
                          boost::proto::less_equal< LeftTerminal , RightTerminal >
                        >
  {
  };

  /*! \brief Greater or greater equal comparison grammar
   */
  struct CompareGreater : boost::proto::or_<
                              boost::proto::greater< LeftTerminal , RightTerminal > ,
                              boost::proto::greater_equal< LeftTerminal , RightTerminal >
                            >
  {
  };

  /*! \brief Comparison grammar
   */
  struct Comparison : boost::proto::or_<
                          CompareEquality ,
                          CompareLess ,
                          CompareGreater
                        >
  {
  };

}}} // namespace Mdt{ namespace ItemEditor{ namespace Expression{

#endif // #ifndef MDT_ITEM_EDITOR_EXPRESSION_COMPARISON_H
