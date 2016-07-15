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
#ifndef MDT_ITEM_EDITOR_ITEM_SELECTION_MODEL_H
#define MDT_ITEM_EDITOR_ITEM_SELECTION_MODEL_H

#include "ControllerState.h"
#include <QItemSelectionModel>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Provides some specific stuff for some editors
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

    /*! \brief Check if it os allowed to change current row
     */
    bool isCurrentRowChangeAllowed() const
    {
      return pvCurrentRowChangeAllowed;
    }

    void setCurrentIndexFromCtl(const QModelIndex & index, QItemSelectionModel::SelectionFlags command);

   public slots:

    /*! \brief Set controller state
     */
    void setControllerState(ControllerState state);

    /*! \brief Re-implemented to prevent row change if not allowed
     */
    void select(const QModelIndex & index, QItemSelectionModel::SelectionFlags command) override;

    /*! \brief Re-implemented to prevent row change if not allowed
     */
    void select(const QItemSelection & selection, QItemSelectionModel::SelectionFlags command) override;

    /*! \brief Re-implemented to prevent row change if not allowed
     */
    void setCurrentIndex(const QModelIndex & index, QItemSelectionModel::SelectionFlags command) override;

   private:

    bool canAcceptIndex(const QModelIndex & index) const;

    bool pvCurrentRowChangeAllowed = true;
    int pvPreviousCurrentRow = -1;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ITEM_SELECTION_MODEL_H
