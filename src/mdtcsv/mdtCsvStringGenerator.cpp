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
#include "mdtCsvStringGenerator.h"
#include <QChar>

QString mdtCsvStringGenerator::getCsvString(const mdtCsvRecord & record) const
{
  QString csv;
  const int lastIndex = record.columnDataList.size() - 1;
  const QString eol = pvCsvSettings.eol.c_str();
  const QChar sep = pvCsvSettings.fieldSeparator;

  if(lastIndex < 0){
    return csv;
  }
  for(int i = 0; i < lastIndex; ++i){
    csv.append(formatFieldData(record.columnDataList.at(i)));
    csv.append(sep);
  }
  csv.append(formatFieldData(record.columnDataList.at(lastIndex)));
  csv.append(eol);

  return csv;
}

QString mdtCsvStringGenerator::getCsvString(const mdtCsvData & data) const
{
  QString csv;

  for(const auto & record : data.recordList){
    csv.append(getCsvString(record));
  }

  return csv;
}

QString mdtCsvStringGenerator::formatFieldData(const QVariant & v) const
{
  QString csv;

  if(isTextField(v)){
    csv = protectedFieldData(v.toString());
  }else{
    csv = v.toString();
  }

  return csv;
}

QString mdtCsvStringGenerator::protectedFieldData(const QString & s) const
{
  QString csv;
  const QChar quote = pvCsvSettings.fieldProtection;
  QString doubleQuote;
  const QChar sep = pvCsvSettings.fieldSeparator;
  const QString eol = pvCsvSettings.eol.c_str();
  const bool allwaysQuote = pvCsvSettings.allwaysProtectTextFields;
  bool mustQuote;

  csv = s;
  // If field payload contains quotes, we must double them
  doubleQuote.append(quote);
  doubleQuote.append(quote);
  csv.replace(quote, doubleQuote);
  // Check if we have to protect field data
  mustQuote = (allwaysQuote ||csv.contains(quote) || csv.contains(sep) || csv.contains(eol));
  if(mustQuote){
    csv.prepend(quote);
    csv.append(quote);
  }

  return csv;
}
