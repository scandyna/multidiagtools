/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_EXPECTED_TEST_H
#define MDT_EXPECTED_TEST_H

#include "Mdt/Expected.h"
#include "Mdt/Error.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QtTest/QtTest>

class ExpectedTest : public QObject
{
  Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void expectedTest();

  void implicitIntUseTest();
  void implicitQStringUseTest();
  void implicitQVariantUseTest();

  void dereferenceQStringTest();

  void expectedResultTest();

 private:

  int incrementInt(int value);
  QString catAtoString(const QString & str);
  QVariant returnVariantAsIs(const QVariant & var);

  template<typename T>
  static Mdt::Expected<T> getValue(const T & value)
  {
    return value;
  }

  template<typename T>
  static Mdt::Expected<T> getError()
  {
    return mdtErrorNew("A error", Mdt::Error::Critical, "ExpectedTest");
  }

};

#endif // #ifndef MDT_EXPECTED_TEST_H
