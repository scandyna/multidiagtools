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
#ifndef MDT_ITEM_EDITOR_QT_DESIGNER_PLUGINS_WIDGET_MAPPER_WIDGET_H
#define MDT_ITEM_EDITOR_QT_DESIGNER_PLUGINS_WIDGET_MAPPER_WIDGET_H

#include <QObject>
#include <QtUiPlugin/QDesignerCustomWidgetInterface>

class WidgetMapperWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface
{
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)

 public:

  explicit WidgetMapperWidgetPlugin(QObject *parent = nullptr);
  bool isContainer() const;
  QIcon icon() const;
  QString domXml() const;
  QString group() const;
  QString includeFile() const;
  QString name() const;
  QString toolTip() const;
  QString whatsThis() const;
  QWidget *createWidget(QWidget *parent);
};

#endif // #ifndef MDT_ITEM_EDITOR_QT_DESIGNER_PLUGINS_WIDGET_MAPPER_WIDGET_H
