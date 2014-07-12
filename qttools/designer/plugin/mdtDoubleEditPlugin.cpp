/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtDoubleEditPlugin.h"
#include "mdtDoubleEdit.h"
#include "mdtDoubleValidator.h"
#include <QtPlugin>

mdtDoubleEditPlugin::mdtDoubleEditPlugin(QObject* parent)
 : QObject(parent)
{
  pvInitialized = false;
}

bool mdtDoubleEditPlugin::isContainer() const
{
  return false;
}

bool mdtDoubleEditPlugin::isInitialized() const
{
  return pvInitialized;
}

QIcon mdtDoubleEditPlugin::icon() const
{
  return QIcon();
}

QString mdtDoubleEditPlugin::domXml() const
{
  return "<ui language=\"c++\">\n"\
         " <widget class=\"mdtDoubleEdit\" name=\"mdtdoubleedit\">\n"\
         " </widget>\n"\
         "</ui>\n";
}

QString mdtDoubleEditPlugin::group() const
{
  return "mdt widgets";
}

QString mdtDoubleEditPlugin::includeFile() const
{
  return "mdtDoubleEdit.h";
}

QString mdtDoubleEditPlugin::name() const
{
  return "mdtDoubleEdit";
}

QString mdtDoubleEditPlugin::toolTip() const
{
  return "Provide a input box for double values with power of 10 suffixes";
}

QString mdtDoubleEditPlugin::whatsThis() const
{
  return "Double values input box";
}

QWidget* mdtDoubleEditPlugin::createWidget(QWidget* parent)
{
  return new mdtDoubleEdit(parent);
}

void mdtDoubleEditPlugin::initialize(QDesignerCustomWidgetInterface * /*core*/)
{
  if(pvInitialized){
    return;
  }
  pvInitialized = true;
}

Q_EXPORT_PLUGIN2(mdtdoubleeditplugin, mdtDoubleEditPlugin)  // For Qt4
