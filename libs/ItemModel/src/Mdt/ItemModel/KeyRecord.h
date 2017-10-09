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
#ifndef MDT_ITEM_MODEL_KEY_RECORD_H
#define MDT_ITEM_MODEL_KEY_RECORD_H

#include "KeyData.h"
#include "MdtItemModelExport.h"
#include <vector>

namespace Mdt{ namespace ItemModel{

  /*! \brief List of data for a specific row and key in a item model
   */
  class MDT_ITEMMODEL_EXPORT KeyRecord
  {
   public:

    /*! \brief Const iterator
     */
    using const_iterator = std::vector<KeyData>::const_iterator;

    /*! \brief Append data for column
     *
     * \pre \a column must be >= 0
     * \pre No data must previously been set for \a column
     */
    void append(int column, const QVariant & data);

    /*! \brief Clear this record
     */
    void clear();

    /*! \brief Get count of columns
     */
    int columnCount() const
    {
      return mRecord.size();
    }

    /*! \brief Check if record is empty
     */
    bool isEmpty() const
    {
      return mRecord.empty();
    }

    /*! \brief Get column at index
     *
     * \pre \a index must be in valid range ( 0 <= index < columnCount() )
     */
    int columnAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < columnCount());
      return mRecord.at(index).column();
    }

    /*! \brief Get data at index
     *
     * \pre \a index must be in valid range ( 0 <= index < columnCount() )
     */
    QVariant dataAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < columnCount());
      return mRecord.at(index).data();
    }

    /*! \brief Get data for column
     *
     * \pre \a column must be >= 0
     */
    QVariant dataForColumn(int column) const;

    /*! \brief Get a const iterator to the beginning
     */
    const_iterator cbegin() const
    {
      return mRecord.cbegin();
    }

    /*! \brief Get a const iterator to the end
     */
    const_iterator cend() const
    {
      return mRecord.cend();
    }

   private:

    const_iterator iteratorForColumn(int column) const;

    std::vector<KeyData> mRecord;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_KEY_RECORD_H
