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
#ifndef MDT_QUERY_EXPRESSION_ABSTRACT_SELECT_QUERY_FACTORY_H
#define MDT_QUERY_EXPRESSION_ABSTRACT_SELECT_QUERY_FACTORY_H

#include "SelectQuery.h"
#include "MdtQueryExpression_CoreExport.h"

namespace Mdt{ namespace QueryExpression{

  /*! \brief Base class to create query factory implementations
   */
  class AbstractSelectQueryFactory
  {
   public:

    /*! \brief Constructor
     */
    explicit AbstractSelectQueryFactory() = default;

    virtual ~AbstractSelectQueryFactory() = default;

    AbstractSelectQueryFactory(const AbstractSelectQueryFactory &) = delete;
    AbstractSelectQueryFactory & operator=(const AbstractSelectQueryFactory &) = delete;
    AbstractSelectQueryFactory(AbstractSelectQueryFactory &&) = delete;
    AbstractSelectQueryFactory & operator=(AbstractSelectQueryFactory &&) = delete;

    /*! \brief Check if this factory is valid
     *
     * The implementation should return true
     *  when the required setup is done.
     */
    virtual bool isValid() const noexcept = 0;

    /*! \brief Create a select query
     */
    virtual SelectQuery createSelectQuery() const = 0;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_ABSTRACT_SELECT_QUERY_FACTORY_H
