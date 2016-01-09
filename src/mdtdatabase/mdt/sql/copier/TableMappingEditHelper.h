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

    /*! \brief Check if item a is less that item b by regarding there destination addresses
     *
     * Compares the smallest field index of a and b
     */
//     static bool itemIsLessThanByDestinationFieldIndexes(const TableMappingItem & a, const TableMappingItem & b)
//     {
//       qDebug() << "CMP, a: " << a.destinationFieldIndexList() << " , b: " << b.destinationFieldIndexList();
//       const auto aDFIndexes = a.destinationFieldIndexList();
//       const auto bDFIndexes = b.destinationFieldIndexList();
// 
//       if( aDFIndexes.isEmpty() || bDFIndexes.isEmpty() ){
//         return false;
//       }
//       return ( std::min_element(aDFIndexes.cbegin(), aDFIndexes.cend()) <
//                std::min_element(bDFIndexes.cbegin(), bDFIndexes.cend()) );
//     }

    /*! \brief Check if field index list a is less than field index b
     *
     * Will compare the smallest element of a and b
     */
//     static bool fieldIndexLlistIsLessThan(const QVector<int> & a, const QVector<int> & b)
//     {
//       auto itA = std::min_element(a.cbegin(), a.cend());
//       auto itB = std::min_element(b.cbegin(), b.cend());
// 
//       // Case of a empty vector
//       if( (itA == a.cend()) || (itB == b.cend()) ){
//         return false;
//       }
//       qDebug() << "minA: " << *itA << " , minB: " << *itB;
// 
//       return (*itA < *itB);
//     }

    /*! \brief Check if given item refers to to at least one of given detsination field indexes
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
      FieldIndexList destinationFieldIndexList;

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

    /*! \brief Get index where to insert a item regarding its destination field indexes
     *
     * \param destinationFieldIndexList Destination field indexes of item that was freshly edited
     * \param allItemsList The actual items list (before inserting newItem)
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

    /*! \brief Get a list of items to add when editing a item
     *
     * \param newItem The item that was freshly edited
     * \param newItemIndex Index of newItem in allItemsList
     * \param allItemsList The actual items list (before inserting newItem)
     * \pre newItemIndex must be in valid range: 0 <= newItemIndex < allItemsList size
     */
    static QVector<int> getItemsToAddIndexList(const TableMappingItem & newItem, int newItemIndex, const QVector<TableMappingItem> & allItemsList)
    {
      Q_ASSERT(newItemIndex >= 0);
      Q_ASSERT(newItemIndex < allItemsList.size());

      QVector<int> itemsToAdd;
      const auto newItemDFIndexes = newItem.destinationFieldIndexList();
      const auto currentItemDFIndexes = allItemsList.at(newItemIndex).destinationFieldIndexList();
      FieldIndexList destinationFieldIndexList;

      // Get list of currentItem's destination field indexes, excluding those of newItem's ones
      std::set_difference( currentItemDFIndexes.cbegin(), currentItemDFIndexes.cend(),
                           newItemDFIndexes.cbegin(), newItemDFIndexes.cend(),
                           std::back_inserter(destinationFieldIndexList) );
      /*
       * For each DF index in destinationFieldIndexList, a new item must be added.
       * Find index where to insert for each item.
       */
      for(const auto dfIndex : destinationFieldIndexList){
//         qDebug() << "dfIndex: " << dfIndex << " , item IDX: " << getIndexOfItemToInsertByDFIndexes({dfIndex}, allItemsList);
        itemsToAdd.append( getIndexOfItemToInsertByDFIndexes(dfIndex, allItemsList) );
      }
//       // Get list of items indexes that have destination field index of this list
//       for(int idx = 0; idx < allItemsList.size(); ++idx){
//         if(itemContainsDestinationFieldIndex(allItemsList.at(idx), destinationFieldIndexList)){
//           itemsToAdd.append(idx);
//         }
//       }

      return itemsToAdd;
    }

  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_EDIT_HELPER_H
