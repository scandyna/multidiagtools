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
#include "mdtSqlCopierTableMappingWidgetItem.h"
#include "mdtSqlCopierTableMappingWidget.h"
#include <QToolButton>

mdtSqlCopierTableMappingWidgetItem::mdtSqlCopierTableMappingWidgetItem(QWidget* parent, mdtSqlCopierTableMappingWidget *owner)
 : QWidget(parent),
   pvOwner(owner)
{
  Q_ASSERT(pvOwner != nullptr);

  setupUi(this);
  connect(tbEditFieldMapping, &QToolButton::clicked, this, &mdtSqlCopierTableMappingWidgetItem::editFieldMapping);
  connect(tbRemoveFieldMapping, &QToolButton::clicked, this, &mdtSqlCopierTableMappingWidgetItem::removeFieldMapping);
}

void mdtSqlCopierTableMappingWidgetItem::setSeparationLineVisible(bool visible)
{
  pvSeparationLine->setVisible(visible);
}

void mdtSqlCopierTableMappingWidgetItem::setSourceFieldName(const QString & name)
{
  lbSourceField->setText(name);
}

void mdtSqlCopierTableMappingWidgetItem::setDestinationFieldName(const QString & name)
{
  lbDestinationField->setText(name);
}

void mdtSqlCopierTableMappingWidgetItem::editFieldMapping()
{
  pvOwner->editFieldMapping(this);
}

void mdtSqlCopierTableMappingWidgetItem::removeFieldMapping()
{
  pvOwner->removeFieldMapping(this);
}
