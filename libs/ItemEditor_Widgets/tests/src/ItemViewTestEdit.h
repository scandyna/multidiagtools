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
#ifndef MDT_ITEM_EDITOR_ITEM_VIEW_TEST_EDIT_H
#define MDT_ITEM_EDITOR_ITEM_VIEW_TEST_EDIT_H

#include "ItemViewTestEditTriggers.h"
#include <QString>

class QAbstractItemView;
class QModelIndex;
class QTestEventList;

/*
 * Helper to edit item views
 */
class ItemViewTestEdit
{
 public:


  static void beginEditing(QAbstractItemView & view, const QModelIndex & index, BeginEditTrigger trigger);

  static void editText(QAbstractItemView & view, const QModelIndex & editingIndex, const QString & str);

  static void endEditing(QAbstractItemView & view, const QModelIndex & editingIndex, EndEditTrigger trigger);

  static void edit(QAbstractItemView & view, const QModelIndex & index, const QString & str, BeginEditTrigger beginEditTrigger, EndEditTrigger endEditTrigger);

 private:

  static void clickOnOtherItem(QAbstractItemView & view, const QModelIndex & editingIndex);
  static void clickEnterKey(QAbstractItemView & view, const QModelIndex & editingIndex);
};

#endif // #ifndef MDT_ITEM_EDITOR_ITEM_VIEW_TEST_EDIT_H
