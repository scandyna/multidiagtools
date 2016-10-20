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

//   struct GetJoinConstraintFieldSql : boost::proto::callable
//   {
//     typedef QString result_type;
// 
//     QString operator()(const JoinConstraintField & f, const QSqlDatabase & db) const;
//   };

  struct GetTerminalSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const JoinConstraintField & f, const QSqlDatabase & db) const;

    template<typename T>
    QString operator()(const T & expr, const QSqlDatabase & db) const
    {
      return QVariant( boost::proto::value(expr) ).toString();
    }
  };

//   struct GetTerminalSql : boost::proto::or_<
//                             boost::proto::when<
//                               JoinConstraintField ,
//                               boost::proto::call<GetJoinConstraintFieldSql(boost::proto::_value, boost::proto::_data)>
//                             >
//                           >
//   {
//   };

//   struct CatSql : boost::proto::callable
//   {
//     typedef QString result_type;
// 
//     QString operator()(const QString & left, const QString & right, const char * const op) const
//     {
//       return "CAT";
//     }
//   };

  struct GetCompareEqualSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  struct GetComparisonSql : boost::proto::or_<
                              boost::proto::when<
                                CompareEqual ,
                                boost::proto::call< GetCompareEqualSql(
                                  boost::proto::call<GetTerminalSql(boost::proto::_left, boost::proto::_data)> ,
                                  boost::proto::call<GetTerminalSql(boost::proto::_right, boost::proto::_data)>
                                ) >
                              >
                            >
  {
  };

  struct GetLogicalAndSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  struct LogicalAndSqlTransform : boost::proto::or_<
                                    boost::proto::when<
                                      boost::proto::logical_and< Comparison , Comparison > ,
                                      boost::proto::call< GetLogicalAndSql(
                                                            GetComparisonSql(boost::proto::_left) ,
                                                            GetComparisonSql(boost::proto::_right)
                                                          ) >
                                    > ,
                                    boost::proto::when<
                                      boost::proto::logical_and< SqlTransform , Comparison > ,
                                      boost::proto::call< GetLogicalAndSql(
                                                            SqlTransform(boost::proto::_left) , /// \todo Seems to work, but why ??
                                                            GetComparisonSql(boost::proto::_right)
                                                          ) >
                                    >
                                  >
  {
  };

  struct SqlTransform : boost::proto::or_<
                          boost::proto::when<
                            Comparison ,
                            GetComparisonSql
                          > ,
                          boost::proto::when<
                            LogicalAnd ,
                            LogicalAndSqlTransform
                          >
                        >
  {
  };

}}}} // namespace Mdt{ namespace Sql{ namespace Expression{ namespace JoinConstraint{

#endif // #ifndef MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_SQL_TRANSFORM_H
