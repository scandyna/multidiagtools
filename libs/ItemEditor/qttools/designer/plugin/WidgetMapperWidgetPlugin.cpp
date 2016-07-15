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
#include "WidgetMapperWidgetPlugin.h"
#include <QIcon>
#include <QString>

#include <QLineEdit>

WidgetMapperWidgetPlugin::WidgetMapperWidgetPlugin(QObject* parent)
 : QObject(parent)
{
}

bool WidgetMapperWidgetPlugin::isContainer() const
{
  return false;
}

QIcon WidgetMapperWidgetPlugin::icon() const
{
  return QIcon();
}

QString WidgetMapperWidgetPlugin::domXml() const
{
  return "<ui language=\"c++\">\n"\
         " <widget class=\"Mdt::ItemEditor::WidgetMapperWidget\" name=\"WidgetMapperWidget\">\n"\
         "  <property name=\"geometry\">\n"\
         "   <rect>\n"\
         "    <x>0</x>\n"\
         "    <y>0</y>\n"\
         "    <width>100</width>\n"\
         "    <height>100</height>\n"\
         "   </rect>\n"\
         "  </property>\n"\
         " </widget>\n"\
         "</ui>\n";
}

QString WidgetMapperWidgetPlugin::group() const
{
  return tr("Item Views (Model-Based)");
}

QString WidgetMapperWidgetPlugin::includeFile() const
{
  return "Mdt/ItemEditor/WidgetMapperWidget.h";
}

QString WidgetMapperWidgetPlugin::name() const
{
  return "Mdt::ItemEditor::WidgetMapperWidget";
}

QString WidgetMapperWidgetPlugin::toolTip() const
{
  return "";
}

QString WidgetMapperWidgetPlugin::whatsThis() const
{
  return tr("Item editor widget based on QDataWidgetMapper");
}

QWidget* WidgetMapperWidgetPlugin::createWidget(QWidget* parent)
{
//   return new Mdt::ItemEditor::WidgetMapperWidget(parent);
  return nullptr;
}
