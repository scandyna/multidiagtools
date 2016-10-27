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
#ifndef MDT_SQL_EXPRESSION_LOGICAL_SQL_TRANSFORM_H
#define MDT_SQL_EXPRESSION_LOGICAL_SQL_TRANSFORM_H

#include <QString>
#include <boost/proto/proto_fwd.hpp>

namespace Mdt{ namespace Sql{ namespace Expression{

  /*! \brief Callable transform that returns SQL representation for && binary expression
   */
  struct GetLogicalAndSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

  /*! \brief Callable transform that returns SQL representation for || binary expression
   */
  struct GetLogicalOrSql : boost::proto::callable
  {
    typedef QString result_type;

    QString operator()(const QString & left, const QString & right) const;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Expression{

#endif // #ifndef MDT_SQL_EXPRESSION_LOGICAL_SQL_TRANSFORM_H
