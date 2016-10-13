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
#ifndef MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_GRAMMAR_H
#define MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_GRAMMAR_H

#include "Mdt/Sql/JoinConstraintField.h"
#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>

namespace Mdt{ namespace Sql{ namespace Expression{ namespace JoinConstraint{

  struct Grammar;

  struct LiteralValue : boost::proto::or_<
                          boost::proto::terminal< boost::proto::convertible_to<int> > ,
                          boost::proto::terminal< const char * const >
                        >
  {
  };

  struct CompareEqual : boost::proto::or_<
                          boost::proto::equal_to< JoinConstraintField, LiteralValue > ,
                          boost::proto::equal_to< JoinConstraintField, JoinConstraintField >
                        >
  {
  };

  struct CompareNotEqual : boost::proto::or_<
                              boost::proto::not_equal_to< JoinConstraintField , LiteralValue > ,
                              boost::proto::not_equal_to< JoinConstraintField , JoinConstraintField >
                            >
  {
  };

  struct CompareLess : boost::proto::or_<
                          boost::proto::less< JoinConstraintField, LiteralValue > ,
                          boost::proto::less< JoinConstraintField, JoinConstraintField > ,
                          boost::proto::less_equal< JoinConstraintField, LiteralValue > ,
                          boost::proto::less_equal< JoinConstraintField, JoinConstraintField >
                        >
  {
  };

  struct CompareGreater : boost::proto::or_<
                            boost::proto::greater< JoinConstraintField, LiteralValue > ,
                            boost::proto::greater< JoinConstraintField, JoinConstraintField > ,
                            boost::proto::greater_equal< JoinConstraintField, LiteralValue > ,
                            boost::proto::greater_equal< JoinConstraintField, JoinConstraintField >
                          >
  {
  };

  struct Comparison : boost::proto::or_<
                        CompareEqual ,
                        CompareNotEqual ,
                        CompareLess ,
                        CompareGreater
                      >
  {
  };

  struct LogicalAnd : boost::proto::or_<
                        boost::proto::logical_and< Comparison , Comparison > ,
                        boost::proto::logical_and< Grammar , Comparison > ,
                        boost::proto::logical_and< Comparison , Grammar > ,
                        boost::proto::logical_and< Grammar , Grammar >
                      >
  {
  };

  struct LogicalOr : boost::proto::or_<
                        boost::proto::logical_or< Comparison , Comparison > ,
                        boost::proto::logical_or< Grammar , Comparison > ,
                        boost::proto::logical_or< Comparison , Grammar > ,
                        boost::proto::logical_or< Grammar , Grammar >
                      >
  {
  };

  struct Grammar : boost::proto::or_<
                                LogicalAnd ,
                                LogicalOr
                              >
  {
  };

}}}} // namespace Mdt{ namespace Sql{ namespace Expression{ namespace JoinConstraint{

#endif // #ifndef MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_GRAMMAR_H
