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
   *
   * For example, say we have to import a CSV file
   *  containing clients with their addresses:
   * <table>
   *  <tr>
   *   <th>ClientName</th>
   *   <th>Address</th>
   *  </tr>
   *  <tr>
   *   <td>Client 1</td>
   *   <td>Street 11</td>
   *  </tr>
   *  <tr>
   *   <td>Client 2</td>
   *   <td>Street 21</td>
   *  </tr>
   *  <tr>
   *   <td>Client 2</td>
   *   <td>Street 22</td>
   *  </tr>
   * </table>
   *
   * Notice that Client 2 is listed 2 times (once for each address).
   * The goal is to import clients to Client_tbl (a database table)
   *  in a unique way:
   * <table>
   *  <tr>
   *   <th>Id_PK</th>
   *   <th>Name</th>
   *  </tr>
   *  <tr>
   *   <td>1</td>
   *   <td>Client 1</td>
   *  </tr>
   *  <tr>
   *   <td>2</td>
   *   <td>Client 2</td>
   *  </tr>
   * </table>
   *
   * During copy process, it must be checked that client (in source)
   *  not allready exists in destination (Client_tbl).
   *
   * Unique insert criteria will also be: Client_tbl.Name = CSV.ClientName
   *
   * \code
   * UniqueInsertCriteria uic;
   * std::vector<ExpressionMatchItem> matchItems;
   *
   * // First argument is source value field index (here 0, which is ClientName in CSV)
   * // Second argument is destination field index (here 1, which is Name in Client_tbl)
   * matchItems.emplace_back(0, 1);
   * uic.setMatchItems(matchItems);
   * \endcode
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
