/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtDataWidgetMapper.h"
#include <QSqlRecord>
#include <QSqlField>

#include <QDebug>

mdtDataWidgetMapper::mdtDataWidgetMapper(QObject *parent)
 : QDataWidgetMapper(parent)
{
  pvSqlTableModel = 0;
}

mdtDataWidgetMapper::~mdtDataWidgetMapper()
{
}

void mdtDataWidgetMapper::setModel(mdtSqlTableModel *model)
{
  Q_ASSERT(model != 0);

  pvSqlTableModel = model;
  QDataWidgetMapper::setModel(model);
}

mdtSqlTableModel *mdtDataWidgetMapper::model()
{
  return pvSqlTableModel;
}

void mdtDataWidgetMapper::addMapping(QWidget *widget, int section)
{
  Q_ASSERT(widget != 0);
  
  setWidgetAttributes(widget, section);
  QDataWidgetMapper::addMapping(widget, section);
}

void mdtDataWidgetMapper::addMapping(QWidget *widget, int section, const QByteArray &propertyName)
{
  Q_ASSERT(widget != 0);
}

void mdtDataWidgetMapper::setModel(QAbstractItemModel *model)
{
}

void mdtDataWidgetMapper::setWidgetAttributes(QWidget *widget, int section)
{
  Q_ASSERT(widget != 0);
  Q_ASSERT(pvSqlTableModel != 0);

  QSqlField field;

  ///field = pvSqlTableModel->record().field(section);
  field = pvSqlTableModel->database().record(pvSqlTableModel->tableName()).field(section);
  qDebug() << "mdtDataWidgetMapper::setWidgetAttributes() , field: " << field;
}
