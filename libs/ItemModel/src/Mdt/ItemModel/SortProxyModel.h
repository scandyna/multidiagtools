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
#ifndef MDT_ITEM_MODEL_SORT_PROXY_MODEL_H
#define MDT_ITEM_MODEL_SORT_PROXY_MODEL_H

#include "ColumnSortOrderList.h"
#include <QSortFilterProxyModel>

namespace Mdt{ namespace ItemModel{

  /*! \brief Provides support for sorting data between another model and a view
   *
   * Example of usage:
   * \code
   * #include "Mdt/ItemModel/SortProxyModel.h"
   * #include <QTableView>
   * #include "ClientTableModel.h"
   *
   * namepsace ItemModel = Mdt::ItemModel;
   * using ItemModel::SortProxyModel;
   *
   * auto *view = new QTableView;
   * auto *model = new ClientTableModel(view);
   * auto *proxyModel = new SortProxyModel(view);
   *
   * proxyModel->setSourceModel(model);
   * view->setModel(proxyModel);
   *
   * // Setup model to sort by columns 0, 2, 1 ascending:
   * proxyModel->addColumnToSortOrder(0, Qt::AscendingOrder);
   * proxyModel->addColumnToSortOrder(2, Qt::AscendingOrder);
   * proxyModel->addColumnToSortOrder(1, Qt::AscendingOrder);
   * \endcode
   *
   * \todo Define:
   *        - dynamicSortFilter
   *        - isSortLocaleAware
   *        - sortCaseSensitivity (which is single-column): ignore ???
   *        - sortRole
   *        - what impact have filter* functions ?
   */
  class SortProxyModel : public QSortFilterProxyModel
  {
   public:

    /*! \brief Construct a sort proxy model
     */
    explicit SortProxyModel(QObject *parent = nullptr);

    // Disable copy
    SortProxyModel(const SortProxyModel &) = delete;
    SortProxyModel & operator=(const SortProxyModel &) = delete;
    // Disable move
    SortProxyModel(SortProxyModel &&) = delete;
    SortProxyModel & operator=(SortProxyModel &&) = delete;

    /*! \brief Add a column to columns sort order
     *
     * Columns sort order is similar meaning
     *  than SQL ORDER BY clause.
     *
     * For example, to sort columns in order 0, 2, 1
     *  all ascending, call:
     * \code
     * proxyModel.clearColumnsSortOrder();  // If a sort order was previously set
     * proxyModel.addColumnToSortOrder(0, Qt::AscendingOrder);
     * proxyModel.addColumnToSortOrder(2, Qt::AscendingOrder);
     * proxyModel.addColumnToSortOrder(1, Qt::AscendingOrder);
     * \endcode
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() ).
     */
    void addColumnToSortOrder(int column, Qt::SortOrder order = Qt::AscendingOrder);

    /*! \brief Add a column to columns sort order
     *
     * \overload
     *
     * This version gives more control of string sorting.
     *  If numericMode is true, numbers in strings are sorted in a natural way.
     *  For example: 10A,2A will be sorted 2A,10A .
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() ).
     */
    void addColumnToSortOrder(int column, Qt::CaseSensitivity caseSensitivity, bool numericMode, Qt::SortOrder order = Qt::AscendingOrder);

    /*! \brief Clear columns sort order
     *
     * \sa addColumnToSortOrder()
     */
    void clearColumnsSortOrder();

    /*! \brief Set string sorting attributes for a given column
     */
    void setColumnStringSortAttribute(int column, Qt::CaseSensitivity caseSensitivity, bool numericMode);

    /*! \brief Sort a specific column
     *
     * If attributes for string sorting where set with setColumnStringSortAttribute()
     *  or addColumnToSortOrder(int, Qt::CaseSensitivity, bool, Qt::SortOrder) ,
     *  they will be used (except sortOrder, which will follow the argument passed to this method).
     *
     * As for QSortFilterProxyModel, if \a column is -1, the sort order of the underlaying model is restored.
     *
     * \pre \a column must be -1 or a valid range ( -1 >= \a column < columnCount() ).
     *
     * \todo Document:
     *        - how does this interact with dynamicSortFilter ?
     */
    void sort(int column, Qt::SortOrder sortOrder = Qt::AscendingOrder) override;

    /*! \brief Sort by multiple columns
     *
     * Sorts the model by each column added with addColumnToSortOrder().
     */
    void sort();

   private:

    /*! \brief Overloaded form QSortFilterProxyModel to privide multi-column and natural sort
     */
    bool lessThan(const QModelIndex & source_left, const QModelIndex & source_right) const override;

    ColumnSortOrderList mColumnSortOrderList;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // MDT_ITEM_MODEL_SORT_PROXY_MODEL_H
