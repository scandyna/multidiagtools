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
#include "EditionStartEventCatchDelegate.h"

#include <QEvent>
#include <QKeyEvent>

#include <QDebug>

namespace Mdt{ namespace ItemEditor{

EditionStartEventCatchDelegate::EditionStartEventCatchDelegate(QObject* parent)
 : ItemDelegateProxy(parent)
{
}

// bool EditionStartEventCatchDelegate::eventFilter(QObject* watched, QEvent* event)
// {
//   qDebug() << "eventFilter() - watched: " << watched << " , event: " << event->type();
//   if( (event->type() == QEvent::KeyPress) || (event->type() == QEvent::Drop) ){
//     qDebug() << "eventFilter() - watched: " << watched << " , event: " << event->type();
//   }
//   return ItemDelegateProxy::eventFilter(watched, event);
// }

// void EditionStartEventCatchDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
// {
//   qDebug() << "setEditorData() ..";
//   ItemDelegateProxy::setEditorData(editor, index);
// }
// 
// bool EditionStartEventCatchDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
// {
//   qDebug() << "editorEvent() : " << event;
//   return ItemDelegateProxy::editorEvent(event, model, option, index);
// }

}} // namespace Mdt{ namespace ItemEditor{
