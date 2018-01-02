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
#ifndef MDT_ITEM_MODEL_READ_ONLY_STL_TABLE_MODEL_H
#define MDT_ITEM_MODEL_READ_ONLY_STL_TABLE_MODEL_H

#include "AbstractStlTableModel.h"
#include "StlContainer.h"
#include "MdtItemModelExport.h"
#include <QModelIndex>
#include <QVariant>

#include <iterator>
// #include <algorithm>

namespace Mdt{ namespace ItemModel{

  /*! \brief Read only table model for STL compliant containers
   *
   * \tparam Table Type of a STL compatible container,
   *    able to work on BidirectionalIterator .
   *    The values of the container are records,
   *    that also must be STL compatible.
   *    To be usable, \a Table class must provide:
   *     - The cbegin() method
   *     - The cend() method
   *     - The value_type (used in this class as record_type) :
   *        must be a STL compatible container,
   *        that also provides cbegin() and cend() .
   *
   * \note Goal of STL table models is to be able to work
   *    on containers on a generic way, using the STL iterators concept.
   *    However, each record of a container must have the same count
   *    of items, refered here as column count of the table model.
   */
  template<typename Table>
  class MDT_ITEMMODEL_EXPORT ReadOnlyStlTableModel : public AbstractStlTableModel
  {
   public:

    /*! \brief STL compatible record type
     */
    using record_type = typename Table::value_type;

    /*! \brief STL compatible value type
     *
     * Must be a type that QVariant can handle.
     *  See QMetaType, especially Q_DECLARE_METATYPE() for custom types.
     */
    using value_type = typename record_type::value_type;

    /*! \brief Set a instance of a container
     *
     * \note \a container will be copied to this model.
     *   Any changes made to \a container directly
     *   will not be reflected in this model.
     *
     * \pre Each record in \a container must contain
     *       exaclty the same count of items.
     */
    void setContainer(const Table & container)
    {
      Q_ASSERT(eachRecordHasSameColumnCount(container));

      beginResetModel();
      mContainer = container;
      endResetModel();
    }

    /*! \brief Acces the internal container instance
     */
    const Table & container() const
    {
      return mContainer;
    }

    /*! \brief Acces the internal container instance
     */
    Table & container()
    {
      return mContainer;
    }

    /*! \brief Check if empty
     */
    bool isEmpty() const
    {
      return (rowCount() == 0);
    }

    /*! \brief Get row count
     */
    int rowCount() const
    {
      return containerSize(mContainer);
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

    /*! \brief Get column count
     */
    int columnCount() const
    {
      if(isEmpty()){
        return 0;
      }
      return containerSize(*mContainer.cbegin());
    }

    /*! \brief Get column count
     */
    int columnCount(const QModelIndex & parent) const override
    {
      if(parent.isValid()){
        return 0;
      }
      return columnCount();
    }

    /*! \brief Access record at \a row
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     */
    const record_type & record(int row) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());

      return *std::next(mContainer.cbegin(), row);
    }

    /*! \brief Access element at \a row and \a column
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    const value_type & at(int row, int column) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());

      return *constIteratorAtRowColumn(mContainer, row, column);
    }

    /*! \brief Get data at \a index
     *
     * If \a index is invalid, a null QVariant is returned.
     *
     * If \a role is different than Qt::DisplayRole,
     *  a null QVariant is returned.
     *
     * \pre If \a index is valid, it must be in a valid range:
     *   - its row must be in valid range ( 0 <= index.row() < rowCount() )
     *   - its column must be in valid range ( 0 <= index.column() < columnCount() )
     */
    QVariant data(const QModelIndex & index, int role) const override
    {
      if(!index.isValid()){
        return QVariant();
      }
      if(role != Qt::DisplayRole){
        return QVariant();
      }
      Q_ASSERT(index.row() >= 0);
      Q_ASSERT(index.row() < rowCount());
      Q_ASSERT(index.column() >= 0);
      Q_ASSERT(index.column() < columnCount());

      return at(index.row(), index.column());
    }

   protected:

   private:

    Table mContainer;

  };
}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_READ_ONLY_STL_TABLE_MODEL_H
