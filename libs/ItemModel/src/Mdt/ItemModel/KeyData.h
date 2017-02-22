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
#ifndef MDT_ITEM_MODEL_KEY_DATA_H
#define MDT_ITEM_MODEL_KEY_DATA_H

#include <QVariant>

namespace Mdt{ namespace ItemModel{

  /*! \brief Contains data for a specific column in a item model
   *
   * \sa KeyRecord
   */
  class KeyData
  {
   public:

    /*! \brief Construct a key data
     *
     * \pre \a column must be >= 0
     */
    explicit KeyData(int column, const QVariant & data);

    /*! \brief Get column
     */
    int column() const
    {
      return mColumn;
    }

    /*! \brief Get data
     */
    QVariant data() const
    {
      return mData;
    }

   private:

    int mColumn;
    QVariant mData;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_KEY_DATA_H
