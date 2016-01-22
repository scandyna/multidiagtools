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
    //static std::vector<int> getItemsToRemoveIndexList(const FieldIndexList & itemDfiList, const QVector<TableMappingItem> & tmItemList)
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

    /*! \brief Get a list of destination field indexes to add when inserting a item
     *
     * \param itemDfiList List of destination field indexes of item to insert
     * \param dfiList List of destination field indexes of item to compare
     */
//     static FieldIndexList getDfiToAddList(const FieldIndexList & itemDfiList, const FieldIndexList & dfiList)
//     {
//       FieldIndexList toAddDfiList;
// 
// //       if(dfiList.count() < 2){
// //         return toAddDfiList;
// //       }
//       // Get list of newItem's destination field indexes, excluding those of currentItem's ones
//       // Get itemDfiList (item to insert), excluding those of dfiList (item to compare)
//       std::set_difference( itemDfiList.cbegin(), itemDfiList.cend(),
//                            dfiList.cbegin(), dfiList.cend(),
//                            std::back_inserter(toAddDfiList) );
// 
//       return toAddDfiList;
//     }

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

    /*! \brief Get a list of destination field indexes to create to end up with a complete table mapping
     *
     * \param itemDfiList List of destination field indexes of item to insert
     * \param tmDfiList List of destination field index of all items, excluding those to remove
     */
//     static FieldIndexList getItemsToAddDfiList(const FieldIndexList & itemDfiList, const std::vector<FieldIndexList> & tmDfiList)
//     {
//       FieldIndexList toCreateDfiList;
//       FieldIndexList currentDfiList;
//       FieldIndexList newDfiList;
// 
//       // Build current DFI list, which contains those of given table mapping
//       for(const auto & dfiList : tmDfiList){
//         std::copy(dfiList.cbegin(), dfiList.cend(), std::back_inserter(currentDfiList));
//       }
//       // Add item's to insert DFI list to current
//       std::copy(itemDfiList.cbegin(), itemDfiList.cend(), std::back_inserter(currentDfiList));
//       // Get the max DFI in current DFI list
//       auto maxDfiIt = std::max_element(currentDfiList.cbegin(), currentDfiList.cend());
//       if(maxDfiIt == currentDfiList.cend()){
//         return toCreateDfiList;
//       }
//       int maxDfi = *maxDfiIt;
//       // Create new DFI list
//       newDfiList.reserve(maxDfi+1);
//       for(int i = 0; i <= maxDfi; ++i){
//         newDfiList.append(i);
//       }
//       // Build list of DFI to create, which is newDfiList - currentDfiList
//       std::set_difference( newDfiList.cbegin(), newDfiList.cend(),
//                            currentDfiList.cbegin(), currentDfiList.cend(),
//                            std::back_inserter(toCreateDfiList) );
// 
//       return toCreateDfiList;
//     }

    /*! \brief Check if given item refers to to at least one of given detsination field indexes
     *
     * \deprecated
     */
    static bool itemContainsDestinationFieldIndex(const TableMappingItem & item, const FieldIndexList & destinationFieldIndexList)
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

    /*! \brief Get a list of destination field indexes to remove when inserting a item
     *
     * \param newItemIndex Index of newItem in allItemsList
     * \param newItem The item that was freshly edited
     * \param allItemsList The actual items list (before inserting newItem)
     * \pre newItemIndex must be in valid range: 0 <= newItemIndex < allItemsList size
     * \deprecated
     */
    static FieldIndexList getDestinationFieldIndexToRemoveList(int newItemIndex, const TableMappingItem & newItem, const QVector<TableMappingItem> & allItemsList)
    {
      Q_ASSERT(newItemIndex >= 0);
      Q_ASSERT(newItemIndex < allItemsList.size());

      FieldIndexList DFIndexToRemoveList;
      const auto newItemDFIndexes = newItem.destinationFieldIndexList();
      const auto currentItemDFIndexes = allItemsList.at(newItemIndex).destinationFieldIndexList();

      // Get list of newItem's destination field indexes, excluding those of currentItem's ones
      std::set_difference( newItemDFIndexes.cbegin(), newItemDFIndexes.cend(),
                           currentItemDFIndexes.cbegin(), currentItemDFIndexes.cend(),
                           std::back_inserter(DFIndexToRemoveList) );

      return DFIndexToRemoveList;
    }

    /*! \brief Get a list of items to remove when editing a item
     *
     * \param newItemIndex Index of newItem in allItemsList
     * \param newItem The item that was freshly edited
     * \param allItemsList The actual items list (before inserting newItem)
     * \pre newItemIndex must be in valid range: 0 <= newItemIndex < allItemsList size
     * \deprecated
     */
    static QVector<int> getItemsToRemoveIndexList(int newItemIndex, const TableMappingItem & newItem, const QVector<TableMappingItem> & allItemsList)
    {
      Q_ASSERT(newItemIndex >= 0);
      Q_ASSERT(newItemIndex < allItemsList.size());

      QVector<int> itemsToRemove;
      const auto DFIndexToRemoveList = getDestinationFieldIndexToRemoveList(newItemIndex, newItem, allItemsList);
//       const auto newItemDFIndexes = newItem.destinationFieldIndexList();
//       const auto currentItemDFIndexes = allItemsList.at(newItemIndex).destinationFieldIndexList();
//       FieldIndexList destinationFieldIndexList;

//       // Get list of newItem's destination field indexes, excluding those of currentItem's ones
//       std::set_difference( newItemDFIndexes.cbegin(), newItemDFIndexes.cend(),
//                            currentItemDFIndexes.cbegin(), currentItemDFIndexes.cend(),
//                            std::back_inserter(destinationFieldIndexList) );
      // Get list of items indexes that have destination field index of this list
      for(int idx = 0; idx < allItemsList.size(); ++idx){
        if(itemContainsDestinationFieldIndex(allItemsList.at(idx), DFIndexToRemoveList)){
          itemsToRemove.append(idx);
        }
      }

      return itemsToRemove;
    }

    /*! \brief Get a list of items to add when editing a item
     *
     * \param newItemIndex Index of newItem in allItemsList
     * \param newItem The item that was freshly edited
     * \param allItemsList The actual items list (before inserting newItem)
     * \pre newItemIndex must be in valid range: 0 <= newItemIndex < allItemsList size
     * \deprecated
     */
    static QVector<TableMappingItem> getItemsToAddList(int newItemIndex, const TableMappingItem & newItem, const QVector<TableMappingItem> & allItemsList)
    {
      Q_ASSERT(newItemIndex >= 0);
      Q_ASSERT(newItemIndex < allItemsList.size());

      QVector<TableMappingItem> itemsToAdd;
      const auto newItemDFIndexes = newItem.destinationFieldIndexList();
      const auto currentItemDFIndexes = allItemsList.at(newItemIndex).destinationFieldIndexList();
      FieldIndexList destinationFieldIndexList;

      // Get list of currentItem's destination field indexes, excluding those of newItem's ones
      std::set_difference( currentItemDFIndexes.cbegin(), currentItemDFIndexes.cend(),
                           newItemDFIndexes.cbegin(), newItemDFIndexes.cend(),
                           std::back_inserter(destinationFieldIndexList) );
      /*
       * For each DF index in destinationFieldIndexList, a new item must be added.
       */
      for(const auto dfIndex : destinationFieldIndexList){
        TableMappingItem item(TableMappingItem::FieldMappingType);
        item.setFieldMapping(-1, dfIndex);
        itemsToAdd.append(item);
      }

      return itemsToAdd;
    }

    /*! \brief Get index where to insert a item regarding its destination field indexes
     *
     * \param destinationFieldIndexList Destination field indexes of item that was freshly edited
     * \param allItemsList The actual items list (before inserting newItem)
     * \deprecated
     */
    static int getIndexOfItemToInsertByDFIndexes(const FieldIndexList & destinationFieldIndexList, const QVector<TableMappingItem> & allItemsList)
    {
      for(int i = 0; i < allItemsList.size(); ++i){
        if( destinationFieldIndexList < allItemsList.at(i).destinationFieldIndexList() ){
          return i;
        }
      }
      return allItemsList.size();
    }

    static int getIndexOfItemToInsertByDFIndexes(int destinationFieldIndex, const QVector<TableMappingItem> & allItemsList)
    {
      FieldIndexList destinationFieldIndexList;
      destinationFieldIndexList.append(destinationFieldIndex);
      return getIndexOfItemToInsertByDFIndexes(destinationFieldIndexList, allItemsList);
    }

    /*! \brief Update table mapping list by inserting given item
     *
     * \param newItemIndex Current index of newItem in allItemsList
     * \param newItem The item that was freshly edited
     * \param allItemsList The actual items list (before inserting newItem)
     * \pre newItemIndex must be in valid range: 0 <= newItemIndex < allItemsList size
     * \deprecated
     */
    static void insertItem(int newItemIndex, const TableMappingItem & newItem, QVector<TableMappingItem> & allItemsList)
    {
      Q_ASSERT(newItemIndex >= 0);
      Q_ASSERT(newItemIndex < allItemsList.size());

      QVector<TableMappingItem> editedItemsList;
//       const auto itemsToRemoveIndexList = getItemsToRemoveIndexList(newItem, newItemIndex, allItemsList);
      const auto DFIndexToRemoveList = getDestinationFieldIndexToRemoveList(newItemIndex, newItem, allItemsList);
      const auto itemsToAddList = getItemsToAddList(newItemIndex, newItem, allItemsList);

      // Remove the item that is in edition
      qDebug() << "-> Removing item at index " << newItemIndex;
      allItemsList.remove(newItemIndex);
      // Copy items from allItemsList that have not DF that must be removed
      for(const auto & item : allItemsList){
        if(!itemContainsDestinationFieldIndex(item, DFIndexToRemoveList)){
          qDebug() << "-> Copy to edited, item DF " << debugFIL(item.destinationFieldIndexList());
          editedItemsList.append(item);
        }
      }
      // Insert new items
      for(const auto & item : itemsToAddList){
        const int idx = getIndexOfItemToInsertByDFIndexes(item.destinationFieldIndexList(), allItemsList);
        qDebug() << "-> Insert at idx " << idx << " , item DF " << debugFIL(item.destinationFieldIndexList());
        editedItemsList.insert(idx, item);
      }
      // Insert edited item
      const int idx = getIndexOfItemToInsertByDFIndexes(newItem.destinationFieldIndexList(), allItemsList);
      qDebug() << "-> Insert E at idx " << idx << " , item DF " << debugFIL(newItem.destinationFieldIndexList());
      editedItemsList.insert(idx, newItem);
      // Affect new table mapping
      allItemsList = editedItemsList;
    }

    static QString debugFIL(const FieldIndexList & dfil)
    {
      QString str;
      for(const auto & dfi : dfil){
        str += QString::number(dfi) + ",";
      }
      return str;
    }
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_EDIT_HELPER_H
