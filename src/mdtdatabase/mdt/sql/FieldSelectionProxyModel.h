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
#ifndef MDT_SQL_FIELD_SELECTION_PROXY_MODEL_H
#define MDT_SQL_FIELD_SELECTION_PROXY_MODEL_H

#include "FieldIndexList.h"
#include <QSortFilterProxyModel>
#include <QBitArray>

namespace mdt{ namespace sql{

  /*! \brief SQL field selection proxy model
   *
   * Displays all rows of source model,
   *  except those specified.
   *
   * FieldSelectionProxyModel is based on QSortFilterProxyModel
   *  to allow filtering.
   *  Note that filterRegExp is ignored.
   */
  class FieldSelectionProxyModel : public QSortFilterProxyModel
  {
   Q_OBJECT

   public:

    /*! \brief Field selection mode
     */
    enum FieldSelectionMode
    {
      MultiSelection,   /*!< Its possible to select more that 1 field (this is the default) */
      SingleSelection   /*!< Only 1 field can be selected */
    };

    /*! \brief Constructor
     */
    FieldSelectionProxyModel(QObject* parent = 0);

    // Copy disabled
    FieldSelectionProxyModel(const FieldSelectionProxyModel &) = delete;
    FieldSelectionProxyModel & operator=(const FieldSelectionProxyModel &) = delete;

    /*! \brief Set list of field indexes to hide
     */
    void setFieldIndexesToHide(const FieldIndexList & indexes);

    /*! \brief Show all field indexes
     */
    void showAllFieldIndexes();

    /*! \brief Set field selection mode
     *
     * Will also clear current field selection.
     */
    void setFieldSelectionMode(FieldSelectionMode mode);

    /*! \brief Get field selection mode
     */
    FieldSelectionMode fieldSelectionMode() const
    {
      return pvFieldSelectionMode;
    }

    /*! \brief Select or deselect field index
     *
     * \param fieldIndex Field index (can be different from row in this model)
     * \param select If true, field index will be selected, else deselected
     */
    void setFieldIndexSelected(int fieldIndex, bool select);

    /*! \brief Clear field index selection
     */
    void clearFieldIndexSelection();

    /*! \brief Get list of selected field indexes
     */
    FieldIndexList getSelectedFieldIndexes() const;

    /*! \brief Get count of columns
     */
//     int columnCount(const QModelIndex & parent = QModelIndex()) const override;

//     QModelIndex mapToSource(const QModelIndex & proxyIndex) const override;
// 
//     QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get header data
     */
//     QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /*! \brief Get data
     */
    QVariant data(const QModelIndex & originalIndex, int role = Qt::DisplayRole) const override;

    /*! \brief Get flags
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    /*! \brief Set data
     */
    bool setData(const QModelIndex & originalIndex, const QVariant & value, int role = Qt::EditRole) override;

    /*! \brief Sort
     */
//     void sort(int column, Qt::SortOrder sortOrder = Qt::AscendingOrder) override;

   private:

    /*! \brief Get selection column data
     */
    QVariant selectionColumnData(int row, int role) const;

    /*! \brief Set selection column data
     */
    bool setSelectionColumnData(int row, const QVariant & value, int role);

    /*! \brief Set selection column value at row
     */
    void setSelectionColumnAt(int row, bool state);

    /*! \brief Signal that field selection changed
     *
     * In MultiSelection mode, only given index will be signaled,
     *  else all selections will
     */
    void signalSelectionColumnDataChanged(const QModelIndex & updatedIndex);

    /*! \brief Reset selection list
     */
    void resetSelectionList();

    /*! \brief Return true if source_row is a index of field to show
     */
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;

    FieldIndexList pvFieldIndexesToHide;
    QBitArray pvSelectionColumnList;
    FieldSelectionMode pvFieldSelectionMode;
  };

}} // namespace mdt{ namespace sql{

#endif // #ifndef MDT_SQL_FIELD_SELECTION_PROXY_MODEL_H
