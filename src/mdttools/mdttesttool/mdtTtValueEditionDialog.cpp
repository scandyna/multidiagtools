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
#include "mdtTtValueEditionDialog.h"
#include "mdtTtValueLimitsWidget.h"
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include <QDebug>

mdtTtValueEditionDialog::mdtTtValueEditionDialog(QWidget* parent)
 : QDialog(parent)
{
  setupUi(this);
  pvLimitsWidget = new mdtTtValueLimitsWidget;
  Q_ASSERT(dynamic_cast<QVBoxLayout*>(layout()) != 0);
  auto *l = static_cast<QVBoxLayout*>(layout());
  l->insertWidget(l->count()-2, pvLimitsWidget);
  connect(deValue, SIGNAL(valueChanged(double,bool)), this, SLOT(onValueChanged(double,bool)));
  connect(pvLimitsWidget, SIGNAL(limitEdited()), this, SLOT(onLimitEdited()));
//   connect(deValue, SIGNAL(valueChanged(double,bool)), this, SLOT(onValueChanged(double,bool)));
//   connect(sbLimitMin, SIGNAL(valueChanged(double)), this, SLOT(onLimitMinChanged(double)));
//   connect(sbLimitMax, SIGNAL(valueChanged(double)), this, SLOT(onLimitMaxChanged(double)));
//   connect(sbFailMin, SIGNAL(valueChanged(double)), this, SLOT(onFailMinChanged(double)));
//   connect(sbFailMax, SIGNAL(valueChanged(double)), this, SLOT(onFailMaxChanged(double)));
  setOkButtonEnabled(false);
}

void mdtTtValueEditionDialog::setValue(const mdtValueDouble & x)
{
  if(x.isNull()){
    setValueVar(QVariant());
  }else{
    setValueVar(x.value());
  }
}

void mdtTtValueEditionDialog::setValueVar(const QVariant & v)
{
  deValue->setValue(v);
}

mdtValueDouble mdtTtValueEditionDialog::value() const
{
  QVariant v = deValue->value();
  if(v.isNull()){
    return mdtValueDouble();
  }
  return v.toDouble();
}

QVariant mdtTtValueEditionDialog::valueVar() const
{
  return deValue->value();
}

void mdtTtValueEditionDialog::setLimits(const mdtTtValueLimits & l)
{
  pvLimitsWidget->setLimits(l);
}

mdtTtValueLimits mdtTtValueEditionDialog::limits() const
{
  return pvLimitsWidget->limits();
}

void mdtTtValueEditionDialog::setUnit(const QString & u)
{
  deValue->setUnit(u);
  pvLimitsWidget->setUnit(u);
}

// QVariant mdtTtValueEditionDialog::limitValueMin() const
// {
//   if(deValue->value().isNull()){
//     return QVariant();
//   }
// }
// 
// QVariant mdtTtValueEditionDialog::limitValueMax() const
// {
//   if(deValue->value().isNull()){
//     return QVariant();
//   }
// 
// }
// 
// QVariant mdtTtValueEditionDialog::failValueMin() const
// {
//   if(deValue->value().isNull()){
//     return QVariant();
//   }
// 
// }
// 
// QVariant mdtTtValueEditionDialog::failValueMax() const
// {
//   if(deValue->value().isNull()){
//     return QVariant();
//   }
// 
// }

void mdtTtValueEditionDialog::onValueChanged(double x, bool isValid)
{
  qDebug() << "onValueChanged() - x: " << x << " , valid: " << isValid;
  if(isValid && (!deValue->isNull()) ){
    validateLimits(x);
  }else{
    validateLimits(mdtValueDouble());
  }
}

void mdtTtValueEditionDialog::onLimitEdited()
{
  if(deValue->isNull()){
    validateLimits(mdtValueDouble());
  }else{
    validateLimits(deValue->valueDouble());
  }
}

void mdtTtValueEditionDialog::validateLimits(const mdtValueDouble & x)
{
  if(x.isNull()){
    setOkButtonEnabled(false);
    return;
  }
  if(pvLimitsWidget->limits().isValid(x)){
    setOkButtonEnabled(true);
  }else{
    setOkButtonEnabled(false);
  }
}

// void mdtTtValueEditionDialog::onLimitMinChanged(double x)
// {
//   qDebug() << "onLimitMinChanged() - x: " << x;
//   // Check coherence with fail min value
//   if(x < sbFailMin->value()){
//     sbFailMin->setValue(x);
//   }
// }
// 
// void mdtTtValueEditionDialog::onLimitMaxChanged(double x)
// {
//   qDebug() << "onLimitMaxChanged() - x: " << x;
//   // Check coherence with fail max value
//   if(x > sbFailMax->value()){
//     sbFailMax->setValue(x);
//   }
// }
// 
// void mdtTtValueEditionDialog::onFailMinChanged(double x)
// {
//   qDebug() << "onFailMinChanged() - x: " << x;
//   // Check coherence with limit min value
//   if(x > sbLimitMin->value()){
//     sbLimitMin->setValue(x);
//   }
// }
// 
// void mdtTtValueEditionDialog::onFailMaxChanged(double x)
// {
//   qDebug() << "onFailMaxChanged() - x: " << x;
//   // Check coherence with limit max value
//   if(x < sbLimitMax->value()){
//     sbLimitMax->setValue(x);
//   }
// }

void mdtTtValueEditionDialog::setOkButtonEnabled(bool enable)
{
  QPushButton *pb = buttonBox->button(QDialogButtonBox::Ok);
  Q_ASSERT(pb != 0);
  pb->setEnabled(enable);
}
