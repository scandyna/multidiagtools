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
#include "mdtCsvGeneratorSettingsWidget.h"
#include <QComboBox>
#include <QCheckBox>
#include <QString>
#include <QChar>
#include <QVariant>

//#include <QDebug>

mdtCsvGeneratorSettingsWidget::mdtCsvGeneratorSettingsWidget(QWidget *parent)
 : QWidget(parent)
{
  setupUi(this);
  // Fill field separator combobox
  cbFieldSeparator->addItem(",", ',');
  cbFieldSeparator->addItem(";", ';');
  cbFieldSeparator->addItem(":", ':');
  cbFieldSeparator->addItem("TAB", '\t');
  cbFieldSeparator->addItem("Space", ' ');
  // Fill field protection combobox
  cbFieldProtection->addItem("\"", '\"');
  cbFieldProtection->addItem("\'", '\'');
  // Fill EOL combobox
  cbEol->addItem("LF (Unix, OS X)", "\n");
  cbEol->addItem("CR (Old Mac OS)", "\r");
  cbEol->addItem("CRLF (Dos, Windows)", "\r\n");
  // Update regarding default settings
  setSettings(mdtCsvGeneratorSettings());
}

void mdtCsvGeneratorSettingsWidget::setSettings(const mdtCsvGeneratorSettings& settings)
{
  selectFieldSeparator(settings.fieldSeparator);
  selectFieldProtection(settings.fieldProtection);
  selectEol(settings.eol);
  cballwaysProtectTextFields->setChecked(settings.allwaysProtectTextFields);
}

mdtCsvGeneratorSettings mdtCsvGeneratorSettingsWidget::getSettings() const
{
  mdtCsvGeneratorSettings settings;

  settings.fieldSeparator = getSelectedFieldSeparator();
  settings.fieldProtection = getSelectedFieldProtection();
  settings.eol = getSelectedEol();
  settings.allwaysProtectTextFields = cballwaysProtectTextFields->isChecked();

  return settings;
}

void mdtCsvGeneratorSettingsWidget::selectFieldSeparator(char sep)
{
  // Try to find in existing items
  int index = cbFieldSeparator->findData(sep);
  if(index >= 0){
    cbFieldSeparator->setCurrentIndex(index);
  }else{
    // User defined separator
    cbFieldSeparator->setCurrentIndex(-1);
    cbFieldSeparator->setCurrentText(QChar(sep));
  }
}

char mdtCsvGeneratorSettingsWidget::getSelectedFieldSeparator() const
{
  char sep = '\0';
  QVariant var;

  // Try to find in existing items
  int index = cbFieldSeparator->currentIndex();
  if(index >= 0){
    var = cbFieldSeparator->itemData(index);
  }
  if(var.isNull()){
    // User defined separator
    QString str = cbFieldSeparator->currentText();
    if(str.size() == 1){
      sep = str.at(0).toLatin1();
    }
  }else{
    // Existing item
    sep = var.toChar().toLatin1();
  }

  return sep;
}

void mdtCsvGeneratorSettingsWidget::selectFieldProtection(char protection)
{
  // Try to find in existing items
  int index = cbFieldProtection->findData(protection);
  if(index >= 0){
    cbFieldProtection->setCurrentIndex(index);
  }else{
    // User defined protection
    cbFieldProtection->setCurrentIndex(-1);
    cbFieldProtection->setCurrentText(QChar(protection));
  }
}

char mdtCsvGeneratorSettingsWidget::getSelectedFieldProtection() const
{
  char protection = '\0';
  QVariant var;

  // Try to find in existing items
  int index = cbFieldProtection->currentIndex();
  if(index >= 0){
    var = cbFieldProtection->itemData(index);
  }
  if(var.isNull()){
    // User defined protection
    QString str = cbFieldProtection->currentText();
    if(str.size() == 1){
      protection = str.at(0).toLatin1();
    }
  }else{
    // Existing item
    protection = var.toChar().toLatin1();
  }

  return protection;
}

void mdtCsvGeneratorSettingsWidget::selectEol(const std::string & eol)
{
  int index = cbEol->findData(QString::fromStdString(eol));
  cbEol->setCurrentIndex(index);
}

std::string mdtCsvGeneratorSettingsWidget::getSelectedEol() const
{
  std::string eol;
  int index = cbEol->currentIndex();

  eol = cbEol->itemData(index).toString().toStdString();

  return eol;
}
