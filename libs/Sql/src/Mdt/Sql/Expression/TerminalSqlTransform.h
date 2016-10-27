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
#ifndef MDT_SQL_EXPRESSION_TERMINAL_SQL_TRANSFORM_H
#define MDT_SQL_EXPRESSION_TERMINAL_SQL_TRANSFORM_H

#include "Terminal.h"
#include <QString>
#include <QVariant>
#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>
#include <boost/proto/transform/when.hpp>
#include <boost/proto/transform.hpp>

class QSqlDatabase;

namespace Mdt{ namespace Sql{ namespace Expression{

  /*! \brief Callable transform that returns SQL representation of a TableField from a TableFieldTerminal or a LiteralValue
   */
  struct GetTerminalSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const TableField & tf, const QSqlDatabase & db) const;

    QString operator()(const QVariant & value, const QSqlDatabase & db) const;
  };

  /*! \brief Terminal SQL transform for the left part of a binary expression
   */
  struct LeftTerminalSqlTransform : boost::proto::when<
                                        LeftTerminal ,
                                        boost::proto::call<GetTerminalSql(boost::proto::_value, boost::proto::_data)>
                                      >
  {
  };

  /*! \brief Terminal SQL transform for the right part of a binary expression
   */
  struct RightTerminalSqlTransform : boost::proto::or_<
                                        LeftTerminalSqlTransform ,
                                        boost::proto::when<
                                          LiteralValue ,
                                          boost::proto::call<GetTerminalSql(boost::proto::_value, boost::proto::_data)>
                                        >
                                      >
  {
  };

}}} // namespace Mdt{ namespace Sql{ namespace Expression{

#endif // #ifndef MDT_SQL_EXPRESSION_TERMINAL_SQL_TRANSFORM_H
