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
#include "mdtCsvSettingsWidget.h"
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QString>
#include <QChar>
#include <QVariant>

//#include <QDebug>

mdtCsvSettingsWidget::mdtCsvSettingsWidget(QWidget *parent)
 : QWidget(parent)
{
  setupUi(this);
  connect(cbParseExp, &QCheckBox::stateChanged, this, &mdtCsvSettingsWidget::parseExpStateChanged);
  // Fill field protection combobox
  cbFieldProtection->addItem("\"", '\"');
  cbFieldProtection->addItem("\'", '\'');
  connect(cbFieldProtection, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &mdtCsvSettingsWidget::fieldProtectionChanged);
  // Setup and fill field separator combobox
  cbFieldSeparator->addItem(",", ',');
  cbFieldSeparator->addItem(";", ';');
  cbFieldSeparator->addItem(":", ':');
  cbFieldSeparator->addItem("TAB", '\t');
  cbFieldSeparator->addItem("Space", ' ');
  connect(cbFieldSeparator, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &mdtCsvSettingsWidget::fieldSeparatorChanged);
  // Fill EOL combobox
  cbEol->addItem("LF (Unix, OS X)", "\n");
  cbEol->addItem("CR (Old Mac OS)", "\r");
  cbEol->addItem("CRLF (Dos, Windows)", "\r\n");
}

void mdtCsvSettingsWidget::setParseExpVisible(bool visible)
{
  cbParseExp->setVisible(visible);
}

void mdtCsvSettingsWidget::setEolVisible(bool visible)
{
  cbEol->setVisible(visible);
  lbEol->setVisible(visible);
}

void mdtCsvSettingsWidget::setAllwaysProtectTextFieldsVisible(bool visible)
{
  cballwaysProtectTextFields->setVisible(visible);
}

void mdtCsvSettingsWidget::selectFieldSeparator(char sep)
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

char mdtCsvSettingsWidget::getSelectedFieldSeparator() const
{
  char sep = '\0';
  QVariant var;

  // Try to find existing item related to current displayed text
  var = findItemDataForItemText(cbFieldSeparator);
  // If we have a null item, the user defined his own separator
  if(var.isNull()){
    var = cbFieldSeparator->currentText();
  }
  // Set sep
  sep = getCharFromVariant(var);

  return sep;
}

void mdtCsvSettingsWidget::selectFieldProtection(char protection)
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

char mdtCsvSettingsWidget::getSelectedFieldProtection() const
{
  char sep = '\0';
  QVariant var;

  // Try to find existing item related to current displayed text
  var = findItemDataForItemText(cbFieldProtection);
  // If we have a null item, the user defined his own protection
  if(var.isNull()){
    var = cbFieldProtection->currentText();
  }
  // Set sep
  sep = getCharFromVariant(var);

  return sep;
}

void mdtCsvSettingsWidget::selectEol(const std::string & eol)
{
  int index = cbEol->findData(QString::fromStdString(eol));
  cbEol->setCurrentIndex(index);
}

std::string mdtCsvSettingsWidget::getSelectedEol() const
{
  std::string eol;
  int index = cbEol->currentIndex();

  eol = cbEol->itemData(index).toString().toStdString();

  return eol;
}

void mdtCsvSettingsWidget::setAllwaysProtectTextFields(bool set)
{
  cballwaysProtectTextFields->setChecked(set);
}

bool mdtCsvSettingsWidget::allwaysProtectTextFields() const
{
  return cballwaysProtectTextFields->isChecked();
}

void mdtCsvSettingsWidget::setParseExp(bool set)
{
  cbParseExp->setChecked(set);
}

bool mdtCsvSettingsWidget::parseExp() const
{
  return cbParseExp->isChecked();
}

QVariant mdtCsvSettingsWidget::findItemDataForItemText(QComboBox* cb) const
{
  Q_ASSERT(cb != nullptr);

  QVariant var;
  int index = cb->findText(cb->currentText());
  if(index < 0){
    return var;
  }
  var = cb->itemData(index);

  return var;
}

char mdtCsvSettingsWidget::getCharFromVariant(const QVariant& var) const
{
  switch(var.type()){
    case QVariant::Char:
    case QVariant::Int:
      return var.toChar().toLatin1();
    case QVariant::String:
    {
      QString str = var.toString();
      if(str.length() == 1){
        return str.at(0).toLatin1();
      }
    }
    default:
      return '\0';
  }
}
