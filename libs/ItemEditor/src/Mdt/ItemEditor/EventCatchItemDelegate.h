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
#ifndef MDT_ITEM_EDITOR_EVENT_CATCH_ITEM_DELEGATE_H
#define MDT_ITEM_EDITOR_EVENT_CATCH_ITEM_DELEGATE_H

#include "ItemDelegateProxy.h"

namespace Mdt{ namespace ItemEditor{

  /*! \brief Catches event from a item delegate
   */
  class EventCatchItemDelegate : public ItemDelegateProxy
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit EventCatchItemDelegate(QObject* parent = nullptr);

    // Copy disabled
    EventCatchItemDelegate(const EventCatchItemDelegate &) = delete;
    EventCatchItemDelegate & operator=(const EventCatchItemDelegate &) = delete;
    // Move disabled
    EventCatchItemDelegate(EventCatchItemDelegate &&) = delete;
    EventCatchItemDelegate & operator=(EventCatchItemDelegate &&) = delete;

    /*! \brief Re-implementation to emit dataEditionStarted()
     */
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override;

    /*! \brief Re-implementation to emit dataEditionDone()
     */
    void destroyEditor(QWidget *editor, const QModelIndex & index) const override;

   signals:

    /*! \brief Emitted when data edition started
     */
    void dataEditionStarted() const;

    /*! \brief Emitted once data edtion was done
     */
    void dataEditionDone() const;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_EVENT_CATCH_ITEM_DELEGATE_H
