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
#ifndef MDT_ITEM_MODEL_VARIANT_TABLE_MODEL_H
#define MDT_ITEM_MODEL_VARIANT_TABLE_MODEL_H

#include "VariantTableModelRow.h"
#include "VariantTableModelStorageRule.h"
#include "MdtItemModelExport.h"
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
  class MDT_ITEMMODEL_EXPORT VariantTableModel : public QAbstractTableModel
  {
   Q_OBJECT

   public:

    /*! \brief Construct a model
     */
    explicit VariantTableModel(VariantTableModelStorageRule storageRule = VariantTableModelStorageRule::GroupDisplayAndEditRoleData, QObject *parent = nullptr);

    /*! \brief Tell if this model must pass roles in data changed signal
     *
     * By default, roles are passed in dataChanged() signal.
     */
    void setPassRolesInDataChaged(bool pass);

    /*! \brief Get row count
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get column count
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get data at index
     *
     * If storage rule (passed in constructor) is SeparateDisplayAndEditRoleData,
     *  data for Qt::EditRole will be distinct from data for Qt::DisplayRole.
     *
     * If \a role is other than Qt::DisplayRole or Qt::EditRole,
     *  an null value is returned.
     *
     * If \a index is not valid, or out of range,
     *  an null value is returned.
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    /*! \brief Get data at row and column
     *
     * \pre row must be in valid range ( 0 <= row < rowCount() )
     * \pre column must be in valid range ( 0 <= column < columnCount() )
     * \sa data(const QModelIndex &, int)
     */
    QVariant data(int row, int column, int role = Qt::DisplayRole) const;

    /*! \brief Set item enabled at index
     *
     * \pre index must be in valid range
     *      ( 0 <= index.row() < rowCount() and 0 <= index.column() < columnCount() )
     */
    void setItemEnabled(const QModelIndex & index, bool enable);

    /*! \brief Set item enabled at row and column
     *
     * \pre row must be in valid range ( 0 <= row < rowCount() )
     * \pre column must be in valid range ( 0 <= column < columnCount() )
     * \sa setItemEnabled(const QModelIndex &, bool)
     */
    void setItemEnabled(int row, int column, bool enable);

    /*! \brief Set item editable at index
     *
     * \pre index must be in valid range
     *      ( 0 <= index.row() < rowCount() and 0 <= index.column() < columnCount() )
     */
    void setItemEditable(const QModelIndex & index, bool editable);

    /*! \brief Set item editable at row and column
     *
     * \pre row must be in valid range ( 0 <= row < rowCount() )
     * \pre column must be in valid range ( 0 <= column < columnCount() )
     * \sa setItemEditable(const QModelIndex &, bool)
     */
    void setItemEditable(int row, int column, bool editable);

    /*! \brief Get flags at index
     *
     * If \a index is not valid, or out of range,
     *  this method returns QAbstractTableModel's flags.
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    /*! \brief Set data at index
     *
     * If storage rule (passed in constructor) is SeparateDisplayAndEditRoleData,
     *  data for Qt::EditRole will be distinct from data for Qt::DisplayRole.
     *
     * If \a role is other than Qt::DisplayRole or Qt::EditRole,
     *  no data is set and this function returns false.
     *
     * If \a index is not valid, or out of range,
     *  this method returns false.
     */
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

    /*! \brief Set data at row and column
     *
     * \pre row must be in valid range ( 0 <= row < rowCount() )
     * \pre column must be in valid range ( 0 <= column < columnCount() )
     * \sa setData(const QModelIndex &, const QVariant &, int)
     */
    bool setData(int row, int column, const QVariant & value, int role = Qt::EditRole);

    /*! \brief Append a row
     */
    void appendRow();

    /*! \brief Prepend a row
     */
    void prependRow();

    /*! \brief Implement QAbstractTableModel::insertRows()
     *
     * \pre \a row must be in correct range ( 0 <= column <= rowCount() )
     * \pre \a count must be >= 1
     */
    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;

    /*! \brief Remove first row
     */
    void removeFirstRow();

    /*! \brief Remove last row
     */
    void removeLastRow();

    /*! \brief Implement QAbstractTableModel::removeRows()
     *
     * \pre \a row must be >= 0
     * \pre \a count must be >= 1
     * \pre \a row + \a count must be in correct range ( 0 <= column <= rowCount() )
     */
    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;

    /*! \brief Append a column
     */
    void appendColumn();

    /*! \brief Prepend a column
     */
    void prependColumn();

    /*! \brief Implement QAbstractTableModel::insertColumns()
     *
     * \pre \a column must be in correct range ( 0 <= column <= columnCount() )
     * \pre \a count must be >= 1
     */
    bool insertColumns(int column, int count, const QModelIndex & parent = QModelIndex()) override;

    /*! \brief Remove first column
     */
    void removeFirstColumn();

    /*! \brief Remove last column
     */
    void removeLastColumn();

    /*! \brief Implement QAbstractTableModel::removeColumns()
     *
     * \pre \a column must be >= 0
     * \pre \a count must be >= 1
     * \pre \a column + \a count must be in correct range ( 0 <= column <= columnCount() )
     */
    bool removeColumns(int column, int count, const QModelIndex & parent = QModelIndex()) override;

    /*! \brief Resize model
     *
     * For new count of rows, respectively columns,
     *  that is greater than current count, new null elements are added.
     *  For count that is less than current one, elements are removed.
     *
     * This method will also reset the model.
     *
     * \pre rows must be >= 0
     * \pre columns must be >= 0
     */
    void resize(int rows, int columns);

    /*! \brief Populate a column with int values
     *
     * For each row, starting from 0,
     *  value at column will be updated.
     *
     * For example, if we have a model with 3 rows and 2 columns:
     * <table class="srcdoc_td_center">
     *  <tr><th>Col 0</th><th>Col 1</th></tr>
     *  <tr><td>A</td><td></td></tr>
     *  <tr><td>B</td><td></td></tr>
     *  <tr><td>C</td><td></td></tr>
     * </table>
     * populateColumnWithInt(1, 2, 4) will update second column:
     * <table class="srcdoc_td_center">
     *  <tr><th>Col 0</th><th>Col 1</th></tr>
     *  <tr><td>A</td><td>2</td></tr>
     *  <tr><td>B</td><td>6</td></tr>
     *  <tr><td>C</td><td>10</td></tr>
     * </table>
     *
     * \pre column must be in valid range ( 0 <= column < columnCount() )
     */
    void populateColumnWithInt(int column, int firstValue, int increment = 1);

    /*! \brief Populate a column with ASCII values
     *
     * For each row, starting from 0,
     *  value at column will be updated.
     *
     * For example, if we have a model with 3 rows and 2 columns:
     * <table class="srcdoc_td_center">
     *  <tr><th>Col 0</th><th>Col 1</th></tr>
     *  <tr><td>A</td><td></td></tr>
     *  <tr><td>B</td><td></td></tr>
     *  <tr><td>C</td><td></td></tr>
     * </table>
     * populateColumnWithAscii(1, 'b', 3) will update second column:
     * <table class="srcdoc_td_center">
     *  <tr><th>Col 0</th><th>Col 1</th></tr>
     *  <tr><td>A</td><td>'b'</td></tr>
     *  <tr><td>B</td><td>'e'</td></tr>
     *  <tr><td>C</td><td>'h'</td></tr>
     * </table>
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     *
     * Note: no checks are done for firstValue and increment.
     */
    void populateColumnWithAscii(int column, char firstValue, int increment = 1);

    /*! \brief Populate a column with data
     *
     * For example, if we have a model with 3 rows and 2 columns:
     * \code
     * model.populateColumn(0, {1,2,3});
     * \endcode
     * will update first column:
     * <table class="srcdoc_td_center">
     *  <tr><th>Col 0</th><th>Col 1</th></tr>
     *  <tr><td>1</td><td></td></tr>
     *  <tr><td>2</td><td></td></tr>
     *  <tr><td>3</td><td></td></tr>
     * </table>
     * If we populate column 0 with less data than rowCount(), like this:
     * \code
     * model.populateColumn(0, {'A','B'});
     * \endcode
     * the last row will be keeped untouched:
     * <table class="srcdoc_td_center">
     *  <tr><th>Col 0</th><th>Col 1</th></tr>
     *  <tr><td>A</td><td></td></tr>
     *  <tr><td>B</td><td></td></tr>
     *  <tr><td>3</td><td></td></tr>
     * </table>
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     * \pre \a data size must be <= rowCount()
     * \pre \a role must be Qt::DisplayRole or Qt::EditRole
     * \pre If \a role is Qt::EditRole, storage strategy must be VariantTableModelStorageRule::SeparateDisplayAndEditRoleData
     */
    void populateColumn(int column, const std::vector<QVariant> & data, Qt::ItemDataRole role = Qt::DisplayRole);

    /*! \brief Repopulate model with data
     *
     * Calling:
     * \code
     * model.populateByColumns({{1,2,3},{"A","B","C"}});
     * \endcode
     * will populate the model:
     * <table class="srcdoc_td_center">
     *  <tr><th>Col 0</th><th>Col 1</th></tr>
     *  <tr><td>1</td><td>A</td></tr>
     *  <tr><td>2</td><td>B</td></tr>
     *  <tr><td>3</td><td>C</td></tr>
     * </table>
     *
     * This method will also reset the model.
     *
     * \pre \a data must have at least 1 row
     * \pre each row must have at least 1 column
     * \pre each column must be the same size
     */
    void repopulateByColumns(const std::vector< std::vector<QVariant> > & data, Qt::ItemDataRole role = Qt::DisplayRole);

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

    /*! \brief Resize rows
     */
    void resizeRowCount(int rows);

    /*! \brief Resize columns
     */
    void resizeColumnCount(int columns);

    VariantTableModelRow generateRowData(int currentRow) const;

    VariantTableModelStorageRule mStorageRule;
    int mColumnCount;
    std::vector<VariantTableModelRow> mData;
    bool mPassRolesInDataChanged = true;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_VARIANT_TABLE_MODEL_H
