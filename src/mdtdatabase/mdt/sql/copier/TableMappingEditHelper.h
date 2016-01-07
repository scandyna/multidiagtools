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
#ifndef MDT_SQL_COPIER_TABLE_MAPPING_EDIT_HELPER_H
#define MDT_SQL_COPIER_TABLE_MAPPING_EDIT_HELPER_H

#include "TableMappingItem.h"
#include <QVector>
#include <algorithm>
#include <iterator>

#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Helper class to edit TableMapping
   *
   * Take for example the following table mapping :
   * <table border="1" cellpadding="5">
   *  <tr><th>TableMappingItem<br>index</th><th>Source<br>field index</th><th>Destination<br>field indexes</th></tr>
   *  <tr align="center"><td>0</td><td>1</td><td>0</td></tr>
   *  <tr align="center"><td>1</td><td>0</td><td>1</td></tr>
   *  <tr align="center"><td>2</td><td>2</td><td>2</td></tr>
   * </table>
   * If we edit item at index 1 as following:
   * <table border="1" cellpadding="5">
   *  <tr><th>TableMappingItem<br>index</th><th>Source<br>field index</th><th>Destination<br>field indexes</th></tr>
   *  <tr align="center"><td>1</td><td>0</td><td>0,1</td></tr>
   * </table>
   * We will end up with forllowing table mapping:
   * <table border="1" cellpadding="5">
   *  <tr><th>TableMappingItem<br>index</th><th>Source<br>field index</th><th>Destination<br>field indexes</th></tr>
   *  <tr align="center"><td>0</td><td>1</td><td>0,1</td></tr>
   *  <tr align="center"><td>1</td><td>2</td><td>2</td></tr>
   * </table>
   *
   */
  class TableMappingEditHelper
  {
   public:

    /*! \brief Check if given item refers to to at least one of given detsination field indexes
     */
    static bool itemContainsDestinationFieldIndex(const TableMappingItem & item, const QVector<int> & destinationFieldIndexList)
    {
      const auto itemDFIndexes = item.destinationFieldIndexList();

      if(destinationFieldIndexList.isEmpty()){
        return false;
      }
      if(itemDFIndexes.isEmpty()){
        return false;
      }

      return (std::find_first_of( itemDFIndexes.cbegin(), itemDFIndexes.cend(),
                                  destinationFieldIndexList.cbegin(), destinationFieldIndexList.cend()
                                ) != itemDFIndexes.cend() );
    }

    /*! \brief Get a list of items to remove when editing a item
     *
     * \param newItem The item that was freshly edited
     * \param newItemIndex Index of newItem in allItemsList
     * \param allItemsList The actual items list (before inserting newItem)
     * \pre newItemIndex must be in valid range: 0 <= newItemIndex < allItemsList size
     */
    static QVector<int> getItemsToRemoveIndexList(const TableMappingItem & newItem, int newItemIndex, const QVector<TableMappingItem> & allItemsList)
    {
      Q_ASSERT(newItemIndex >= 0);
      Q_ASSERT(newItemIndex < allItemsList.size());

      QVector<int> itemsToRemove;
      const auto newItemDFIndexes = newItem.destinationFieldIndexList();
      const auto currentItemDFIndexes = allItemsList.at(newItemIndex).destinationFieldIndexList();
      QVector<int> destinationFieldIndexList;

      // Get list of newItem's destination field indexes, excluding those of currentItem's ones
      std::set_difference( newItemDFIndexes.cbegin(), newItemDFIndexes.cend(),
                           currentItemDFIndexes.cbegin(), currentItemDFIndexes.cend(),
                           std::back_inserter(destinationFieldIndexList) );
      // Get list of items indexes that have destination field index of this list
      for(int idx = 0; idx < allItemsList.size(); ++idx){
        if(itemContainsDestinationFieldIndex(allItemsList.at(idx), destinationFieldIndexList)){
          itemsToRemove.append(idx);
        }
      }

      return itemsToRemove;
    }
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_EDIT_HELPER_H
