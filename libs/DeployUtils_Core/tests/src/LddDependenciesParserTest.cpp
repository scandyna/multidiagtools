/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "LddDependenciesParserTest.h"
#include "Mdt/DeployUtils/Impl/Ldd/DependenciesParserImpl.h"
#include "Mdt/DeployUtils/LddDependenciesParser.h"
#include "Mdt/PlainText/StringConstIterator.h"
#include "Mdt/PlainText/StringRecord.h"
#include "Mdt/PlainText/StringRecordList.h"
#include <QString>
#include <QStringList>

using namespace Mdt::DeployUtils;
using namespace Mdt::PlainText;

void LddDependenciesParserTest::initTestCase()
{
}

void LddDependenciesParserTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void LddDependenciesParserTest::recordGrammarTest()
{
  QFETCH(QString, sourceData);
  QFETCH(StringRecord, expectedRecord);
  QFETCH(bool, expectedOk);

  StringRecord record;
  Mdt::DeployUtils::Impl::Ldd::DependenciesRecordGrammar<StringConstIterator> grammar;
  StringConstIterator first = sourceData.cbegin();
  StringConstIterator last = sourceData.cend();
  bool ok = boost::spirit::qi::parse(first, last, grammar, record);
  // Check
  QCOMPARE(ok, expectedOk);
  QCOMPARE(record, expectedRecord);
}

void LddDependenciesParserTest::recordGrammarTest_data()
{
  QTest::addColumn<QString>("sourceData");
  QTest::addColumn<StringRecord>("expectedRecord");
  QTest::addColumn<bool>("expectedOk");

  const bool Ok = true;
  const bool Nok = false;

  QTest::newRow("")
   << "" << StringRecord{} << Nok;

  QTest::newRow("libc.so.6 => /lib/libc.so.6 (0x0123456789ABCDEF)")
   << "\tlibc.so.6 => /lib/libc.so.6 (0x0123456789ABCDEF)" << StringRecord{"libc.so.6","/lib/libc.so.6"} << Ok;

  QTest::newRow("linux-vdso.so.1 =>  (0x00007ffd4f1f7000)")
   << "\tlinux-vdso.so.1 =>  (0x00007ffd4f1f7000)" << StringRecord{"linux-vdso.so.1"} << Ok;

  QTest::newRow("liba.so.0 => /opt/a b/ (0x0123456789ABCDEF)")
   << "\tliba.so.0 => /opt/a b/liba.so.0 (0x0123456789ABCDEF)" << StringRecord{"liba.so.0","/opt/a b/liba.so.0"} << Ok;
}

void LddDependenciesParserTest::parserTest()
{
  QFETCH(QString, sourceData);
  QFETCH(StringRecordList, expectedRecordList);
  QFETCH(bool, expectedOk);

  StringRecordList recordList;
  LddDependenciesParser parser;
  bool ok = parser.parse(sourceData);
  QCOMPARE(ok, expectedOk);
  QCOMPARE(parser.rawDependencies(), expectedRecordList);
}

void LddDependenciesParserTest::parserTest_data()
{
  QTest::addColumn<QString>("sourceData");
  QTest::addColumn<StringRecordList>("expectedRecordList");
  QTest::addColumn<bool>("expectedOk");

  const bool Ok = true;
  const bool Nok = false;

  QTest::newRow("0")
   << "" << StringRecordList{} << Nok;

  QTest::newRow("1")
   << "\tlibc.so.6 => /lib/libc.so.6 (0x0123456789ABCDEF)"
   << StringRecordList{{"libc.so.6","/lib/libc.so.6"}} << Ok;

  QTest::newRow("2")
   << "\tlinux-vdso.so.1 =>  (0x0123456789ABCDEF)\n"
      "\tlibc.so.6 => /lib/libc.so.6 (0x0123456789ABCDEF)"
   << StringRecordList{{"linux-vdso.so.1"},{"libc.so.6","/lib/libc.so.6"}} << Ok;
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  LddDependenciesParserTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
