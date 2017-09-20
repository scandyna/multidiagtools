/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "CsvParserSettings.h"
#include <QCoreApplication>
#include <QString>

#define tr(sourceText) QCoreApplication::translate("MyClass", sourceText)

namespace Mdt{ namespace PlainText{

void CsvParserSettings::setParseExp(bool parse)
{
  mParseExp = parse;
}

bool CsvParserSettings::isValid() const
{
  // Check basis
  if(!CsvCommonSettings::isValid()){
    return false;
  }
  // Check that EXP is not used as a field separator or protection
  if( fieldSeparator() == '~' ){
    const auto msg = tr("Field separator can not be the Excel Protection Marker (~).");
    auto error = mdtErrorNew(msg, Mdt::Error::Warning, "CsvCommonSettings");
    setLastError(error);
    return false;
  }
  if( fieldProtection() == '~' ){
    const auto msg = tr("Field protection can not be the Excel Protection Marker (~).");
    auto error = mdtErrorNew(msg, Mdt::Error::Warning, "CsvCommonSettings");
    setLastError(error);
    return false;
  }
  // Seems OK
  return true;
}

void CsvParserSettings::clear()
{
  CsvCommonSettings::clear();
  mParseExp = true;
}

}} // namespace Mdt{ namespace PlainText{
