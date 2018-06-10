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
#ifndef MDT_ITEM_MODEL_REMOVE_ROWS_H
#define MDT_ITEM_MODEL_REMOVE_ROWS_H

#include "RowRange.h"
#include "RowSelection.h"
#include "MdtItemModelExport.h"
#include <QAbstractItemModel>
#include <QItemSelectionModel>

namespace Mdt{ namespace ItemModel{

  /*! \brief Remove rows defined in \a rowRange from \a model
   *
   * \pre \a rowRange must be valid
   * \pre \a rowRange must be a valid range in \a model ( rowRange.lastRow() < model->rowCount() ).
   */
  bool MDT_ITEMMODEL_EXPORT removeRows(QAbstractItemModel *model, const RowRange & rowRange);

  /*! \brief Remove rows defined in \a selection from \a model
   *
   * \pre \a selection must not be empty
   * \pre \a selection must have valid ranges for \a model ( for each range: range.firstRow() >= 0 AND range.lastRow() < model->rowCount() )
   */
  bool MDT_ITEMMODEL_EXPORT removeRows(QAbstractItemModel *model, const RowSelection & selection);

  /*! \brief Remove the selected rows in \a selectionModel from \a model
   *
   * Will take in account each selected row for all columns in \a selectionModel
   *  ( it does not call QItemSelectionModel::selectedRows(int column) ).
   *  To know how the row selection is made, see RowSelection .
   */
  bool MDT_ITEMMODEL_EXPORT removeRows(QAbstractItemModel *model, const QItemSelectionModel * selectionModel);

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_REMOVE_ROWS_H
