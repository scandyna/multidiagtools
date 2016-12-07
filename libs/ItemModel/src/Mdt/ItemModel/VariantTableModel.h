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
#ifndef MDT_ITEM_MODEL_VARIANT_TABLE_MODEL_H
#define MDT_ITEM_MODEL_VARIANT_TABLE_MODEL_H

#include "VariantTableModelRow.h"
#include "VariantTableModelStorageRule.h"
#include <QAbstractTableModel>
#include <vector>

namespace Mdt{ namespace ItemModel{

  /*! \brief Table model that stores QVariant data
   *
   * Example:
   * \code
   * #include "Mdt/ItemModel/VariantTableModel.h"
   * #include <QTableView>
   *
   * Mdt::ItemModel::VariantTableModel model;
   * QTableView view;
   *
   * view.setModel(&model);
   * view.show();
   * model.populate(3, 2);
   * \endcode
   *
   * This class is mostly used in unit tests in Mdt libraries.
   */
  class VariantTableModel : public QAbstractTableModel
  {
   public:

    /*! \brief Construct a model
     */
    VariantTableModel(VariantTableModelStorageRule storageRule = VariantTableModelStorageRule::GroupDisplayAndEditRoleData, QObject *parent = nullptr);

    /*! \brief Get column count
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get row count
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get data at index
     *
     * If storage rule (passed in constructor) is SeparateDisplayAndEditRoleData,
     *  data for Qt::EditRole will be distinct from data for Qt::DisplayRole.
     *
     * If role is other than Qt::DisplayRole or Qt::EditRole,
     *  an null value is returned.
     *
     * \pre index must be in valid range
     *      ( 0 <= index.row() < rowCount() and 0 <= index.column() < columnCount() )
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    /*! \brief Set item enabled at index
     *
     * \pre index must be in valid range
     *      ( 0 <= index.row() < rowCount() and 0 <= index.column() < columnCount() )
     */
    void setItemEnabled(const QModelIndex & index, bool enable);

    /*! \brief Set item editable at index
     *
     * \pre index must be in valid range
     *      ( 0 <= index.row() < rowCount() and 0 <= index.column() < columnCount() )
     */
    void setItemEditable(const QModelIndex & index, bool editable);

    /*! \brief Get flags at index
     *
     * \pre index must be in valid range
     *      ( 0 <= index.row() < rowCount() and 0 <= index.column() < columnCount() )
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    /*! \brief Set data at index
     *
     * If storage rule (passed in constructor) is SeparateDisplayAndEditRoleData,
     *  data for Qt::EditRole will be distinct from data for Qt::DisplayRole.
     *
     * If role is other than Qt::DisplayRole or Qt::EditRole,
     *  no data is set and this function returns false.
     *
     * \pre index must be in valid range
     *      ( 0 <= index.row() < rowCount() and 0 <= index.column() < columnCount() )
     */
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

    /*! \brief Populate model with data
     *
     * Form is:
     * For row 0: 0A, 0B, ..., 0Z
     * For row 1: 1A, 1B, ..., 1Z
     *
     * If storage rule (passed in constructor) is SeparateDisplayAndEditRoleData,
     *  data for Qt::EditRole will not be set.
     */
    void populate(int rows, int columns);

    /*! \brief Clear all data
     *
     * rowCount() and columnCount() will return 0 after this call
     */
    void clear();

   private:

    VariantTableModelRow generateRowData(int currentRow) const;

    VariantTableModelStorageRule mStorageRule;
    int mColumnCount;
    std::vector<VariantTableModelRow> mData;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_VARIANT_TABLE_MODEL_H
