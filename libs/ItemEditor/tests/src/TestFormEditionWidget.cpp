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
#include "TestFormEditionWidget.h"
#include <QAbstractItemModel>

#include <QDebug>

using namespace Mdt::ItemEditor;

TestFormEditionWidget::TestFormEditionWidget(QWidget* parent)
 : AbstractEditorWidget(parent)
{
  pvController = new WidgetMapperController(this);
  setController(pvController);
  setupUi(this);
}

void TestFormEditionWidget::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  pvController->setModel(model);
  pvController->addMapping(leName, 0);
  pvController->addMapping(leRemarks, 1);
}
