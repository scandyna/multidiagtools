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
#include "AssertTest.h"
#include "Mdt/Assert.h"
#include <QCoreApplication>

class LiteralTestType
{
 public:

  constexpr LiteralTestType(int value) noexcept
   : mValue(value)
   {
     MDT_ASSERT(mValue > 0);
   }

  constexpr int value() const noexcept
  {
    return mValue;
  }

 private:

  int mValue;
};

/*
 * Tests
 */

void AssertTest::literalTypeTest()
{
  LiteralTestType lt1(1);
  QCOMPARE(lt1.value(), 1);

  constexpr LiteralTestType lt2(2);
  static_assert(lt2.value() == 2, "");

  // This should assert on debug build
  //LiteralTestType ltBad(0);
}


/*
 * Main
 */
int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  AssertTest errorTest;

  return QTest::qExec(&errorTest, argc, argv);
}
