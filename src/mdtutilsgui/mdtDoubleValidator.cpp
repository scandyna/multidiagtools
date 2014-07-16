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
#include "mdtDoubleValidator.h"
#include "mdtDoubleEdit.h"
#include <QChar>
#include <limits>

//#include <QDebug>

mdtDoubleValidator::mdtDoubleValidator(QObject* parent)
 : QValidator(parent)
{
}

void mdtDoubleValidator::setSuffix(const QString& s)
{
  pvSuffix = s;
}

void mdtDoubleValidator::setBottom(double min)
{
  pvValidator.setBottom(min);
}

void mdtDoubleValidator::setTop(double max)
{
  pvValidator.setTop(max);
}

void mdtDoubleValidator::setRange(double min, double max, int decimals)
{
  pvValidator.setRange(min, max, decimals);
}

QValidator::State mdtDoubleValidator::validate(QString & input, int & pos) const
{
  /*
   * We have a special case for -inf and +inf
   */
  if((input == mdtDoubleEdit::infinityString())||(input == ("-" + mdtDoubleEdit::infinityString()))||(input == "-inf")||(input == "inf")||(input == "+inf")){
    return QValidator::Acceptable;
  }
  /*
   * Input that not beginns with a number, or a sign, is not valid.
   */
  if(!strBeginIsNumber(input)){
    return QValidator::Invalid;
  }
  /*
   * Check that input contains only digits or accepted chars.
   */
  if(!strHasOnlyAccpedElements(input)){
    return QValidator::Invalid;
  }
  /*
   * If input does not end correctly,
   *  let the user do more input.
   */
  if(!strEndsCorrectly(input)){
    return QValidator::Intermediate;
  }
  /*
   * Here we have a number with suffix.
   * Submit number part without suffix to Qt's validator
   */
  int posCpy = pos;
  QString inputCpy;
  QValidator::State s;

  // Copy input and remove power of 10 chars + pvSuffix + spaces
  inputCpy = input.trimmed();
  inputCpy.remove(pvSuffix);
  inputCpy.remove('a');
  inputCpy.remove('f');
  inputCpy.remove('p');
  inputCpy.remove('n');
  inputCpy.remove('u');
  inputCpy.remove('m');
  inputCpy.remove('c');
  inputCpy.remove('d');
  inputCpy.remove('h');
  inputCpy.remove('k');
  inputCpy.remove('M');
  inputCpy.remove('G');
  inputCpy.remove('T');
  inputCpy.remove('P');
  inputCpy.remove('E');
  inputCpy = inputCpy.trimmed();
  // Validate number
  s = pvValidator.validate(inputCpy, posCpy);

  return s;
}

bool mdtDoubleValidator::strBeginIsNumber(const QString & str) const
{
  QChar c;

  if(str.isEmpty()){
    return true;
  }
  c = str.at(0);
  if(c.isDigit()){
    return true;
  }
  // c is a char, check if we can accept it
  if((c == '-')||(c == '.')){
    return true;
  }

  return false;
}

bool mdtDoubleValidator::strHasOnlyAccpedElements(const QString & str) const
{
  int i;

  for(i = 0; i < str.size(); ++i){
    if(!charIsAccepted(str.at(i))){
      return false;
    }
  }

  return true;
}

bool mdtDoubleValidator::charIsAccepted(const QChar & c) const
{
  if(c.isDigit()){
    return true;
  }
  if(c.isSpace()){
    return true;
  }
  if((c == '.')||(c == ',')||(c == '\'')){
    return true;
  }
  if((c == '-')||(c == '+')){
    return true;
  }
  if((c == 'e')||(c == 'E')){
    return true;
  }
  if(charIsPowerOf10Suffix(c)){
    return true;
  }
  if(pvSuffix.contains(c)){
    return true;
  }
  return false;
}

bool mdtDoubleValidator::charIsPowerOf10Suffix(const QChar & c) const
{
  return (qAbs<double>(mdtDoubleEdit::factor(c) - 1.0) >  std::numeric_limits<double>::min());
}

bool mdtDoubleValidator::strEndsCorrectly(const QString & str) const
{
  QChar c;

  if(str.isEmpty()){
    return false;
  }
  c = str.at(str.size() - 1);
  if(c.isDigit()){
    return true;
  }
  if(c.isSpace()){
    return true;
  }
  if(strEndsWithPowerOf10Suffix(str)){
    return true;
  }
  if(str.endsWith(pvSuffix)){
    return true;
  }

  return false;
}

bool mdtDoubleValidator::strEndsWithPowerOf10Suffix(const QString & str) const
{
  if(str.isEmpty()){
    return false;
  }
  return charIsPowerOf10Suffix(str.at(str.size() - 1));
}
