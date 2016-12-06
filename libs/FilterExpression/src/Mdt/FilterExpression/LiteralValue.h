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
#ifndef MDT_FILTER_EXPRESSION_LITERAL_VALUE_H
#define MDT_FILTER_EXPRESSION_LITERAL_VALUE_H

#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>

namespace Mdt{ namespace FilterExpression{

  /*! \brief Literal value grammar
   */
  struct LiteralValue : boost::proto::or_<
                          boost::proto::terminal< boost::proto::convertible_to<int> > ,
                          boost::proto::terminal< const char * const >
                        >
  {
  };

}} // namespace Mdt{ namespace FilterExpression{

#endif // #ifndef MDT_FILTER_EXPRESSION_LITERAL_VALUE_H
