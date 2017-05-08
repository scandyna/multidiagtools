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
#ifndef MDT_ITEM_EDITOR_FOREIGN_KEY_VISIBILITY_MAP_H
#define MDT_ITEM_EDITOR_FOREIGN_KEY_VISIBILITY_MAP_H

#include "Mdt/ItemModel/ForeignKey.h"
#include "Mdt/ItemModel/ForeignKeyList.h"
#include "Mdt/ItemModel/ColumnList.h"
#include <QString>
#include <QHash>
#include <vector>

namespace Mdt{ namespace ItemEditor{

  /*! \internal Class used by ForeignKeyColumnVisibilityMap
   */
  class ForeignKeyColumnVisibilityMapItem
  {
   public:

    ForeignKeyColumnVisibilityMapItem() = default;

    /*! \brief Constructor
     */
    ForeignKeyColumnVisibilityMapItem(const ItemModel::ForeignKey & fk);

    /*! \brief Get foreign key
     */
    ItemModel::ForeignKey foreignKey() const
    {
      return mFk;
    }

    /*! \brief Check if foreign key constains a column
     */
    bool foreignKeyContainsColumn(int column) const
    {
      return mFk.containsColumn(column);
    }

    /*! \brief Set foreign key hidden
     */
    void setForeignKeyHidden(bool hide);

    /*! \brief Check if foreign key is hidden
     */
    bool isForeignKeyHidden() const
    {
      return mIsFkHidden;
    }

   private:

    bool mIsFkHidden = false;
    ItemModel::ForeignKey mFk;
  };

  /*! \brief ForeignKeyColumnVisibilityMap holds columns visibility regarding a set of foreign keys and columns hidden flags
   */
  class ForeignKeyColumnVisibilityMap
  {
   public:

    /*! \brief Construct a empty columns visibility map
     */
    ForeignKeyColumnVisibilityMap() = default;

    /*! \brief Set the list of foreign keys
     */
    void setForeignKeyList(const ItemModel::ForeignKeyList & list);

    /*! \brief Get a list of the foreign keys that exists in this map
     */
    ItemModel::ForeignKeyList getForeignKeyList() const;

    /*! \brief Set foreign key hidden
     *
     * \pre \a foreignEntityName must not be empty
     * \pre A foreign key referencing \a foreignEntityName must exist
     */
    void setForeignKeyHidden(const QString & foreignEntityName, bool hide);

    /*! \brief Set all foreign keys hidden
     */
    void setAllForeignKeysHidden(bool hide);

    /*! \brief Check if the foreign key referencing a entity is hidden
     *
     * \pre \a foreignEntityName must not be empty
     * \pre A foreign key referencing \a foreignEntityName must exist
     */
    bool isForeignKeyHidden(const QString & foreignEntityName) const;

    /*! \brief Check if a column must is visible
     *
     * Returns true if \a column is at least part of a foreign key that is not hidden,
     *  otherwise false.
     */
    bool isColumnVisible(int column) const;

    /*! \brief Get a list of columns to show
     *
     * \note The first call of this method after setForeignKeyList()
     *        will also return the columns that are no longer part of a foreign key,
     *        as long as no modifier is called.
     */
    ItemModel::ColumnList getColumnsToShow() const;

    /*! \brief Get a list of columns to hide
     */
    ItemModel::ColumnList getColumnsToHide() const;

   private:

    void updateColumnsToBeShownAndHidden();
    void setColumnsNoLongerPartOfForeignKey(const QHash<QString, ForeignKeyColumnVisibilityMapItem> & previousMap);
    ItemModel::ColumnList getColumnsToBeHidden() const;
    ItemModel::ColumnList getColumnsToBeVisible() const;
    static void copyItemFkToList(const Mdt::ItemEditor::ForeignKeyColumnVisibilityMapItem& item, Mdt::ItemModel::ColumnList& list);

    QHash<QString, ForeignKeyColumnVisibilityMapItem> mMap;
    ItemModel::ColumnList mColumnsToShow;
    ItemModel::ColumnList mColumnsToHide;
    ItemModel::ColumnList mHiddenColumns;
    mutable ItemModel::ColumnList mColumnsNoLongerPartOfMap;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_FOREIGN_KEY_VISIBILITY_MAP_H
