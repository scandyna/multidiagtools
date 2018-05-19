/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_ABSTRACT_TABLE_MODEL_H
#define MDT_ITEM_MODEL_ABSTRACT_TABLE_MODEL_H

#include "Mdt/IndexRange/RowRange.h"
#include "MdtItemModelExport.h"
#include <QAbstractTableModel>

namespace Mdt{ namespace ItemModel{

  /*! \brief Provides a base to create table models
   *
   * The common way to create a custom table model is to subclass QAbstractTableModel .
   *  While QAbstractTableModel provides a good API, some boilerplate code has
   *  to be written many times.
   *  The goal of AbstractTableModel is to reduce such code for the most common cases.
   *
   * Example for a simple read only table model:
   * \code
   * class ReadOnlyTableModel : public Mdt::ItemModel::AbstractTableModel
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   ReadOnlyTableModel(QObject *parent = nullptr)
   *    : AbstractTableModel(parent)
   *   {
   *   }
   *
   *  private:
   *
   *   int rowCountImpl() const override
   *   {
   *     return mTable.size();
   *   }
   *
   *   int columnCountImpl() const override
   *   {
   *     return 2;
   *   }
   *
   *   QVariant displayRoleData(int row, int column) const
   *   {
   *     return mTable[row][column];
   *   }
   *
   *   using Record = std::array<QVariant, 2>;
   *   using Table = std::vector<Record>;
   *
   *   Table mTable;
   * };
   * \endcode
   *
   * Example for a editable and resizable table model:
   * \code
   * class EditableTableModel : public Mdt::ItemModel::AbstractTableModel
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   EditableTableModel(QObject *parent = nullptr)
   *    : AbstractTableModel(parent)
   *   {
   *     setModelEditable(true);
   *   }
   *
   *  private:
   *
   *   int rowCountImpl() const override
   *   {
   *     return mTable.size();
   *   }
   *
   *   int columnCountImpl() const override
   *   {
   *     return 2;
   *   }
   *
   *   QVariant displayRoleData(int row, int column) const
   *   {
   *     return mTable[row][column];
   *   }
   *
   *   bool insertRowsImpl(int row, int count) override
   *   {
   *     Mdt::Container::insertToContainer(mTable, row, count, Record());
   *     return true;
   *   }
   *
   *   bool removeRowsImpl(int row, int count) override
   *   {
   *     Mdt::Container::removeFromContainer(mTable, row, count);
   *     return true;
   *   }
   *
   *   using Record = std::array<QVariant, 2>;
   *   using Table = std::vector<Record>;
   *
   *   Table mTable;
   * };
   * \endcode
   *
   * \note If a table model that directly contains the data is needed,
   *   see if ReadOnlyStlTableModel , EditableStlTableModel or RowResizableStlTableModel
   *   can be used .
   */
  class MDT_ITEMMODEL_EXPORT AbstractTableModel : public QAbstractTableModel
  {
   Q_OBJECT

   public:

    /*! \brief Construct a abstract table model
     */
    AbstractTableModel(QObject *parent = nullptr);

    /*! \brief Get count of rows
     *
     * Will call rowCountImpl() if parent is not valid,
     *  otherwise returns 0.
     *
     * \sa rowCount()
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get count of columns
     *
     * Will call columnCountImpl() if parent is not valid,
     *  otherwise returns 0.
     *
     * \sa columnCount()
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get flags for \a index
     *
     * If \a index is not valid, Qt::NoItemFlags is returned.
     *
     * If this model is read only, QAbstractTableModel::flags() is returned.
     *  If this model is editable, Qt::ItemIsEditable is alos added to QAbstractTableModel::flags().
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    /*! \brief Get the data at \a index for \a role
     *
     * Will call displayRoleData() if \a index is valid (and in valid ranges),
     *  and \a role is Qt::DisplayRole, otherwise a invalid QVariant is returned.
     *
     * For more complex models that supports other roles than Qt::DisplayRole,
     *  this method should be re-implemented.
     *
     * \note For other stuff as data access, for example formating,
     *   using a proxy model should be a good solution.
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    /*! \brief Set data for \a role at \a index
     *
     * Will call setEditRoleData() if \a index is valid (and in valid range)
     *  and \a role is Qt::EditRole , otherwise does noting and returns false.
     *
     * If setEditRoleData() returns true, this method will also emit the dataChanged() signal,
     *  and returns true.
     *
     * \sa setEditRoleData()
     */
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

    /*! \brief Insert \a count rows before \a row into the model
     *
     * Will call insertRowsImpl() if \a parent is not valid,
     *  otherwise it does nothing and returns false.
     *
     * \pre \a row must be in valid range ( 0 <= \a row <= rowCount() )
     * \pre \a count muste be >= 1
     */
    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;

    /*! \brief Add a row to the beginning of this model
     *
     * \sa insertRows()
     */
    bool prependRow();

    /*! \brief Add a row to the end of this model
     *
     * \sa insertRows()
     */
    bool appendRow();

    /*! \brief Remove \a count rows starting from \a row
     *
     * Will call removeRowsImpl() if \a parent is not valid,
     *  otherwise it does nothing and returns false.
     *
     * \pre \a row must be >= 0
     * \pre \a count muste be >= 1
     * \pre \a row + \a count must be in valid range ( 1 <= \a row + \a count <= rowCount() ).
     */
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    /*! \brief Remove the first row
     */
    bool removeFirstRow();

    /*! \brief Remove the last row
     */
    bool removeLastRow();

   protected:

    /*! \brief Get count of rows
     *
     * This method is called by rowCount(const QModelIndex &).
     */
    virtual int rowCountImpl() const = 0;

    /*! \brief Get count of columns
     *
     * This method is called by columnCount(const QModelIndex &).
     */
    virtual int columnCountImpl() const = 0;

    /*! \brief Get display role data
     *
     * This method is called by data() if all preconditions ar satisfied.
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    virtual QVariant displayRoleData(int row, int column) const = 0;

    /*! \brief Get edit role data
     *
     * If the table model has to display specific data while editing a item,
     *  this method can be re-implemented.
     *
     * This implementation returns displayRoleData() .
     *
     * This method is called by data() if all preconditions ar satisfied.
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    virtual QVariant editRoleData(int row, int column) const;

    /*! \brief Set \a value at \a row and \a column
     *
     * This method is called by setData() if all preconditions ar satisfied.
     *
     * This implementation does nothing and returns false.
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    virtual bool setEditRoleData(int row, int column, const QVariant & value);

    /*! \brief Insert \a count rows before \a row into the model
     *
     * If the model supports inserting rows, this method should be implemented.
     *
     * This method is called from insertRows() if all preconditions ar satisfied.
     *  The implementation should not call beginInsertRows() and endInsertRows(),
     *  this is done in insertRows() .
     *
     * This default implementation does nothing and returns false.
     *
     * \pre \a row must be in valid range ( 0 <= \a row <= rowCount() )
     * \pre \a count muste be > 0
     */
    virtual bool insertRowsImpl(int row, int count);

    /*! \brief Remove \a count rows starting from \a row
     *
     * If the model supports removing rows, this method should be implemented.
     *
     * This method is called from removeRows() if all preconditions ar satisfied.
     *  The implementation should not call beginRemoveRows() and endRemoveRows(),
     *  this is done in insertRows() .
     *
     * This default implementation does nothing and returns false.
     *
     * \pre \a row must be >= 0
     * \pre \a count muste be >= 1
     * \pre \a row + \a count must be in valid range ( 1 <= \a row + \a count <= rowCount() ).
     */
    virtual bool removeRowsImpl(int row, int count);

    /*! \brief Set this model editable
     */
    void setModelEditable(bool editable);

    /*! \brief Emit the dataChanged() signal
     *
     * This helper can be used when a single item
     *  at \a index has changed.
     *
     * The role argument of dataChanged() will not be populated,
     *  meaning that all roles should be considered modified.
     *
     * \pre \a index must be valid
     * \pre \a index must be in valid range ( \a index.row() < rowCount() and \a index.column() < columnCount() )
     */
    void emitDataChanged(const QModelIndex & index);

    /*! \brief Emit the dataChanged() signal
     *
     * This helper can be used when a range of rows has changed.
     *
     * The role argument of dataChanged() will not be populated,
     *  meaning that all roles should be considered modified.
     *
     * \pre \a firstRow must be in valid range ( 0 <= \a firstRow < rowCount() )
     * \pre \a lastRow must be in valid range ( 0 <= \a lastRow < rowCount() )
     */
    void emitDataAtRowsChanged(int firstRow, int lastRow);

    /*! \brief Emit the headerDataChanged() signal for the vertical header
     *
     * \pre \a rowRange must not be null
     */
    void emitVerticalHeaderDataChanged(const Mdt::IndexRange::RowRange & rowRange);

    /*! \brief Emit the headerDataChanged() signal for the vertical header
     *
     * \pre \a row must be in valid range (0 <= \a row < rowCount()).
     */
    void emitVerticalHeaderDataChanged(int row);

    /*! \brief Emit the headerDataChanged() signal for the vertical header
     *
     * \pre \a firstRow must be in valid range (0 <= \a firstRow < rowCount()).
     * \pre \a lastRow must be in valid range (0 <= \a lastRow < rowCount()).
     */
    void emitVerticalHeaderDataChanged(int firstRow, int lastRow);

   private:

    bool mIsModelEditable = false;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ABSTRACT_TABLE_MODEL_H
