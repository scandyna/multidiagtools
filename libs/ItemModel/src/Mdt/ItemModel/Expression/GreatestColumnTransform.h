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
#ifndef MDT_ITEM_MODEL_EXPRESSION_GREATEST_COLUMN_TRANSFORM_H
#define MDT_ITEM_MODEL_EXPRESSION_GREATEST_COLUMN_TRANSFORM_H

#include "../FilterColumn.h"
#include "../ParentModelColumn.h"
#include "ColumnTerminal.h"
#include <boost/proto/context.hpp>

namespace Mdt{ namespace ItemModel{ namespace Expression{

  /*! \brief Evaluation context to get greatest columns in a filter expression
   */
  struct GetGreatestColumnContext : boost::proto::callable_context<GetGreatestColumnContext, boost::proto::null_context>
  {
    typedef void result_type;

    template<typename T>
    void operator()(boost::proto::tag::terminal, const T & t)
    {
      evalTerminal(t);
    }

    /*! \brief Get greatest column
     */
    int greatestColumn() const
    {
      return mGreatestColumn;
    }

    /*! \brief Get greatest parent model column
     */
    int greatestParentModelColumn() const
    {
      return mGreatestParentModelColumn;
    }

   private:

    void evalTerminal(const FilterColumnData & fcd);

    void evalTerminal(const ParentModelColumnData & pmcd);

    template<typename T>
    void evalTerminal(const T &)
    {
    }

    int mGreatestColumn = -1;
    int mGreatestParentModelColumn = -1;
  };

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{

#endif // #ifndef MDT_ITEM_MODEL_EXPRESSION_GREATEST_COLUMN_TRANSFORM_H
