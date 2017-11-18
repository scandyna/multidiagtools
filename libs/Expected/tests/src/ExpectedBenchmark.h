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
#ifndef MDT_EXPECTED_BENCHMARK_H
#define MDT_EXPECTED_BENCHMARK_H

#include "Mdt/Expected.h"
#include "MdtExpectedExport.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QtTest/QtTest>

class MDT_EXPECTED_EXPORT ExpectedBenchmark : public QObject
{
  Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void getIntBenchmark();
  void getExpectedIntBenchmark();
  void getAndCheckExpectedIntBenchmark();

 private:

  template<typename T>
  static T getValue(const T & value)
  {
    return value;
  }

  template<typename T>
  static Mdt::Expected<T> getExpectedValue(const T & value)
  {
    return value;
  }

};

#endif // #ifndef MDT_EXPECTED_BENCHMARK_H
