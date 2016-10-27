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
#ifndef MDT_SQL_EXPRESSION_COMPARISON_SQL_TRANSFORM_H
#define MDT_SQL_EXPRESSION_COMPARISON_SQL_TRANSFORM_H

#include "TerminalSqlTransform.h"
#include <QString>
#include <QVariant>
#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>
#include <boost/proto/transform/when.hpp>
#include <boost/proto/transform.hpp>

namespace Mdt{ namespace Sql{ namespace Expression{

  /*! \brief Callable transform that returns SQL representation for == binary expression
   */
  struct GetCompareEqualToSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  /*! \brief SQL transform for == binary expression
   */
  struct CompareEqualToSqlTransform : boost::proto::when<
                                        boost::proto::equal_to< LeftTerminal , RightTerminal > ,
                                        boost::proto::call<GetCompareEqualToSql( LeftTerminalSqlTransform(boost::proto::_left), RightTerminalSqlTransform(boost::proto::_right) )>
                                      >
  {
  };

  /*! \brief Callable transform that returns SQL representation for != binary expression
   */
  struct GetCompareNotEqualToSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  /*! \brief SQL transform for != binary expression
   */
  struct CompareNotEqualToSqlTransform : boost::proto::when<
                                        boost::proto::not_equal_to< LeftTerminal , RightTerminal > ,
                                        boost::proto::call<GetCompareNotEqualToSql( LeftTerminalSqlTransform(boost::proto::_left), RightTerminalSqlTransform(boost::proto::_right) )>
                                      >
  {
  };

  /*! \brief SQL transform for == or != binary expression
   */
  struct CompareEqualitySqlTransform : boost::proto::or_<
                                          CompareEqualToSqlTransform ,
                                          CompareNotEqualToSqlTransform
                                        >
  {
  };

  /*! \brief Callable transform that returns SQL representation for < binary expression
   */
  struct GetCompareLessSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  /*! \brief Callable transform that returns SQL representation for <= binary expression
   */
  struct GetCompareLessEqualSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  /*! \brief SQL transform for < or <= binary expression
   */
  struct CompareLessSqlTransform : boost::proto::or_<
                                      boost::proto::when<
                                        boost::proto::less< LeftTerminal, RightTerminal > ,
                                        boost::proto::call< GetCompareLessSql( LeftTerminalSqlTransform(boost::proto::_left), RightTerminalSqlTransform(boost::proto::_right) ) >
                                      > ,
                                      boost::proto::when<
                                        boost::proto::less_equal< LeftTerminal, RightTerminal > ,
                                        boost::proto::call< GetCompareLessEqualSql( LeftTerminalSqlTransform(boost::proto::_left), RightTerminalSqlTransform(boost::proto::_right) ) >
                                      >
                                    >
  {
  };

  /*! \brief Callable transform that returns SQL representation for > binary expression
   */
  struct GetCompareGreaterSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  /*! \brief Callable transform that returns SQL representation for >= binary expression
   */
  struct GetCompareGreaterEqualSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  /*! \brief SQL transform for > or >= binary expression
   */
  struct CompareGreaterSqlTransform : boost::proto::or_<
                                          boost::proto::when<
                                            boost::proto::greater< LeftTerminal , RightTerminal > ,
                                            boost::proto::call< GetCompareGreaterSql( LeftTerminalSqlTransform(boost::proto::_left), RightTerminalSqlTransform(boost::proto::_right) ) >
                                          > ,
                                          boost::proto::when<
                                            boost::proto::greater_equal< LeftTerminal , RightTerminal > , 
                                            boost::proto::call< GetCompareGreaterEqualSql( LeftTerminalSqlTransform(boost::proto::_left), RightTerminalSqlTransform(boost::proto::_right) ) >
                                          >
                                        >
  {
  };

  /*! \brief SQL transform for valid comparison binary expression
   */
  struct ComparisonSqlTransform : boost::proto::or_<
                                      CompareEqualitySqlTransform ,
                                      CompareLessSqlTransform ,
                                      CompareGreaterSqlTransform
                                    >
  {
  };

}}} // namespace Mdt{ namespace Sql{ namespace Expression{

#endif // #ifndef MDT_SQL_EXPRESSION_COMPARISON_SQL_TRANSFORM_H
