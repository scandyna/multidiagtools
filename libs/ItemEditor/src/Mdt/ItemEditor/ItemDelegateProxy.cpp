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

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

ItemDelegateProxy::ItemDelegateProxy(QObject* parent)
 : QStyledItemDelegate(parent)
{
}

ItemDelegateProxy::~ItemDelegateProxy()
{
}

void ItemDelegateProxy::setItemDelegate(QAbstractItemDelegate* delegate)
{
  Q_ASSERT(delegate != nullptr);
  auto *styledDelegate = qobject_cast<QStyledItemDelegate*>(delegate);
  Q_ASSERT(styledDelegate != nullptr);
  pvDelegate = styledDelegate;
}

QStyledItemDelegate* ItemDelegateProxy::itemDelegate() const
{
  return pvDelegate.data();
}

QWidget* ItemDelegateProxy::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    return QStyledItemDelegate::createEditor(parent, option, index);
  }
  return pvDelegate->createEditor(parent, option, index);
}

void ItemDelegateProxy::destroyEditor(QWidget* editor, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    QStyledItemDelegate::destroyEditor(editor, index);
  }else{
    pvDelegate->destroyEditor(editor, index);
  }
}

bool ItemDelegateProxy::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
  if(pvDelegate.isNull()){
    return QStyledItemDelegate::editorEvent(event, model, option, index);
  }
  // QStyledItemDelegate::editorEvent() is protected, we must do some cast here
  return reinterpret_cast<QAbstractItemDelegate*>(pvDelegate.data())->editorEvent(event, model, option, index);
}

bool ItemDelegateProxy::helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& option, const QModelIndex& index)
{
  if(pvDelegate.isNull()){
    return QStyledItemDelegate::helpEvent(event, view, option, index);
  }
  return pvDelegate->helpEvent(event, view, option, index);
}

void ItemDelegateProxy::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    QStyledItemDelegate::paint(painter, option, index);
  }else{
    pvDelegate->paint(painter, option, index);
  }
}

void ItemDelegateProxy::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    QStyledItemDelegate::setEditorData(editor, index);
  }else{
    pvDelegate->setEditorData(editor, index);
  }
}

void ItemDelegateProxy::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    QStyledItemDelegate::setModelData(editor, model, index);
  }else{
    pvDelegate->setModelData(editor, model, index);
  }
}

QSize ItemDelegateProxy::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    return QStyledItemDelegate::sizeHint(option, index);
  }
  return pvDelegate->sizeHint(option, index);
}

void ItemDelegateProxy::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if(pvDelegate.isNull()){
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
  }else{
    pvDelegate->updateEditorGeometry(editor, option, index);
  }
}

QString ItemDelegateProxy::displayText(const QVariant& value, const QLocale& locale) const
{
  if(pvDelegate.isNull()){
    return QStyledItemDelegate::displayText(value, locale);
  }
  return pvDelegate->displayText(value, locale);
}

QItemEditorFactory* ItemDelegateProxy::itemEditorFactory() const
{
  if(pvDelegate.isNull()){
    return QStyledItemDelegate::itemEditorFactory();
  }
  return pvDelegate->itemEditorFactory();
}

void ItemDelegateProxy::setItemEditorFactory(QItemEditorFactory* factory)
{
  if(pvDelegate.isNull()){
    QStyledItemDelegate::setItemEditorFactory(factory);
  }else{
    pvDelegate->setItemEditorFactory(factory);
  }
}

}} // namespace Mdt{ namespace ItemEditor{