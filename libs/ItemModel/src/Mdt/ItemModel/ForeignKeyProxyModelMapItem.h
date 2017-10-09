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
#ifndef MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_MAP_ITEM_H
#define MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_MAP_ITEM_H

#include "ForeignKey.h"
#include "MdtItemModelExport.h"
// #include <QtGlobal>

namespace Mdt{ namespace ItemModel{

  /*! \brief Used for implementation of ForeignKeyProxyModel
   */
  class ForeignKeyProxyModelMapItemFlags
  {
   public:

    /*! \brief Constructor
     */
    constexpr ForeignKeyProxyModelMapItemFlags() noexcept = default;

    /*! \brief Set foreign key editable
     */
    constexpr void setForeignKeyEditable(bool editable) noexcept
    {
      mIsFkEditable = editable;
    }

    /*! \brief Check if foreign key is editable
     */
    constexpr bool isForeignKeyEditable() const noexcept
    {
      return mIsFkEditable;
    }

    /*! \brief Set foreign key items enabled
     */
    constexpr void setForeignKeyItemsEnabled(bool enable) noexcept
    {
      mIsFkItemsEnabled = enable;
    }

    /*! \brief Check if foreign key items are enabled
     */
    constexpr bool isForeignKeyItemsEnabled() const noexcept
    {
      return mIsFkItemsEnabled;
    }

   private:

    bool mIsFkEditable = true;
    bool mIsFkItemsEnabled = true;
  };

  /*! \brief Used for implementation of ForeignKeyProxyModel
   */
  class MDT_ITEMMODEL_EXPORT ForeignKeyProxyModelMapItem
  {
  public:

    /*! \brief Constructor
     */
    ForeignKeyProxyModelMapItem() = default;

    /*! \brief Constructor
     */
    ForeignKeyProxyModelMapItem(const ForeignKey & fk);

    /*! \breif Get foreign key
     */
    ForeignKey foreignKey() const
    {
      return mFk;
    }

    /*! \brief Check if the foreign key of this item contains a column
     */
    bool foreignKeyContainsColumn(int column) const
    {
      return mFk.containsColumn(column);
    }

    /*! \brief Get flags
     */
    ForeignKeyProxyModelMapItemFlags flags() const
    {
      return mFlags;
    }

    /*! \brief Set foreign key editable
     */
    void setForeignKeyEditable(bool editable);

    /*! \brief Check if foreign key is editable
     */
    bool isForeignKeyEditable() const
    {
      return mFlags.isForeignKeyEditable();
    }

    /*! \brief Set foreign key items enabled
     */
    void setForeignKeyItemsEnabled(bool enable);

    /*! \brief Check if foreign key items are enabled
     */
    bool isForeignKeyItemsEnabled() const
    {
      return mFlags.isForeignKeyItemsEnabled();
    }

  private:

    ForeignKeyProxyModelMapItemFlags mFlags;
    ForeignKey mFk;
  };

}} // namespace Mdt{ namespace ItemModel{

// QT_BEGIN_NAMESPACE
// Q_DECLARE_TYPEINFO(Mdt::ItemModel::ForeignKeyProxyModelMapItem, Q_MOVABLE_TYPE); /// Warin: ForeignKey uses a std::vector !
// QT_END_NAMESPACE

#endif // #ifndef MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_MAP_ITEM_H
