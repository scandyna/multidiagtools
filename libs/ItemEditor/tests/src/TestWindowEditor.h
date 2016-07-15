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
#ifndef MDT_ITEM_EDITOR_TEST_WINDOW_EDITOR_H
#define MDT_ITEM_EDITOR_TEST_WINDOW_EDITOR_H

#include "Mdt/ItemEditor/StandardWindow.h"

class TestTableEditionWidget;
class TestFormEditionWidget;
class QAbstractTableModel;

class TestWindowEditor : public Mdt::ItemEditor::StandardWindow
{
 Q_OBJECT

 public:

  TestWindowEditor(QWidget* parent = nullptr);

  void setModel(QAbstractTableModel *model);

 private:

//   TestTableEditionWidget *pvTableEditionWidget;
  TestFormEditionWidget *pvFormEditionWidget;
};

#endif // #ifndef MDT_ITEM_EDITOR_TEST_WINDOW_EDITOR_H
