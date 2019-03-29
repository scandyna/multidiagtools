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
#include "ExpectedTest.h"
#include "Mdt/CoreApplication.h"
#include "Mdt/Expected.h"

// #include <QDebug>

void ExpectedTest::initTestCase()
{
}

void ExpectedTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ExpectedTest::expectedTest()
{
  using Mdt::Expected;

  /*
   * Check constructors
   */
  // Default constructor
  Expected<int> da;
  QVERIFY(!da.hasValue());
  QVERIFY(!da);
  QVERIFY(da.hasError());
  QVERIFY(da.error().text().isEmpty());
  // Value constructor
  int aVal = 1;
  Expected<int> a(aVal);
  QVERIFY(a.hasValue());
  QVERIFY(a);
  QVERIFY(!a.hasError());
  QCOMPARE(a.value(), 1);
  QCOMPARE(aVal, 1);  // Do something with aVal to assure we using value constructor (not move)
  // Copy constructor (case with value)
  Expected<int> b(a);
  QVERIFY(b.hasValue());
  QVERIFY(!b.hasError());
  QCOMPARE(b.value(), 1);
  // Value move constructor
  Expected<int> c(std::move(2));
  QVERIFY(c.hasValue());
  QVERIFY(c);
  QVERIFY(!c.hasError());
  QCOMPARE(c.value(), 2);
  // Move constructor (case with value)
  Expected<int> d(std::move(Expected<int>(3)));
  QVERIFY(d.hasValue());
  QVERIFY(d);
  QVERIFY(!d.hasError());
  QCOMPARE(d.value(), 3);
  // Error constructor
  auto error1 = mdtErrorNewQ("error1", Mdt::Error::Critical, this);
  Expected<int> e(error1);
  QVERIFY(!e.hasValue());
  QVERIFY(!e);
  QVERIFY(e.hasError());
  QCOMPARE(e.error().text(), error1.text());
  // Copy constructor (case with error)
  Expected<int> f(e);
  QVERIFY(!f.hasValue());
  QVERIFY(!f);
  QVERIFY(f.hasError());
  QCOMPARE(f.error().text(), error1.text());
  // Error move constructor
  Expected<int> g(std::move(mdtErrorNewQ("error2", Mdt::Error::Warning, this)));
  QVERIFY(!g.hasValue());
  QVERIFY(!g);
  QVERIFY(g.hasError());
  QCOMPARE(g.error().text(), QString("error2"));
  // Move constructor (case with error)
  Expected<int> h(std::move(Expected<int>(mdtErrorNewQ("error3", Mdt::Error::Info, this))));
  QVERIFY(!h.hasValue());
  QVERIFY(!h);
  QVERIFY(h.hasError());
  QCOMPARE(h.error().text(), QString("error3"));
  /*
   * Value assignment by copy
   */
  Expected<int> i;
  // Assign a value to expected that has a value
  aVal = 5;
  QCOMPARE(aVal, 5);  // Use aVal to avoid move
  i = aVal;
  QVERIFY(i.hasValue());
  QVERIFY(!i.hasError());
  QCOMPARE(i.value(), 5);
  // Assign a error to expected that has a value
  i = error1;
  QVERIFY(!i.hasValue());
  QVERIFY(!i);
  QVERIFY(i.hasError());
  QCOMPARE(i.error().text(), QString("error1"));
  // Assign a value to expected that has a error
  aVal = 6;
  QCOMPARE(aVal, 6);  // Use aVal to avoid move
  i = aVal;
  QVERIFY(i.hasValue());
  QVERIFY(!i.hasError());
  QCOMPARE(i.value(), 6);
  // Assign a error to expected that has a error
  i = error1;
  QVERIFY(i.hasError());
  auto error2 = mdtErrorNewQ("error2", Mdt::Error::Critical, this);
  QCOMPARE(error2.text(), QString("error2")); // Use error2 to avoid move
  i = error2;
  QVERIFY(!i.hasValue());
  QVERIFY(!i);
  QVERIFY(i.hasError());
  QCOMPARE(i.error().text(), QString("error2"));
  /*
   * Value assignment (move)
   */
  i = 7;
  QVERIFY(i.hasValue());
  // Assign a value to expected that has a value
  i = std::move(8);
  QVERIFY(i.hasValue());
  QVERIFY(!i.hasError());
  QCOMPARE(i.value(), 8);
  // Assign a error to expected that has a value
  i = std::move(mdtErrorNewQ("error4", Mdt::Error::Info, this));
  QVERIFY(!i.hasValue());
  QVERIFY(!i);
  QVERIFY(i.hasError());
  QCOMPARE(i.error().text(), QString("error4"));
  // Assign a value to expected that has a error
  i = std::move(9);
  QVERIFY(i.hasValue());
  QVERIFY(!i.hasError());
  QCOMPARE(i.value(), 9);
  // Assign a error to expected that has a error
  i = error1;
  QVERIFY(i.hasError());
  i = std::move(mdtErrorNewQ("error5", Mdt::Error::Info, this));
  QVERIFY(!i.hasValue());
  QVERIFY(!i);
  QVERIFY(i.hasError());
  QCOMPARE(i.error().text(), QString("error5"));
  /*
   * Expected assignment by copy
   */
  i = 5;
  QVERIFY(i.hasValue());
  // Assign expected with a value to expected that has a value
  QVERIFY(a.hasValue());
  QCOMPARE(a.value(), 1);
  i = a;
  QVERIFY(i.hasValue());
  QVERIFY(!i.hasError());
  QCOMPARE(i.value(), 1);
  // Assign expected with a error to expected that has a value
  QVERIFY(e.hasError());
  QCOMPARE(e.error().text(), error1.text());
  i = e;
  QVERIFY(!i.hasValue());
  QVERIFY(!i);
  QVERIFY(i.hasError());
  QCOMPARE(i.error().text(), error1.text());
  // Assign expected with a value to expected that has a error
  QVERIFY(a.hasValue());
  QCOMPARE(a.value(), 1);
  i = a;
  QVERIFY(i.hasValue());
  QVERIFY(!i.hasError());
  QCOMPARE(i.value(), 1);
  // Assign expected with a error to expected that has a error
  i = std::move(mdtErrorNewQ("error6", Mdt::Error::Info, this));
  QVERIFY(i.hasError());
  QVERIFY(g.hasError());
  QCOMPARE(g.error().text(), QString("error2"));
  i = g;
  QVERIFY(!i.hasValue());
  QVERIFY(!i);
  QVERIFY(i.hasError());
  QCOMPARE(i.error().text(), QString("error2"));
  // Assign from same object
  i = i;
  QVERIFY(!i.hasValue());
  QVERIFY(i.hasError());
  QCOMPARE(i.error().text(), QString("error2"));
  /*
   * Expected assignment (move)
   */
  i = 1;
  QVERIFY(i.hasValue());
  // Assign expected with a value to expected that has a value
  i = std::move(10);
  QVERIFY(i.hasValue());
  QVERIFY(!i.hasError());
  QCOMPARE(i.value(), 10);
  // Assign expected with a error to expected that has a value
  i = Expected<int>(std::move(mdtErrorNewQ("error10", Mdt::Error::Info, this)));
  QVERIFY(!i.hasValue());
  QVERIFY(!i);
  QVERIFY(i.hasError());
  QCOMPARE(i.error().text(), QString("error10"));
  // Assign expected with a value to expected that has a error
  i = std::move(11);
  QVERIFY(i.hasValue());
  QVERIFY(!i.hasError());
  QCOMPARE(i.value(), 11);
  // Assign expected with a error to expected that has a error
  i = mdtErrorNewQ("error", Mdt::Error::Info, this);
  QVERIFY(i.hasError());
  i = Expected<int>(std::move(mdtErrorNewQ("error11", Mdt::Error::Info, this)));
  QVERIFY(!i.hasValue());
  QVERIFY(!i);
  QVERIFY(i.hasError());
  QCOMPARE(i.error().text(), QString("error11"));
  /*
   * Check that we can store to containers
   */
  // Check with std::vector
  std::vector<Expected<int>> v;
  v.push_back(a);
  v.push_back(b);
  v.push_back(c);
  v.push_back(d);
  v.push_back(e);
  v.push_back(f);
  v.push_back(g);
  v.push_back(h);
  QCOMPARE(v.at(0).value(), 1);
  QCOMPARE(v.at(1).value(), 1);
  QCOMPARE(v.at(2).value(), 2);
  QCOMPARE(v.at(3).value(), 3);
  QCOMPARE(v.at(4).error().text(), QString("error1"));
  QCOMPARE(v.at(5).error().text(), QString("error1"));
  QCOMPARE(v.at(6).error().text(), QString("error2"));
  QCOMPARE(v.at(7).error().text(), QString("error3"));
  v.clear();
  // Check with emplace back
  v.emplace_back(10);
  v.emplace_back(11);
  v.emplace_back(mdtErrorNewQ("error12", Mdt::Error::Critical, this));
  QCOMPARE(v.at(0).value(), 10);
  QCOMPARE(v.at(1).value(), 11);
  QCOMPARE(v.at(2).error().text(), QString("error12"));
  v.clear();
  // Check with QVector
  QVector<Expected<int>> qv;
  qv.append(a);
  qv.append(b);
  qv.append(c);
  qv.append(d);
  qv.append(e);
  qv.append(f);
  qv.append(g);
  qv.append(h);
  QCOMPARE(qv.at(0).value(), 1);
  QCOMPARE(qv.at(1).value(), 1);
  QCOMPARE(qv.at(2).value(), 2);
  QCOMPARE(qv.at(3).value(), 3);
  QCOMPARE(qv.at(4).error().text(), QString("error1"));
  QCOMPARE(qv.at(5).error().text(), QString("error1"));
  QCOMPARE(qv.at(6).error().text(), QString("error2"));
  QCOMPARE(qv.at(7).error().text(), QString("error3"));
  qv.clear();
  /*
   * Assure this not crash
   *  Bug discovered at 20160205
   */
  Expected<bool> b1(std::move(true));
  QVERIFY(b1);
  b1 = std::move(Expected<bool>(std::move(true)));
}

void ExpectedTest::implicitIntUseTest()
{
  auto x = getValue(2);
  QVERIFY(x);
  int y = incrementInt(*x);
  QCOMPARE(y, 3);
  // Check also with const expected
  const auto cx = getValue(5);
  y = incrementInt(*cx);
  QCOMPARE(y, 6);
}

void ExpectedTest::implicitQStringUseTest()
{
  auto d = getValue<QString>("D");
  QVERIFY(d);
  QString s = catAtoString(*d);
  QCOMPARE(s, QString("DA"));
  // Check also with const expected
  const auto c = getValue<QString>("C");
  s = catAtoString(*c);
  QCOMPARE(s, QString("CA"));
}

void ExpectedTest::implicitQVariantUseTest()
{
  auto v1 = getValue<QVariant>(3000);
  QVariant v2 = returnVariantAsIs(*v1);
  QCOMPARE(v2, QVariant(3000));
  // Check also with const expected
  const auto cv = getValue<QVariant>(500);
  v2 = returnVariantAsIs(*cv);
  QCOMPARE(v2, QVariant(500));
}

void ExpectedTest::dereferenceQStringTest()
{
  QString s;

  auto es = getValue<QString>("A");
  QVERIFY(es);
  QCOMPARE(*es, QString("A"));
  QCOMPARE(es->length(), 1);
  es->prepend("Z");
  QCOMPARE(*es, QString("ZA"));
  s = *es;
  QCOMPARE(s, QString("ZA"));
  // Check also with const expected
  const auto ces = getValue<QString>("CA");
  QVERIFY(ces);
  QCOMPARE(*ces, QString("CA"));
  QCOMPARE(ces->length(), 2);
}

void ExpectedTest::expectedResultTest()
{
  Mdt::ExpectedResult nullResult;
  QVERIFY(!nullResult);

  const Mdt::ExpectedResult resultOk = Mdt::ExpectedResultOk();
  QVERIFY(resultOk);

  const Mdt::ExpectedResult resultNok = mdtErrorNew("Nok", Mdt::Error::Critical, "ExpectedTest");
  QVERIFY(!resultNok);
  QCOMPARE(resultNok.error().text(), QLatin1String("Nok"));
}

/*
 * Helper functions
 */

int ExpectedTest::incrementInt(int value)
{
  return value + 1;
}

QString ExpectedTest::catAtoString(const QString& str)
{
  return str + "A";
}

QVariant ExpectedTest::returnVariantAsIs(const QVariant& var)
{
  return var;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ExpectedTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
