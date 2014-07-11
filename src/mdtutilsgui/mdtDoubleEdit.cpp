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
#include "mdtDoubleEdit.h"
#include "mdtDoubleValidator.h"
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <limits>

#include <QDebug>

mdtDoubleEdit::mdtDoubleEdit(QWidget* parent)
 : QWidget(parent)
{
  QHBoxLayout *l = new QHBoxLayout;
  // Push buttons layout
  QVBoxLayout *pbLayout = new QVBoxLayout;
  pbLayout->setContentsMargins(0, 0, 0, 0);
  pbLayout->setSpacing(0);
  pbSetInfinity = new QPushButton(" \u221E");
  pbSetMinusInfinity = new QPushButton("-\u221E");
  // Main layout
  pvLineEdit = new QLineEdit;
  pvLineEdit->setAlignment(Qt::AlignRight);
  l->addWidget(pvLineEdit);
  l->addWidget(pbSetMinusInfinity);
  l->addWidget(pbSetInfinity);
  setLayout(l);

  connect(pbSetMinusInfinity, SIGNAL(clicked()), this, SLOT(setMinusInfinity()));
  connect(pbSetInfinity, SIGNAL(clicked()), this, SLOT(setInfinity()));
  
  pvValidator = new mdtDoubleValidator(this);
  pvLineEdit->setValidator(pvValidator);
  connect(pvLineEdit, SIGNAL(editingFinished()), this, SLOT(setValueFromLineEdit()));
  // Set initial value
  pvValue = 0.0;
  pvValueIsValid = false;
}

void mdtDoubleEdit::setUnit(const QString& unit)
{
  pvUnit = unit;
  pvValidator->setSuffix(pvUnit);
}


double mdtDoubleEdit::factor(const QChar & c)
{
  switch(c.toAscii()){
    case 'a':
      return 1e-18;
    case 'f':
      return 1e-15;
    case 'p':
      return 1e-12;
    case 'n':
      return 1e-9;
    case 'u':
      return 1e-6;
    case 'm':
      return 1e-3;
    case 'c':
      return 1e-2;
    case 'd':
      return 1e-1;
    case 'h':
      return 1e2;
    case 'k':
      return 1e3;
    case 'M':
      return 1e6;
    case 'G':
      return 1e9;
    case 'T':
      return 1e12;
    case 'P':
      return 1e15;
    case 'E':
      return 1e18;
    default:
      return 1.0;
  }
}

/**
void mdtDoubleEdit::setValue(const QString& val)
{

}
*/

void mdtDoubleEdit::setMinusInfinity()
{
  pvValue = -std::numeric_limits<double>::infinity();
  pvValueIsValid = true;
  displayValue();
}

void mdtDoubleEdit::setInfinity()
{
  pvValue = std::numeric_limits<double>::infinity();
  pvValueIsValid = true;
  displayValue();
}

void mdtDoubleEdit::setValueFromLineEdit()
{
  convertAndSetValue(pvLineEdit->text());
  displayValue();
}

void mdtDoubleEdit::convertAndSetValue(QString str)
{
  double f;
  QChar fc;

  str = str.trimmed();
  // Remove unit part
  str.remove(pvUnit);
  str = str.trimmed();
  // Get power of 10 factor and remove it from str
  if(str.isEmpty()){
    pvValueIsValid = false;
    displayValue();
    return;
  }
  fc = str.at(str.size() - 1);
  f = factor(fc);
  if(qAbs<double>(f - 1.0) <=  std::numeric_limits<double>::min()){
    fc = '\0';
  }
  if(!fc.isNull()){
    str.remove(fc);
  }
  str = str.trimmed();
  qDebug() << "str: " << str;
  // Do conversion
  pvValue = str.toDouble(&pvValueIsValid);
  if(pvValueIsValid){
    pvValue = pvValue * f;
  }
}

void mdtDoubleEdit::displayValue()
{
  double f;
  double x;
  QChar fc;
  QString str;

  if(!pvValueIsValid){
    pvLineEdit->clear();
    return;
  }
  // Check if we are - or + infinity
  if(pvValue == -std::numeric_limits<double>::infinity()){
    pvLineEdit->setText("-\u221E");
    return;
  }
  if(pvValue == std::numeric_limits<double>::infinity()){
    pvLineEdit->setText("\u221E");
    return;
  }
  // Select a power of 10 regarding value - Note: we ignore c and d and h
  x = qAbs<double>(pvValue);
  if(x < 1.e-18){
    f = 1.e18;
    fc = 'a';
  }else if(isInRange(x, 1e-18, 1e-15)){
    f = 1e15;
    fc = 'f';
  }else if(isInRange(x, 1e-15, 1e-12)){
    f = 1e12;
    fc = 'p';
  }else if(isInRange(x, 1e-9, 1e-6)){
    f = 1e9;
    fc = 'n';
  }else if(isInRange(x, 1e-6, 1e-3)){
    f = 1e6;
    fc = 'u';
  }else if(isInRange(x, 1e-3, 1.0)){
    f = 1e3;
    fc = 'm';
  }else if(isInRange(x, 1.0, 1e3)){
    f = 1.0;
  }else if(isInRange(x, 1e3, 1e6)){
    f = 1e-3;
    fc = 'k';
  }else if(isInRange(x, 1e6, 1e9)){
    f = 1e-6;
    fc = 'M';
  }else if(isInRange(x, 1e9, 1e12)){
    f = 1e-9;
    fc = 'G';
  }else if(isInRange(x, 1e12, 1e15)){
    f = 1e-12;
    fc = 'T';
  }else if(isInRange(x, 1e15, 1e18)){
    f = 1e-15;
    fc = 'P';
  }else{
    f = 1e-18;
    fc = 'E';
  }
  // Build string to display
  str.setNum(pvValue * f);
  str += " ";
  if(!fc.isNull()){
    str += fc;
  }
  str += pvUnit;
  // Display str
  pvLineEdit->setText(str);
}

bool mdtDoubleEdit::isInRange(double x, double min, double max)
{
  ///x = qAbs<double>(x);
  
  // x in [min, max[ => (x >= min)&&(x < max)
  qDebug() << "x: " << x << ", min: " << min << " , max: " << max;
  return ( ((x > min)||(qAbs<double>(x-min) < std::numeric_limits<double>::min()) ) && (x < max) );
/**
  if(x < 1.0){
    // x in ]min, max] => (x > min)&&(x <= max)
    return ((x > min) && (qAbs<double>(x-max) < std::numeric_limits<double>::min()));
  }else{
    // x in [min, max[ => (x >= min)&&(x < max)
    return ((qAbs<double>(x-min) < std::numeric_limits<double>::min()) && (x < max));
  }
  */
}
