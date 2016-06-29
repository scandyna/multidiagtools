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
#include "ItemDelegateProxy.h"

namespace Mdt{ namespace ItemEditor{

ItemDelegateProxy::ItemDelegateProxy(QObject* parent)
 : QAbstractItemDelegate(parent)
{
}

ItemDelegateProxy::~ItemDelegateProxy()
{
}

void ItemDelegateProxy::setItemDelegate(QAbstractItemDelegate* delegate)
{
  Q_ASSERT(delegate != nullptr);
  pvDelegate = delegate;
}

QAbstractItemDelegate* ItemDelegateProxy::itemDelegate() const
{
  return pvDelegate.data();
}

QWidget* ItemDelegateProxy::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    return QAbstractItemDelegate::createEditor(parent, option, index);
  }
  return pvDelegate->createEditor(parent, option, index);
}

void ItemDelegateProxy::destroyEditor(QWidget* editor, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    QAbstractItemDelegate::destroyEditor(editor, index);
  }else{
    pvDelegate->destroyEditor(editor, index);
  }
}

bool ItemDelegateProxy::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
  if(pvDelegate.isNull()){
    return QAbstractItemDelegate::editorEvent(event, model, option, index);
  }
  return pvDelegate->editorEvent(event, model, option, index);
}

bool ItemDelegateProxy::helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& option, const QModelIndex& index)
{
  if(pvDelegate.isNull()){
    return QAbstractItemDelegate::helpEvent(event, view, option, index);
  }
  return pvDelegate->helpEvent(event, view, option, index);
}

void ItemDelegateProxy::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if(!pvDelegate.isNull()){
    pvDelegate->paint(painter, option, index);
  }
}

void ItemDelegateProxy::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    QAbstractItemDelegate::setEditorData(editor, index);
  }else{
    pvDelegate->setEditorData(editor, index);
  }
}

void ItemDelegateProxy::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    QAbstractItemDelegate::setModelData(editor, model, index);
  }else{
    pvDelegate->setModelData(editor, model, index);
  }
}

QSize ItemDelegateProxy::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    return QSize();
  }
  return pvDelegate->sizeHint(option, index);
}

void ItemDelegateProxy::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    QAbstractItemDelegate::updateEditorGeometry(editor, option, index);
  }else{
    pvDelegate->updateEditorGeometry(editor, option, index);
  }
}

}} // namespace Mdt{ namespace ItemEditor{