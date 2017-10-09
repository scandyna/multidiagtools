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
#ifndef MDT_ITEM_MODEL_RELATION_KEY_H
#define MDT_ITEM_MODEL_RELATION_KEY_H

#include "RelationColumnPair.h"
#include "MdtItemModelExport.h"
#include <QtGlobal>
#include <vector>

namespace Mdt{ namespace ItemModel{

  class PrimaryKey;
  class ForeignKey;

  /*! \brief Relation map between a key in parent model and a key in child model
   */
  class MDT_ITEMMODEL_EXPORT RelationKey
  {
   public:

    /*! \brief STL const iterator
     */
    typedef std::vector<RelationColumnPair>::const_iterator const_iterator;

    /*! \brief Add a pair of columns
     *
     * \pre parentModelColumn must be >= 0
     * \pre childModelColumn must be >= 0
     */
    void addColumnPair(int parentModelColumn, int childModelColumn);

    /*! \brief Set rekation key from parent model primary key and child model foreign key
     *
     * \pre parentModelPk must not be null
     * \pre childModelFk must not be null
     * \pre Both parentModelPk and childModelFk must have the same count of fields
     */
    void setKey(const PrimaryKey & parentModelPk, const ForeignKey & childModelFk);

    /*! \brief Get count of column pairs in this key
     */
    int columnPairCount() const
    {
      return mColumnPairList.size();
    }

    /*! \brief Check if this container allready has a column pair
     */
    bool containsColumnPair(int parentModelColumn, int childModelColumn) const;

    /*! \brief Get column pair at index
     *
     * \pre \a index must be in a valid range ( 0 <= index < columnPairCount() )
     */
    RelationColumnPair columnPairAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < columnPairCount());
      return mColumnPairList[index];
    }

    /*! \brief Get the greatest column for parent model
     *
     * Returns -1 if this key is empty.
     */
    int greatestParentModelColumn() const;

    /*! \brief Get the greatest column for child model
     *
     * Returns -1 if this key is empty.
     */
    int greatestChildModelColumn() const;

    /*! \brief Check if this relation key is null
     */
    bool isNull() const
    {
      return mColumnPairList.empty();
    }

    /*! \brief Clear
     */
    void clear();

    /*! \brief Get const iterator to the beginning
     */
    const_iterator begin() const
    {
      return mColumnPairList.cbegin();
    }

    /*! \brief Get const iterator to the end
     */
    const_iterator end() const
    {
      return mColumnPairList.cend();
    }

   private:

    std::vector<RelationColumnPair> mColumnPairList;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_RELATION_KEY_H
