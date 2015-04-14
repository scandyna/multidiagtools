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
#include "mdtDoubleEditDialog.h"

mdtDoubleEditDialog::mdtDoubleEditDialog(QWidget* parent)
 : QDialog(parent)
{
  setupUi(this);
  setLabelText("");
}

void mdtDoubleEditDialog::setLabelText(const QString & text)
{
  lbLabel->setText(text);
}

QString mdtDoubleEditDialog::labelText() const
{
  return lbLabel->text();
}

double mdtDoubleEditDialog::minimum() const
{
  return deEdit->minimum();
}

void mdtDoubleEditDialog::setMinimumToMinusInfinity()
{
  deEdit->setMinimumToMinusInfinity();
}

void mdtDoubleEditDialog::setMinimum(double min)
{
  deEdit->setMinimum(min);
}

double mdtDoubleEditDialog::maximum() const
{
  return deEdit->maximum();
}

void mdtDoubleEditDialog::setMaximum(double max)
{
  deEdit->setMaximum(max);
}

void mdtDoubleEditDialog::setMaximumToInfinity()
{
  deEdit->setMaximumToInfinity();
}

void mdtDoubleEditDialog::setRange(double min, double max)
{
  deEdit->setRange(min, max);
}

void mdtDoubleEditDialog::setValue(const QVariant & v)
{
  deEdit->setValue(v, false);
}

void mdtDoubleEditDialog::setValueDouble(double v)
{
  deEdit->setValueDouble(v, false);
}

double mdtDoubleEditDialog::valueDouble() const
{
  return deEdit->valueDouble();
}

QVariant mdtDoubleEditDialog::value() const
{
  return deEdit->value();
}

void mdtDoubleEditDialog::accept()
{
  if(deEdit->validate()){
    QDialog::accept();
  }
}
