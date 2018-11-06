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
#ifndef MDT_TEST_LIB_ITEM_MODEL_H
#define MDT_TEST_LIB_ITEM_MODEL_H

#include "MdtTestLib_CoreExport.h"
#include <Qt>

class QAbstractItemModel;
class QVariant;

namespace Mdt{ namespace TestLib{

  /*! \brief Get flags from \a model at \a row and \a column for \a role
   *
   * \pre \a model must be a valid pointer
   * \pre \a row must be in valid range ( 0 <= \a row < model->rowCount() )
   * \pre \a column must be in valid range ( 0 <= \a column < model->columnCount() )
   */
  Qt::ItemFlags MDT_TESTLIB_CORE_EXPORT getModelFlags(const QAbstractItemModel *model, int row, int column);

  /*! \brief Get flags from \a model at \a row and \a column for \a role
   *
   * \pre \a row must be in valid range ( 0 <= \a row < model->rowCount() )
   * \pre \a column must be in valid range ( 0 <= \a column < model->columnCount() )
   */
  Qt::ItemFlags MDT_TESTLIB_CORE_EXPORT getModelFlags(const QAbstractItemModel & model, int row, int column);

  /*! \brief Get the default flags for a QAbstractTableModel
   */
  Qt::ItemFlags MDT_TESTLIB_CORE_EXPORT getDefaultQAbstractTableModelFlags();

  /*! \brief Get data from \a model at \a row and \a column for \a role
   *
   * \pre \a model must be a valid pointer
   * \pre \a row must be in valid range ( 0 <= \a row < model->rowCount() )
   * \pre \a column must be in valid range ( 0 <= \a column < model->columnCount() )
   */
  QVariant MDT_TESTLIB_CORE_EXPORT getModelData(const QAbstractItemModel *model, int row, int column, Qt::ItemDataRole role = Qt::DisplayRole);

  /*! \brief Get data from \a model at \a row and \a column for \a role
   *
   * \sa getModelData(const QAbstractItemModel *, int, int, Qt::ItemDataRole)
   */
  QVariant MDT_TESTLIB_CORE_EXPORT getModelData(const QAbstractItemModel & model, int row, int column, Qt::ItemDataRole role = Qt::DisplayRole);

  /*! \brief Set \a value to \a model at \a row and \a column for \a role
   *
   * \pre \a model must be a valid pointer
   * \pre \a row must be in valid range ( 0 <= \a row < model->rowCount() )
   * \pre \a column must be in valid range ( 0 <= \a column < model->columnCount() )
   */
  bool MDT_TESTLIB_CORE_EXPORT setModelData(QAbstractItemModel *model, int row, int column, const QVariant & value, Qt::ItemDataRole role = Qt::EditRole);

  /*! \brief Set \a value to \a model at \a row and \a column for \a role
   *
   * \sa setModelData(QAbstractItemModel *, int, int, const QVariant &, Qt::ItemDataRole);
   */
  bool MDT_TESTLIB_CORE_EXPORT setModelData(QAbstractItemModel & model, int row, int column, const QVariant & value, Qt::ItemDataRole role = Qt::EditRole);

  /*! \brief Prepend a row to \a model
   */
  bool MDT_TESTLIB_CORE_EXPORT prependRowToModel(QAbstractItemModel *model);

  /*! \brief Prepend a row to \a model
   */
  bool MDT_TESTLIB_CORE_EXPORT prependRowToModel(QAbstractItemModel & model);

  /*! \brief Append a row to \a model
   */
  bool MDT_TESTLIB_CORE_EXPORT appendRowToModel(QAbstractItemModel *model);

  /*! \brief Append a row to \a model
   */
  bool MDT_TESTLIB_CORE_EXPORT appendRowToModel(QAbstractItemModel & model);

  /*! \brief Remove the first row from \a model
   *
   * \pre \a model must not be empty
   */
  bool MDT_TESTLIB_CORE_EXPORT removeFirstRowFromModel(QAbstractItemModel *model);

  /*! \brief Remove the first row from \a model
   *
   * \pre \a model must not be empty
   */
  bool MDT_TESTLIB_CORE_EXPORT removeFirstRowFromModel(QAbstractItemModel & model);

  /*! \brief Remove the last row from \a model
   *
   * \pre \a model must not be empty
   */
  bool MDT_TESTLIB_CORE_EXPORT removeLastRowFromModel(QAbstractItemModel *model);

  /*! \brief Remove the last row from \a model
   *
   * \pre \a model must not be empty
   */
  bool MDT_TESTLIB_CORE_EXPORT removeLastRowFromModel(QAbstractItemModel & model);

}} // namespace Mdt{ namespace TestLib{

#endif // #ifndef MDT_TEST_LIB_ITEM_MODEL_H
