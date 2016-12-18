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
#ifndef MDT_ITEM_MODEL_EXPRESSION_PARENT_MODEL_EVAL_DATA_H
#define MDT_ITEM_MODEL_EXPRESSION_PARENT_MODEL_EVAL_DATA_H

#include <QPointer>
#include <QAbstractItemModel>

namespace Mdt{ namespace ItemModel{ namespace Expression{

  /*! \brief Holds data of parent model for FilterEval
   */
  class ParentModelEvalData
  {
   public:

    /*! \brief Construct null data
     */
    explicit ParentModelEvalData() = default;

    /*! \brief Construct valid data
     *
     * \pre model must be a valid pointer
     * \pre row must be in valid range ( 0 <= row < model->rowCount() )
     */
    explicit ParentModelEvalData(const QAbstractItemModel * const model, int row)
     : mRow(row),
       mModel(model)
    {
      Q_ASSERT(!mModel.isNull());
      Q_ASSERT(mRow >= 0);
      Q_ASSERT(mRow < mModel->rowCount());
    }

    /*! \brief Check if data is null
     */
    bool isNull() const noexcept
    {
      return mModel.isNull();
    }

    /*! \brief Get model
     */
    const QAbstractItemModel *model() const noexcept
    {
      return mModel;
    }

    /*! \brief Get row
     */
    int row() const noexcept
    {
      return mRow;
    }

   private:

    const int mRow = -1;
    const QPointer<const QAbstractItemModel> mModel;
  };

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{

#endif // #ifndef MDT_ITEM_MODEL_EXPRESSION_PARENT_MODEL_EVAL_DATA_H
