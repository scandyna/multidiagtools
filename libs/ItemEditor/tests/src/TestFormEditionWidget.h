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
#ifndef MDT_ITEM_EDITOR_TEST_FORM_EDITION_WIDGET_H
#define MDT_ITEM_EDITOR_TEST_FORM_EDITION_WIDGET_H

#include "Mdt/ItemEditor/WidgetMapperWidget.h"

#include "Mdt/ItemEditor/AbstractEditorWidget.h"
#include "Mdt/ItemEditor/WidgetMapperController.h"
#include "ui_TestFormEditionWidget.h"

class QAbstractItemModel;

class TestFormEditionWidget : public Mdt::ItemEditor::AbstractEditorWidget, Ui::TestFormEditionWidget
{
 Q_OBJECT

 public:

  explicit TestFormEditionWidget(QWidget* parent = nullptr);

  void setModel(QAbstractItemModel *model);

 private:

  Mdt::ItemEditor::WidgetMapperController *pvController;
};

#endif // #ifndef MDT_ITEM_EDITOR_TEST_FORM_EDITION_WIDGET_H
