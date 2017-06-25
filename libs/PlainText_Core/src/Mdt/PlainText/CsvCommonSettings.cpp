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
#include "CsvCommonSettings.h"
#include <QString>
#include <QCoreApplication>

#define tr(sourceText) QCoreApplication::translate("CsvCommonSettings", sourceText);

namespace Mdt{ namespace PlainText{

void CsvCommonSettings::setFieldSeparator(char separator)
{
  mFieldSeparator = separator;
}

void CsvCommonSettings::setFieldProtection(char protection)
{
  mFieldProtection = protection;
}

bool CsvCommonSettings::isValid() const
{
  // Check that field separator and protection are not the same char
  if(mFieldSeparator == mFieldProtection){
    const auto msg = tr("Field separator can not be the same than field protection.");
    auto error = mdtErrorNew(msg, Mdt::Error::Warning, "CsvCommonSettings");
    setLastError(error);
    return false;
  }
  // Seems OK
  return true;
}

void CsvCommonSettings::clear()
{
  mFieldSeparator = ',';
  mFieldProtection = '\"';
}

void CsvCommonSettings::setLastError(const Mdt::Error & error) const
{
  mLastError = error;
  mLastError.commit();
}

}} // namespace Mdt{ namespace PlainText{
