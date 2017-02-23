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
#ifndef MDT_ITEM_MODEL_PK_FK_PROXY_MODEL_BASE_H
#define MDT_ITEM_MODEL_PK_FK_PROXY_MODEL_BASE_H

#include "ColumnList.h"
#include "KeyRecord.h"
#include <QIdentityProxyModel>

namespace Mdt{ namespace ItemModel{

  /*! \brief Common base for PrimaryKeyProxyModel and ForeignKeyProxyModel
   */
  class PkFkProxyModelBase : public QIdentityProxyModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit PkFkProxyModelBase(QObject* parent = nullptr);

    /*! \brief Get item flags for given index
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override;

   protected:

    /*! \brief Set key
     */
    void setKey(const ColumnList & key);

    /*! \brief Get key
     */
    ColumnList key() const
    {
      return mKey;
    }

    /*! \brief Set key editable
     */
    void setKeyEditable(bool editable);

    /*! \brief Check if key is editable
     */
    bool isKeyEditable() const
    {
      return mIsKeyEditable;
    }

    /*! \brief Set key items enabled
     */
    void setKeyItemsEnabled(bool enable);

    /*! \brief Check if key items are enabled
     */
    bool isKeyItemsEnabled() const
    {
      return mIsKeyItemsEnabled;
    }

    /*! \brief Get key record for row
     *
     * \pre \a row must be in correct range ( 0 <= row < rowCount() )
     */
    KeyRecord keyRecord(int row) const;

    /*! \brief Find first row that matches key record
     *
     * Returns the first row that matches, otherwise -1
     *
     * \pre \a record must contain the same columns than key set with setKey()
     */
    int findFirstRowForKeyRecord(const KeyRecord & record) const;

   private:

    bool rowMatchesKeyRecord(int row, const KeyRecord & record) const;

    bool mIsKeyEditable = true;
    bool mIsKeyItemsEnabled = true;
    ColumnList mKey;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_PK_FK_PROXY_MODEL_BASE_H
