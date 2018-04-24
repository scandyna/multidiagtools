/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "CompareRowList.h"
#include "Mdt/TestLib/CompareHelper.h"
#include <QtTest/QtTest>
#include <QLatin1String>

namespace QTest{

bool qCompare(const Mdt::Container::RowList& actualList, const Mdt::Container::RowList& expectedList,
              const char* actual, const char* expected, const char* file, int line)
{
  const int actualSize = actualList.size();
  const int expectedSize = expectedList.size();
  if(actualSize != expectedSize){
    const QString msg = QString::fromLatin1("Compared row lists have different row counts.\n"
                                            "   Actual   (%1) rows: %2\n"
                                            "   Expected (%3) rows: %4")
                                            .arg(QLatin1String(actual)).arg(actualSize)
                                            .arg(QLatin1String(expected)).arg(expectedSize);
    return Mdt::TestLib::CompareHelper::compareNok(msg, actual, expected, file, line);
  }
  const int n = actualSize;
  for(int i = 0; i < n; ++i){
    if(actualList.at(i) != expectedList.at(i)){
      const auto msg = QString::fromLatin1("Compared row lists differs at index %1.\n"
                                           "   Actual   (%2): %3\n"
                                           "   Expected (%4): %5")
                                           .arg(i)
                                           .arg(QLatin1String(actual)).arg(actualList.at(i))
                                           .arg(QLatin1String(expected)).arg(expectedList.at(i));
      return Mdt::TestLib::CompareHelper::compareNok(msg, actual, expected, file, line);
    }
  }

  return Mdt::TestLib::CompareHelper::compareOk(actual, expected, file, line);
}

} // namespace QTest{