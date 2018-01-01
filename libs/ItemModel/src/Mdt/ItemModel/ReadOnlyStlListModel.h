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
#ifndef MDT_ITEM_MODEL_READ_ONLY_STL_LIST_MODEL_H
#define MDT_ITEM_MODEL_READ_ONLY_STL_LIST_MODEL_H

#include "AbstractStlListModel.h"
#include <QModelIndex>
#include <QVariant>
#include <iterator>

namespace Mdt{ namespace ItemModel{

  /*! \brief Read only list model for STL compliant container
   *
   * \tparam Container Type of a STL compatible container,
   *    able to work on BidirectionalIterator .
   *    To be usable, \a Container class must provide:
   *     - The cbegin() method
   *     - The cend() method
   *     - The value_type : must be a type that QVariant can handle.
   *           See QMetaType, especially Q_DECLARE_METATYPE() for custom types.
   */
  template<typename Container>
  class ReadOnlyStlListModel : public AbstractStlListModel
  {
   public:

    /*! \brief STL compatible value type
     */
    using value_type = typename Container::value_type;

    /*! \brief Set a instance of a container
     *
     * \note \a container will be copied to this model.
     *   Any changes made to \a container directly
     *   will not be reflected in this model.
     */
    void setContainer(const Container & container)
    {
      beginResetModel();
      mContainer = container;
      endResetModel();
    }

    /*! \brief Acces the internal container instance
     */
    const Container & container() const
    {
      return mContainer;
    }

    /*! \brief Get row count
     */
    int rowCount() const
    {
      return std::distance(mContainer.cbegin(), mContainer.cend());
    }

    /*! \brief Get row count
     */
    int rowCount(const QModelIndex & parent) const override
    {
      if(parent.isValid()){
        return 0;
      }
      return rowCount();
    }

    /*! \brief Access element at \a row
     *
     * \pre \a row must be in valid range ( 0 <= row < rowCount() ).
     */
    const value_type & at(int row) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());

      return *std::next(mContainer.cbegin(), row);
    }

    /*! \brief Get data at \a index
     *
     * \pre if \a index is valid, its row must be in valid range ( 0 <= index.row() < rowCount() ).
     */
    QVariant data(const QModelIndex & index, int role) const override
    {
      if(!index.isValid()){
        return QVariant();
      }
      if( (role != Qt::DisplayRole) && (role != Qt::EditRole) ){
        return QVariant();
      }
      Q_ASSERT(index.row() >= 0);
      Q_ASSERT(index.row() < rowCount());

      return at(index.row());
    }

   private:

    Container mContainer;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_READ_ONLY_STL_LIST_MODEL_H
