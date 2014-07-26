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
#include "mdtTtValueEditionDialog.h"
#include <QDoubleSpinBox>

#include <QDebug>

mdtTtValueEditionDialog::mdtTtValueEditionDialog(QWidget* parent)
 : QDialog(parent)
{
  setupUi(this);
  connect(deValue, SIGNAL(valueChanged(double,bool)), this, SLOT(onValueChanged(double,bool)));
  connect(sbLimitMin, SIGNAL(valueChanged(double)), this, SLOT(onLimitMinChanged(double)));
  connect(sbLimitMax, SIGNAL(valueChanged(double)), this, SLOT(onLimitMaxChanged(double)));
  connect(sbFailMin, SIGNAL(valueChanged(double)), this, SLOT(onFailMinChanged(double)));
  connect(sbFailMax, SIGNAL(valueChanged(double)), this, SLOT(onFailMaxChanged(double)));
}

QVariant mdtTtValueEditionDialog::limitValueMin() const
{
  if(deValue->value().isNull()){
    return QVariant();
  }
}

QVariant mdtTtValueEditionDialog::limitValueMax() const
{
  if(deValue->value().isNull()){
    return QVariant();
  }

}

QVariant mdtTtValueEditionDialog::failValueMin() const
{
  if(deValue->value().isNull()){
    return QVariant();
  }

}

QVariant mdtTtValueEditionDialog::failValueMax() const
{
  if(deValue->value().isNull()){
    return QVariant();
  }

}

void mdtTtValueEditionDialog::onValueChanged(double x, bool isValid)
{
  qDebug() << "onValueChanged() - x: " << x << " , valid: " << isValid;
}

void mdtTtValueEditionDialog::onLimitMinChanged(double x)
{
  qDebug() << "onLimitMinChanged() - x: " << x;
  // Check coherence with fail min value
  if(x < sbFailMin->value()){
    sbFailMin->setValue(x);
  }
}

void mdtTtValueEditionDialog::onLimitMaxChanged(double x)
{
  qDebug() << "onLimitMaxChanged() - x: " << x;
  // Check coherence with fail max value
  if(x > sbFailMax->value()){
    sbFailMax->setValue(x);
  }
}

void mdtTtValueEditionDialog::onFailMinChanged(double x)
{
  qDebug() << "onFailMinChanged() - x: " << x;
  // Check coherence with limit min value
  if(x > sbLimitMin->value()){
    sbLimitMin->setValue(x);
  }
}

void mdtTtValueEditionDialog::onFailMaxChanged(double x)
{
  qDebug() << "onFailMaxChanged() - x: " << x;
  // Check coherence with limit max value
  if(x < sbLimitMax->value()){
    sbLimitMax->setValue(x);
  }
}
