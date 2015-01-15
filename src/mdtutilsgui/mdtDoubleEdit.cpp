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
#include "mdtDoubleEdit.h"
#include "mdtDoubleValidator.h"
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QChar>
#include <QRegExp>
#include <QMetaType>
#include <limits>
#include <cmath>

#include <QDebug>

mdtDoubleEdit::mdtDoubleEdit(QWidget* parent)
 : QWidget(parent)
{
  QHBoxLayout *l;

  pbSetNull = new QPushButton(QChar(0x2205));
  pbSetMinusInfinity = new QPushButton("-" + infinityString());
  pbSetInfinity = new QPushButton(" " + infinityString());
  // Main layout
  l = new QHBoxLayout;
  l->setSpacing(0);
  pvLineEdit = new QLineEdit;
  pvLineEdit->setAlignment(Qt::AlignRight);
  l->addWidget(pvLineEdit);
  l->addWidget(pbSetMinusInfinity);
  l->addWidget(pbSetNull);
  l->addWidget(pbSetInfinity);
  setLayout(l);
  connect(pbSetNull, SIGNAL(clicked()), this, SLOT(setNull()));
  connect(pbSetMinusInfinity, SIGNAL(clicked()), this, SLOT(setMinusInfinity()));
  connect(pbSetInfinity, SIGNAL(clicked()), this, SLOT(setInfinity()));
  // Validator
  pvValidator = new mdtDoubleValidator(this);
  pvLineEdit->setValidator(pvValidator);
  connect(pvLineEdit, SIGNAL(editingFinished()), this, SLOT(setValueFromLineEdit()));
  // Set initial value
  pvValue = 0.0;
  pvValueIsValid = false;
  pvUnitExponent = 1;
}

void mdtDoubleEdit::setReadOnly(bool ro)
{
  pbSetMinusInfinity->setEnabled(!ro);
  pbSetInfinity->setEnabled(!ro);
  pvLineEdit->setReadOnly(ro);
}

void mdtDoubleEdit::setUnit(mdtDoubleEdit::unitSymbol_t u)
{
  switch(u){
    case omega:
      pvUnit = QChar(0x03C9);
      break;
    case OmegaCapital:
      pvUnit = QChar(0x03A9);
      break;
  }
  pvValidator->setSuffix(pvUnit);
}

void mdtDoubleEdit::setMinimum(double min)
{
  if(min > maximum()){
    min = maximum();
  }
  if(min > -std::numeric_limits<double>::infinity()){
    pbSetMinusInfinity->setVisible(false);
  }else{
    pbSetMinusInfinity->setVisible(true);
  }
  if(pvValue < min){
    pvValue = min;
  }
  pvValidator->setBottom(min);
}

void mdtDoubleEdit::setMinimumToMinusInfinity()
{
  setMinimum(-std::numeric_limits<double>::infinity());
}

double mdtDoubleEdit::minimum() const
{
  return pvValidator->bottom();
}

void mdtDoubleEdit::setMaximum(double max)
{
  if(max < minimum()){
    max = minimum();
  }
  if(max < std::numeric_limits<double>::infinity()){
    pbSetInfinity->setVisible(false);
  }else{
    pbSetInfinity->setVisible(true);
  }
  if(pvValue > max){
    pvValue = max;
  }
  pvValidator->setTop(max);
}

void mdtDoubleEdit::setMaximumToInfinity()
{
  setMaximum(std::numeric_limits<double>::infinity());
}

double mdtDoubleEdit::maximum() const
{
  return pvValidator->top();
}

void mdtDoubleEdit::setRange(double min, double max)
{
  setMinimum(min);
  setMaximum(max);
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

int mdtDoubleEdit::p10exponent(const QChar& c)
{
  switch(c.toAscii()){
    case 'a':
      return -18;
    case 'f':
      return -15;
    case 'p':
      return -12;
    case 'n':
      return -9;
    case 'u':
      return -6;
    case 'm':
      return -3;
    case 'c':
      return -2;
    case 'd':
      return -1;
    case 'h':
      return 2;
    case 'k':
      return 3;
    case 'M':
      return 6;
    case 'G':
      return 9;
    case 'T':
      return 12;
    case 'P':
      return 15;
    case 'E':
      return 18;
    default:
      return 0;
  }
}

void mdtDoubleEdit::setValueString(const QString & s, bool emitValueChanged)
{
  int pos = 0;
  QString str = s.trimmed();

  pvValue = 0.0;
  pvValueIsValid = false;
  if(pvValidator->validate(str, pos) == QValidator::Acceptable){
    convertAndSetValue(str);
  }
  displayValue();
  if(emitValueChanged){
    emit valueChanged(pvValue, pvValueIsValid);
  }
}

void mdtDoubleEdit::setValueDouble(double v, bool emitValueChanged)
{
  if(v < minimum()){
    pvValue = minimum();
  }else if(v > maximum()){
    pvValue = maximum();
  }else{
    pvValue = v;
  }
  pvValueIsValid = true;
  displayValue();
  if(emitValueChanged){
    emit valueChanged(pvValue, pvValueIsValid);
  }
}

void mdtDoubleEdit::setValue(const QVariant& v, bool emitValueChanged)
{
  qDebug() << "setValue() - v: " << v << " , type: " << v.type();
  if((v.type() == QVariant::Double)||(v.type() == QVariant::Int)||(v.type() == QVariant::LongLong)){
    setValueDouble(v.toDouble(), emitValueChanged);
    return;
  }
  if(v.type() == QVariant::String){
    setValueString(v.toString(), emitValueChanged);
    return;
  }
  pvValue = 0.0;
  pvValueIsValid = false;
  displayValue();
  if(emitValueChanged){
    emit valueChanged(pvValue, pvValueIsValid);
  }
}

QVariant mdtDoubleEdit::value() const
{
  QVariant v;

  if(pvValueIsValid){
    v = pvValue;
  }

  return v;
}

QString mdtDoubleEdit::text() const
{
  return pvLineEdit->text();
}

QString mdtDoubleEdit::infinityString()
{
  return QChar(0x221E);
}

void mdtDoubleEdit::setUnit(const QString & u)
{
  pvUnitExponent = 1;
  if(u == "w"){
    setUnit(omega);
    return;
  }
  if(u == "Ohm"){
    setUnit(OmegaCapital);
    return;
  }
  pvUnit = u;
  pvValidator->setSuffix(pvUnit);
  // Extract unit exponent if one is passed
  bool ok;
  QString s = pvUnit.right(1);
  pvUnitExponent = s.toInt(&ok);
  if(!ok){
    pvUnitExponent = 1;
  }
}

void mdtDoubleEdit::setNull()
{
  setValue(QVariant(), true);
}

void mdtDoubleEdit::setMinusInfinity()
{
  setValueDouble(-std::numeric_limits<double>::infinity(), true);
}

void mdtDoubleEdit::setInfinity()
{
  setValueDouble(std::numeric_limits<double>::infinity(), true);
}

void mdtDoubleEdit::setValueFromLineEdit()
{
  convertAndSetValue(pvLineEdit->text());
  displayValue();
  emit valueChanged(pvValue, pvValueIsValid);
}

void mdtDoubleEdit::convertAndSetValue(QString str)
{
  ///double f = 1.0;
  int p10e;
  QChar fc;
  int unitIdex;

  str = str.trimmed();
  
  qDebug() << "convertAndSetValue() - str: " << str;
  
  /*
   * Remove unit part.
   * It happens that a power of 10 factor is the same as unit.
   * Typical case is for [m] unit (meter), that is expressed in mm (milimeter).
   * Here we must only remove 'm' once, so we keep the m (mili) factor.
   */
  unitIdex = str.indexOf(pvUnit, Qt::CaseSensitive);
  if(unitIdex >= 0){
    str.remove(unitIdex, pvUnit.size());
  }
  ///str.remove(pvUnit);
  ///str.remove(QRegExp("^" + pvUnit + "$"));
  qDebug() << "convertAndSetValue() - str (2): " << str;
  
  str = str.trimmed();
  if(str.isEmpty()){
    pvValueIsValid = false;
    displayValue();
    return;
  }
  // If str is not a infinity, get power of 10 exponent and remove it from str
  if(!strIsInfinity(str)){
    fc = str.at(str.size() - 1);
    p10e = p10exponent(fc);
    if(p10e == 0){
      fc = '\0';
    }
    /**
    f = factor(fc);
    if(qAbs<double>(f - 1.0) <=  std::numeric_limits<double>::min()){
      fc = '\0';
    }
    */
    if(!fc.isNull()){
      str.remove(fc);
    }
    str = str.trimmed();
  }
  // Do conversion
  qDebug() << "convertAndSetValue() - str(3): " << str << " , p10e: " << p10e << " , ue: " << pvUnitExponent;
  pvValue = str.toDouble(&pvValueIsValid);
  qDebug() << "convertAndSetValue() - v (1): " << pvValue << " - valid: " << pvValueIsValid;
  if(pvValueIsValid){
    pvValue = pvValue * std::pow(10, (double)(p10e * pvUnitExponent) );
  }
  qDebug() << "convertAndSetValue() - v (2): " << pvValue;
}

void mdtDoubleEdit::displayValue()
{
  ///double f;
  int p10e;
  double x;
  QChar fc;
  QString str;

  if(!pvValueIsValid){
    pvLineEdit->clear();
    return;
  }
  
  qDebug() << "displayValue() - v: " << pvValue;
  
  // Check if we are - or + infinity
  if( (pvValue < 0.0) && (std::isinf(pvValue)) ){
  ///if(pvValue == -std::numeric_limits<double>::infinity()){
    pvLineEdit->setText("-" + infinityString());
    return;
  }
  if( (pvValue > 0.0) && (std::isinf(pvValue)) ){
  ///if(pvValue == std::numeric_limits<double>::infinity()){
    pvLineEdit->setText(infinityString());
    return;
  }
  // Select a power of 10 regarding value - Note: we ignore c and d and h
  x = qAbs<double>(pvValue);
  if(x <= std::numeric_limits<double>::min()){
    p10e = 0;
    ///f = 1.0;
  ///}else if(x < 1.e-15){
  }else if(x < std::pow(10, -15.0*pvUnitExponent)){
    p10e = -18;
    /// f = 1.e18;
    fc = 'a';
  ///}else if(isInRange(x, 1e-15, 1e-12)){
  }else if(isInRange(x, std::pow(10, -15.0*pvUnitExponent), std::pow(10, -12.0*pvUnitExponent))){
    p10e = -15;
    ///f = 1e15;
    fc = 'f';
  ///}else if(isInRange(x, 1e-12, 1e-9)){
  }else if(isInRange(x, std::pow(10, -12.0*pvUnitExponent), std::pow(10, -9.0*pvUnitExponent))){
    p10e = -12;
    ///f = 1e12;
    fc = 'p';
  ///}else if(isInRange(x, 1e-9, 1e-6)){
  }else if(isInRange(x, std::pow(10, -9.0*pvUnitExponent), std::pow(10, -6.0*pvUnitExponent))){
    p10e = -9;
    ///f = 1e9;
    fc = 'n';
  ///}else if(isInRange(x, 1e-6, 1e-3)){
  }else if(isInRange(x, std::pow(10, -6.0*pvUnitExponent), std::pow(10, -3.0*pvUnitExponent))){
    p10e = -6;
    ///f = 1e6;
    fc = 'u';
  ///}else if(isInRange(x, 1e-3, 1.0)){
  }else if(isInRange(x, std::pow(10, -3.0*pvUnitExponent), 1.0)){
    p10e = -3;
    ///f = 1e3;
    fc = 'm';
  ///}else if(isInRange(x, 1.0, 1e3)){
  }else if(isInRange(x, 1.0, std::pow(10, 3.0*pvUnitExponent))){
    p10e = 0;
    ///f = 1.0;
  ///}else if(isInRange(x, 1e3, 1e6)){
  }else if(isInRange(x, std::pow(10, 3.0*pvUnitExponent), std::pow(10, 6.0*pvUnitExponent))){
    p10e = 3;
    ///f = 1e-3;
    fc = 'k';
  ///}else if(isInRange(x, 1e6, 1e9)){
  }else if(isInRange(x, std::pow(10, 6.0*pvUnitExponent), std::pow(10, 9.0*pvUnitExponent))){
    p10e = 6;
    ///f = 1e-6;
    fc = 'M';
  ///}else if(isInRange(x, 1e9, 1e12)){
  }else if(isInRange(x, std::pow(10, 9.0*pvUnitExponent), std::pow(10, 12.0*pvUnitExponent))){
    p10e = 9;
    ///f = 1e-9;
    fc = 'G';
  ///}else if(isInRange(x, 1e12, 1e15)){
  }else if(isInRange(x, std::pow(10, 12.0*pvUnitExponent), std::pow(10, 15.0*pvUnitExponent))){
    p10e = 12;
    ///f = 1e-12;
    fc = 'T';
  ///}else if(isInRange(x, 1e15, 1e18)){
  }else if(isInRange(x, std::pow(10, 15.0*pvUnitExponent), std::pow(10, 18.0*pvUnitExponent))){
    p10e = 15;
    ///f = 1e-15;
    fc = 'P';
  }else if(x >= std::pow(10, 18.0*pvUnitExponent)){
    p10e = 18;
    ///f = 1e-18;
    fc = 'E';
  }else{
    p10e = 0;
  }
  qDebug() << "displayValue() - p10e: " << p10e << " , pvUnitExponent: " << pvUnitExponent << " , p10e used: " << p10e*pvUnitExponent;
  ///f = f * std::pow(10, pvUnitExponent);
  // Build string to display
  str.setNum( pvValue / pow(10, (double)(p10e * pvUnitExponent) ) );
  if((!fc.isNull())||(!pvUnit.isEmpty())){
    str += " ";
  }
  if(!fc.isNull()){
    str += fc;
  }
  str += pvUnit;
  // Display str
  pvLineEdit->setText(str);
}

bool mdtDoubleEdit::isInRange(double x, double min, double max)
{
  return ( ((x > min)||(qAbs<double>(x-min) < std::numeric_limits<double>::min()) ) && (x < max) );
}

bool mdtDoubleEdit::strIsInfinity(const QString & str) const
{
  if((str == "-inf")||(str == "inf")){
    return true;
  }
  if((str == "-\u221E")||(str == "\u221E")){
    return true;
  }
  return false;
}
