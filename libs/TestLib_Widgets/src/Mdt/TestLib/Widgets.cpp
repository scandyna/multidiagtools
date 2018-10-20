/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "Widgets.h"
#include <QWidget>
#include <QComboBox>
#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <Qt>
#include <QtTest/QTest>

// #include <QDebug>

namespace Mdt{ namespace TestLib{

bool isItemSelected(QAbstractItemView & view, const QModelIndex & index)
{
  Q_ASSERT(view.selectionModel() != nullptr);

  return view.selectionModel()->isSelected(index);
}

void mouseClickOnItem(QAbstractItemView & view, const QModelIndex & index)
{
  Q_ASSERT(view.model() != nullptr);
  Q_ASSERT(index.isValid());

  // Get view port (which is the widget to which event must be sent)
  QWidget *viewPort = view.viewport();
  Q_ASSERT(viewPort != nullptr);
  // Get center of region in view port that concerns given index
  QPoint itemCenter = view.visualRect(index).center();
  // Now, click to select new index
  QTest::mouseClick(viewPort, Qt::LeftButton, 0, itemCenter);
}

void mouseSelectRow(QComboBox & comboBox, int row)
{
  Q_ASSERT(comboBox.view() != nullptr);
  Q_ASSERT(comboBox.model() != nullptr);
  Q_ASSERT(comboBox.model()->columnCount() > 0);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < comboBox.model()->rowCount());

  const auto index = comboBox.model()->index(row, 0);
  Q_ASSERT(index.isValid());

  QTest::mouseClick(&comboBox, Qt::LeftButton);
  mouseClickOnItem(*comboBox.view(), index);
  QTest::keyClick(comboBox.view(), Qt::Key_Enter);
}

}} // namespace Mdt{ namespace TestLib{
