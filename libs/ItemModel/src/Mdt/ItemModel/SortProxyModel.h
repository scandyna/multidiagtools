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
#include "StringNumericMode.h"
#include "ColumnSortStringAttributesList.h"
#include <QSortFilterProxyModel>
#include <QCollator>

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
   * proxyModel->setSortLocaleAware(true);
   * proxyModel->setSourceModel(model);
   * view->setModel(proxyModel);
   *
   * // Setup model to sort by columns 0, 2, 1 ascending:
   * proxyModel->addColumnToSortOrder(0, Qt::AscendingOrder);
   * proxyModel->addColumnToSortOrder(2, StringNumericMode::Natural, Qt::AscendingOrder);
   * proxyModel->addColumnToSortOrder(1, Qt::AscendingOrder);
   *
   * proxyModel->sort();
   * \endcode
   *
   * \sa FilterProxyModel
   *
   * \todo Define:
   *        - columnCount() changed due to:
   *          -> inser columns
   *          -> remove columns
   *          -> model reset
   *        - change model
   *        - dynamicSortFilter
   *        - what impact have filter* functions ?
   */
  class SortProxyModel : public QSortFilterProxyModel
  {
   Q_OBJECT

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
     * When source model changes, resets, or its columns count changes,
     *  it is assumed that columns added to sort order are still valid.
     *  If, after such event, a column does not exist in model, it will simply be ignored.
     *
     * \pre \a column must be >= 0
     */
    void addColumnToSortOrder(int column, Qt::SortOrder sortOrder = Qt::AscendingOrder);

    /*! \brief Add a column to columns sort order
     *
     * This version gives more control of string sorting.
     *
     * \pre \a column must be >= 0
     * \sa addColumnToSortOrder(int, Qt::SortOrder)
     * \sa setColumnStringSortAttributes()
     */
    void addColumnToSortOrder(int column, StringNumericMode numericMode, Qt::SortOrder sortOrder = Qt::AscendingOrder, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);

    /*! \brief Get a list of column sort order
     */
    ColumnSortOrderList columnSortOrderList() const
    {
      return mColumnSortOrderList;
    }

    /*! \brief Clear columns sort order
     *
     * \sa addColumnToSortOrder()
     * \sa clearColumnsStringSortAttributes()
     */
    void clearColumnsSortOrder();

    /*! \brief Set string sorting attributes for a given column
     *
     *  If numericMode is Natural, numbers in strings are sorted in a natural way.
     *  For example: 10A,2A will be sorted 2A,10A .
     *
     * \note If QSortFilterProxyModel::isSortLocaleAware() is true,
     *       sorting is allways case insensitive on some platforms (for example on POSIX).
     *       Also, if \a numericMode is Natural, sorting is allways done local aware.
     *
     * When source model changes, resets, or its columns count changes,
     *  it is assumed that each column, for which a attribute was set, is still valid.
     *  If, after such event, a column does not exist in model, it will simply be ignored.
     *
     * \pre \a column must be >= 0
     * \sa clearColumnsStringSortAttributes()
     */
    void setColumnStringSortAttributes(int column, StringNumericMode numericMode, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);

    /*! \brief Clear string sort attributes
     *
     * \sa setColumnStringSortAttributes()
     */
    void clearColumnsStringSortAttributes();

    /*! \brief Sort a specific column
     *
     * Different attributes for string sorting can be set for any column.
     *  They can be set using setColumnStringSortAttributes() ,
     *  or directly while adding a column using addColumnToSortOrder(int, StringNumericMode, Qt::SortOrder, Qt::CaseSensitivity) .
     *
     * If case sensitivity was set for \a column , it will be used,
     *  else, QSortFilterProxyModel::sortCaseSensitivity() is considered.
     *
     * \a sortOrder is allways used, regardless of any attributes set for \a column .
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
     *
     * For each column, that is in sort order, and that have a case sensitivity defined by a string sort sttributes,
     *  this one will be used.
     *  For other columns, that are in sort order, QSortFilterProxyModel::sortCaseSensitivity() is considered.
     */
    void sort();

    /*! \brief Re-implemented from QSortFilterProxyModel
     */
    void setDynamicSortFilter(bool enable);

    /*! \brief Re-implemented from QSortFilterProxyModel
     */
    void setSortCaseSensitivity(Qt::CaseSensitivity caseSensitivity);

    /*! \brief Re-implemented from QSortFilterProxyModel
     */
    void setSortLocaleAware(bool on);

    /*! \brief Re-implemented from QSortFilterProxyModel
     *
     * Note that \a role will be the same for all columns in sort order.
     */
    void setSortRole(int role);

    /*! \brief Re-implemented from QSortFilterProxyModel
     */
    void setSourceModel(QAbstractItemModel* sourceModel) override;

    /*! \brief Re-implemented from QSortFilterProxyModel
     */
//     bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

   signals:

    /*! \brief Emitted when model was sorted
     *
     * This signal is only emitted when sort() was called.
     *  If sort(int, Qt::SortOrder) is called, this signal is not emitted.
     */
    void modelSorted();

    /*! \brief Emitted when a column was sorted
     */
    void columnSorted(int column);

   private slots:

    void onRowsInserted(const QModelIndex & parent, int first, int last);
    void onModelAboutToBeReset();
    void onModelReset();
    void onDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> & roles);

   private:

    /*! \brief Overloaded form QSortFilterProxyModel to privide multi-column and natural sort
     */
    bool lessThan(const QModelIndex & source_left, const QModelIndex & source_right) const override;

    /*! \brief Helper to sort strings
     */
    bool lessThatString(const QString & left, const QString & right, int column) const;

    /*! \brief Check if mColumnSortOrderList contains a column that is in range [left,right]
     */
    bool isColumnInSortOrder(int left, int right) const;

    ColumnSortOrderList mColumnSortOrderList;
    ColumnSortStringAttributesList mColumnSortStringAttributesList;
    mutable QCollator mCollator;
    bool pvDynamicSortWasEnabled; // Used only by onModelAboutToBeReset() and onModelReset()
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // MDT_ITEM_MODEL_SORT_PROXY_MODEL_H
