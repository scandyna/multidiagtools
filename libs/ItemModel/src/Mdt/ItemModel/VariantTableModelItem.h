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
#ifndef MDT_ITEM_MODEL_VARIANT_TABLE_MODEL_ITEM_H
#define MDT_ITEM_MODEL_VARIANT_TABLE_MODEL_ITEM_H

#include "VariantTableModelStorageRule.h"
#include "MdtItemModelExport.h"
#include <QVariant>

namespace Mdt{ namespace ItemModel{

  /*! \brief Item of a VariantTableModel
   */
  class MDT_ITEMMODEL_EXPORT VariantTableModelItem
  {
   public:

    /*! \brief Construct a item
     */
    VariantTableModelItem(VariantTableModelStorageRule storageRule)
     : mStorageRule(storageRule)
    {
    }

    /*! \brief Check if edit role data is stored separately from display role data
     */
    bool isEditRoleDataSeparate() const
    {
      return (mStorageRule == VariantTableModelStorageRule::SeparateDisplayAndEditRoleData);
    }

    /*! \brief Set data for display role
     */
    void setDisplayRoleData(const QVariant & data);

    /*! \brief Get data for display role
     */
    QVariant displayRoleData() const
    {
      return mDisplayRoleData;
    }

    /*! \brief Set data for edit role
     *
     * If isEditRoleDataSeparate() is false,
     *  this will be the same as setDisplayRoleData()
     */
    void setEditRoleData(const QVariant & data);

    /*! \brief Get data for edit role
     *
     * If isEditRoleDataSeparate() is true,
     *  returns data set by setEditRoleData(),
     *  else it returns the same as displayRoleData()
     */
    QVariant editRoleData() const
    {
      if( isEditRoleDataSeparate() ){
        return mEditRoleData;
      }
      return mDisplayRoleData;
    }

    /*! \brief Set data
     *
     * \pre role must be Qt::DisplayRole or Qt::EditRole
     */
    void setData(const QVariant & data, int role);

    /*! \brief Get data
     *
     * \pre role must be Qt::DisplayRole or Qt::EditRole
     */
    QVariant data(int role) const
    {
      Q_ASSERT( (role == Qt::DisplayRole) || (role == Qt::EditRole) );

      if(role == Qt::DisplayRole){
        return displayRoleData();
      }else{
        return editRoleData();
      }
    }

    /*! \brief Set item enabled/disabled
     *
     * Default is enabled
     */
    void setEnabled(bool enable)
    {
      mIsEnabled = enable;
    }

    /*! \brief Check if item is enabled
     */
    bool isEnabled() const
    {
      return mIsEnabled;
    }

    /*! \brief Set item editable/read only
     *
     * Default id editable
     */
    void setEditable(bool editable)
    {
      mIsEditable = editable;
    }

    /*! \brief Check if item is editable
     */
    bool isEditable() const
    {
      return mIsEditable;
    }

    /*! \brief Get flags
     *
     * Returns currentFlags with Qt::ItemIsEditable and ItemIsEnabled
     *  set or unset regarding isEditable() and isEnabled().
     */
    Qt::ItemFlags flags(Qt::ItemFlags currentFlags) const;

   private:

    VariantTableModelStorageRule mStorageRule;
    bool mIsEditable = true;
    bool mIsEnabled = true;
    QVariant mDisplayRoleData;
    QVariant mEditRoleData;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_VARIANT_TABLE_MODEL_ITEM_H
