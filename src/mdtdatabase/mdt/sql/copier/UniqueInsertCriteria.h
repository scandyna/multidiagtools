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
#ifndef MDT_SQL_COPIER_UNIQUE_INSERT_CRITERIA_H
#define MDT_SQL_COPIER_UNIQUE_INSERT_CRITERIA_H

#include "ExpressionMatchItem.h"
#include <vector>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Unique insert criteria for SQL copier
   */
  class UniqueInsertCriteria
  {
   public:

    /*! \brief Check if unique insert criteria is null
     */
    bool isNull() const
    {
      if(pvMatchItems.empty()){
        return true;
      }
      for(const auto & item : pvMatchItems){
        if( (item.sourceValueFieldIndex < 0) || (item.destinationFieldIndex < 0) ){
          return true;
        }
      }
      return false;
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvMatchItems.clear();
    }

    /*! \brief Get count of match items
     */
    int matchItemsCount() const
    {
      return pvMatchItems.size();
    }

    /*! \brief Set match items
     */
    void setMatchItems(const std::vector<ExpressionMatchItem> & items)
    {
      pvMatchItems = items;
    }

    /*! \brief Get match items
     */
    std::vector<ExpressionMatchItem> matchItems() const
    {
      return pvMatchItems;
    }

  private:

    std::vector<ExpressionMatchItem> pvMatchItems;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_UNIQUE_INSERT_CRITERIA_H
