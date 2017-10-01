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
#ifndef MDT_ITEM_MODEL_EXPRESSION_FILTER_EVAL_DATA_H
#define MDT_ITEM_MODEL_EXPRESSION_FILTER_EVAL_DATA_H

#include "ParentModelEvalData.h"
#include <QPointer>
#include <QAbstractItemModel>
#include <Qt>

namespace Mdt{ namespace ItemModel{ namespace Expression{

  /*! \brief Data container for FilterEval
   */
  class FilterEvalData
  {
   public:

    /*! \brief Construct eval data
     *
     * \pre model must be a valid pointer
     * \pre row must be in valid range ( 0 <= row < model.rowCount() )
     */
    FilterEvalData(const QAbstractItemModel * const model, int row, Qt::CaseSensitivity cs)
     : mModel(model),
       mRow(row),
       mCaseSensitivity(cs),
       mParentData()
    {
      Q_ASSERT(mModel != nullptr);
      Q_ASSERT(mRow >= 0);
      Q_ASSERT(mRow < mModel->rowCount());
    }

    /*! \brief Construct eval data
     *
     * \pre \a model must be a valid pointer
     * \pre \a row must be in valid range ( 0 <= row < model.rowCount() )
     * \pre \a parentModelData must not be null
     */
    FilterEvalData(const QAbstractItemModel * const model, int row, const ParentModelEvalData & parentModelData, Qt::CaseSensitivity cs)
     : mModel(model),
       mRow(row),
       mCaseSensitivity(cs),
       mParentData(parentModelData)
    {
      Q_ASSERT(mModel != nullptr);
      Q_ASSERT(mRow >= 0);
      Q_ASSERT(mRow < mModel->rowCount());
      Q_ASSERT(!mParentData.isNull());
    }

    /*! \brief Get model
     */
    const QAbstractItemModel *model() const
    {
      return mModel;
    }

    /*! \brief Get row
     */
    int row() const
    {
      return mRow;
    }

    /*! \brief Get parent model
     */
    const QAbstractItemModel *parentModel() const
    {
      return mParentData.model();
    }

    /*! \brief Get parent model row
     */
    int parentModelRow() const
    {
      return mParentData.row();
    }

    /*! \brief Get case sensitivity
     */
    Qt::CaseSensitivity caseSensitivity() const
    {
      return mCaseSensitivity;
    }

   private:

    const QPointer<const QAbstractItemModel> mModel;
    const int mRow;
    const Qt::CaseSensitivity mCaseSensitivity;
    const ParentModelEvalData mParentData;
  };

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{

#endif // #ifndef MDT_ITEM_MODEL_EXPRESSION_FILTER_EVAL_DATA_H
