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
#ifndef MDT_ITEM_EDITOR_EVENT_CATCH_ITEM_DELEGATE_H
#define MDT_ITEM_EDITOR_EVENT_CATCH_ITEM_DELEGATE_H

#include "ItemDelegateProxy.h"
#include "ControllerState.h"

#include <QPointer>

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

    /*! \brief Re-implementation from ItemDelegateProxy
     *
     * If edition is allowed, dataEditionStarted() is emitted
     *  and ItemDelegateProxy::createEditor() is called.
     *  Otherwise, a nullptr is returned.
     *
     * \todo Define if this has sense, and for which state(s) exactly.
     *       If a main/parent widget is editing, a better solution
     *       is to disable its childs, this alos gives a feedback to the user.
     */
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override;

    /*! \brief Re-implementation to emit dataEditionDone()
     */
    void destroyEditor(QWidget *editor, const QModelIndex & index) const override;

    /*! \brief Commit current editor data to model
     */
    void commitCurrentEditorData();

    /*! \brief Close current editor
     */
    void closeCurrentEditor();

//    public slots:
// 
//     /*! \brief Set controller state
//      */
//     void setControllerState(ControllerState state);

   signals:

    /*! \brief Emitted when data edition started
     */
    void dataEditionStarted() const;

    /*! \brief Emitted once data edtion was done
     */
    void dataEditionDone() const;
    
  private:
    
    mutable QPointer<QWidget> pvCurrentEditor;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_EVENT_CATCH_ITEM_DELEGATE_H
