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
#include "mdtCsvParserSettingsWidget.h"

#include <QDebug>

mdtCsvParserSettingsWidget::mdtCsvParserSettingsWidget(QWidget *parent)
 : mdtCsvSettingsWidget(parent)
{
  setEolVisible(false);
  setAllwaysProtectTextFieldsVisible(false);
  // Update regarding default settings
  setSettings(mdtCsvParserSettings());

  connect(this, &mdtCsvParserSettingsWidget::fieldProtectionChanged, this, &mdtCsvParserSettingsWidget::onSettingsSelectionChanged);
  connect(this, &mdtCsvParserSettingsWidget::fieldSeparatorChanged, this, &mdtCsvParserSettingsWidget::onSettingsSelectionChanged);
  connect(this, &mdtCsvParserSettingsWidget::parseExpStateChanged, this, &mdtCsvParserSettingsWidget::onSettingsSelectionChanged);
}

void mdtCsvParserSettingsWidget::setSettings(const mdtCsvParserSettings & settings)
{
  selectFieldSeparator(settings.fieldSeparator);
  selectFieldProtection(settings.fieldProtection);
  setParseExp(settings.parseExp);
}

mdtCsvParserSettings mdtCsvParserSettingsWidget::getSettings() const
{
  mdtCsvParserSettings settings;

  settings.fieldSeparator = getSelectedFieldSeparator();
  settings.fieldProtection = getSelectedFieldProtection();
  settings.parseExp = parseExp();

  return settings;
}

void mdtCsvParserSettingsWidget::onSettingsSelectionChanged()
{
  qDebug() << "onSettingsSelectionChanged() ...";

  auto settings = getSettings();

  if(settings.isValid()){
    qDebug() << "----> CSV settings valid, signal..";
    emit csvSettingsChanged(settings);
  }
}
