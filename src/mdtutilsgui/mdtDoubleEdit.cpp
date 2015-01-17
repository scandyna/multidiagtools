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

//#include <QDebug>

mdtDoubleEdit::mdtDoubleEdit(QWidget* parent)
 : QWidget(parent)
{
  QHBoxLayout *l;

  pbSetNull = new QPushButton(QChar(0x2205));
  pbSetMinusInfinity = new QPushButton("-" + infinityString());
  pbSetInfinity = new QPushButton(" " + infinityString());
  // Main layout
  l = new QHBoxLayout;
  pvLineEdit = new QLineEdit;
  pvLineEdit->setAlignment(Qt::AlignRight);
  l->addWidget(pvLineEdit);
  l->addWidget(pbSetMinusInfinity);
  l->addWidget(pbSetNull);
  l->addWidget(pbSetInfinity);
  // Some setup on layout
  l->setStretchFactor(pvLineEdit, 5);
  l->setStretchFactor(pbSetMinusInfinity, 0);
  l->setStretchFactor(pbSetNull, 0);
  l->setStretchFactor(pbSetInfinity, 0);
  l->setSpacing(2);
  l->setContentsMargins(0, 0, 0, 0);
  setLayout(l);
  connect(pbSetNull, SIGNAL(clicked()), this, SLOT(setNull()));
  connect(pbSetMinusInfinity, SIGNAL(clicked()), this, SLOT(setMinusInfinity()));
  connect(pbSetInfinity, SIGNAL(clicked()), this, SLOT(setInfinity()));
  // Validator
  pvValidator = new mdtDoubleValidator(this);
  pvLineEdit->setValidator(pvValidator);
  connect(pvLineEdit, SIGNAL(editingFinished()), this, SLOT(setValueFromLineEdit()));
  connect(pvLineEdit, SIGNAL(textEdited(const QString&)), this, SIGNAL(valueEdited()));
  // Set initial value
  pvValue = 0.0;
  pvIsNull = true;
  pvValueIsValid = false;
  pvUnitExponent = 1;
  pvEditionMode = DefaultEditionMode;
  buildDefaultUnitRanges();
  // Setup focus (help also Qt Designer to know we accept StrongFocus, else we never appear in tab order editor)
  setFocusPolicy(Qt::StrongFocus);
  setFocusProxy(pvLineEdit);
}

void mdtDoubleEdit::setReadOnly(bool ro)
{
  pbSetMinusInfinity->setEnabled(!ro);
  pbSetInfinity->setEnabled(!ro);
  pvLineEdit->setReadOnly(ro);
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

int mdtDoubleEdit::p10exponent(const QChar & c)
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
  if( (pvValidator->validate(str, pos) == QValidator::Acceptable) || (str.isEmpty()) ){
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
  pvIsNull = false;
  pvValueIsValid = true;
  displayValue();
  if(emitValueChanged){
    emit valueChanged(pvValue, pvValueIsValid);
  }
}

void mdtDoubleEdit::setValue(const QVariant & v, bool emitValueChanged)
{
  if((v.type() == QVariant::Double)||(v.type() == QVariant::Int)||(v.type() == QVariant::LongLong)){
    setValueDouble(v.toDouble(), emitValueChanged);
    return;
  }
  if(v.type() == QVariant::String){
    setValueString(v.toString(), emitValueChanged);
    return;
  }
  if(v.isNull()){
    pvIsNull = true;
    pvValueIsValid = true;
  }else{
    pvIsNull = true;
    pvValueIsValid = false;
  }
  pvValue = 0.0;
  displayValue();
  if(emitValueChanged){
    emit valueChanged(pvValue, pvValueIsValid);
  }
}

QVariant mdtDoubleEdit::value() const
{
  QVariant v;

  if(pvValueIsValid && (!pvIsNull)){
    v = pvValue;
  }

  return v;
}

bool mdtDoubleEdit::validate()
{
  convertAndSetValue(pvLineEdit->text());
  displayValue();
  return pvValueIsValid;
}

QString mdtDoubleEdit::text() const
{
  return pvLineEdit->text();
}

QString mdtDoubleEdit::infinityString()
{
  return QChar(0x221E);
}

/*
 * setUnit() and setEditionMode() are called by Qt Designer.
 * The call order of this functions depends on order in witch user set these properties.
 * To prevent incoherent behaviours, rules are:
 *  - When a edition mode else than default, unit setup must be done without calling setUnit()
 *  - setUnit() has effect only in default edition mode
 */
void mdtDoubleEdit::setUnit(const QString & u)
{
  if(pvEditionMode != DefaultEditionMode){
    return;
  }
  pvUnit = formatedUnit(u);
  pvValidator->setSuffix(pvUnit);
  // Extract unit exponent if one is passed
  bool ok;
  QString s = pvUnit.right(1);
  pvUnitExponent = s.toInt(&ok);
  if(ok){
    // unit ends with a exponent, see witch
    if(pvUnitExponent == 2){
      buildDefaultSquareUnitRanges();
    }
  }else{
    // No exponent in unit
    pvUnitExponent = 1;
    buildDefaultUnitRanges();
  }
  clear();
}

void mdtDoubleEdit::setEditionMode(mdtDoubleEdit::EditionMode_t mode)
{
  pvEditionMode = mode;
  switch(pvEditionMode){
    case DefaultEditionMode:
      return;
    case WireEditionMode:
      setWireSectionEditionMode();
      return;
  }
}

void mdtDoubleEdit::setNull()
{
  setValue(QVariant(), true);
  emit valueEdited();
}

void mdtDoubleEdit::setMinusInfinity()
{
  setValueDouble(-std::numeric_limits<double>::infinity(), true);
  emit valueEdited();
}

void mdtDoubleEdit::setInfinity()
{
  setValueDouble(std::numeric_limits<double>::infinity(), true);
  emit valueEdited();
}

void mdtDoubleEdit::setValueFromLineEdit()
{
  convertAndSetValue(pvLineEdit->text());
  displayValue();
  emit valueChanged(pvValue, pvValueIsValid);
}

void mdtDoubleEdit::setWireSectionEditionMode()
{
  pvUnit = "m2";
  pvUnitExponent = 2;
  pvValidator->setSuffix(pvUnit);
  buildWireSectionRanges();
}

void mdtDoubleEdit::clear()
{
  pvValueIsValid = false;
  pvIsNull = true;
  pvValue = 0.0;
  pvLineEdit->clear();
}

QString mdtDoubleEdit::specialSymbolChar(mdtDoubleEdit::UnitSymbol_t symbol) const
{
  switch(symbol){
    case UnitSymbol_t::omega:
      return QChar(0x03C9);
    case UnitSymbol_t::OmegaCapital:
      return QChar(0x03A9);
  }
  return "";
}

QString mdtDoubleEdit::formatedUnit(const QString & u) const
{
  if(u == "w"){
    return specialSymbolChar(UnitSymbol_t::omega);
  }
  if(u == "W"){
    return specialSymbolChar(UnitSymbol_t::OmegaCapital);
  }
  if(u == "Ohm"){
    return specialSymbolChar(UnitSymbol_t::OmegaCapital);
  }
  if(u == "Ohm/m"){
    return specialSymbolChar(UnitSymbol_t::OmegaCapital) + "/m";
  }
  return u;
}

void mdtDoubleEdit::convertAndSetValue(QString str)
{
  int p10e = 1;
  QChar fc;
  int unitIndex;

  str = str.trimmed();
  /*
   * Remove unit part.
   * It happens that a power of 10 factor is the same as unit.
   * Typical case is for [m] unit (meter), that is expressed in mm (milimeter).
   * Here we must only remove 'm' once, so we keep the m (mili) factor.
   */
  unitIndex = str.indexOf(pvUnit, Qt::CaseSensitive);
  if(unitIndex >= 0){
    str.remove(unitIndex, pvUnit.size());
  }
  // Will also remove space that was between value and unit
  str = str.trimmed();
  if(str.isEmpty()){
    pvIsNull = true;
    pvValueIsValid = true;
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
    if(!fc.isNull()){
      str.remove(fc);
    }
    str = str.trimmed();
  }
  // Do conversion
  pvValue = str.toDouble(&pvValueIsValid);
  if(pvValueIsValid){
    pvValue = pvValue * std::pow(10, (double)(p10e * pvUnitExponent) );
    pvIsNull = false;
  }
}

void mdtDoubleEdit::displayValue()
{
  mdtDoubleEditUnitRange r;
  double x;
  QChar fc;
  QString str;

  if((!pvValueIsValid) || pvIsNull){
    pvLineEdit->clear();
    return;
  }
  // Check if we are - or + infinity
  if( (pvValue < 0.0) && (std::isinf(pvValue)) ){
    pvLineEdit->setText("-" + infinityString());
    return;
  }
  if( (pvValue > 0.0) && (std::isinf(pvValue)) ){
    pvLineEdit->setText(infinityString());
    return;
  }
  // Select power of 10 range
  x = qAbs<double>(pvValue);
  if(x > std::numeric_limits<double>::min()){
    r = getRange(x);
  }
  /*
   * Build string to display
   * Note: because range checking varies with unit exponent,
   *  the the power of 10 exponent is also allready calculeted with unit exponent in r
   */
  str.setNum( pvValue / pow(10, r.po10exponent) );
  if((!r.po10prefix.isNull())||(!pvUnit.isEmpty())){
    str += " ";
  }
  if(!r.po10prefix.isNull()){
    str += r.po10prefix;
  }
  str += pvUnit;
  // Display str
  pvLineEdit->setText(str);
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

mdtDoubleEditUnitRange mdtDoubleEdit::getRange(double x)
{
  return mdtAlgorithms::rangeOf(x, pvUnitRanges);
}

void mdtDoubleEdit::buildDefaultUnitRanges()
{
  pvUnitRanges.clear();
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(0.0, 1e-15, 'a', -18));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e-15, 1e-12, 'f', -15));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e-12, 1e-9, 'p', -12));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e-9, 1e-6, 'n', -9));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e-6, 1e-3, 'u', -6));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e-3, 1e-1, 'm', -3));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e-1, 1.0, 'd', -1));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1.0, 1e2, '\0', 0));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e2, 1e3, 'h', 2));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e3, 1e6, 'k', 3));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e6, 1e9, 'M', 6));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e9, 1e12, 'G', 9));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e12, 1e15, 'T', 12));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e15, 1e18, 'P', 15));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e18, std::numeric_limits<double>::infinity(), 'E', 18));
}

void mdtDoubleEdit::buildDefaultSquareUnitRanges()
{
  pvUnitRanges.clear();
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(0.0, 1e-30, 'a', -36));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e-30, 1e-24, 'f', -30));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e-24, 1e-18, 'p', -24));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e-18, 1e-12, 'n', -18));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e-12, 1e-6, 'u', -12));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e-6, 1e-2, 'm', -6));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e-2, 1.0, 'd', -2));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1.0, 1e4, '\0', 0));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e4, 1e6, 'h', 4));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e6, 1e12, 'k', 6));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e12, 1e18, 'M', 12));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e18, 1e24, 'G', 18));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e24, 1e30, 'T', 24));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e30, 1e36, 'P', 30));
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(1e36, std::numeric_limits<double>::infinity(), 'E', 36));
}

void mdtDoubleEdit::buildWireSectionRanges()
{
  pvUnitRanges.clear();
  pvUnitRanges.push_back(mdtDoubleEditUnitRange(0.0, std::numeric_limits<double>::infinity(), 'm', -6));
}
