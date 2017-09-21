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
#include "ObjdumpBinaryFormatParserTest.h"
#include "Mdt/DeployUtils/Impl/Objdump/BinaryFormatParserImpl.h"
#include "Mdt/DeployUtils/ObjdumpBinaryFormatParser.h"
#include "Mdt/PlainText/StringConstIterator.h"
#include "Mdt/PlainText/StringRecord.h"
#include <QString>
#include <QStringList>

using namespace Mdt::DeployUtils;
using namespace Mdt::PlainText;

void ObjdumpBinaryFormatParserTest::initTestCase()
{
}

void ObjdumpBinaryFormatParserTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ObjdumpBinaryFormatParserTest::fileFormatGrammarTest()
{
  QFETCH(QString, sourceData);
  QFETCH(QString, expectedFormat);
  QFETCH(bool, expectedOk);

  QString format;
  Mdt::DeployUtils::Impl::Objdump::FileFormatGrammar<StringConstIterator> grammar;
  StringConstIterator first = sourceData.cbegin();
  StringConstIterator last = sourceData.cend();
  bool ok = boost::spirit::qi::parse(first, last, grammar, format);
  // Check
  QCOMPARE(ok, expectedOk);
  QCOMPARE(format, expectedFormat);
}

void ObjdumpBinaryFormatParserTest::fileFormatGrammarTest_data()
{
  QTest::addColumn<QString>("sourceData");
  QTest::addColumn<QString>("expectedFormat");
  QTest::addColumn<bool>("expectedOk");

  const bool Ok = true;
  const bool Nok = false;

  QTest::newRow("0")
   << "" << "" << Nok;

  QTest::newRow("1")
   << "some_executable:   file format elf64-x86-64" << "elf64-x86-64" << Ok;

  QTest::newRow("2")
   << "some_executable:  format de fichier elf64-x86-64" << "elf64-x86-64" << Ok;

  QTest::newRow("3")
   << "some_executable:   file format pei-i386" << "pei-i386" << Ok;

  QTest::newRow("elf64-x86-64")
   << "file format elf64-x86-64" << "elf64-x86-64" << Ok;

  QTest::newRow("elf32-i386")
   << "file format elf32-i386" << "elf32-i386" << Ok;

  QTest::newRow("elf32-iamcu")
   << "file format elf32-iamcu" << "elf32-iamcu" << Ok;

  QTest::newRow("elf32-x86-64")
   << "file format elf32-x86-64" << "elf32-x86-64" << Ok;

  QTest::newRow("a.out-i386-linux")
   << "file format a.out-i386-linux" << "a.out-i386-linux" << Ok;

  QTest::newRow("pei-i386")
   << "file format pei-i386" << "pei-i386" << Ok;

  QTest::newRow("pei-x86-64")
   << "file format pei-x86-64" << "pei-x86-64" << Ok;

  QTest::newRow("elf64-l1om")
   << "file format elf64-l1om" << "elf64-l1om" << Ok;

  QTest::newRow("elf64-k1om")
   << "file format elf64-k1om" << "elf64-k1om" << Ok;

  QTest::newRow("elf64-little")
   << "file format elf64-little" << "elf64-little" << Ok;

  QTest::newRow("elf64-big")
   << "file format elf64-big" << "elf64-big" << Ok;

  QTest::newRow("elf32-little")
   << "file format elf32-little" << "elf32-little" << Ok;

  QTest::newRow("elf32-big")
   << "file format elf32-big" << "elf32-big" << Ok;

  QTest::newRow("pe-x86-64")
   << "file format pe-x86-64" << "pe-x86-64" << Ok;

  QTest::newRow("pe-bigobj-x86-64")
   << "file format pe-bigobj-x86-64" << "pe-bigobj-x86-64" << Ok;

  QTest::newRow("pe-i386")
   << "file format pe-i386" << "pe-i386" << Ok;


}

void ObjdumpBinaryFormatParserTest::architectureGrammarTest()
{
  QFETCH(QString, sourceData);
  QFETCH(QString, expectedFormat);
  QFETCH(bool, expectedOk);

  QString format;
  Mdt::DeployUtils::Impl::Objdump::ArchitectureGrammar<StringConstIterator> grammar;
  StringConstIterator first = sourceData.cbegin();
  StringConstIterator last = sourceData.cend();
  bool ok = boost::spirit::qi::parse(first, last, grammar, format);
  // Check
  QCOMPARE(ok, expectedOk);
  QCOMPARE(format, expectedFormat);

}

void ObjdumpBinaryFormatParserTest::architectureGrammarTest_data()
{
  QTest::addColumn<QString>("sourceData");
  QTest::addColumn<QString>("expectedFormat");
  QTest::addColumn<bool>("expectedOk");

  const bool Ok = true;
  const bool Nok = false;

  QTest::newRow("0")
   << "" << "" << Nok;

  QTest::newRow("i386:x86-64")
   << "architecture: i386:x86-64" << "i386:x86-64" << Ok;

  QTest::newRow("i386")
   << "architecture: i386" << "i386" << Ok;

  QTest::newRow("i386:x64-32")
   << "architecture: i386:x64-32" << "i386:x64-32" << Ok;

  QTest::newRow("i8086")
   << "architecture: i8086" << "i8086" << Ok;

  QTest::newRow("i386:intel")
   << "architecture: i386:intel" << "i386:intel" << Ok;

  QTest::newRow("i386:x86-64:intel")
   << "architecture: i386:x86-64:intel" << "i386:x86-64:intel" << Ok;

  QTest::newRow("i386:x64-32:intel")
   << "architecture: i386:x64-32:intel" << "i386:x64-32:intel" << Ok;

  QTest::newRow("i386:nacl")
   << "architecture: i386:nacl" << "i386:nacl" << Ok;

  QTest::newRow("i386:x86-64:nacl")
   << "architecture: i386:x86-64:nacl" << "i386:x86-64:nacl" << Ok;

  QTest::newRow("i386:x64-32:nacl")
   << "architecture: i386:x64-32:nacl" << "i386:x64-32:nacl" << Ok;

  QTest::newRow("iamcu")
   << "architecture: iamcu" << "iamcu" << Ok;

  QTest::newRow("iamcu:intel")
   << "architecture: iamcu:intel" << "iamcu:intel" << Ok;

  QTest::newRow("l1om")
   << "architecture: l1om" << "l1om" << Ok;

  QTest::newRow("l1om:intel")
   << "architecture: l1om:intel" << "l1om:intel" << Ok;

  QTest::newRow("k1om")
   << "architecture: k1om" << "k1om" << Ok;

  QTest::newRow("k1om:intel")
   << "architecture: k1om:intel" << "k1om:intel" << Ok;
}

void ObjdumpBinaryFormatParserTest::parserTest()
{
  QFETCH(QString, sourceData);
  QFETCH(OperatingSystem, expectedOperatingSystem);
  QFETCH(Processor, expectedProcessor);
  QFETCH(bool, expectedOk);

//   qDebug() << sourceData;

  ObjdumpBinaryFormatParser parser;
  bool ok = parser.parse(sourceData);
  QCOMPARE(ok, expectedOk);
  if(ok){
    QCOMPARE(parser.operatindSystem(), expectedOperatingSystem);
    QCOMPARE(parser.processor(), expectedProcessor);
  }
}

void ObjdumpBinaryFormatParserTest::parserTest_data()
{
  QTest::addColumn<QString>("sourceData");
  QTest::addColumn<OperatingSystem>("expectedOperatingSystem");
  QTest::addColumn<Processor>("expectedProcessor");
  QTest::addColumn<bool>("expectedOk");

  const bool Ok = true;
  /*
   * Once we have more experience (f.ex. bugs),
   * we should add tests on non valid data.
   */
//   const bool Nok = false;

  QTest::newRow("1")
   << "\n"
      "tests/mdtdeployutils_objdumpbinaryformatparsertest:     format de fichier elf64-x86-64\n"
      "architecture: i386:x86-64, fanions 0x00000112:\n"
      "EXEC_P, HAS_SYMS, D_PAGED\n"
      "adresse de départ 0x00000000004078f0\n"
   << OperatingSystem::Linux << Processor::X86_64 << Ok;

  QTest::newRow("2")
   << "\r\n"
      "../../../cross/win32/release/bin/mdtalgorithmtest.exe:     format de fichier pei-i386\r\n"
      "architecture: i386, fanions 0x0000013a:\r\n"
      "EXEC_P, HAS_DEBUG, HAS_SYMS, HAS_LOCALS, D_PAGED\r\n"
      "adresse de départ 0x004014e0\r\n"
   << OperatingSystem::Windows << Processor::X86_32 << Ok;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ObjdumpBinaryFormatParserTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
