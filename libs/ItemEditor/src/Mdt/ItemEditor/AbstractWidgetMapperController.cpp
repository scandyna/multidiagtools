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
#include "AbstractWidgetMapperController.h"
#include <QDataWidgetMapper>

namespace Mdt{ namespace ItemEditor{

AbstractWidgetMapperController::AbstractWidgetMapperController(QDataWidgetMapper *mapper, QObject* parent)
 : AbstractController(parent),
   pvWidgetMapper(mapper)
{
  Q_ASSERT(pvWidgetMapper != nullptr);
  pvWidgetMapper->setParent(this);
  connect(this, &AbstractWidgetMapperController::currentRowChanged, pvWidgetMapper, &QDataWidgetMapper::setCurrentIndex);
}

void AbstractWidgetMapperController::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  referenceItemModel(model);
  pvWidgetMapper->setModel(model);
  registerItemModel();
}

void AbstractWidgetMapperController::addMapping(QWidget* widget, int section)
{
  pvWidgetMapper->addMapping(widget, section);
}

void AbstractWidgetMapperController::addMapping(QWidget* widget, int section, const QByteArray& propertyName)
{
  pvWidgetMapper->addMapping(widget, section, propertyName);
}


}} // namespace Mdt{ namespace ItemEditor{
