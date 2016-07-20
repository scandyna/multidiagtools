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
#ifndef MDT_ITEM_EDITOR_EDITION_START_EVENT_CATCH_DELEGATE_H
#define MDT_ITEM_EDITOR_EDITION_START_EVENT_CATCH_DELEGATE_H

#include "ItemDelegateProxy.h"

class QWidget;

namespace Mdt{ namespace ItemEditor{

  /*! \brief EditionStartEventCatchDelegate is used by AbstractWidgetMapperController to catch edition beginning of its mapped editors
   */
  class EditionStartEventCatchDelegate : public ItemDelegateProxy
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit EditionStartEventCatchDelegate(QObject* parent);

//     bool eventFilter(QObject *watched, QEvent *event) override;

    /*! \brief Re-implemented to catch starting of edition
     */
//     void setEditorData(QWidget *editor, const QModelIndex & index) const override;

    /*! \brief Process editor events
     */
//     bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem & option, const QModelIndex & index) override;

    /*! \brief Connect notify signal for propertyName of widget
     *
     * \pre propertyName must refer a existing property,
     *       which must have a notify signal
     * \pre widget must be a valid pointer
     */
//     void connectEditor(QWidget * const widget, const QByteArray & propertyName);

    /*! \brief Connect notify signal for user property of widget
     *
     * \pre widget must have a user property,
     *       which must have a notify signal
     * \pre widget must be a valid pointer
     */
//     void connectEditor(QWidget * const widget, const QByteArray & propertyName);

   private:

    
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_EDITION_START_EVENT_CATCH_DELEGATE_H
