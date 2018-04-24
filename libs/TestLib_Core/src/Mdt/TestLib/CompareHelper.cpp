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
#include "CompareHelper.h"
#include <QtTest/QtTest>

namespace Mdt{ namespace TestLib{

bool CompareHelper::compareOk(char* val1, char* val2, const char* actual, const char* expected, const char* file, int line)
{
  return QTest::compare_helper(true, nullptr, val1, val2, actual, expected, file, line);
}

bool CompareHelper::compareNok(const QString& failureMsg, char* val1, char* val2, const char* actual, const char* expected, const char* file, int line)
{
  return QTest::compare_helper(false, failureMsg.toLocal8Bit().constData(), val1, val2, actual, expected, file, line);
}

}} // namespace Mdt{ namespace TestLib{
