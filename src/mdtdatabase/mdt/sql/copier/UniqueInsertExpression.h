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
#ifndef MDT_SQL_COPIER_UNIQUE_INSERT_EXPRESSION_H
#define MDT_SQL_COPIER_UNIQUE_INSERT_EXPRESSION_H

#include "AbstractTableMappingItem.h"
#include "ExpressionMatchItem.h"

#include "mdtSqlWhereOperator.h"

#include <QVector>
#include <vector>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Unique insert expression for SQL copier
   *
   * Can be used when copying primary keys has no sense,
   *  or source does not contain any primary key.
   *  To solve this, we must provide a couple of fields
   *  in source that can identify a record uniquely in destination.
   *
   * Then, at insertion time (into destination),
   *  the defined (primary) key is fetched.
   */
  class UniqueInsertExpression : public AbstractTableMappingItem
  {
   public:

    /*! \brief Create a invalid expression
     */
    UniqueInsertExpression()
     : AbstractTableMappingItem()
    {
      qDebug() << "C  UniqueInsertExpression::UniqueInsertExpression() - ref: " << ref.load();
    }

    /*! \brief Destructor
     */
    ~UniqueInsertExpression()
    {
      qDebug() << "D  UniqueInsertExpression::~UniqueInsertExpression() - ref: " << ref.load();
    }

    /*! \brief Copy constructor
     */
    UniqueInsertExpression(const UniqueInsertExpression & other)
     : AbstractTableMappingItem(other),
       pvMatchItems(other.pvMatchItems)
    {
      qDebug() << "CPY  UniqueInsertExpression::UniqueInsertExpression(other) - ref: " << ref.load();
    }

    /*! \brief Copy assignment
     */
    UniqueInsertExpression & operator=(const UniqueInsertExpression & other)
    {
      if(&other != this){
        copyMembersOfAbstract(other);
        pvMatchItems = other.pvMatchItems;
      }
      return *this;
    }

    /*! \brief Polymorphic copy
     */
    UniqueInsertExpression* clone() const override
    {
      qDebug() << " UniqueInsertExpression::clone() - ref: " << ref.load();
      return new UniqueInsertExpression(*this);
    }

    /*! \brief Check if field mapping is null
     *
     * Expression is null if not match was set or not destination was set
     */
    bool isNull() const override
    {
      return ( pvMatchItems.empty() || (destinationFieldIndexCount() < 1) );
    }

    /*! \brief Add a destination field index
     */
    void addDestinationFieldIndex(int index)
    {
      Q_ASSERT(index >= 0);
      AbstractTableMappingItem::addDestinationFieldIndex(index);
    }

    /*! \brief Set destination field index list
     */
    void setDestinationFieldIndexList(const FieldIndexList & indexList)
    {
      AbstractTableMappingItem::setDestinationFieldIndexList(indexList);
    }

    /*! \brief Add a match item
     *
     * A match item is a criteria between a value of source
     *  and a field in destination.
     *
     * \param sourceValueFieldIndex Index of field, in source, in which value will be fetched during copy
     * \param destinationFieldIndex Index of field, in destination, for which the value must match
     * \pre sourceValueFieldIndex and destinationFieldIndex must be >= 0
     */
    void addMatchItem(int sourceValueFieldIndex, int destinationFieldIndex)
    {
//       Q_ASSERT(sourceValueFieldIndex >= 0);
//       Q_ASSERT(destinationFieldIndex >= 0);
      pvMatchItems.emplace_back(sourceValueFieldIndex, destinationFieldIndex);
    }

    /*! \brief Add a match item
     *
     * A match item is a criteria between a value of source
     *  and a field in destination.
     *
     * \param operatorWithPrevious WHERE clause operator (AND, OR) with previous match item
     * \param sourceValueFieldIndex Index of field, in source, in which value will be fetched during copy
     * \param destinationFieldIndex Index of field, in destination, for which the value must match
     * \pre sourceValueFieldIndex and destinationFieldIndex must be >= 0
     */
    void addMatchItem(mdtSqlWhereOperator::Type operatorWithPrevious, int sourceValueFieldIndex, int destinationFieldIndex)
    {
//       Q_ASSERT(sourceValueFieldIndex >= 0);
//       Q_ASSERT(destinationFieldIndex >= 0);
      pvMatchItems.emplace_back(operatorWithPrevious, sourceValueFieldIndex, destinationFieldIndex);
    }

    /*! \brief Remove match items
     *
     * \param startIndex Index of first item to remove
     * \param count Count of items to remove, starting from startIndex
     * \pre startIndex + count must be <= matchItemsCount()
     */
    void removeMatchItems(int startIndex, int count)
    {
      Q_ASSERT((startIndex + count) <= matchItemsCount());

      auto first = pvMatchItems.cbegin() + startIndex;
      auto last = first + count;

      pvMatchItems.erase(first, last);
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

    /*! \brief Set match item at given itemIndex
     *
     * \pre itemIndex must be valid
     */
//     void setMatchItemAt(int itemIndex, const UniqueInsertMatchExpressionItem & item)
    void setMatchItemAt(int itemIndex, const ExpressionMatchItem & item)
    {
      Q_ASSERT(itemIndex >= 0);
      Q_ASSERT(itemIndex < matchItemsCount());
      pvMatchItems[itemIndex] = item;
    }

    /*! \brief Get match item at given itemIndex
     *
     * \pre itemIndex must be valid
     */
//     UniqueInsertMatchExpressionItem matchItemAt(int itemIndex) const
    ExpressionMatchItem matchItemAt(int itemIndex) const
    {
      Q_ASSERT(itemIndex >= 0);
      Q_ASSERT(itemIndex < matchItemsCount());
      return pvMatchItems[itemIndex];
    }

    /*! \brief Get list of source value field indexes
     *
     * Will collect, in a unique way, source value field indexe for each match item.
     */
    QVector<int> getSourceValueFieldIndexList() const
    {
      QVector<int> indexList;

      for(const auto & item : pvMatchItems){
        const int idx = item.sourceValueFieldIndex;
        if(!indexList.contains(idx)){
          indexList.append(idx);
        }
      }

      return indexList;
    }

  private:

    /*! \brief Clear expression
     */
    void clearItem() override
    {
      pvMatchItems.clear();
    }

//     std::vector<UniqueInsertMatchExpressionItem> pvMatchItems;
    std::vector<ExpressionMatchItem> pvMatchItems;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_UNIQUE_INSERT_EXPRESSION_H
