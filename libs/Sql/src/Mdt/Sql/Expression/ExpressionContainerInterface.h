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
#ifndef MDT_SQL_EXPRESSION_EXPRESSION_CONTAINER_INTERFACE_H
#define MDT_SQL_EXPRESSION_EXPRESSION_CONTAINER_INTERFACE_H

#include <QString>

class QSqlDatabase;

namespace Mdt{ namespace Sql{ namespace Expression{

  /*! \brief Interface to implement expression container used by ExpressionContainer
   */
  struct ExpressionContainerInterface
  {
    // Constructors / destructors
    ExpressionContainerInterface() = default;
    virtual ~ExpressionContainerInterface() = default;
    // Copy disabled
    ExpressionContainerInterface(const ExpressionContainerInterface &) = delete;
    ExpressionContainerInterface & operator=(const ExpressionContainerInterface &) = delete;
    // Move disabled
    ExpressionContainerInterface(ExpressionContainerInterface &&) = delete;
    ExpressionContainerInterface & operator=(ExpressionContainerInterface &&) = delete;

    /*! \brief Get SQL string of the expression
     *
     * \pre db must be valid (must have a driver loaded)
     */
    virtual QString toSql(const QSqlDatabase & db) const = 0;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Expression{

#endif // #ifndef MDT_SQL_EXPRESSION_EXPRESSION_CONTAINER_INTERFACE_H
