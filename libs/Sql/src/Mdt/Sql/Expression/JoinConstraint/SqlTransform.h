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
#ifndef MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_SQL_TRANSFORM_H
#define MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_SQL_TRANSFORM_H

#include "Mdt/Sql/JoinConstraintField.h"
#include <QString>
#include <QVariant>
#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>
#include <boost/proto/transform/when.hpp>
#include <boost/proto/transform.hpp>

/// \todo See how to split ?
#include "Grammar.h"

class QSqlDatabase;

namespace Mdt{ namespace Sql{ namespace Expression{ namespace JoinConstraint{

  struct SqlTransform;

  struct GetTerminalSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const TableField & tf, const QSqlDatabase & db) const;

    QString operator()(const QVariant & value, const QSqlDatabase & db) const;

  };

  struct LeftTerminalSqlTransform : boost::proto::when<
                                        JoinConstraintField ,
                                        boost::proto::call<GetTerminalSql(boost::proto::_value, boost::proto::_data)>
                                      >
  {
  };

  struct RightTerminalSqlTransform : boost::proto::or_<
                                        LeftTerminalSqlTransform ,
                                        boost::proto::when<
                                          LiteralValue ,
                                          boost::proto::call<GetTerminalSql(boost::proto::_value, boost::proto::_data)>
                                        >
                                      >
  {
  };

  struct GetCompareEqualToSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  struct CompareEqualToSqlTransform : boost::proto::when<
                                        boost::proto::equal_to<LeftTerminalSqlTransform, RightTerminalSqlTransform> ,
                                        boost::proto::call<GetCompareEqualToSql( LeftTerminalSqlTransform(boost::proto::_left), RightTerminalSqlTransform(boost::proto::_right) )>
                                      >
  {
  };

  struct GetCompareNotEqualToSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  struct CompareNotEqualToSqlTransform : boost::proto::when<
                                          boost::proto::not_equal_to<LeftTerminalSqlTransform, RightTerminalSqlTransform> ,
                                          boost::proto::call<GetCompareNotEqualToSql( LeftTerminalSqlTransform(boost::proto::_left), RightTerminalSqlTransform(boost::proto::_right) )>
                                      >
  {
  };

  struct CompareEqualitySqlTransform : boost::proto::or_<
                                          CompareEqualToSqlTransform ,
                                          CompareNotEqualToSqlTransform
                                        >
  {
  };

  struct GetCompareLessSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  struct GetCompareLessEqualSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  struct CompareLessSqlTransform : boost::proto::or_<
                                      boost::proto::when<
                                        boost::proto::less< LeftTerminalSqlTransform, RightTerminalSqlTransform > ,
                                        boost::proto::call< GetCompareLessSql( LeftTerminalSqlTransform(boost::proto::_left), RightTerminalSqlTransform(boost::proto::_right) ) >
                                      > ,
                                      boost::proto::when<
                                        boost::proto::less_equal< LeftTerminalSqlTransform, RightTerminalSqlTransform > ,
                                        boost::proto::call< GetCompareLessEqualSql( LeftTerminalSqlTransform(boost::proto::_left), RightTerminalSqlTransform(boost::proto::_right) ) >
                                      >
                                    >
  {
  };

  struct GetCompareGreaterSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  struct GetCompareGreaterEqualSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  struct CompareGreaterSqlTransform : boost::proto::or_<
                                          boost::proto::when<
                                            boost::proto::greater< LeftTerminalSqlTransform , RightTerminalSqlTransform > ,
                                            boost::proto::call< GetCompareGreaterSql( LeftTerminalSqlTransform(boost::proto::_left), RightTerminalSqlTransform(boost::proto::_right) ) >
                                          > ,
                                          boost::proto::when<
                                            boost::proto::greater_equal< LeftTerminalSqlTransform , RightTerminalSqlTransform > , 
                                            boost::proto::call< GetCompareGreaterEqualSql( LeftTerminalSqlTransform(boost::proto::_left), RightTerminalSqlTransform(boost::proto::_right) ) >
                                          >
                                        >
  {
  };

  struct ComparisonSqlTransform : boost::proto::or_<
                                      CompareEqualitySqlTransform ,
                                      CompareLessSqlTransform ,
                                      CompareGreaterSqlTransform
                                    >
  {
  };

  struct GetLogicalAndSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  struct LogicalAndSqlTransform : boost::proto::when<
                                      boost::proto::logical_and< SqlTransform , SqlTransform > ,
                                      boost::proto::call< GetLogicalAndSql( SqlTransform(boost::proto::_left), SqlTransform(boost::proto::_right) ) >
                                    >
  {
  };

  struct GetLogicalOrSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  struct LogicalOrSqlTransform : boost::proto::when<
                                    boost::proto::logical_or< SqlTransform, SqlTransform > ,
                                    boost::proto::call< GetLogicalOrSql( SqlTransform(boost::proto::_left), SqlTransform(boost::proto::_right) ) >
                                  >
  {
  };

  struct SqlTransform : boost::proto::or_<
                          LogicalAndSqlTransform ,
                          LogicalOrSqlTransform ,
                          ComparisonSqlTransform
                        >
  {
  };

}}}} // namespace Mdt{ namespace Sql{ namespace Expression{ namespace JoinConstraint{

#endif // #ifndef MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_SQL_TRANSFORM_H
