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

namespace Mdt{ namespace ItemEditor{

  /*! \internal Class used by ForeignKeyColumnVisibilityMap
   */
  class ForeignKeyColumnVisibilityMapItem
  {
   public:

    ForeignKeyColumnVisibilityMapItem() = delete;

    /*! \brief Constructor
     */
    ForeignKeyColumnVisibilityMapItem(const ItemModel::ForeignKey & fk);

    /*! \brief Get foreign key
     */
    ItemModel::ForeignKey foreignKey() const
    {
      return mFk;
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

    /*! \brief Set all foreign keys hidden
     */
    void setAllForeignKeysHidden(bool hide);

    /*! \brief Get a list of columns to show
     */
    ItemModel::ColumnList getColumnsToShow() const;

    /*! \brief Get a list of columns to hide
     */
    ItemModel::ColumnList getColumnsToHide() const;

   private:

    QHash<QString, ForeignKeyColumnVisibilityMapItem> mMap;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_FOREIGN_KEY_VISIBILITY_MAP_H
