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
#ifndef MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_MAPPER_H
#define MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_MAPPER_H

#include <QObject>
#include <QPointer>
#include <QModelIndex>

class QAbstractItemModel;
class QItemSelectionModel;

namespace Mdt{ namespace ItemEditor{

  /*! \brief Signal/slot proxy that relays current row and row count
   *
   * Collects row count and current row from various Qt item/view objects
   *  and send uniform signals.
   */
  class RowChangeEventMapper : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Construct a RowChangeEventMapper
     */
    RowChangeEventMapper(QObject *parent = nullptr);

    /*! \brief Set selection model
     *
     * Will do required signal/slot connections
     *  and update item model by calling setModel().
     */
    void setSelectionModel(QItemSelectionModel *model);

   public slots:

    /*! \brief Set model
     *
     * This will also do required signal/slot connections
     */
    void setModel(QAbstractItemModel *model);

   private slots:

    /*! \brief Set current row
     *
     * Called from selection model
     */
    void setCurrentIndex(const QModelIndex & current, const QModelIndex & previous);

    /*! \brief Called when model was reset
     */
    void onModelReset();

    /*! \brief Called when rows has been inserted to the model
     */
    void onRowsInserted(const QModelIndex & parent, int first, int last);

    /*! \brief Called when rows has been removed from the model
     */
    void onRowsRemoved(const QModelIndex & parent, int first, int last);

   signals:

    /*! \brief Emited whenever row count changes
     */
    void rowCountChanged(int rowCount);

    /*! \brief Emited whenever current row changes
     */
    void currentRowChanged(int row);

   private:

    QPointer<QAbstractItemModel> pvModel;
    QPointer<QItemSelectionModel> pvSelectionModel;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_MAPPER_H
