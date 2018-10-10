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
#include "ItemViewTestEdit.h"
#include <QAbstractItemView>
#include <QModelIndex>
#include <QTestEventList>
#include <QtTest/QtTest>
#include <QApplication>

#include <QDebug>

void ItemViewTestEdit::beginEditing(QAbstractItemView & view, const QModelIndex & index, BeginEditTrigger trigger)
{
  Q_ASSERT(index.isValid());
  Q_ASSERT(index.model() == view.model());

  QApplication::setActiveWindow(&view);
  if(!QTest::qWaitForWindowActive(&view)){
    qWarning() << "beginEditing() - waiting for window active failed on view " << &view << " -> Did you call show() on view ?";
  }
  if(static_cast<QWidget *>(&view) != QApplication::activeWindow()){
    qWarning() << "beginEditing() - view " << &view << " is not the active window";
  }
  // Get view port (which is the widget to which event must be sent)
  QWidget *viewPort = view.viewport();
  Q_ASSERT(viewPort != nullptr);
  // Get center of region in view port that concerns given index
  QPoint itemCenter = view.visualRect(index).center();
  // Begin editing with requested trigger
  if(trigger == BeginEditTrigger::DoubleClick){
    Q_ASSERT(view.editTriggers() & QAbstractItemView::DoubleClicked);
    Q_ASSERT(view.isVisible());
    // Edition beginns after double click. With QTest, we must click before
    QTest::mouseClick(viewPort, Qt::LeftButton, 0, itemCenter);
    QTest::mouseDClick(viewPort, Qt::LeftButton, 0, itemCenter);
  }else if(trigger == BeginEditTrigger::F2KeyClick){
    Q_ASSERT( (view.editTriggers() & QAbstractItemView::EditKeyPressed) || (view.editTriggers() & QAbstractItemView::AnyKeyPressed) );
    Q_ASSERT(view.isVisible());
    view.setCurrentIndex(index);
    QTest::keyClick(viewPort, Qt::Key_F2);
  }else{
    qFatal("Unknown begin edit trigger");
  }
  // Make shure that triggers where handled before we return
  QApplication::processEvents();
}

void ItemViewTestEdit::editText(QAbstractItemView& view, const QModelIndex& editingIndex, const QString& str)
{
  Q_ASSERT(editingIndex.isValid());
  Q_ASSERT(editingIndex.model() == view.model());
  /*
   * We cannot send key clicks directly to view port of the view,
   * because a editor was created.
   * Get this editor and send events to it
   */
  auto *widget = view.indexWidget(editingIndex);
  Q_ASSERT(widget != nullptr);
  QTest::keyClicks(widget, str);
}

void ItemViewTestEdit::endEditing(QAbstractItemView & view, const QModelIndex & editingIndex, EndEditTrigger trigger)
{
  Q_ASSERT(editingIndex.isValid());
  Q_ASSERT(editingIndex.model() == view.model());

  switch(trigger){
    case EndEditTrigger::IndexChange:
      clickOnOtherItem(view, editingIndex);
      break;
    case EndEditTrigger::EnterKeyClick:
      clickEnterKey(view, editingIndex);
      break;
  }
}

void ItemViewTestEdit::edit(QAbstractItemView & view, const QModelIndex & index, const QString & str,
                            BeginEditTrigger beginEditTrigger, EndEditTrigger endEditTrigger)
{
  Q_ASSERT(index.isValid());
  Q_ASSERT(index.model() == view.model());

  beginEditing(view, index, beginEditTrigger);
  editText(view, index, str);
  endEditing(view, index, endEditTrigger);
}

void ItemViewTestEdit::clickOnOtherItem(QAbstractItemView & view, const QModelIndex & editingIndex)
{
  Q_ASSERT(!(view.editTriggers() & QAbstractItemView::SelectedClicked));
  Q_ASSERT(editingIndex.isValid());
  Q_ASSERT(editingIndex.model() == view.model());

  auto *model = view.model();
  Q_ASSERT(model != nullptr);

  // Choose a other row or column
  int row = editingIndex.row();
  int column = editingIndex.column();
  if(model->rowCount() > 1){
    if(row == 0){
      ++row;
    }else{
      --row;
    }
  }else{
    Q_ASSERT_X(model->columnCount() > 1, "ItemViewTestEdit::endEditing()", "model must at least have 2 rows or 2 columns when using IndexChange trigger");
    if(column == 0){
      ++column;
    }else{
      --column;
    }
  }
  auto index = model->index(row, column);
  Q_ASSERT(index.isValid());
  // Get view port (which is the widget to which event must be sent)
  QWidget *viewPort = view.viewport();
  Q_ASSERT(viewPort != nullptr);
  // Get center of region in view port that concerns given index
  QPoint itemCenter = view.visualRect(index).center();
  // Now, click to select new index
  QTest::mouseClick(viewPort, Qt::LeftButton, 0, itemCenter);
}

void ItemViewTestEdit::clickEnterKey(QAbstractItemView& view, const QModelIndex& editingIndex)
{
  // We click enter on current editor
  auto *widget = view.indexWidget(editingIndex);
  Q_ASSERT(widget != nullptr);
  QTest::keyClick(widget, Qt::Key_Enter);
  // Make shure that the key click was handled before we return
  QApplication::processEvents();
}
