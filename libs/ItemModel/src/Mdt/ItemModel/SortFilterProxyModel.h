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
#ifndef MDT_ITEM_MODEL_SORT_FILTER_PROXY_MODEL_H
#define MDT_ITEM_MODEL_SORT_FILTER_PROXY_MODEL_H

#include <QSortFilterProxyModel>

namespace Mdt{ namespace ItemModel{

  /*! \brief Common base class for proxy models that do sorting or filtering
   */
  class SortFilterProxyModel : public QSortFilterProxyModel
  {
   Q_OBJECT

   public:

    /*! \brief Construct a proxy model
     */
    explicit SortFilterProxyModel(QObject* parent = nullptr);

    // Destructor - unique_ptr needs to know how to destruct RelationKeyCopier
//     ~SortFilterProxyModel();
    // Disable copy
    SortFilterProxyModel(const SortFilterProxyModel &) = delete;
    SortFilterProxyModel & operator=(const SortFilterProxyModel &) = delete;
    // Disable move
    SortFilterProxyModel(SortFilterProxyModel &&) = delete;
    SortFilterProxyModel & operator=(SortFilterProxyModel &&) = delete;

    /*! \brief Reimplemented from QSortFilterProxyModel
     */
    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;

   protected:

    /*! \brief Reimplemented from QSortFilterProxyModel
     */
//     bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

    /*! \brief Check if rows are about to be inserted into this proxy model
     */
    bool insertingRows() const
    {
      return mInsertingRows;
    }

   private:

    bool mInsertingRows = false;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_SORT_FILTER_PROXY_MODEL_H
