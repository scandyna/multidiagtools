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
#ifndef MDT_ITEM_MODEL_RELATION_KEY_COPIER_H
#define MDT_ITEM_MODEL_RELATION_KEY_COPIER_H

#include "RelationKey.h"
#include "RowRange.h"
#include "ColumnRange.h"
#include "RowList.h"
#include <QPointer>
#include <QFlags>
#include <QModelIndex>

class QAbstractItemModel;

namespace Mdt{ namespace ItemModel{

  class PrimaryKey;
  class ForeignKey;

  /*! \brief Copies data from parent model to child model regarding relation key
   *
   * \par Keep models and relation key coherent
   *  There is no good solution for RelationKeyCopier to check
   *   if relation key is valid, regarding models, in a reliable way.
   *   The user of this class should take care of this.
   *   When setting parent model, child model, and relation key,
   *   no check will be done about columns that are to big regarding models.
   *   Then processing the copy, out of bound check is done,
   *   and nothing happens for a pair of columns when at least one is in invalid range.
   *   This also alows to change models and relation key at any time.
   */
  class RelationKeyCopier
  {
   public:

    /*! \brief Set parent model
     *
     * Will also set parent model current row to -1.
     *
     * RelationKeyCopier will not own the model
     *  (it will not delete it).
     *
     * \pre \a model must be a valid pointer
     */
    void setParentModel(QAbstractItemModel *model);

    /*! \brief Set child model
     *
     * RelationKeyCopier will not own the model
     *  (it will not delete it).
     *
     * \pre \a model must be a valid pointer
     */
    void setChildModel(QAbstractItemModel *model);

    /*! \brief Add a pair of columns
     *
     * \pre \a parentModelColumn must be >= 0
     * \pre \a childModelColumn must be >= 0
     */
    void addColumnPair(int parentModelColumn, int childModelColumn);

    /*! \brief Set relation key from parent model primary key and child model foreign key
     *
     * \pre parentModelPk must not be null
     * \pre childModelFk must not be null
     * \pre Both parentModelPk and childModelFk must have the same count of column indexes
     */
    void setKey(const PrimaryKey & parentModelPk, const ForeignKey & childModelFk);

    /*! \brief Set relation key
     *
     * \pre \a key must not be null
     */
    void setKey(const RelationKey & key);

    /*! \brief Get relation key
     */
    RelationKey key() const
    {
      return mKey;
    }

    /*! \brief Get parent model current row
     */
    int parentModelCurrentRow() const
    {
      return mParentModelCurrentRow;
    }

    /*! \brief Copy all key data from parent model to child model for given row range for child model
     *
     * \todo Define and fix preconditions
     */
    bool copyAllKeyData(RowRange childModelRowRange, const QModelIndex & parent = QModelIndex());

    /*! \brief Copy key data from parent model to child model
     *
     * Will only affect rows from \a childModelRowList in child model.
     *  Will also only copy data that exists in key regarding \a parentModelColumnRange .
     *
     * \todo Define and fix preconditions
     */
    bool copyKeyData(const RowList & childModelRowList, ColumnRange parentModelColumnRange);

    /*! \brief Set current row in parent model
     *
     * If parent model was not set, this slot does nothing.
     *
     * \pre If parent model is valid, row must be in range
     *       -1 <= row < parentModel->rowCount()
     */
    void setParentModelCurrentRow(int row);

   private:

    /*! \brief Copy key data from parent model to child model
     */
    bool copyKeyDataForRow(int childModelRow);

    /*! \brief Copy key data from parent model to child model
     */
    bool copyKeyDataForRow(int childModelRow, ColumnRange parentModelColumnRange);

    int mParentModelCurrentRow = -1;
    RelationKey mKey;
    QPointer<QAbstractItemModel> mParentModel;
    QPointer<QAbstractItemModel> mChildModel;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_RELATION_KEY_COPIER_H
