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
#include "EventCatchItemDelegate.h"

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

EventCatchItemDelegate::EventCatchItemDelegate(QObject* parent)
 : ItemDelegateProxy(parent)
{
}

QWidget* EventCatchItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  pvCurrentEditor = ItemDelegateProxy::createEditor(parent, option, index);
  emit dataEditionStarted();
  return pvCurrentEditor;
}

void EventCatchItemDelegate::destroyEditor(QWidget* editor, const QModelIndex& index) const
{
  pvCurrentEditor.clear();
  ItemDelegateProxy::destroyEditor(editor, index);
  emit dataEditionDone();
}

void EventCatchItemDelegate::commitCurrentEditorData()
{
  if(!pvCurrentEditor.isNull()){
    commitData(pvCurrentEditor);
    closeEditor(pvCurrentEditor, EventCatchItemDelegate::NoHint);
  }
}

void EventCatchItemDelegate::closeCurrentEditor()
{
  if(!pvCurrentEditor.isNull()){
    closeEditor(pvCurrentEditor, EventCatchItemDelegate::NoHint);
  }
}

// void EventCatchItemDelegate::setControllerState(ControllerState state)
// {
// 
// }

}} // namespace Mdt{ namespace ItemEditor{
