/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_EXPRESSION_GET_RELATION_KEY_FOR_EQUALITY_H
#define MDT_ITEM_MODEL_EXPRESSION_GET_RELATION_KEY_FOR_EQUALITY_H

#include "../FilterColumn.h"
#include "../ParentModelColumn.h"
#include "../RelationKey.h"
#include "MdtItemModelExport.h"
#include "ColumnTerminal.h"
#include <boost/proto/context.hpp>

namespace Mdt{ namespace ItemModel{ namespace Expression{

  /*! \brief Callable context to get a RelationKey for equality
   */
  struct MDT_ITEMMODEL_EXPORT GetRelationKeyForEqualityContext : boost::proto::callable_context<GetRelationKeyForEqualityContext, boost::proto::null_context>
  {
    typedef void result_type;

    template<typename L, typename R>
    void operator()(boost::proto::tag::equal_to, const L & left, const R & right)
    {
      evalNode( boost::proto::value(left), boost::proto::value(right) );
    }

    template<typename L, typename R>
    void operator()(boost::proto::tag::less_equal, const L & left, const R & right)
    {
      evalNode( boost::proto::value(left), boost::proto::value(right) );
    }

    template<typename L, typename R>
    void operator()(boost::proto::tag::greater_equal, const L & left, const R & right)
    {
      evalNode( boost::proto::value(left), boost::proto::value(right) );
    }

    RelationKey relationKey() const
    {
      return mKey;
    }

   private:

    void evalNode(const FilterColumnData & fcd, const ParentModelColumnData & pmcd);

    template<typename L, typename R>
    void evalNode(const L &, const R &)
    {
    }

    RelationKey mKey;
  };

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{

#endif // #ifndef MDT_ITEM_MODEL_EXPRESSION_GET_RELATION_KEY_FOR_EQUALITY_H
