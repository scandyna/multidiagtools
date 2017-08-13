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
#include "ObjdumpDependenciesParserTest.h"
#include "Mdt/DeployUtils/Impl/Objdump/DependenciesParserImplWindows.h"
#include "Mdt/DeployUtils/ObjdumpDependenciesParser.h"
#include "Mdt/PlainText/StringConstIterator.h"
#include "Mdt/PlainText/StringRecord.h"
#include "Mdt/PlainText/StringRecordList.h"
#include <QString>
#include <QStringList>

using namespace Mdt::DeployUtils;
using namespace Mdt::PlainText;

void ObjdumpDependenciesParserTest::initTestCase()
{
}

void ObjdumpDependenciesParserTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ObjdumpDependenciesParserTest::recordGrammarWindowsTest()
{
  QFETCH(QString, sourceData);
  QFETCH(StringRecord, expectedRecord);
  QFETCH(bool, expectedOk);

  StringRecord record;
  Mdt::DeployUtils::Impl::Objdump::DependenciesRecordGrammarWindows<StringConstIterator> grammar;
  StringConstIterator first = sourceData.cbegin();
  StringConstIterator last = sourceData.cend();
  bool ok = boost::spirit::qi::parse(first, last, grammar, record);
  // Check
  QCOMPARE(ok, expectedOk);
  QCOMPARE(record, expectedRecord);
}

void ObjdumpDependenciesParserTest::recordGrammarWindowsTest_data()
{
  QTest::addColumn<QString>("sourceData");
  QTest::addColumn<StringRecord>("expectedRecord");
  QTest::addColumn<bool>("expectedOk");

  const bool Ok = true;
  const bool Nok = false;

  QTest::newRow("0")
   << "" << StringRecord{} << Nok;

  QTest::newRow("1")
   << "DLL Name: libgcc.dll" << StringRecord{"libgcc.dll"} << Ok;

}

void ObjdumpDependenciesParserTest::parserWindowsTest()
{
  QFETCH(QString, sourceData);
  QFETCH(StringRecordList, expectedRecordList);
  QFETCH(bool, expectedOk);

//   qDebug() << sourceData;

  StringRecordList recordList;
  ObjdumpDependenciesParser parser;
  bool ok = parser.parse(sourceData);
  QCOMPARE(ok, expectedOk);
  QCOMPARE(parser.rawDependencies(), expectedRecordList);
}

void ObjdumpDependenciesParserTest::parserWindowsTest_data()
{
  QTest::addColumn<QString>("sourceData");
  QTest::addColumn<StringRecordList>("expectedRecordList");
  QTest::addColumn<bool>("expectedOk");

  const bool Ok = true;
  const bool Nok = false;

  QTest::newRow("0")
   << ""
   << StringRecordList{} << Ok;

  QTest::newRow("1")
   << "DLL Name: liba.dll"
   << StringRecordList{{"liba.dll"}} << Ok;

  QTest::newRow("2")
   << "Some header\nDLL Name: libb.dll"
   << StringRecordList{{"libb.dll"}} << Ok;

  QTest::newRow("3")
   << "Some header\nDLL Name: libc.dll\nSome other text"
   << StringRecordList{{"libc.dll"}} << Ok;

  QTest::newRow("4")
   << objdumpOutputWindows( QStringList{"libd.dll"} )
   << StringRecordList{{"libd.dll"}} << Ok;

  QTest::newRow("5")
   << objdumpOutputWindows( QStringList{"libe.dll","libf.dll","libg.dll"} )
   << StringRecordList{{"libe.dll"},{"libf.dll"},{"libg.dll"}} << Ok;

  // Library with no dependencies
  QTest::newRow("6")
    << objdumpHeaderWindows()
    << StringRecordList{} << Ok;
}

/*
 * Helpers
 */

QString ObjdumpDependenciesParserTest::objdumpHeaderWindows()
{
  return "../../../cross/win32/release/bin/mdtcpbindeps.exe:     format de fichier pei-i386\n"
         "\n"
         "Caractéristiques 0x107\r\n"
         "relocations stripped\n"
         "executable\r\n"
         "line numbers stripped\r\n"
         "32 bit words\r\n"
         "\r\n"
         "DllCharacteristics  00000000\r\n"
         "\n"
         "The Data Directory\n"
         "Entry 0 00000000 00000000 Export Directory [.edata (or where ever we found it)]\n";
}

QString ObjdumpDependenciesParserTest::objdumpImportTablesHeaderWindows()
{
  return "Il y a une table d'importation dans .idata à 0x40a000\n"
         "\n"
         "Les tables d'importation (contenus interprété de la section .idata)\n"
         "vma:            Hint    Temps     Avant    DLL       Premier\n"
         "                Table   Estampil. Chaîne   Nom       Thunk\n"
         "0000a000 0000a0c8 00000000 00000000 0000b4b8 0000a2f0";
}

QString ObjdumpDependenciesParserTest::objdumpEntryWindows(const QString& libraryName)
{
  return "DLL Name: " + libraryName + "\n"
         "vma:  Hint/Ord Membre      Lien\r\n"
         "a518     13  _ZN3Mdt11Application4initENS0_26MultipleInstancePermissionE\n"
         "a558     14  _ZN3Mdt11ApplicationC1ERiPPc";
}

QString ObjdumpDependenciesParserTest::objdumpEntryListWindows(const QStringList& libraryNameList)
{
  QString entry;

  for(const auto & libraryName : libraryNameList){
    entry += objdumpEntryWindows(libraryName) + "\n";
  }

  return entry;
}

QString ObjdumpDependenciesParserTest::objdumpOutputWindows(const QStringList& libraryNameList)
{
  return objdumpHeaderWindows() + "\n" + objdumpImportTablesHeaderWindows() + "\n" + objdumpEntryListWindows(libraryNameList) + "\n";
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ObjdumpDependenciesParserTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
