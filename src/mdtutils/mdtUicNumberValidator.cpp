/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtUicNumberValidator.h"
#include <QString>
#include <QChar>

#include <QDebug>

mdtUicNumberValidator::mdtUicNumberValidator(QObject *parent)
 : QValidator(parent)
{
}

mdtUicNumberValidator::~mdtUicNumberValidator()
{
}

QValidator::State mdtUicNumberValidator::validate(QString &input, int &pos) const
{
  QChar c;
  QString number;

  // Check if last char is valid
  if(pos > 0){
    c = input.at(pos-1);
    if((!c.isDigit())&&(c != ' ')&&(c != '-')){
      return QValidator::Invalid;
    }
  }

  // Try to build the number...

  //Remove unusable chars
  if(input.size() > 5){
    number = input.simplified();
    number.remove(" ");
    number.remove("-");
    // Check length:
    //  - We only know about 6 and 11 digits UIC numbers
    //  - If we have 7 or 12 digits, the control key is given
    if((number.size() == 6)||(number.size() == 7)||(number.size() == 11)||(number.size() == 12)){
      return QValidator::Acceptable;
    }
  }

  return QValidator::Intermediate;
}

