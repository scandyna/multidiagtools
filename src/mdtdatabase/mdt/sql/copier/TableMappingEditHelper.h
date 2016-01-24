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
#include <vector>

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
   * If we  insert the following item:
   * <table border="1" cellpadding="5">
   *  <tr><th>Source<br>field index</th><th>Destination<br>field indexes</th></tr>
   *  <tr align="center"><td>0</td><td>0,1</td></tr>
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

    /*! \brief Check if map item contains at least one of destination field index
     *
     * \param itemDfiList Destination field index list of item to check
     * \param dfiList Destination field index list maybe contained
     */
    static bool itemContainsDfiList(const FieldIndexList & itemDfiList, const FieldIndexList & dfiList)
    {
      if( itemDfiList.isEmpty() || dfiList.isEmpty() ){
        return false;
      }
      return ( std::find_first_of( itemDfiList.cbegin(), itemDfiList.cend(),
                                   dfiList.cbegin(), dfiList.cend()
                                 ) != itemDfiList.cend() );
    }

    /*! \brief Get a list of item indexes to remove when inserting a item
     *
     * \param itemDfiList List of destination field indexes of item to insert
     * \param tmItemList All items from table mapping
     * \param ignoreNotMappedItems If true, items that are not mapped will not be added to the list
     */
    static QVector<int> getItemsToRemoveIndexList(const FieldIndexList & itemDfiList, const QVector<TableMappingItem> & tmItemList, bool ignoreNotMappedItems)
    {
      //std::vector<int> itemIndexList;
      QVector<int> itemIndexList;

      for(int index = 0; index < tmItemList.size(); ++index){
        if(ignoreNotMappedItems && (tmItemList.at(index).mappingState() == TableMappingItemState::MappingNotSet) ){
          continue;
        }
        if(itemContainsDfiList(tmItemList.at(index).destinationFieldIndexList(), itemDfiList)){
          itemIndexList.push_back(index);
        }
      }
      qDebug() << "--> indexes to remove: " << itemIndexList.size();

      return itemIndexList;
    }

    /*! \brief Remove items to remove when inserting a item
     *
     * \param itemDfiList List of destination field indexes of item to insert
     * \param tmItemList All items from table mapping
     */
    static void removeItems(const FieldIndexList & itemDfiList, QVector<TableMappingItem> & tmItemList)
    {
      qDebug() << "--> Remove items, tmItemList size: " << tmItemList.size() << " , remove ...";
      auto pred = [&itemDfiList](const TableMappingItem & item){
        qDebug() << "--> Contains: " << itemContainsDfiList(item.destinationFieldIndexList(), itemDfiList);
        return itemContainsDfiList(item.destinationFieldIndexList(), itemDfiList);
      };
      auto newEnd = std::remove_if(tmItemList.begin(), tmItemList.end(), pred);
      if(newEnd == tmItemList.end()){
        return;
      }
      tmItemList.erase(newEnd, tmItemList.end());
      qDebug() << "--> Remove items done, tmItemList size: " << tmItemList.size();
    }

    /*! \brief Get a list of table mapping items to create to end up with a complete table mapping
     *
     * \param itemDfiList List of destination field indexes of item to insert
     * \param tmItemList All items from table mapping
     */
    static QVector<TableMappingItem> getItemsToCreateList(const FieldIndexList & itemDfiList, const QVector<TableMappingItem> & tmItemList)
    {
      QVector<TableMappingItem> toCreateItems;
      FieldIndexList toCreateDfiList;
      FieldIndexList currentDfiList;
      FieldIndexList newDfiList;

      // Build current DFI list, which contains those of given table mapping
      for(const auto & item : tmItemList){
        const auto & dfiList = item.destinationFieldIndexList();
        std::copy(dfiList.cbegin(), dfiList.cend(), std::back_inserter(currentDfiList));
      }
      // Add item's to insert DFI list to current
      std::copy(itemDfiList.cbegin(), itemDfiList.cend(), std::back_inserter(currentDfiList));
      // Get the max DFI in current DFI list
      auto maxDfiIt = std::max_element(currentDfiList.cbegin(), currentDfiList.cend());
      if(maxDfiIt == currentDfiList.cend()){
        return toCreateItems;
      }
      int maxDfi = *maxDfiIt;
      // Create new DFI list
      newDfiList.reserve(maxDfi+1);
      for(int i = 0; i <= maxDfi; ++i){
        newDfiList.append(i);
      }
      // Build list of DFI to create, which is newDfiList - currentDfiList
      std::sort(newDfiList.begin(), newDfiList.end());
      std::sort(currentDfiList.begin(), currentDfiList.end());
      std::set_difference( newDfiList.cbegin(), newDfiList.cend(),
                           currentDfiList.cbegin(), currentDfiList.cend(),
                           std::back_inserter(toCreateDfiList) );
      // Create a field mapping (with no source field) for each DFI to create
      for(int dfi : toCreateDfiList){
        TableMappingItem item(TableMappingItem::FieldMappingType);
        item.setFieldMapping(-1, dfi);
        toCreateItems.append(item);
      }

      return toCreateItems;
    }

    /*! \brief Insert given item in given table mapping item list
     *
     * \param item item to insert
     * \param tmItemList All items from table mapping
     * \pre item must have at least 1 DFI set
     */
    static void insertItem(const TableMappingItem & item, QVector<TableMappingItem> & tmItemList)
    {
      Q_ASSERT(!item.destinationFieldIndexList().isEmpty());
      Q_ASSERT(item.destinationFieldIndexList().at(0) >= 0);

      // Remove items that must be
      removeItems(item.destinationFieldIndexList(), tmItemList);
      // Create items that must be
      const auto itemsToCreate = getItemsToCreateList(item.destinationFieldIndexList(), tmItemList);
      std::copy(itemsToCreate.cbegin(), itemsToCreate.cend(), std::back_inserter(tmItemList));
      // Add item
      tmItemList.append(item);
      // Sort items by DFI
      std::sort(tmItemList.begin(), tmItemList.end());
    }
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_EDIT_HELPER_H
