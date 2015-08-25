/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtSqlTableMappingWidget.h"
#include "mdtSqlTableMappingWidgetItem.h"
#include "mdtSqlFieldMappingData.h"
#include "mdtSqlFieldMappingDialog.h"
#include <QToolButton>
#include <QVBoxLayout>

#include <QDebug>

mdtSqlTableMappingWidget::mdtSqlTableMappingWidget(QWidget* parent)
 : QWidget(parent),
   pvItemsContainerWidget(new QWidget),
   pvItemsContainerLayout(new QVBoxLayout)
{
  setupUi(this);
  pvItemsContainerLayout->addStretch(1);
  pvItemsContainerWidget->setLayout(pvItemsContainerLayout);
  saItems->setWidget(pvItemsContainerWidget);
  connect(tbRemoveFieldMapping, &QToolButton::clicked, this, &mdtSqlTableMappingWidget::addFieldMapping);
}

void mdtSqlTableMappingWidget::addFieldMapping()
{
  Q_ASSERT(pvItemsContainerLayout->count() > 0);  // A stretch was added in constructor

  auto *item = new mdtSqlTableMappingWidgetItem(this, this);
  pvItemsContainerLayout->insertWidget(pvItemsContainerLayout->count() - 1, item);
}

void mdtSqlTableMappingWidget::editFieldMapping(mdtSqlTableMappingWidgetItem* item)
{
  Q_ASSERT(item != nullptr);

  mdtSqlFieldMappingDialog dialog(this);

  dialog.exec();
}

void mdtSqlTableMappingWidget::removeFieldMapping(mdtSqlTableMappingWidgetItem* item)
{
  Q_ASSERT(item != nullptr);

  pvItemsContainerLayout->removeWidget(item);
  delete item;
}
