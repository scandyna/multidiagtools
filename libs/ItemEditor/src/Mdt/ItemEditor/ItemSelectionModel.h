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
#ifndef MDT_ITEM_EDITOR_ITEM_SELECTION_MODEL_H
#define MDT_ITEM_EDITOR_ITEM_SELECTION_MODEL_H

#include <QItemSelectionModel>

namespace Mdt{ namespace ItemEditor{

  /*! \brief ItemSelectionModel is used by some controllers to handle row changes
   *
   * \sa RowChangeEventDispatcher
   * \sa AbstractController
   */
  class ItemSelectionModel : public QItemSelectionModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    ItemSelectionModel(QAbstractItemModel* model);

    /*! \brief Constructor
     */
    ItemSelectionModel(QAbstractItemModel* model, QObject *parent);

    // Copy disabled
    ItemSelectionModel(const ItemSelectionModel &) = delete;
    ItemSelectionModel & operator=(const ItemSelectionModel &) = delete;
    // Move disabled
    ItemSelectionModel(ItemSelectionModel &&) = delete;
    ItemSelectionModel & operator=(ItemSelectionModel &&) = delete;

   signals:

    /*! \brief Emitted when changing current row is requested
     */
    void currentRowChangeRequested(int row);

   public slots:

    /*! \brief Update current row
     *
     * Calls QAbstractItemModel setCurrentIndex() implementation,
     *  which will cause currentChanged() to be emitted, but not currentRowChangeRequested().
     */
    void updateCurrentRow(int row);

    /*! \brief Re-implemented to intercept row change
     *
     * \sa setCurrentIndex()
     */
    void select(const QModelIndex & index, QItemSelectionModel::SelectionFlags command) override;

    /*! \brief Re-implemented to intercept row change
     *
     * \sa setCurrentIndex()
     */
    void select(const QItemSelection & selection, QItemSelectionModel::SelectionFlags command) override;

    /*! \brief Re-implemented to intercept row change
     *
     * If index produces a column change, QItemSelectionModel implementation is called,
     *  else currentRowChangeRequested() is emited.
     */
    void setCurrentIndex(const QModelIndex & index, QItemSelectionModel::SelectionFlags command) override;

   private:

    bool canChangeToIndex(const QModelIndex & index) const;

    int mRequestedColumn = -1;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ITEM_SELECTION_MODEL_H
